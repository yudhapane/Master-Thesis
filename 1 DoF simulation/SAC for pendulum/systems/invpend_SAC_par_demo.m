function [par, theta, xi] = invpend_SAC_par_demo(sys)
%INVPEND_DEMO Parameters for demo simulation of the invpend set-up
%
% PAR = INVPEND_PAR_DEMO Returns the simulation parameters of the 
% inverted pendulum set-up for a DEMO simulation
%
% INPUT:    SYS: Structure containing system parameters 
%
% OUTPUT:   PAR: Structure containing simulation parameters
%           
%           THETA: Critic parameter vector
%
%           XI: Energy-shaping parameter vector
%
%           PSI: Damping injection parameter vector
%
% This is a DEMO of the simulation experiment:
%
% simulations/ebac/invpend/identification/identifiedboth
%
% PLEASE feel free to play around with the parameters in this file
% in order to get a better understanding of the EBAC method.

% Simulation
par.trials      = 200;              % Number of trials
par.Tt          = 3;                % Time per trial
par.Ttsim       = 3;                % Time used for simulation after learning
par.Ts          = 0.03;             % Sample time
par.Ns          = par.Tt/par.Ts;    % Number of samples per trial
par.gamma       = 0.97;             % Discount factor; exp(-Ts) if changing Ts
par.lambda      = 0.65;             % Eligibility trace decay rate
par.odesolver   = @ode4_ti;         % ODE solver. Choose from 'ode4', 'ode4_ti' or a class of normal ODE solvers, e.g. 'ode45', 'ode23', etc.

% Plot settings
par.plotonline  = 'on';             % choose 'on', 'off' or 'iter' for online plot
par.radiusplot  = 1.5; 
par.clipspeed   = 1;

% Bounds on state-space
par.p.min       = -8e0*pi*sys.b;
par.p.max       = 8e0*pi*sys.b;
par.p.sattype   = 'plain';          % Velocity is cut-off above certain value
par.q.min       = -pi;
par.q.max       = pi;
par.q.sattype   = 'plain'; 

% Initial condition, exploration, control saturation
par.x0          = 'fixed';          % Starting position, choose from 'fixed' or 'random'
par.x0learning  = [pi; 0];          % x0 per trial
par.x0sim       = [pi-0.04; 0];     % Small perturbation for simulation after learning to avoid zero measure (necessary for symmetrical Fourier Basis)
par.randutype   = 'zmwn';           % Type of perturbation (only 'zmwn' can be chosen)
par.randu       = 1;                % Size of saturation
par.randudis    = 1;                % Per how many trials a random action must be performed
par.u.max       = 3;                % Max control input
par.u.min       = -3;               % Min control input
par.u.sattype   = 'plain';          % choose 'plain', 'tanh' or 'atan'
par.u.skew      = 0.5;              % skewness of saturation: only for use with 'tanh' or 'atan'

% Reward settings
par.rewtype     = 'cosine';                   % choose 'quad' or 'cosine'
par.Q           = diag([5, 0.1/sys.b^2]);     % State penalty
par.R           = 0;                          % Control action penalty
par.cosspeed    = 0.1/sys.b^2;                % Speed penalty on cosine reward
par.cosangle    = 25;                         % Position penalty on cosine reward

% Desired equilibrium
par.xdes        = [0; 0];

% CRITIC basis function settings for FOURIER BASIS
par.cBF.type               = 'four';
par.cBF.N                  = 4;                                             % Order of Fourier Basis
par.cBF.a                  = 1e-2;                                          % Learning rate
par.cBF.r                  = [par.q.min par.q.max; par.p.min par.p.max];    % State-space bounds
par.cBF.rn                 = [-1 1;-1 1];                                   % Projection x -> \bar{x}
par.cBF.f                  = 2;                                             % Which type of Fourier approximation. Choose '0' (sine), '1' (cosine) or 2 (sine+cosine). Note: '2' will generate twice as much parameters
par.cBF.T                  = 2;                                             % Period of Fourier Basis
[par.cBF.pb, par.cBF.alpha] = fourgen(par.cBF);                             % Generate frequency matrix and learning rate vector
par.cBF.alpha = [par.cBF.alpha;par.cBF.alpha];

% ACTOR ES basis function settings for FOURIER BASIS
par.aBF.type                  = 'four'; 
par.aBF.N                     = 4;                                        % Order of Fourier Basis
par.aBF.a                     = 5e-5;                                 % Learning rate
par.aBF.r                     = [par.q.min par.q.max; par.p.min par.p.max];    % State-space bounds
par.aBF.rn                    = [-1 1;-1 1];                                   % Projection x -> \bar{x}
par.aBF.f                     = 2;                                        % Which type of Fourier approximation. Choose '0' (sine), '1' (cosine) or 2 (sine+cosine). Note: '2' will generate twice as much parameters
par.aBF.T                     = 2;                                        % Period of Fourier Basis
[par.aBF.pb, par.aBF.alpha]  = fourgen(par.aBF);                       % Generate frequency matrix and learning rate vector
par.aBF.alpha = [par.aBF.alpha;par.aBF.alpha];





% Generate parameter vectors
theta   = zeros(length(par.cBF.alpha),1);                                               % Initial critic parameter vector based on value function initialization
xi      = zeros(length(par.aBF.alpha),1);                                    % Initial energy-shaping parameter vector
