% Changelog
% - unlimited to PAST_STEPS
% - left = 1 to left = 0
% - show BP error rhat
% - trace spikes
% - show force (optional)
% - rate pre-learning and post-learning with fixed weights
% - sample time dt 0.01 to 0.02
function Demo()
% clc
clf;
clear all;
global TxtEpisode TxtSteps goal f1 f2 grafica balanced FinalMaxSteps
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
f1 = subplot(3,1,1);
box off
f2 = subplot(3,1,2);
f3 = subplot(3,1,3);
grafica = false;
P2 = ['setgrafica();'];
PushBut2=uicontrol(gcf,'Style','togglebutton','Units','normalized', ...
    'Position',[0.83 .9 0.16 0.05],'string','GRAPHIC', ...
    'Callback',P2,'visible','on','BackgroundColor',[0.8 0.8 0.8]);
set(gcf,'name','Reinforcement Learning with Spiking Neural Networks');
set(gcf,'Color','w')
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
grid off	% turns on grid
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
set(gco,'BackingStore','off') % for realtime inverse kinematics
set(gco,'Units','data')
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% REINFORCEMENT LEARNING LOOP
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
drawnow;

tic

FinalMaxSteps = 0;
total = 5;
bal = 0;
% save statistics in log files
% record videos
for i = 1:total
    fprintf('Run %d:\n', i);
    Cart_Pole_NN
    if balanced
        bal = bal + 1;
        disp(['Balanced = ' num2str(bal)]);
    end
end

toc

% report.m
disp(['Final Max Steps: ' num2str(FinalMaxSteps)]);
disp(['Success rate: ' num2str(100.0*bal/total) '% (' num2str(bal) '/' num2str(total) ')']);


