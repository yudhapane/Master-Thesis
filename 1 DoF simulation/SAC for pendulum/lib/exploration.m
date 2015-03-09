function Delta_u = exploration(par,jj)
%EXPORATION Calculates exploratory action
%
% DELTA_U = EXPLORATION(PAR) Calculates the action Delta_u for
% exploration in RL
%
% INPUTS:   PAR: Structure containing simulation parameter settings
%
% OUTPUT:   DELTA_U: Exploratory action according to par
%

if strcmp(par.randutype,'zmwn')
    if mod(jj,par.randudis) == 0
        Delta_u    = par.randu*randn();
    else
        Delta_u = 0;
    end
end