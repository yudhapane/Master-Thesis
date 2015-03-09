function xP = invpend_xp(Y,par)
%INVPEND_XP Calculates the next state xP 
%
% XP = INVPEND_XP(Y,PAR) Calculates the next state xP based on the state vector
% Y that is supplied
%
% INPUTS:   Y: State vector from simulation of the PH-system
%
%           PAR: Structure containing simulation parameters
%
% OUTPUTS:  xP: Modified next state x
%

xP = [wrapToPi(Y(1)); sat(Y(2),par.p,0)];

% xP = [wrapToPi(Y(1)); Y(2)];