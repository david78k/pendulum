function [xpoints, ypoints] = plot_xy (xpoints, ypoints, failures, steps)

% clear all;

kk = failures;
xpoints(kk)=kk - 1;
ypoints(kk)=steps;
subplot(3,1,2);
plot(xpoints,ypoints);
        %     title(['Episode: ',int2str(failures),' epsilon: ',num2str(epsilon)])
xlabel('Episodes')
ylabel('Steps')
drawnow


