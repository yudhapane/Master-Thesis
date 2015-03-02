close all;

params.J = 1.91e-4;     % pendulum inertia [kgm^2]
params.m = 5.50e-2;     % mass [kg]
params.g = 9.81;        % gravity [m/s^2]
params.l = 4.2e-2;      % link's length [m]
params.b = 3e-6;        % damping [Nms]
params.K = 5.36e-2;     % torque constant [Nm/A]
params.R = 9.5;         % rotor resistance (ohm)    
    
q0 = [0 0];             % initial state
ts = 0.008;             % sampling time
qref = [pi/2 0];        % desired state
t = 0:ts:20;            % time

Q = zeros(1,2);         % state memory
T = 0;                  % time memory
U = 0;                  % control input memory
E = 0;                  % error memory
SE = 0;                 % sum of error memory

% PID gains
Kp = 1;
Kd = 0.1;
Ki = .01;
K = [Kp Kd Ki];

sum_error = 0;
for j =1 : length(t)-1
    time = [t(j) t(j+1)];
    qm = q0(end,:);
    qm(1) = wrapTo2Pi(qm(1));
    error = qref-qm;
    sum_error = sum_error+error(1);
    err = [error sum_error];
    SE = [SE; sum_error];
    
    E =[E; err(2)];
    u = K*err';
    U = [U;u];

    [ti,q] = ode45(@(t,q) oneDofRobot(t, q, u, params), time, q0);
    q(1) = wrapTo2Pi(q(1));
    Q = [Q; q];
    q0 = q(end,:);
    T = [T; ti];
end

Q = Q(:,1);
animateRobot(T(1:40:end), Q(1:40:end));
figure; subplot(3,1,1);
plot(T(1:10:end), Q(1:10:end), '.'); title('angle position [rad]');
subplot(3,1,2); plot(E,'r'); title('angle pos error [rad]');
subplot(3,1,3); plot(U,'g'); title('control input [V]');