close all;

params.J = 1.91e-4;     % pendulum inertia [kgm^2]
params.m = 5.50e-2;     % mass [kg]
params.g = 9.81;        % gravity [m/s^2]
params.l = 4.2e-2;      % link's length [m]
params.b = 3e-6;        % damping [Nms]
params.K = 5.36e-2;     % torque constant [Nm/A]
params.R = 9.5;         % rotor resistance (ohm)    
params.uSat = 4;

q0 = [3.1 0];           % initial state
ts = 0.008;             % sampling time
qref = [0 0];           % desired state
t = 0:ts:20;            % time

Q = zeros(1,2);         % state memory
T = 0;                  % time memory
U = 0;                  % control input memory
E = 0;                  % error memory
SE = 0;                 % sum of error memory

% PID gains
Kp = 4;
Kd = 0;
Ki = 0.001;
K = [Kp Kd Ki];

sum_error = 0;
for j =1 : length(t)-1
    time = [t(j) t(j+1)];
    qm = q0(end,:);             % measured state
    qm(1) = wrapToPi(qm(1));   % convert to 2*pi range
    error = qref-qm;
    sum_error = sum_error+error(1);
%     sum_error = wrapToPi(sum_error);
    err = [error sum_error];
    SE = [SE; sum_error];
    
    E =[E; err(2)];
    u = K*err';
    u = actSaturate(u, params);
    U = [U;u];
    
    [ti,q] = ode45(@(t,q) oneDofRobot(t, q, u, params), time, q0);
    q(1) = wrapToPi(q(1));
    Q = [Q; q];
    q0 = q(end,:);
    T = [T; ti];
    
end

Q = Q(:,1);
spacing = 30;
animateRobot(T(1:spacing:end), Q(1:spacing:end));
figure; subplot(3,1,1);
plot(T(1:10:end), Q(1:10:end), '.'); title('angle position [rad]');
subplot(3,1,2); plot(E,'r'); title('angle pos error [rad]');
subplot(3,1,3); plot(U,'g'); title('control input [V]');