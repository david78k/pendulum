function Cart_Pole_NN
% Two-layer neural network: action network and evaluation network
% network architecture: 5 x 5 x 1
plot = 1;   % boolean for plotting. 1: plot, 0: no plot

BETA    = 0.2;      % Learning rate for action weights, a. 
BETAH   = 0.05;     % Learning rate for action weights, b, c.
RHO     = 1.0;      % Learning rate for critic weights, d. 
RHOH    = 0.2;      % Learning rate for critic weights, e, f.
GAMMA   = 0.9;      % ratio of current prediction, v
TAU     = 0.02;

MAX_POS = 2.4;
MAX_VEL = 1.5;
MAX_ANGLE = 0.2094;
MAX_ANGVEL = 2.01;

% MAX_FAILURES  =  100;      % Termination criterion for quantized Boxes version.
% MAX_STEPS   =     50000;
MAX_FAILURES  =  10000;      % Termination criterion for unquantized version. 
MAX_STEPS   =     100000;

steps = 0; actualMaxSteps = 0;
failures=0;

global grafica
grafica = false; % indicates if display the graphical interface
xpoints = [];
ypoints = [];

% subplot(2,1,2);
% xpoints(1) = 10;
% ypoints(1) = 50;
% plot(xpoints,ypoints)
% disp(['success'])        

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

tic
% state evaluation
[v, y] = eval_forward(x, a, b, c);

F = [];
push = [];

% Iterate through the action-learn loop. 
while (steps < MAX_STEPS && failures < MAX_FAILURES)
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
        right = 1; 
    else
        right = 0;
    end
    
    if rand <= p(2)
        left = 0;
    else
        left = 1;
    end
   
    % q = 1.0/0.5/0 best: 586 steps
    if right == 1 && left == 0
%         push = 1;   
        q = 1.0; pp = p(1);
    elseif right == 0 && left == 1
%         push = -1;  
        q = 0.5; pp = p(2);
    else
%         push = 0;   
        q = 0; pp = 0;
    end
%     rnd = rand;
%     if 0.6667 <= p   % right push
%         push =1; q = 1.0; 
%     elseif 0.3333 <= p < 0.6667           % left push
%         push= -1; q = 0.5;
%     else
%         push = 0; q = 0;
%     end
    unusualness = q - pp; 
           
%     F(steps) = 0;
    push = 0;
%     push(steps) = mod(randi(3),3) - 1;
    for k = 1:steps
        push = push + getForce(push, (steps - k)*TAU);
    end
%     push = F(steps);
    
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
%         disp(['Episode ' int2str(failures) ': steps '  num2str(steps)]);
                       
%         test
        [xpoints, ypoints] = plot_xy(xpoints, ypoints, failures, steps);
        
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
    
    if steps > 0.95*MAX_STEPS
        grafica = true;
    end
    if actualMaxSteps < steps;
        actualMaxSteps = steps;
    end
end
  
global balanced
if (failures == MAX_FAILURES)
    disp(['Pole not balanced. Stopping after ' int2str(failures) ' failures ' ]);
    balanced = false;
else
    disp(['Pole balanced successfully for at least ' int2str(steps) ' steps ' ]);
    balanced = true;
end

disp(['Max steps: ' int2str(actualMaxSteps)]);
toc
