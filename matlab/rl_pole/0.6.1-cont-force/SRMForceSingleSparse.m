% function SRMForce
clf; clear all;

fmax = 1000; % F(-7.3576, 7.3576)
% fmax = 2000; % F(-14.7152, 14.7152)
% fmax = 10000; % F(-73.5759, 73.5759)
% dt = 0.02; % 20ms, tau = 1, Fmax = 0.3679, period = 9.18sec 460steps (300-400 ok)
dt = 0.001; % 1ms, tau = 1, Fmax = 0.36788, period = 9.199sec, 9200steps
% dt = 0.001; % 1ms, tau = 0.02, Fmax = 0.0073576, period = 0.179sec, 180steps
% tau = 1; % 
% tau = 0.5; % dt = 0.02, Fmax = 0.18394, period = 4.38sec, 220steps
tau = 0.02; % dt = 0.001, Fmax = 0.0073576, period = 0.18sec, 10steps
last_steps = 200; % 460, 8000, 9200
total_steps = 2000; % 1sec

steps = []; F = []; push = [];

tic
for step = 1:(total_steps)
%     step
%     endpoint = min(last_steps, step + last_steps);
    startpoint = mod(step, last_steps);
    endpoint = startpoint + last_steps - 1;
    if startpoint == 1   
        push = mod(randi(3),3) - 1;
        for i = startpoint: endpoint
            steps(step + i) = (step + i - 1)*dt;
            t = steps(i);            
%             F(step + i) = t*exp(-t/tau);
            F(step + i) = fmax * push * t*exp(-t/tau);
%             steps(step + i) = step * dt;
%             F(step + i) = step+ i;
        end
    end
%     step = step + 1;
end

disp(['steps(end): ' num2str(steps(end)) ', F(end): ' num2str(F(end)) ', F(' num2str(min(F)) ', ' num2str(max(F)) ')' ] );

hold on;
plot(steps,F);
ylabel('Force');
xlabel('time');
hold off;

toc