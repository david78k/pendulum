% function SRMForce
clf; clear all;

% F = [-8, 8]
fmax = 1;
dt = 0.02;
tau = 1; % [-10, 10] for (fmax = 1, dt = 0.02)
% tau = 0.5; % [-3, 3]
% tau = 0.02; % [-0.018, 0.018], fmax = 1
% tau = 0.02; % [-0.05, 0.05], fmax = 3
% tau = 0.02; % [0.09, 0.09], fmax = 5
% tau = 0.02; % [-0.18, 0.18], fmax = 10
% tau = 0.02; % [-10, 10], fmax = 600

steps = []; F = []; push = []; f = [];

tic

for step = 1:180000
    steps(step) = (step - 1)*dt;
    push(step) = mod(randi(3),3) - 1;
%     f(step) = fmax * push(step) * t * exp(-t/tau);
    sum = 0;
%     for k = 1:step
    upto = min(step, 500);
    for k = 1:upto
%         k
        t = k*dt;
%         t = (step - k)*dt;
        sum = sum + fmax * push(step + 1 - k) * t * exp(-t/tau);
%         F(step) = F(step) + getForce(push(k), (step - k)*sampleTime, tau);
%         F(step) = F(step) + getForce(push(k), (step - k)*tau);
%         disp(['k = ' num2str(k) ' sum = ' num2str(sum)]);
    end
    F(step) = sum;
end

hold on;
plot(steps,F);
ylabel('Force');
xlabel('time');
% plot(steps,push);
hold off;

toc