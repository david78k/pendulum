function [w, v, xbar, e] = nn_init(N_BOXES)

% Initialize action and heuristic critic weights and traces
w =zeros(N_BOXES,1);
v = zeros(N_BOXES,1);
xbar= zeros(N_BOXES,1);
e= zeros(N_BOXES,1);