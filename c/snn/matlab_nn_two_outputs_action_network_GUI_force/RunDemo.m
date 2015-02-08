function RunDemo

global balanced

total = 2;
bal = 0;
% save statistics in log files
% record videos
for i = 1:total
    i
    Demo
    if balanced
        bal = bal + 1
    end
end

% calculate success rate
% bal/total
bal/total

% average trials: success, failed cases
% min success trials:
% max success trials:

% average running time
% max running time
% min running time
    