% function rhats = plot_rhats (steps, rhats, rhat)
function rhats = plot_rhats (rhats)

% kk = steps + 1;
% xpoints(kk)=kk;
% rhats(kk)=rhat;
subplot(3,1,3);
plot(rhats);
% ylim([-2,2]);
axis([1,inf,-1,1]);
%     title(['Episode: ',int2str(failures),' epsilon: ',num2str(epsilon)])
% xlabel('Steps')
xlabel('Episode')
ylabel('rhat')
drawnow


