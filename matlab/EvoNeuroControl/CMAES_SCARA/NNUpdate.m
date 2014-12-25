function [ output ] = NNUpdate( inputs,individual )
%NNUPDATE Summary of this function goes here
%   Detailed explanation goes here


ni = 2+1; % +1 for bias node
nh = 5;
no = 2;



N = numel(individual);

wi = individual(1:ni*nh);
wi = reshape(wi,ni,nh);

wo = individual(ni*nh+1:N);
wo = reshape(wo,nh,no);



%input activations
ai     = [inputs 1]; % 1 for bias node;


% hidden activations
sum    = ai * wi;
ah     = tanh(sum);

% output activations
sum    =  ah * wo;
output = tanh(sum);

