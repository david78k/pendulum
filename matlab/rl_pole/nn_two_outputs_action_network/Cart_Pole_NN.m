% Two-layer neural network: action network and evaluation network
% network architecture: 5 x 5 x 1
plot = 0;   % boolean for plotting. 1: plot, 0: no plot

BETA    = 0.2;      % Learning rate for action weights, a. 
BETAH   = 0.05;     % Learning rate for action weights, b, c.
RHO     = 1.0;      % Learning rate for critic weights, d. 
RHOH    = 0.2;      % Learning rate for critic weights, e, f.
GAMMA   = 0.9;      % ratio of current prediction, v

MAX_POS = 2.4;
MAX_VEL = 1.5;
MAX_ANGLE = 0.2094;
MAX_ANGVEL = 2.01;

MAX_FAILURES  =  10000;      % Termination criterion for unquantized version. 
MAX_STEPS   =     100000;

steps = 0;  actualMaxSteps = 0;
failures=0;

% Initialize action and heuristic critic weights and traces
[a,b,c,d,e,f] = init_weights();

% Starting state is (0 0 0 0)
[h, h_dot, theta, theta_dot] = init_state(MAX_POS, MAX_VEL, MAX_ANGLE, MAX_ANGVEL);

% Find box in state space containing start state
[x] = setInputValues(h, h_dot, theta, theta_dot, ...
    MAX_POS, MAX_VEL, MAX_ANGLE, MAX_ANGVEL);

% Turning on the double buffering to plot the cart and pole
if plot 
    handle = figure(1);
    set(handle,'doublebuffer','on')
end

% state evaluation
[v, y] = eval_forward(x, a, b, c);

% Iterate through the action-learn loop. 
while (steps < MAX_STEPS && failures < MAX_FAILURES)
    % Plot the cart and pole with the x and theta
    if plot
        plot_Cart_Pole(h,theta)
    end
    
    %Choose action randomly, biased by current weight. 
    % p has two outputs
    [p, z] = action_forward(x, d, e, f);
    
    % needs a threshold?
    if rand <= p(1)   % right push
        right = 1; %unusualness = 1.0 - p;
    else                % left push
        right = 0; %unusualness = -p;
    end
    
    if rand <= p(2)   % left push
        left = 1; %unusualness = 1.0 - p;
    else                % left push
        left = 0; %unusualness = -p;
    end
    
    if right == 1 && left == 0,
        push = 1;   q = 1.0;   
%         pt = 0.3333 * p(1) * p(2) /0.5 + 0.66667;
        pt = p(1);
    elseif right == 0 && left == 1,
        push = -1;  q = 0.5;
%         pt = 0.3333 * p(1) * p(2) /0.5 + 0.3333;
        pt = p(2);
    else
        push = 0;   q = 0;  pt = p(1) * p(2);
%         if right == 0 && left == 0
%             pt = 0.3333 * p(1) * p(2) /0.25;
%         else
%             pt = 0.3333 * (p(1) * p(2) - 0.25) /0.75;
%         end
    end
%     unusualness = q - p(1)*p(2);
    unusualness = q - pt;
    
    %Preserve current activities in evaluation network
    % Remember prediction of failure for current state
    v_old = v;
    % remember inputs and hidden unit values
    for i = 1:5,
        xold(i) = x(i); % state variables
        yold(i) = y(i); % hidden units
    end

    %Apply action to the simulated cart-pole
    % failure: r
    [h,h_dot,theta,theta_dot, failure] = ...
        Cart_Pole(push,h,h_dot,theta,theta_dot, MAX_POS, MAX_ANGLE);
    
    [x] = setInputValues(h, h_dot, theta, theta_dot, ...
        MAX_POS, MAX_VEL, MAX_ANGLE, MAX_ANGVEL);

    % state evaluation
    [v, y] = eval_forward(x, a, b, c);
    
    if (failure < 0) % r = -1, Failure occurred
	    failures=failures+1;
        disp(['Trial was ' int2str(failures) ' steps '  num2str(steps)]);
        
        steps = 0;
        
        %Reset state to (0 0 0 0).  Find the box. ---*/
	    [h, h_dot, theta, theta_dot] = init_state(MAX_POS, MAX_VEL, MAX_ANGLE, MAX_ANGVEL);

        %Reinforcement upon failure is -1. Prediction of failure is 0.
%         r = -1.0;
% 	    p = 0.;
        rhat = failure - v_old;
        failure = 0;
    else   % r = 0, Not a failure.
        %Reinforcement is 0. Prediction of failure given by v weight.
%         r = 0;
        steps=steps+1;
% 	    p= v(box);

        %Heuristic reinforcement is:   current reinforcement
        %     + gamma * new failure prediction - previous failure prediction
        rhat = failure + GAMMA * v - v_old;
    end
        
    [a,b,c,d,e,f] = updateWeights (BETA, RHO, BETAH, RHOH, rhat, ...
    unusualness, xold, yold, a, b, c, d, e, f, z);

    if actualMaxSteps < steps;
        actualMaxSteps = steps;
    end
end
  
if (failures == MAX_FAILURES)
    disp(['Pole not balanced. Stopping after ' int2str(failures) ' failures ' ]);
else
    disp(['Pole balanced successfully for at least ' int2str(steps) ' steps ' ]);
end

disp(['Max steps: ' int2str(actualMaxSteps)]);
