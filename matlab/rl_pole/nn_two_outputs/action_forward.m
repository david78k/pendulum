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

for i = 1:2,
    s = 0.0;
    for j = 1:5,
        s = s + e(j,i) * x(j) + f(j, i) * z(j); % I-O * input + H-O * hidden
    end
    p(i) = sigmoid(s); % output layer
end