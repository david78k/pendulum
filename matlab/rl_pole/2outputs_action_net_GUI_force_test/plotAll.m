function plotAll (forces, thetas, thetadots, rhats, ltrain, rtrain)
% plot force, theta, thetadot, h, hdot
% rhat last, 

% convergence criteria: |theta| < 0.1, |thetadot| < 1.5
nsubplots = 6;

figure
% kk = steps + 1;
% xpoints(kk)=kk;
% rhats(kk)=rhat;
subplot(nsubplots,1,1);
plot(ltrain);
% ylim([-2,2]);
% axis([1,inf,-1,1]);
%     title(['Episode: ',int2str(failures),' epsilon: ',num2str(epsilon)])
% xlabel('Step')
ylabel('Left train')
% ylabel('Left firing rate (spikes/sec)')

subplot(nsubplots,1,2);
plot(rtrain);
ylabel('Right train')
% ylabel('Right firing rate (spikes/sec)')

subplot(nsubplots,1,3);
plot(rhats);
ylabel('rhat')

subplot(nsubplots,1,4);
plot(thetas);
% xlabel('Step')
ylabel('theta')

subplot(nsubplots,1,nsubplots - 1);
plot(thetadots);
% xlabel('Step')
ylabel('theta\_dot')

subplot(nsubplots,1,nsubplots);
plot(forces);
xlabel('Step')
ylabel('Force')

drawnow


