function [ xg yg ] = MakeGoalArray(N)
%MAKEGOALARRAY Summary of this function goes here
%   Detailed explanation goes here
xg=[];
yg =[];

for i=1:N
    [xg(i) yg(i)] = randgoal();    
end