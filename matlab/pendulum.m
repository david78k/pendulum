function pendulum(dt)

% Cleveland State University
% Department of Electrical and Computer Engineering
% EEC 644 / 744
% Optimal Control Systems

% Inverted pendulum simulation
% dt = integration/measurement period

if ~exist('dt', 'var')
    dt = 0.02; % integration step size
end
tf = 6; % simulation time
thetainit = 0.5; % initial pendulum angle (radians)

m = 0.2; % pendulum mass (kg)
M = 1.0; % cart mass (kg)
g = 9.81; % acceleration due to gravity (m/s^2)
B = 0.1; % coefficient of viscous friction on cart wheels
l = 1; % length of pendulum (meters)
r = 0.02; % radius of pendulum mass (meters)
J = m * r * r / 2; % moment of inertia of pendulum mass (cylinder)

theta = thetainit; % theta = angle pendulum makes with vertical
thetadot = 0;
thetadotdot = 0;
d = 0; % d = horizontal displacement of cart
ddot = 0;
ddotdot = 0;

theta_array = [theta];
thetadot_array = [thetadot];
d_array = [d];
ddot_array = [ddot];

% Create Jacobian matrix for EKF
temp = J * (M + m) + M * m * l^2;
F42 = m * l * B / temp;
F43 = m * g * l * (M + m) / temp;
F22 = -m * l * F42 / (M + m) - B / (M + m);
F23 = -m * l * F43 / (M + m);
F = [0 1 0 0;
    0 F22 F23 0;
    0 0 0 1;
    0 F42 F43 0];
G4 = -m * l / temp;
G2 = 1 / (M + m) - m * l * G4 / (M + m);
G = [0; G2; 0; G4];
K = place(F, G, [-2-2*j -2+2*j -1-1*j -1+1*j]);

x = [0; 0; thetainit; 0]; % initial state
xArr = x;
xLin = x;
xLinArray = xLin;

for t = dt : dt : tf
    % Nonlinear system simulation
    theta_old = theta;
    thetadot_old = thetadot;
    thetadotdot_old = thetadotdot;
    d_old = d;
    ddot_old = ddot;
    ddotdot_old = ddotdot;
    sine = sin(theta);
    cosine = cos(theta);
    % Compute the new state values.
    u = -K * [d ddot theta thetadot]';
    theta = theta + thetadot_old * dt;
    thetadot = thetadot + thetadotdot_old * dt;
    thetadotdot = (m*g*l*sine*(M+m) - m*l*cosine*(u+ ...
        m*l*thetadot_old^2*sine - B*ddot_old)) / ...
        ((J + m*l^2)*(M+m) - m^2*l^2*cosine^2);
    d = d + ddot_old * dt;
    ddot = ddot + ddotdot_old * dt;
    ddotdot = (u - m*l*thetadotdot_old*cosine + ...
        m*l*thetadot_old^2*sine - B*ddot_old) / (M + m);
    % Save the state values in arrays for later plotting.
    theta_array = [theta_array theta];
    thetadot_array = [thetadot_array thetadot];
    d_array = [d_array d];
    ddot_array = [ddot_array ddot];
    % Linear system simulation
    u = -K * xLin;
    xLindot = F * xLin + G * u;
    xLin = xLin + xLindot * dt;
    xLinArray = [xLinArray xLin];
end

% Plot results
close all;
t = 0 : dt : tf;
figure; set(gcf,'Color','White');

subplot(2,2,1); set(gca,'FontSize',12);
plot(t, 180/pi*theta_array, 'b-'); hold on;
plot(t, 180/pi*xLinArray(3,:), 'r--');
ylabel('angle (deg)');
legend('nonlinear', 'linearized');

subplot(2,2,2); set(gca,'FontSize',12);
plot(t, d_array, 'b-'); hold on;
plot(t, xLinArray(1,:), 'r--');
ylabel('cart pos (meters)');

subplot(2,2,3); set(gca,'FontSize',12);
plot(t, thetadot_array, 'b-'); hold on;
plot(t, xLinArray(4,:), 'r--');
ylabel('ang vel (deg/s)');
xlabel('time (s)');

subplot(2,2,4); set(gca,'FontSize',12);
plot(t, ddot_array, 'b-'); hold on;
plot(t, xLinArray(2,:), 'r--');
ylabel('cart vel (meters/s)');
xlabel('time (s)');