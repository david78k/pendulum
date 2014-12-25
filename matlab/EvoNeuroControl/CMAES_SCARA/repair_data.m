function [ y ] = repair_data()
%REPAIR_DATA Summary of this function goes here
%   Detailed explanation goes here

x = load('cmaes_scara_mse.txt');
N = numel(x);

x_ant = x(1);
for i=2:N
    if x(i)==0
        x(i)=x_ant;
    end
    x_ant=x(i);
end

y = x;