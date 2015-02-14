% function SRMForce
clf;
clear all;

dt = 0.02; % 20ms, Fmax = 0.3679, period = 9.18sec 460steps
% dt = 0.001; % 1ms, Fmax = 0.36788, period = 9.199sec, 9200steps
tau = 1; % 
% tau = 0.5; % [-3, 3]
% tau = 0.02; % [-0.018, 0.018], fmax = 1
% tau = 0.02; % [-0.05, 0.05], fmax = 3
% tau = 0.02; % [0.09, 0.09], fmax = 5
% tau = 0.02; % [-0.18, 0.18], fmax = 10
% tau = 0.02; % [-10, 10], fmax = 600

steps = []; F = []; push = [];

tic
% for step = 1:9200
for step = 1:460
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