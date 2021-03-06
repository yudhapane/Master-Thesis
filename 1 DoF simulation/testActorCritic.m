%testActorCritic run an actor critic RL controller for a 1-DoF inverted pendulum
%
% Copyright 2015 Yudha Pane

clear; clc; close all;
loadParams;

xinit       = [pi ;0];                  % initial state
x0          = [pi; 0];
e0          = 1;                        % initial eligibility trace
k           = 1;                        % initialize index variable
Phi(:,k)    = params.phi;               % actor parameters memory
Theta(:,k)  = params.theta;             % critic parameters memory
Ret(:,k)    = zeros(params.Ntrial,1);   % return memory
V(k)        = 0;                        % initialize value function
uSel(k)     = 1;

for counter = 1:params.Ntrial
    X(:,k) = xinit;     
    if mod(k,2)==0          % generate a randomized initial input
        u(k) = 1.5 + rand;
    else
        u(k) = -1.5 + rand;
    end
    uc(k) = u(k);           % subbu's
%     u(k) = randn;
    urand(k) = 0;       % initialize input and exploration signal
    r(k) = 0;           % initialize reward/cost
    z_c(:,k) = zeros(params.NrbfX*params.NrbfY,1);
    z_a(:,k) = zeros(params.NrbfX*params.NrbfY,1);
    x0 = xinit;         % reset state to initial position

%     E(k) = e0;      % eligibility trace
%     counter             % check counter   
    for t = 0: params.ts: params.t_end
        time = [t t+params.ts];
        
        % apply u(k), measure x(k+1), receive r(k+1)        
%         Delta_u     = actSaturate(u(k), params) - u(k);
        [clock,x]   = ode45(@(t,x) oneDofRobot(t, x, uc(k), params), time, x0); % subbu's, note the uc
        x           = transpose(x);
        x(1,:)      = wrapTo180(rad2deg(x(1,:)));           % convert the angle to 2pi rad
        x(1,:)      = deg2rad(x(1,:));
        X(:,k+1)    = x(:,end);                             % get the state measurement
        r(k+1)      = calcCost(X(:,k), uc(k), params);      % calculate the cost / reward  % subbu's note the uc
        Ret(counter,k+1)    = params.gamma*Ret(counter,k) + r(k+1);  % update return value
        
        % compute next action u(k+1)
        if mod(k,3) == 0
            urand(k+1)  = randn;            % explore only once every 12 time steps                           
        else
            urand(k+1)  = 0;
        end
 
        Delta_u = urand(k+1);       % subbu's

%         u(k+1)      = actor(X(:,k+1), params) + urand(k+1);
%         [u(k+1), uSel(k+1)]      = actSaturate(u(k+1), params);                  % input saturation

        % Based on subbu's code
        u(k+1)      = actor(X(:,k+1), params); % subbu's
        [uc(k+1), uSel(k+1)]      = sat(u(k+1), params, urand(k+1));% subbu's
        Delta_u = uc(k+1) - u(k+1);            % subbu's

        % evaluate states and calculate the temporal difference signal 
        V(k)        = critic(X(:,k), params);                       % V(x(k))
        V(k+1)      = critic(X(:,k+1), params);                     % V(x(k+1))
        delta(k)    = r(k+1) + params.gamma*V(k+1) - V(k);          % temporal difference 
        
        
        %% Formula from [1] section IV A
%         if (k > 1)
%             z_a(:,k) = params.lambda_a*z_a(:,k-1) + (1-params.lambda_a)*u(k)*rbf(X(:,end),params);
%             z_c(:,k) = params.lambda_c*z_c(:,k-1) + (1-params.lambda_c)*rbf(X(:,end),params);
%         end
%         % Update actor and critic
%         % using formula from [1]
%         params.theta    = params.theta + params.alpha_c*delta(k).*z_c(:,k);           % critic
%         params.phi      = params.phi + params.alpha_a*delta(k)*urand(k).*z_a(:,k);    % actor
        
        %% Formula I created on my own
        % Update actor and critic
        params.theta    = params.theta + params.alpha_c*delta(k)*rbf(X(:,k), params);         % critic
        params.phi      = params.phi + params.alpha_a*delta(k)*Delta_u*uSel(k)*rbf(X(:,k),params);   % actor
        
        % save the parameters to memory
        Phi(:,k+1)      = params.phi;
        Theta(:,k+1)    = params.theta;
        
        % final state of this time step becomes initial state of the next
        x0              = x(:,end);                
        k               = k+1;      % update index variable
        
    end
    
    % Plotting purpose
    if mod(counter,3) == 0
        subplot(2,1,1); 
        plotOut = plotrbf(params, 'critic',params.plotopt); title(['\bf{CRITIC}  Iteration: ' int2str(counter)]);
        xlabel('$\theta  \hspace{1mm}$ [rad]','Interpreter','Latex'); ylabel('$\dot{\theta}$ \hspace{1mm} [rad/s]','Interpreter','Latex'); colorbar 
        subplot(2,1,2); 
        plotOut = plotrbf(params, 'actor',params.plotopt); title('\bf{ACTOR}'); xlabel('$\theta \hspace{1mm}$ [rad]','Interpreter','Latex'); ylabel('$\dot{\theta} \hspace{1mm}$ [rad/s]','Interpreter','Latex'); colorbar 
        pause(0.2);    
    end
end

% animation purpose
Q       = X(1,:);
spacing = 10;
animateRobotRL(0, Q(1:spacing:end));     
figure; 
% subplot(2,1,1); plot(Ret(1,:)); hold on; plot(Ret(end,:),'r');
% subplot(2,1,2); 
plot(sum(Ret,2)); title('Average return');


%% References:
% [1] A Survey of Actor-Critic Reinforcement Learning: Standard and Natural Policy Gradients
