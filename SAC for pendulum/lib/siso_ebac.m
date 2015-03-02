function [theta, xi, psi, par, sys, bk] = siso_ebac(sys,par,theta,xi,psi)
%SISO_EBAC Run the Energy-Balancing Actor-Critic algorithm for a SISO
%system
%
% [THETA, XI, PSI, PAR, SYS, BK] = SISO_EBAC(SYSTEM,EXPERIMENT) Runs an
% experiment for the EBAC-algorithm and returns the parameter vectors THETA
% for the Critic, XI for the energy-shaping actor and Psi for the damping
% injection actor such that these parameters can be used in simulation for
% a SISO system.
% 
% INPUTS:   SYSTEM: Type of system. Choose from 'invpend', 'doublepend',
%           'msd' or 'cartpole'
%
%           PAR: Structure containing simulation variables
%
%           THETA: Critic initialization parameter vector, usually zeros
%
%           XI: Actor 1 initialization parameter vector, usually zeros
%
%           PSI: Actor 2 initialization parameter vector, usually zeros
% 
% OUTPUTS:  THETA: Critic parameter vector
%
%           XI: Energy-shaping actor
%
%           PSI: Damping-injection actor
%           
%           PAR: Structure containing simulation parameters used
%
%           SYS: Structure containing system parameters used
%
%           BK: Structure containing important bookkeeping variables. 
%           See also bookkeeping


% Generate system matrices
[J, R, ~, g]    = feval(strcat(sys.name,'_eom'),zeros(sys.n,1),sys);

% Compute on which states energy can be shaped
sys.shape       = ebpbc_pde(J,R,g); 

% Bookkeeping
bk              = bookkeeping(par.xdes,par,sys);

% Energy-Balancing & Damping Injection Actor-Critic RL
for kk = 1:par.trials
    % Initial condition
    x                   = zeros(sys.n, par.Ns+1);
    x(:,1)              = feval(strcat(sys.name,'_x0'),sys,par);
    % Eligibility trace
    e_c                 = 0;

    for k = 1:par.Ns        
        % Actor feature values
        [phiA.ES dphiA.ES]      = feval(par.aESBF.type,x(sys.shape,k),par.aESBF);                 % Energ-balancing actor basis functions for x
        [phiA.DI ~]             = feval(par.aDIBF.type,x(par.damping,k),par.aDIBF);               % Damping injection actor
        
        % Compute random action
        Delta_u                 = exploration(par,k);
        bk.Delta_u(kk,k,:)        = Delta_u;
        % Choose action u(k)
        [J, R, dHdx, g]         = feval(strcat(sys.name,'_eom'),x(:,k),sys.sim);    % System matrices
        F                       = J-R; 
        dHddx                   = dHdx;
        dHddx(sys.shape)        = xi'*dphiA.ES;                                     % Energy-shaping
        Kd                      = psi'*phiA.DI;                                     % Damping-injection
        u                       = pinv(g)*(F*(dHddx-dHdx)) - Kd*g'*dHddx;           % EB-PBC control law
        udi = Kd*g'*dHddx;
        [uc ducdu]              = sat(u,par.u,Delta_u);
%         if(any(any(abs(pinv(g)*F) > 1)))
%             pinv(g)*F
%             dHddx-dHdx
%             ducdu
%             k
%             kk
%         end
%         Kd*g'
%         if(any(abs(dHddx-dHdx) > 1e-1))
%             pinv(g)*(F*(dHddx-dHdx))
%             k
%             kk
%         end
        
        
        Delta_u                 = uc - u;
        
        % Apply action uc(k) and return next state x(k+1)
        x(:,k+1)                = simph(x(:,k),uc,par,sys);
        
        % Receive reward
        rP                      = feval(strcat(sys.name,'_reward'),x(:,k+1),uc,par);
        
        % Critic feature values for x, xP
        [phiC dphiCdx]          = feval(par.cBF.type,x(:,k),par.cBF);                
        [phiPC ~]               = feval(par.cBF.type,x(:,k+1),par.cBF); 
        
        % CRITIC UPDATE
        dk                      = rP + par.gamma*phiPC'*theta - phiC'*theta;        % Temporal difference
        e_c                     = par.gamma*par.lambda*e_c + phiC;                  % Eligibility trace
        thetaP                  = theta + dk*par.cBF.alpha.*e_c;                    % Critic parameter update
        
        % ACTOR UPDATES
        [polgradES polgradDI]   = feval(strcat(sys.name,'_ebpbc_polgrad'),phiA,dphiA,dphiCdx,theta,g,dHddx,ducdu,dk,Delta_u,par);   % policy gradient
        xiP                     = xi + par.aESBF.alpha.*polgradES;       % Update ES
        psiP                    = psi + par.aDIBF.alpha.*polgradDI;      % Update DI

        % Calculate parameter error
        err                     = siso_ebac_param_error(theta,thetaP,xi,xiP,psi,psiP);
        
        % Fill bookkeeping variables        
        bk.rc(kk)               = bk.rc(kk) + rP;
        bk.dkc(kk)              = bk.dkc(kk) + dk;
%         bk.error(kk,:)          = bk.error(kk,:) + err';
        bk.x_all(kk,k,:)        = x(:,k);
        bk.uc(kk,k,:)        = uc;
        bk.u(kk,k,:)        = u;
        bk.Delta_u(kk,k,:)        = Delta_u;
%         bk.polgrad.es(kk,k,:)   = polgradES;
%         bk.polgrad.di(kk,k,:)   = polgradDI;
        bk.Hd(kk,k) = xi'*phiA.ES;
        bk.Rd(kk,k) = R(2,2)+psi'*phiA.DI;
        % Assign updated values to variables
        theta                   = thetaP;
        xi                      = xiP;
        psi                     = psiP;
    end
    
   % Online plot 
%    onlineplot(bk,kk,par,sys);
   
end

        
        
        
        
        
        
        
        
        