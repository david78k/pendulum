function SRM

xpoints = [];
ypoints = [];
fpoints = [];

steps = [];
tau = 0.02;

fun = @(t) t .* exp(-t);

for i = 1:500,
    steps(i) = (i - 1)*tau;
    fpoints(i) = (i - 1)*2*tau;
%     steps, fun(steps(i))
    
    %     subplot(2,1,2);
%     xpoints(i) = i;
%     ypoints(i) = i;
%     plot(xpoints,ypoints)
    % disp(['success'])
end

% plot(steps,fun(steps));
plot(fun(fpoints));
