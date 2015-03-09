%testActorCritic run an actor critic RL controller for a 1-DoF inverted pendulum
%
% Copyright 2015 Yudha Pane

clear; clc; close all;
loadParams;
% load params_trial19.mat

xinit       = [pi ;0];                  % initial state
x0          = [pi; 0];
e0          = 1;                        % initial eligibility trace
k           = 1;                        % initialize index variable
Phi(:,k)    = params.phi;               % actor parameters memory
Theta(:,k)  = params.theta;             % critic parameters memory
Ret(:,k)    = zeros(params.Ntrial,1);   % return memory
V(k)        = 0;                        % initialize value function
uSel(k)     = 1;
oldCounter  = 1;

for counter = 1:params.Ntrial
    X(:,k) = xinit;     
    urand(k) = 0;       % initialize input and exploration signal
    r(k) = 0;           % initialize reward/cost
    z_c(:,k) = zeros(params.NrbfX*params.NrbfY,1);
    z_a(:,k) = zeros(params.NrbfX*params.NrbfY,1);
    x0 = xinit;         % reset state to initial position
    e_c = 0;
    
    %% Timing & tuning parameters 
%     if (counter == 1000)
%         params.varRand = 0.7;
%         disp('exploration variance reduced to 0.4');
%     end
%     if (counter == 300)
%         params.varRand = 0.4;
%         disp('exploration variance reduced to 0.2');
%     end
    if(counter == 250)
        params.expSteps = 10;
        disp('exploration frequency reduced to 1/6');
    end  
    if(counter == 500)
        params.varRand = 0.5;
        disp('exploration frequency reduced to 1/6');
    end      
        
%     E(k) = e0;      % eligibility trace
    for t = 0: params.ts: params.t_end
        time = [t t+params.ts];
        
        %% Calculate control input
        if k==1                                 % generate a randomized initial input
            u(k)        = 5*randn;
            uc(k)       = u(k);   
            Delta_u     = 1;
        else
            if mod(k,params.expSteps) == 0      % explore only once every defined time steps
                urand(k)  = params.varRand*randn;                                      
            else
                urand(k)  = 0;
            end
            Delta_u         = urand(k);                     % subbu's
            u(k)            = actor(X(:,k), params);        % subbu's
            [uc(k), uSel(k)]= sat(u(k), params, Delta_u);   % subbu's
            Delta_u         = uc(k) - u(k);                 % subbu's
        end  
 
        if (counter> oldCounter)
            uc(k) = params.varInitInput*randn;
            oldCounter = counter;
        end
        
        %% Apply the control input & receive reward
        [clock,x]   = ode45(@(t,x) oneDofRobot(t, x, uc(k), params), time, x0); % subbu's, note the uc
        x           = transpose(x);
        x(1,:)      = wrapTo180(rad2deg(x(1,:)));           % convert the angle to 2pi rad
        x(1,:)      = deg2rad(x(1,:));
        X(:,k+1)    = x(:,end);                             % get the state measurement
        r(k+1)      = calcCost(X(:,k), uc(k), params);      % calculate the cost / reward  % subbu's note the uc
        
        %% Compute temporal difference & eligibility trace
        V(k)        = critic(X(:,k), params);                       % V(x(k))
        V(k+1)      = critic(X(:,k+1), params);                     % V(x(k+1))
        delta(k)    = r(k+1) + params.gamma*V(k+1) - V(k);          % temporal difference 
        e_c         = params.gamma*params.lambda*e_c + rbf(X(:,k), params);

        %% Update critic and actor parameters
        % Formula I programmed on my own
        % Update actor and critic
        params.theta    = params.theta + params.alpha_c*delta(k)*rbf(X(:,k), params);             % critic
%         params.theta    = params.theta + params.alpha_c*delta(k)*e_c.*rbf(X(:,k), params);             % critic
        params.phi      = params.phi + params.alpha_a*delta(k)*Delta_u*uSel(k)*rbf(X(:,k),params);  % actor
        
        % save the parameters to memory
        Phi(:,k+1)      = params.phi;
        Theta(:,k+1)    = params.theta;
        
        % Formula from [1] section IV A
%         if (k > 1)
%             z_a(:,k) = params.lambda_a*z_a(:,k-1) + (1-params.lambda_a)*u(k)*rbf(X(:,end),params);
%             z_c(:,k) = params.lambda_c*z_c(:,k-1) + (1-params.lambda_c)*rbf(X(:,end),params);
%         end
%         % Update actor and critic
%         % using formula from [1]
%         params.theta    = params.theta + params.alpha_c*delta(k).*z_c(:,k);           % critic
%         params.phi      = params.phi + params.alpha_a*delta(k)*urand(k).*z_a(:,k);    % actor        
        
        %% Compute return 
        Ret(counter,k+1)    = params.gamma*Ret(counter,k) + r(k+1);  % update return value

        %% Update time step and initial state
%         x0  = x(:,end); 
        x0  = X(:,k+1); 
        k   = k+1;          % update index variable
        
    end
    
    % Plotting purpose
    if mod(counter,3) == 0
        clf;
        subplot(2,2,1); 
        plotOut = plotrbf(params, 'critic',params.plotopt); title(['\bf{CRITIC}  Iteration: ' int2str(counter)]);
        xlabel('$\theta  \hspace{1mm}$ [rad]','Interpreter','Latex'); ylabel('$\dot{\theta}$ \hspace{1mm} [rad/s]','Interpreter','Latex'); colorbar 
        subplot(2,2,3); 
        plotOut = plotrbf(params, 'actor',params.plotopt); title('\bf{ACTOR}'); xlabel('$\theta \hspace{1mm}$ [rad]','Interpreter','Latex'); ylabel('$\dot{\theta} \hspace{1mm}$ [rad/s]','Interpreter','Latex'); colorbar 
        subplot(2,2,2);
        plot(delta); title('\bf{Temporal difference}');
        subplot(2,2,4);
        plot(sum(Ret,2)); title('\bf{Average return}');
        pause(0.2);    
    end
end

figure;
% subplot(2,1,1); plot(Ret(1,:)); hold on; plot(Ret(end,:),'r');
% subplot(2,1,2); 
% plot(sum(Ret,2)); title('Average return');
% figure; 
% plot(delta); title('Temporal difference');

% animation purpose
Q       = X(1,:);
spacing = 1;
animateRobotRL(0, Q(end-1000:spacing:end));     
figure; 


%% References:
% [1] A Survey of Actor-Critic Reinforcement Learning: Standard and Natural Policy Gradients
