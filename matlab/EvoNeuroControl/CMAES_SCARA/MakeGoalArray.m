function [ xg yg zg ] = MakeGoalArray(N)
%MAKEGOALARRAY Summary of this function goes here
%   Detailed explanation goes here
xg =[];
yg =[];
zg =[];
for i=1:N
    [xg(i) yg(i) zg(i)] = randgoal();    
end