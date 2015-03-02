clear; clc;
loadParams;

k = 1;
for i = 1: params.Ntrial
    x0 = zeros(2,1);
    u(k) = 0.1; urand(k) = 0;
    r(k) = 0;
    
    % Memory variabes    
    X(:,k) = x0;
    Phi(:,k) = params.phi;
    Theta(:,k) = params.theta;  
    for t = 0: params.ts: params.t_end
        time = [t t+params.ts];
        
        % apply u(k), measure x(k+1), receive r(k+1)
        [clock,x] = ode45(@(t,x) oneDofRobot(t, x, u(k), params), time, x0);
        x = transpose(x);
        X(:,k+1) = wrapTo2Pi(x(:,end));
        r(k+1) = calcCost(x(:,end), u(k), params); % change this to tracking 
        
        % choose next action u(k+1)
        urand(k+1) = 0.1*randn;
        u(k+1) = actor(x(:,end), params) + urand(k+1);
        
        % temporal difference signal
        delta(k) = r(k+1) + critic(X(:,k+1), params) - critic(X(:,k), params);
        
        V(k) = critic(X(:,k), params); clc
        V(k+1) = critic(X(:,k+1), params);
        
        % Update actor and critic
        if k == 1
            params.phi = params.phi + params.alpha*delta(k)*urand(k+1)*(u(k)-0)./(Phi(:,k)-zeros(size(params.phi)));
            params.theta = params.theta + params.alpha*delta(k)*(V(k)-zeros(size(V(k))))./(Theta(:,k)-zeros(size(params.theta)));
        else
            params.phi = params.phi + params.alpha*delta(k)*urand(k)*(u(k)-u(k-1))./(Phi(:,k)-Phi(:,k-1));            
%             params.alpha*delta(k)*(V(k)-zeros(size(V(k))))./(Theta(:,k)-zeros(size(params.theta)))
            params.theta = params.theta + params.alpha*delta(k)*(V(k)-V(k-1))./(Theta(:,k)-Theta(:,k-1));
        end
        Phi(:,k+1) = params.phi;
        Theta(:,k+1) = params.theta;
        x0 = X(:,end);
        k = k+1;
    end
end

Q = X(1,:);
animateRobot(0, Q(1:end));