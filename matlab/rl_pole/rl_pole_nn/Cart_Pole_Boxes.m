% /*----------------------------------------------------------------------
% This file contains a simulation of the cart and pole dynamic system and
% a procedure for learning to balance the pole. Both are described in
% Barto, Sutton, and Anderson, "Neuronlike Adaptive Elements That Can Solve
% Difficult Learning Control Problems," IEEE Trans. Syst., Man, Cybern.,
% Vol. SMC-13, pp. 834--846, Sept.--Oct. 1983, and in Sutton, "Temporal
% Aspects of Credit Assignment in Reinforcement Learning", PhD
% Dissertation, Department of Computer and Information Science, University
% of Massachusetts, Amherst, 1984. The following routines are included:
%
% main: controls simulation interations and implements
% the learning system.
%
% cart_and_pole: the cart and pole dynamics; given action and
% current state, estimates next state
%
% get_box: The cart-pole's state space is divided into 162
% boxes. get_box returns the index of the box into
% which the current state appears.
%
% These routines were written by Rich Sutton and Chuck Anderson. Claude Sammut
% translated parts from Fortran to C. Please address correspondence to
% sutton@gte.com or anderson@cs.colostate.edu
% The code has been converted to MATLAB by Amir Hesami and a simulator is added to show
% the cart's and pole's movement
N_BOXES = 162; % Number of disjoint boxes of state space.
ALPHA = 1000; % Learning rate for action weights, w.
BETA = 0.5; % Learning rate for critic weights, v.
GAMMA = 0.95; % Discount factor for critic.
LAMBDAw = 0.9; % Decay rate for w eligibility trace.
LAMBDAv = 0.8; % Decay rate for v eligibility trace
% MAX_FAILURES = 100; % Termination criterion.
% MAX_STEPS = 50000;
MAX_FAILURES = 50; % Termination criterion.
MAX_STEPS = 5000;
steps = 0;
failures=0;
% Initialize action and heuristic critic weights and traces
w =zeros(N_BOXES,1);
v = zeros(N_BOXES,1);
xbar= zeros(N_BOXES,1);
e= zeros(N_BOXES,1);
% Starting state is (0 0 0 0)
x = 0; % cart position, meters
x_dot = 0; % cart velocity
theta = 0; % pole angle, radians
theta_dot = 0.0; % pole angular velocity
% Find box in state space containing start state
box = get_box(x, x_dot, theta, theta_dot);
% Turning on the double buffering to plot the cart and pole
h = figure(1);
set(h,'doublebuffer','on')
% Iterate through the action-learn loop.
while (steps < MAX_STEPS & failures < MAX_FAILURES)
    % Plot the cart and pole with the x and theta
    plot_Cart_Pole(x,theta)
    %Choose action randomly, biased by current weight.
    if Random_Pole_Cart<prob_push_right(w(box))
        y =1;
    else
        y=0;
    end
    %Update traces.
    e(box)= e(box) + (1.0 - LAMBDAw) * (y - 0.5);
    xbar(box) =xbar(box)+ (1.0 - LAMBDAv);
    %Remember prediction of failure for current state
    oldp = v(box);
    %Apply action to the simulated cart-pole
    [x,x_dot,theta,theta_dot]=Cart_Pole(y,x,x_dot,theta,theta_dot);
    %Get box of state space containing the resulting state
    box = get_box(x, x_dot, theta, theta_dot);
    if (box < 0)
        %Failure occurred
        failed = 1;
        failures=failures+1;
        disp(['Trial was ' int2str(failures) ' steps ' num2str(steps)]);
        steps = 0;
        %Reset state to (0 0 0 0). Find the box. ---*/
        x =0;
        x_dot = 0;
        theta =0;
        theta_dot = 0.0;
        box = get_box(x, x_dot, theta, theta_dot);
        %Reinforcement upon failure is -1. Prediction of failure is 0.
        r = -1.0;
        p = 0.;
    else
        %Not a failure.
        failed = 0;
        %Reinforcement is 0. Prediction of failure given by v weight.
        r = 0;
        steps=steps+1;
        p= v(box);
    end
    %Heuristic reinforcement is: current reinforcement
    % + gamma * new failure prediction - previous failure prediction
    rhat = r + GAMMA * p - oldp;
    for i=1:N_BOXES,
        %Update all weights.
        w(i) = w(i)+ ALPHA * rhat * e(i);
        v(i) = v(i)+ BETA * rhat * xbar(i);
        if (v(i) < -1.0)
            v(i) = v(i);
        end
        if (failed)
            %If failure, zero all traces.
            e(i) = 0.;
            xbar(i) = 0.;
        else
            e(i)=e(i) * LAMBDAw;
            xbar(i) =xbar(i)* LAMBDAv;
        end
    end
    %Plot the cart and pole using the new x and theta after applying the action
    plot_Cart_Pole(x,theta)
end
if (failures == MAX_FAILURES)
    disp(['Pole not balanced. Stopping after ' int2str(failures) ' failures ' ]);
else
    disp(['Pole balanced successfully for at least ' int2str(steps) ' steps ' ]);
end