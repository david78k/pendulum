% function SRMForce
clf;
clear all;

dt = 0.02; % 20ms, Fmax = 0.3679, period = 9.18sec 460steps
% dt = 0.001; % 1ms, Fmax = 0.36788, period = 9.199sec, 9200steps
dt = 0.001; % 1ms, Fmax = 0.0073576, period = 0.179sec, 180steps
tau = 1; % 
tau = 0.5; % Fmax = 0.18394, period = 4.38sec, 220steps
tau = 0.02; % Fmax = 0.0073576, period = 0.18sec, 10steps

steps = []; F = []; push = [];

tic
% for step = 1:9200
% for step = 1:460
for step = 1:180
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