function SRMForce

steps = [];
tau = 0.02;

fun = @(t) t .* exp(-t);
fun2 = @(t) -t.*exp(-t);
final = [];
sum = [];

F = [];
push = [];

tic

for step = 1:6000,
    steps(step) = (step - 1)*tau;
%     ypoints(i) = (i + 100 )*tau; % old 100 steps before
%     fpoints(i) = (i + 200)*tau;  % old 200 steps past
    F(step) = 0;
    push(step) = mod(randi(3),3) - 1;
    for k = 1:step
        F(step) = F(step) + getForce(push(k), (step - k)*tau);
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