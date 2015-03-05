%
% Simulation DEMO
%
% In this simulation demo, the SISO-EBAC algorithm is used for the
% invpend set-up model using the same settings as for the simulation:
%
% simulations/ebac/invpend/identification/identifiedboth
%
% to give a demo. These settings are also used in the paper, and the 
% equivalent experiment can be found in:
%
% experiments/ebac/exp3_MMandIdentified
%
% for which a demo can be run in:
%
% demo/EBAC_experiment
%
% Important simulation parameters are:
%
%       REWARD:             Cosine plus quadratic speed type
%       SATURATION:         Plain. See also sat
%       BASIS FUNCTIONS:    Fourier Basis. See also four
%       PARAMETERS:         Identifiedboth. This means the identified
%                           parameters are used for learning and simulation
%
% PLEASE feel free to play around with the parameters in invpend_par_demo
% in order to get a better understanding of the method.

clear
close all
clc

%% Run demo
simu.system      = 'invpend';               
simu.name        = 'this';                
simu.param       = 'model';         % Choose from 'model', 'identified' and 'identifiedboth'

disp('Running demo');
% Get system parameters
sys                             = invpend_sys(simu);
% Get simulation parameters
[par, theta, xi]           = invpend_SAC_par_demo(sys);
% Run SISO-EBAC
[theta, xi, bk, DK]  = siso_SAC(sys,par,theta,xi);
figure; plot(DK);
% clc
% pause(2)
% close(gcf)
%% Play clip
% figure()
% clip(bk.x_all(par.trials,:,:),sys,par)
% pause(2)
% close(gcf)
% %% Evaluate demo 
% % This will create and save figures in the folder /demo_figures. If the
% % figures are already present, they will be overwritten.
% eval_demo