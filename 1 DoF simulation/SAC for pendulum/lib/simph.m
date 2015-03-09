function xP = simph(x, u, par, sys)
%SIMPH Simulates the PH system and returns the next state
%
% XP = SIMPH(X, U, PAR, SYS) simulates the system sys.eom and returns the new
% state XP according to sys.xp
%
% INPUTS:   X: [n x 1] state of the system
%           
%           U: [m x 1] input vector           
%
%           PAR: structure containing at least # trials (par.trials), sample time
%           (par.Ts) and simtime (par.Tt)
%           
%           SYS: structure containing system information. At least
%           sys.eom and sys.xp are required.

if strcmp(func2str(par.odesolver),'ode4') || strcmp(func2str(par.odesolver),'ode4_ti')
    Y  = par.odesolver(@phsystem,[0 par.Ts],x,u,sys);
else
    [~, Y]  = par.odesolver(@phsystem,[0 par.Ts],x,[],u,sys);
end
xP      = feval(strcat(sys.name,'_xp'),Y(end,:),par);