% function SRMForce
clf; clear all;

% dt = 0.02; % 20ms, tau = 1, Fmax = 0.3679, period = 9.18sec 460steps (300-400 ok)
dt = 0.001; % 1ms, tau = 1, Fmax = 0.36788, period = 9.199sec, 9200steps
% dt = 0.001; % 1ms, tau = 0.02, Fmax = 0.0073576, period = 0.179sec, 180steps
% tau = 1; % 
% tau = 0.5; % dt = 0.02, Fmax = 0.18394, period = 4.38sec, 220steps
tau = 0.02; % dt = 0.001, Fmax = 0.0073576, period = 0.18sec, 10steps
last_steps = 150; % 460, 8000, 9200

steps = []; F = []; push = [];

tic
for step = 1:last_steps
    steps(step) = (step - 1)*dt;
    t = steps(step);
    F(step) = t*exp(-t/tau);
end

disp(['steps(end): ' num2str(steps(end)) ', F(end): ' num2str(F(end)) ', max(F): ' num2str(max(F))]);

hold on;
plot(steps,F);
ylabel('Force');
xlabel('time');
hold off;

toc