% system structure
M = 0.5; % cart mass
m = 0.2; % pendulum mass
b = 0.1; % coefficient of friction for cart
I = 0.006; % mass moment of itertia of the pendulum
g = 9.8; % gravity
l = 0.3; % length to pendulum center of mass
% F = force applied to the cart
% x = cart position coordinate
% theta = pendulum angle from vertical (down)

q = (M+m)*(I+m*l^2)-(m*l)^2;
s = tf('s');
P_pend = (m*l*s/q)/(s^3 + (b*(I + m*l^2))*s^2/q - ((M + m)*m*g*l)*s/q - b*m*g*l/q);

% PID control
Kp = 1;
Ki = 1;
Kd = 1;
C = pid(Kp,Ki,Kd);
T = feedback(P_pend,C);
t=0:0.01:10;
impulse(T,t)
title('Response of Pendulum Position to an Impulse Disturbance under PID Control: Kp = 1, Ki = 1, Kd = 1');

figure;
Kp = 100;
Ki = 1;
Kd = 1;
C = pid(Kp,Ki,Kd);
T = feedback(P_pend,C);
t=0:0.01:10;
impulse(T,t)
axis([0, 2.5, -0.2, 0.2]);
title('Response of Pendulum Position to an Impulse Disturbance under PID Control: Kp = 100, Ki = 1, Kd = 1');

figure;
Kp = 100;
Ki = 1;
Kd = 20;
C = pid(Kp,Ki,Kd);
T = feedback(P_pend,C);
t=0:0.01:10;
impulse(T,t)
axis([0, 2.5, -0.2, 0.2]);
title('Response of Pendulum Position to an Impulse Disturbance under PID Control: Kp = 100, Ki = 1, Kd = 20');
