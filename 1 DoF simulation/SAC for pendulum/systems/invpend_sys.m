function sys = invpend_sys(simu)
%INVPEND_SYS Contains parameters for the invpend set-up
%
% SYS = INVPEND_SYS Returns the parameters of the inverted pendulum set-up. 
% See also invpend_eom
%
% INPUT:    SIMU: Structure containing global simulation settings
%
% OUTPUT:   SYS: Structure containing system parameters
%

% 'True' system parameters
sys.name        = 'invpend';                    
sys.n           = 2;                           % Dimension of system

if strcmp(simu.param,'model') == 1
    sys.Jp          = 1.91*10^(-4);                % pendulum parameters
    sys.Mp          = 5.50*10^(-2);                % pendulum mass
    sys.gp          = 9.81;                        % gravity
    sys.lp          = 4.20*10^(-2);                % pendulum length
    sys.bp          = 3*10^(-6);                   % damping
    sys.Kp          = 5.36*10^(-2);                % torque constant
    sys.Rp          = 9.50;                        % Rotor resistance
    % 
    sys.a           = sys.Mp*sys.lp*sys.gp;
    sys.b           = sys.Jp;
    sys.c           = (sys.bp+(sys.Kp^2/sys.Rp));
    sys.d           = sys.Kp/sys.Rp;

% Simulation parameters for use in control law. This facilitates studying
% sensitivity effects (to uncertain/partially unknown parameters)
    sys.sim.Jp          = 1.91*10^(-4);                % pendulum parameters
    sys.sim.Mp          = 5.50*10^(-2);                % pendulum mass
    sys.sim.gp          = 9.81;                        % gravity
    sys.sim.lp          = 4.20*10^(-2);                % pendulum length
    sys.sim.bp          = 3*10^(-6);                   % damping
    sys.sim.Kp          = 5.36*10^(-2);                % torque constant
    sys.sim.Rp          = 9.50;                        % Rotor resistance
    % 
    sys.sim.a           = sys.sim.Mp*sys.sim.lp*sys.sim.gp;
    sys.sim.b           = sys.sim.Jp;
    sys.sim.c           = (sys.sim.bp+(sys.sim.Kp^2/sys.sim.Rp));
    sys.sim.d           = sys.sim.Kp/sys.sim.Rp;
elseif strcmp(simu.param,'identified') == 1
    load('invpend_parameters')
    % Simulation parameters for use in control law. This facilitates studying
    % sensitivity effects (to uncertain/partially unknown parameters)
    sys.sim.Jp          = 1.91*10^(-4);                % pendulum parameters
    sys.sim.Mp          = 5.50*10^(-2);                % pendulum mass
    sys.sim.gp          = 9.81;                        % gravity
    sys.sim.lp          = 4.20*10^(-2);                % pendulum length
    sys.sim.bp          = 3*10^(-6);                   % damping
    sys.sim.Kp          = 5.36*10^(-2);                % torque constant
    sys.sim.Rp          = 9.50;                        % Rotor resistance
    % 
    sys.sim.a           = sys.sim.Mp*sys.sim.lp*sys.sim.gp;
    sys.sim.b           = sys.sim.Jp;
    sys.sim.c           = (sys.sim.bp+(sys.sim.Kp^2/sys.sim.Rp));
    sys.sim.d           = sys.sim.Kp/sys.sim.Rp;
elseif strcmp(simu.param,'identifiedboth') == 1
    load('invpend_parameters')
    sys.sim.a           = sys.a;
    sys.sim.b           = sys.b;
    sys.sim.c           = sys.c;
    sys.sim.d           = sys.d;
    sys.sim.e           = sys.e;    
end
