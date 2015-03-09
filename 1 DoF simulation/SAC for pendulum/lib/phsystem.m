function xdot = phsystem(t,x,u,sys)
%PHSYSTEM Calculates response of a PH-system
%
% XDOT = PHSYSTEM(T,X,U,SYS) calculates the response of the 
% input-state-output port-Hamiltonian (PH) system
% 
%       xdot = (J(x)-R(x))*dHdx(x) + g(x)u
%
% INPUTS:   T: time vector, for simulation not necessary
%           
%           X: [n x 1] state vector
%           
%           U: [m x 1] control input vector
%           
%           SYS: Structure containing system information. At least SYS.EOM
%           is required for the type of system.
%
% OUTPUTS:  XDOT: [n x 1] xdot.

% Calculate J(x), R(x), dHdx(x) and g(x)
[J, R, dHdx, g] = feval(strcat(sys.name,'_eom'),x,sys);

% Calculate xdot
xdot = (J-R)*dHdx + g*u;