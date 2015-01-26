% action network forward function: sigmoid-sigmoid
% 5-5-2
% x: input layer. 
% z: hidden layer. sigmoid
% p: output layer. two sigmoid neurons
% d: I-H synapses
% e: I-O synapses
% f: H-O synapses
function [p, z] = action_forward(x, d, e, f)

% output: action
for i = 1: 5,
    s = 0.0;
    for j = 1:5,
        s = s + d(i,j) * x(j); % I-H * input
    end
    z(i) = sigmoid(s);  % hidden layer
end

for j = 1:2,
    s = 0.0;
    for i = 1:5,
        s = s + e(i,j) * x(i) + f(i, j) * z(i); % I-O * input + H-O * hidden
    end
    p(j) = sigmoid(s); % output layer
end