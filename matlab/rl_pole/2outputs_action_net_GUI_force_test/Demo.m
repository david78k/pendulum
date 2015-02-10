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
global learned failures
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
totalRuns = 5;
bal = 0;
% save statistics in log files. record videos
for i = 1:totalRuns
    fprintf('--------- Run %d ---------\n', i);;
    balanced = false; learned = false;
    Cart_Pole_NN
    if balanced
        bal = bal + 1;
        disp(['Balanced = ' num2str(bal)]);
        learned = true;
        break;
    end
end

toc

% report.m
disp(['=============== SUMMARY ==============']);
disp(['Final Max Steps: ' num2str(FinalMaxSteps)]);
disp(['Success rate: ' num2str(100.0*bal/i) '% (' num2str(bal) '/' num2str(i) ')']);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% TEST LOOP
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if learned
    tic
    
    FinalMaxSteps = 0;
    totalRuns = 10;
    bal = 0; sumTrials = 0; maxTrial = 1; minTrial = 100;
    
    for i = 1:totalRuns
        fprintf('--------- Test %d ---------\n', i);
        balanced = false;
        Cart_Pole_NN
        if balanced
            bal = bal + 1;
            disp(['Balanced = ' num2str(bal)]);            
        end
        sumTrials = sumTrials + failures;
        maxTrial = max(maxTrial, failures);
        minTrial = min(minTrial, failures);
    end
    
    toc
    
    % report.m
    disp(['=============== TEST SUMMARY ==============']);
    disp(['Success rate: ' num2str(100.0*bal/i) '% (' num2str(bal) '/' num2str(i) ')']);
    disp(['Average trials: ' num2str(sumTrials/i) ' (' num2str(sumTrials) '/' num2str(i) ')' ...
        '  max ' num2str(maxTrial) ' min ' num2str(minTrial)]);
end
