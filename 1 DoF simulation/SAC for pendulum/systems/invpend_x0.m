function x0  = invpend_x0(sys,par)
%INVPEND_X0 Generate initial condition for simulation
%
% X0  = INVPEND_X0(SYS) Generates the initial condition for each simulation
% 
% INPUTS:   SYS: Structure containing system parameters 
%
%           PAR: Structure containing simulation parameter settings
%

if strcmp(par.x0,'fixed')
    x0  = par.x0learning;
elseif strcmp(par.x0,'random')
    x0 = [0.75*par.x0learning(1) + 0.5*par.x0learning(1).*rand;par.p.min + 2*par.p.max.*rand];
end