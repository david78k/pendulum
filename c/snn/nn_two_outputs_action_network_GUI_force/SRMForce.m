% function SRMForce
clf;
clear all;

sampleTime = 0.02;
% tau = 1; % [-5, 7]
% tau = 0.5; % [-3, 3]
tau = 0.02; % [-0.018, 0.018], fmax = 1
% tau = 0.02; % [-0.05, 0.05], fmax = 3
% tau = 0.02; % [0.09, 0.09], fmax = 5
% tau = 0.02; % [-0.18, 0.18], fmax = 10
% tau = 0.02; % [-10, 10], fmax = 600

steps = [];
F = [];
push = [];

tic

for step = 1:6000,
    steps(step) = (step - 1)*sampleTime;
%     ypoints(i) = (i + 100 )*tau; % old 100 steps before
%     fpoints(i) = (i + 200)*tau;  % old 200 steps past
    F(step) = 0;
    push(step) = mod(randi(3),3) - 1;
    for k = 1:step
        F(step) = F(step) + getForce(push(k), (step - k)*sampleTime, tau);
%         F(step) = F(step) + getForce(push(k), (step - k)*tau);
    end
    
    %     subplot(2,1,2);
%     plot(xpoints,ypoints)
    % disp(['success'])
end

hold on;
plot(steps,F);
ylabel('Force');
xlabel('time');
% plot(steps,push);
hold off;

toc