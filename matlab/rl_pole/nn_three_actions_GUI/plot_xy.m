function [xpoints, ypoints] = plot_xy (xpoints, ypoints, failures, steps)

% clear all;

kk = failures;
xpoints(kk)=kk - 1;
ypoints(kk)=steps;
subplot(2,1,2);
plot(xpoints,ypoints);
        %     title(['Episode: ',int2str(failures),' epsilon: ',num2str(epsilon)])
xlabel('Episodes')
ylabel('Steps')
drawnow

% for tv = 1:5,
% subplot(2,1,2);
% xpoints(tv) = tv;
% ypoints(tv) = tv;
% plot(xpoints,ypoints)
% % disp(['success'])
% end

