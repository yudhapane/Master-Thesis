function rP = invpend_reward(x,uc,par)
%INVPEND_REWARD Calculates the numerical reward for the inverted pendulum
%
% RP = INVPEND_REWARD(X,UC,PAR) Calculates the numerical reward for the
% inverted pendulum set-up. Options are 'quad' and 'cosine'
%
% INPUTS:   X: [2 x 1] state vector
%           
%           UC: scalar control action
%
%           PAR: Structure containing simulation parameters
%

if strcmp(par.rewtype,'quad')
    rP    = -(x-par.xdes)'*par.Q*(x-par.xdes)-uc*par.R*uc;
elseif strcmp(par.rewtype,'cosine')
    rP    = par.cosangle*(cos(x(1)-par.xdes(1))-1)-x(2)*par.cosspeed*x(2)-uc*par.R*uc;
end