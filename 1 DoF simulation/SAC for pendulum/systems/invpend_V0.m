function theta0 = invpend_V0(par,V0)
% INVPEND_V0 Calculates the initialization of the value function
%
% THETA = INVPEND_V0WORST(PAR) Determines the feature value for each state and
% assigns the value V0 to that state. Then the value
% function parameter theta0 is computed
%
% INPUTS:   PAR: Structure containing simulation parameters
%
% OUTPUTS:  THETA0: Parameter vector 
%
%
% WORKS ONLY WITH THE FOURIER BASIS

% Calculate theta0 corresponding to intial V0 for all x
theta0                           = [V0; zeros(length(par.cBF.pb)-1,1)];