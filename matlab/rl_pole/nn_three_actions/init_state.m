function [h, h_dot, theta, theta_dot] = init_state(max_pos, max_vel, max_angle, max_angvel)

% Starting state is (0 0 0 0)
% h         = 0;       % cart position, meters 
% h_dot     = 0;       % cart velocity
% theta     = 0;       % pole angle, radians
% theta_dot = 0.0;     % pole angular velocity

h         = rand * 2 * max_pos - max_pos;       % cart position, meters 
h_dot     = rand * 2 * max_vel - max_vel;       % cart velocity
theta     = rand * 2 * max_angle - max_angle;       % pole angle, radians
theta_dot = rand * 2 * max_angvel - max_angvel;     % pole angular velocity