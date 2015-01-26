% action network forward function: sigmoid-sigmoid
% x: input layer. 
% z: hidden layer. sigmoid
% p: output layer. sigmoid
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

s = 0.0;
for i = 1:5,
    s = s + e(i) * x(i) + f(i) * z(i); % I-O * input + H-O * hidden
end

p = sigmoid(s); % output layer
