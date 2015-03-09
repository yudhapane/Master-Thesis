function [par, theta, xi, psi] = invpend_par_demo(sys)
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
par.cBF.N                  = 3;                                             % Order of Fourier Basis
par.cBF.a                  = 0.05;                                          % Learning rate
par.cBF.r                  = [par.q.min par.q.max; par.p.min par.p.max];    % State-space bounds
par.cBF.rn                 = [-1 1;-1 1];                                   % Projection x -> \bar{x}
par.cBF.f                  = 1;                                             % Which type of Fourier approximation. Choose '0' (sine), '1' (cosine) or 2 (sine+cosine). Note: '2' will generate twice as much parameters
par.cBF.T                  = 2;                                             % Period of Fourier Basis
[par.cBF.pb par.cBF.alpha] = fourgen(par.cBF);                              % Generate frequency matrix and learning rate vector


% ACTOR ES basis function settings for FOURIER BASIS
par.aESBF.type                  = 'four'; 
par.aESBF.N                     = 3;                                        % Order of Fourier Basis
par.aESBF.a                     = 1e-9*0.01;                                 % Learning rate
par.aESBF.r                     = [par.q.min par.q.max];                    % State-space bounds
par.aESBF.rn                    = [-1 1];                                   % Projection x -> \bar{x}
par.aESBF.f                     = 1;                                        % Which type of Fourier approximation. Choose '0' (sine), '1' (cosine) or 2 (sine+cosine). Note: '2' will generate twice as much parameters
par.aESBF.T                     = 2;                                        % Period of Fourier Basis
[par.aESBF.pb par.aESBF.alpha]  = fourgen(par.aESBF);                       % Generate frequency matrix and learning rate vector


% ACTOR DI basis function settings for FOURIER BASIS
par.damping                     = [1 2];                                    % This means on which states damping will be injected. 
par.aDIBF.type                  = 'four';
par.aDIBF.N                     = 3;                                        % Order of Fourier Basis
par.aDIBF.a                     = 1e-0*0.2;                                 % Learning rate
par.aDIBF.r                     = [par.q.min par.q.max; par.p.min par.p.max];  % State-space bounds
par.aDIBF.rn                    = [-1 1;-1 1];                              % Projection x -> \bar{x}
par.aDIBF.f                     = 1;                                        % Which type of Fourier approximation. Choose '0' (sine), '1' (cosine) or 2 (sine+cosine). Note: '2' will generate twice as much parameters                                   
par.aDIBF.T                     = 2;                                        % Period of Fourier Basis
[par.aDIBF.pb par.aDIBF.alpha]  = fourgen(par.aDIBF);                       % Generate frequency matrix and learning rate vector


% Calculate arbitrary initial Value function V0
rPworst                         = invpend_reward([par.q.min; par.p.min],par.u.max,par); % Worst possible reward
V0                              = 1/(1-par.gamma)*rPworst;                              % Worst possible value function 
V0                              = 0;                                        % Choose e.g. '-1000' to initialize value function realistically, or comment out to use worst possible.


% Generate parameter vectors
theta   = invpend_V0(par,V0);                                               % Initial critic parameter vector based on value function initialization
xi      = zeros(length(par.aESBF.pb),1);                                    % Initial energy-shaping parameter vector
psi     = zeros(length(par.aDIBF.pb),1);                                    % Initial damping-injection parameter vector