function [J, R, dHdx, g]  = invpend_eom(x,sys)
%INVPEND_EOM Calculates J, R, dHdx and g for the invpend setup
%
% [J, R, DHDX, G] = INVPEND_EOM(X,SYS) calculates the port-Hamiltonian
% matrices J, R, DHDX, G for the inverted pendulum setup such that it can be
% simulated. See also phsystem
%
% INPUTS:   X: [2 x 1] vector containing states of system
%
%           SYS: structure containing system parameters
%
% OUTPUTS:  J: [2 x 2] matrix containing the interconnection structure for
%           which it holds that J(x) = -J(x)^T
%           
%           R: [2 x 2] matrix containing the damping matrix for
%           which it holds that R(x) = R(x)^T > 0
%
%           DHDX: [2 x 1] vector of gradients of Hamiltonian H.
%
%           G: [2 x 1] input matrix

% Define position and momentum
q       = x(1);
p       = x(2);

% Calculate Hamiltonian gradients


dHdq    = -sys.a*sin(q);
dHdp    = p/sys.b;
dHdx    = [dHdq; dHdp];

% Calculate other system matrices
J       = [0 1;-1 0];
if isfield(sys,'e')
    R       = [0 0;0 sys.c+sys.e*pinv(abs(dHdp))];
else
    R       = [0 0;0 sys.c];
end
g       = [0; sys.d];


