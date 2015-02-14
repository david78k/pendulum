function test

spike_times = [0 1 3 7];
spike_times2 = [2 3 4 6];

for i=1:4
    line([spike_times(i) spike_times(i)], [0 1]);
    line([spike_times2(i) spike_times2(i)], [1 2]);
end

xlabel('Step')
ylabel('Right Train                 Left Train')

% line([2 2], [0 1]);


% plot([[1 1] [2 2]], [[0 1] [0 1]]);
% plot([1 1], [0 1]);
% hold on;
% plot([2 2], [0 1]);