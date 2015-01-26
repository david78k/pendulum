% evaluation network forward function: sigmoid-linear
% x: input layer. 
% y: hidden layer. sigmoid
% v: output layer. linear
% a: I-H synapses
% b: I-O synapses
% c: H-O synapses
function [v, y] = eval_forward(x, a, b, c)

% output: state evaluation
for i = 1: 5,
    s = 0.0;
    for j = 1:5,
        s = s + a(i,j) * x(j); % I-H * input
    end
    y(i) = sigmoid(s);  % hidden layer
end

s = 0.0;
for i = 1:5,
    s = s + b(i) * x(i) + c(i) * y(i); % I-O * input + H-O * hidden
end

v = s; % output layer
