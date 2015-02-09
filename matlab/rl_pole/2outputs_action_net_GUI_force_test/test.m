function test
% test if the controller learned fully
% choose a random initial state

% Iterate through the action-learn loop. 
while (steps < MAX_STEPS)
%     if steps == 100
%         grafica = false;
%     end
    
    % Plot the cart and pole with the x and theta
    if grafica
        plot_Cart_Pole(h,theta)
    end
    
    %Choose action randomly, biased by current weight. 
    [p, z] = action_forward(x, d, e, f);
    
    if rand <= p(1) 
        right = 1; rspikes = rspikes + 1; 
    else
        right = 0;
    end
    
    if rand <= p(2)
        left = 0; %lspikes = lspikes + 1;
    else
        left = 1; lspikes = lspikes + 1;
    end
    rtrain(steps + 1) = right;
    ltrain(steps + 1) = left;
    
    % q = 1.0/0.5/0 best: 586 steps
    if right == 1 && left == 0
        push = 1;   
        q = 1.0; pp = p(1);
    elseif right == 0 && left == 1
        push = -1;  
        q = 0.5; pp = p(2);
    else
        push = 0;   
        q = 0; pp = 0;
    end
    unusualness = q - pp; 
           
%     F(steps) = 0;
    fsum = 0;
    for k = 1:steps
%     for k = 1:min(steps, LAST_STEPS)
        fsum = fsum + getForce(push, (steps - k)*dt, TAU);
    end
%     push = F(steps);
    push = fsum;
%     fprintf('%d: %f\n', steps, push);

    %Preserve current activities in evaluation network
    % Remember prediction of failure for current state
    v_old = v;
    % remember inputs and hidden unit values
    for i = 1:5,
        xold(i) = x(i); % state variables
        yold(i) = y(i); % hidden units
    end
    
    %Apply action to the simulated cart-pole: failure = r
    [h,h_dot,theta,theta_dot, failure] = ...
        Cart_Pole(push,h,h_dot,theta,theta_dot, MAX_POS, MAX_ANGLE, dt);
    thetas(steps + 1) = theta;
    thetadots(steps + 1) = theta_dot;
    forces(steps + 1) = push;
    
    [x] = setInputValues(h, h_dot, theta, theta_dot, ...
        MAX_POS, MAX_VEL, MAX_ANGLE, MAX_ANGVEL);

    % state evaluation
%     [v, y] = eval_forward(x, a, b, c);
    
    if (failure < 0) % r = -1, Failure occurred
	    failures=failures+1;
%         disp(['Episode ' int2str(failures) ': steps '  num2str(steps)]);
                       
        [xpoints, ypoints] = plot_xy(xpoints, ypoints, failures, steps);
%         if steps > 500
            plot_rhats(rhats);
%         end
        rhat_max = max(rhat_max, max(rhats));
        rhat_min = min(rhat_min, min(rhats));
        rhatsi = []; ltrain = []; rtrain = [];
        forces = []; thetas = []; thetadots = [];
        steps = 0;
        
        %Reset state to (0 0 0 0).  Find the box. ---*/
	    [h, h_dot, theta, theta_dot] = init_state(MAX_POS, MAX_VEL, MAX_ANGLE, MAX_ANGVEL);

        %Reinforcement upon failure is -1. Prediction of failure is 0.
        rhat = failure - v_old;
        failure = 0;      
        rhatsi(steps + 1) = rhat;
    else   % r = 0, Not a failure.
        %Reinforcement is 0. Prediction of failure given by v weight.
        %Heuristic reinforcement is:   current reinforcement
        %     + gamma * new failure prediction - previous failure prediction
        rhat = failure + GAMMA * v - v_old;
        rhatsi(steps + 1) = rhat;
        
        steps=steps+1;
    end
    rhats(failures + 1) = rhat;
    
%     rhats = plot_rhats(steps, rhats, rhat);
%     if mod(steps, 500) == 0
%     if abs(rhat) > 1
%         fprintf('trial %d step %d rhat %.4f\n', failures, steps, rhat);
%     end
%     [a,b,c,d,e,f] = updateWeights (BETA, RHO, BETAH, RHOH, rhat, ...
%     unusualness, xold, yold, a, b, c, d, e, f, z); 
    
%     if steps > 0.95*MAX_STEPS
%         grafica = true;
%     end
    if actualMaxSteps < steps
        actualMaxSteps = steps;
    end
    totalSteps = totalSteps + 1;
end