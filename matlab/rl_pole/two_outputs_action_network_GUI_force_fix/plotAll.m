function plotAll (forces, thetas, thetadots, rhats)
% plot forces, rhat
% theta, thetadot, h, hdots
% spikes

figure

% kk = steps + 1;
% xpoints(kk)=kk;
% rhats(kk)=rhat;
subplot(4,1,1);
plot(rhats);
% ylim([-2,2]);
% axis([1,inf,-1,1]);
%     title(['Episode: ',int2str(failures),' epsilon: ',num2str(epsilon)])
% xlabel('Step')
ylabel('rhat')

subplot(3,1,2);
plot(thetas);
% xlabel('Step')
ylabel('theta')

subplot(3,1,3);
plot(thetadots);
% xlabel('Step')
ylabel('theta\_dot')

subplot(4,1,4);
plot(forces);
xlabel('Step')
ylabel('Force')

drawnow


