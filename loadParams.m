params.Ntrial   = 100;                                % no of trials
params.NrbfX    = 10;                                   % no of rbfs in q dimension
params.NrbfY    = 10;                                   % no of rbfs in qdot dimension

params.phi      = ones(params.NrbfX*params.NrbfY,1);     % actor parameters
params.theta    = ones(params.NrbfX*params.NrbfY,1);	% critic parameters
params.ref      = [-pi/2; 0];

% generate center of rbf (2xN matrix)
qc = linspace(-pi, pi, params.NrbfX);   
qc = repmat(qc, [params.NrbfY,1]);
qc = reshape(qc, [1, params.NrbfX*params.NrbfY]);
qdotc = linspace(-25, 25, params.NrbfY);
qdotc = repmat(qdotc, [1, params.NrbfX]);
params.c = [qc; qdotc];                     % center coordinate of all rbfs

params.t_end = 5;                           % final time    [s]
params.ts = 1/125;                          % sampling time [s]

% RL parameters
params.B = [50 0; 0 50];
params.R = 1;
params.Q = [10 0; 0 3];
params.alpha_a = 0.000001;
params.alpha_c = 0.0001;
params.gamma = 0.5;

% Robot Parameters
params.J = 1.91e-4;     % pendulum inertia [kgm^2]
params.m = 5.50e-2;     % mass [kg]
params.g = 9.81;        % gravity [m/s^2]
params.l = 4.2e-2;      % link's length [m]
params.b = 3e-6;        % damping [Nms]
params.K = 5.36e-2;     % torque constant [Nm/A]
params.R = 9.5;         % rotor resistance (ohm)
params.uSat = 1;        % voltage saturation value