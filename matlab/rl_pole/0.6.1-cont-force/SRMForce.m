% function SRMForce
clf; clear all;

% F = [-8, 8]
fmax = 20000;
total_steps = 360000; % [-0.17, 0.17], [-0.1, 0.1] for dt0.001 tau0.02 last200 or last100
last_steps = 100; % for dt = 0.001
% last_steps = 400; % for dt = 0.02
dt = 0.001;
% dt = 0.02;
% tau = 1; % [-10, 10] for (fmax = 1, dt = 0.02)
% tau = 0.5; % [-3, 3]
tau = 0.02; % [-0.018, 0.018], fmax = 1
% tau = 0.02; % [-0.05, 0.05], fmax = 3
% tau = 0.02; % [0.09, 0.09], fmax = 5
% tau = 0.02; % [-0.18, 0.18], fmax = 10
% tau = 0.02; % [-10, 10], fmax = 600

steps = []; F = []; push = []; f = [];
maxf = 0; minf = 100000;

tic

for step = 1:total_steps
    steps(step) = (step - 1)*dt;
    push(step) = mod(randi(3),3) - 1;
%     f(step) = fmax * push(step) * t * exp(-t/tau);
    sum = 0;
%     for k = 1:step
    upto = min(step, last_steps);
%     upto = step;
    for k = 1:upto
        t = k*dt;
%         t = (step - k)*dt;
        sum = sum + fmax * push(step + 1 - k) * t * exp(-t/tau);
%         F(step) = F(step) + getForce(push(k), (step - k)*sampleTime, tau);
%         F(step) = F(step) + getForce(push(k), (step - k)*tau);
%         disp(['k = ' num2str(k) ' sum = ' num2str(sum)]);
    end
    F(step) = sum;
    maxf = max(sum, maxf);
    minf = min(sum, minf);
end

hold on;
plot(steps,F);
ylabel('Force');
xlabel('time');
% plot(steps,push);
hold off;

disp(['Total ' num2str(total_steps) ' steps ' num2str(toc) ' sec. (' num2str(minf) ', ' num2str(maxf) ')']);