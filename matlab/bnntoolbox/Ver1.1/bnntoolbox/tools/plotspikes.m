function plotspikes(net , neuron_no)
% PLOTSPIKES	Plot spike times for a BNN
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

switch nargin
    case 0
        error('You must enter a BNN object')
        
    case 1
        spikes  = getspikes(net);
        time    = gettime(net);
        for k = 1:length(spikes)
            temp_spikes = spikes{k};
            stem(temp_spikes , ones(size(temp_spikes)) , '.')
            hold on
        end
        hold off
        axis([time(1) time(end) 0 1.2])
    case 2
        spikes  = getspikes(net , neuron_no);
        time    = gettime(net);
        stem(spikes , ones(size(spikes)) , '.')
        axis([time(1) time(end) 0 1.2])
end