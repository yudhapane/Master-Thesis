function [theta, xi, bk, DK] = siso_SAC(sys,par,theta,xi)
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



% Bookkeeping
bk              = bookkeeping(par.xdes,par,sys);
DK(1) = 0;
count = 1;
% Energy-Balancing & Damping Injection Actor-Critic RL
for kk = 1:par.trials
    % Initial condition
    x                   = zeros(sys.n, par.Ns);
    x(:,1)              = feval(strcat(sys.name,'_x0'),sys,par);
    % Eligibility trace
    e_c                 = 0;

    for k = 1:par.Ns        
        % Actor feature values
        [phiA, ~]                    = four(x(:,k),par.aBF);
        
        % Compute random action
        if(kk>150)
            par.randu = 1/10;
        end
        Delta_u                 = exploration(par,k);
        bk.Delta_u(kk,k,:)      = Delta_u;
        % Choose action u(k)

        u                       = xi'*phiA;           % EB-PBC control law
        [uc, ducdu]             = sat(u,par.u,Delta_u);
        
        
        Delta_u                 = uc - u;
        
        % Apply action uc(k) and return next state x(k+1)
        x(:,k+1)                = simph(x(:,k),uc,par,sys);
        
        % Receive reward
        rP                      = feval(strcat(sys.name,'_reward'),x(:,k+1),uc,par);
        
        % Critic feature values for x, xP
        [phiC, ~]          		= feval(par.cBF.type,x(:,k),par.cBF);                
        [phiPC, ~]              = feval(par.cBF.type,x(:,k+1),par.cBF); 
        
        % CRITIC UPDATE
        dk                      = rP + par.gamma*phiPC'*theta - phiC'*theta;        % Temporal difference
        e_c                     = par.gamma*par.lambda*e_c + phiC                  % Eligibility trace
        thetaP                  = theta + dk*par.cBF.alpha.*phiC;                    % Critic parameter update
        
        % ACTOR UPDATES
        xiP                     = xi + dk*ducdu*Delta_u*par.aBF.alpha.*phiA;       % Update ES
        
        % Calculate parameter error
%         err                     = siso_ebac_param_error(theta,thetaP,xi,xiP,psi,psiP);
        
        % Fill bookkeeping variables        
        bk.rc(kk)               = bk.rc(kk) + rP;
        bk.dkc(kk)              = bk.dkc(kk) + dk;
        bk.x_all(kk,k,:)        = x(:,k);
        bk.uc(kk,k,:)           = uc;
        bk.u(kk,k,:)            = u;
        % Assign updated values to variables
        theta                   = thetaP;
        xi                      = xiP;
        DK(count) = dk;
        count = count+1;
    end
    
   % Online plot 
   onlineplot(bk,kk,par,sys);
   
end

        
        
        
        
        
        
        
        
        