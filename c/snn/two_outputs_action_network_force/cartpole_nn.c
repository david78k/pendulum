#include "header.h"

//function Cart_Pole_NN
// Two-layer neural network: action network and evaluation network
// network architecture: 5 x 5 x 2, 5 x 5 x 1

void cartpole() {

	printf("cartpole");
//global grafica
//grafica = false; % indicates if display the graphical interface
//xpoints = []; ypoints = [];

/*
//% Initialize action and heuristic critic weights and traces
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

tStart = tic;
% state evaluation
[v, y] = eval_forward(x, a, b, c);

F = [];
% push = [];

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
        right = 1; rspikes = rspikes + 1;
    else
        right = 0;
    end
    
    if rand <= p(2)
        left = 0; lspikes = lspikes + 1;
    else
        left = 1;
    end
   
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
        fsum = fsum + getForce(push, (steps - k)*sampleTime, TAU);
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
        Cart_Pole(push,h,h_dot,theta,theta_dot, MAX_POS, MAX_ANGLE, sampleTime);
       
    [x] = setInputValues(h, h_dot, theta, theta_dot, ...
        MAX_POS, MAX_VEL, MAX_ANGLE, MAX_ANGVEL);

    % state evaluation
    [v, y] = eval_forward(x, a, b, c);
    
    if (failure < 0) % r = -1, Failure occurred
	    failures=failures+1;
%         disp(['Episode ' int2str(failures) ': steps '  num2str(steps)]);
                       
        [xpoints, ypoints] = plot_xy(xpoints, ypoints, failures, steps);
        
        steps = 0;
        
        %Reset state to (0 0 0 0).  Find the box. ---
	    [h, h_dot, theta, theta_dot] = init_state(MAX_POS, MAX_VEL, MAX_ANGLE, MAX_ANGVEL);

        %Reinforcement upon failure is -1. Prediction of failure is 0.
        rhat = failure - v_old;
        failure = 0;      
    else   % r = 0, Not a failure.
        %Reinforcement is 0. Prediction of failure given by v weight.
        steps=steps+1;

        %Heuristic reinforcement is:   current reinforcement
        %     + gamma * new failure prediction - previous failure prediction
        rhat = failure + GAMMA * v - v_old;
    end
        
    [a,b,c,d,e,f] = updateWeights (BETA, RHO, BETAH, RHOH, rhat, ...
    unusualness, xold, yold, a, b, c, d, e, f, z); 
    
    if steps > 0.95*MAX_STEPS
        grafica = true;
    end
    if actualMaxSteps < steps
        actualMaxSteps = steps;
    end
    totalSteps = totalSteps + 1;
end
  
global balanced
if (failures == MAX_FAILURES)
    disp(['Pole not balanced. Stopping after ' int2str(failures) ' failures ' ]);
    balanced = false;
else
    disp(['Pole balanced successfully for at least ' int2str(steps) ' steps at ' num2str(failures) ' trials' ]);
    balanced = true;
end

disp(['Max steps: ' int2str(actualMaxSteps) ', Total Steps: ' num2str(totalSteps)]);

global FinalMaxSteps
if FinalMaxSteps < actualMaxSteps
    FinalMaxSteps = actualMaxSteps;
end

toc(tStart)

% stats.m
% firing rates: L, R, all
rl = lspikes / totalSteps; % left rate
rr = rspikes / totalSteps; % right rate
ra = (lspikes + rspikes) / totalSteps; % all rate
disp(['Firing rate = ' num2str(ra) ' (L: ' num2str(rl) ', R: ' num2str(rr) ')']);
*/
}
