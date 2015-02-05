function [x] = setInputValues(h, h_dot, theta, theta_dot, max_pos, max_vel, max_angle, max_angvel)

x(1) = ( h + max_pos ) / (2 * max_pos);   
x(2) = ( h_dot + max_vel ) / (2 * max_vel);  
x(3) = ( theta + max_angle ) / (2 * max_angle);  
x(4) = ( theta_dot + max_angvel ) / (2 * max_angvel);  
x(5) = 0.5;

