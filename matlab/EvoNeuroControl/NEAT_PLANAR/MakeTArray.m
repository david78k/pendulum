function [ T1 T2 T3 ] = MakeTArray( N )
%MAKETARRAY Summary of this function goes here
%   Detailed explanation goes here
T1=[];
T2=[];
T3=[];
for i=1:N   
    T1(i) = randint(1,1,[-90 -10]);
    T2(i) = randint(1,1,[-90 0]);
    T3(i) = randint(1,1,[-90 0]);
end

