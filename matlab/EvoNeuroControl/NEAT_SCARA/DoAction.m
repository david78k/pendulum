function [ xt ret_state ] = DoAction(state)
%DoAction: executes the action (a) into the SCARA Robot
% a: is the force to be applied to the car
% x: is the vector containning the position and speed of the car
% xp: is the vector containing the new position and velocity of the car






ret_state = state;
pos = scarapos(ret_state(1),ret_state(2),ret_state(3),ret_state(4));
xt  = pos(5,:);







