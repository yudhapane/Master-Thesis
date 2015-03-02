function qdot = oneDofRobot(t, q, u, params)
    qdot = [q(2); 1/params.J*(params.m*params.g*params.l*sin(q(1)) - (params.b+params.K^2/params.R)*q(2) + params.K/params.R*u)]; %
    