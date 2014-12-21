function plotspikes2d(net)
% PLOTSPIKES2D	Plot spike times for a BNN in 2D
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
            plot(temp_spikes , k*ones(size(temp_spikes)) , '.')
            hold on
        end
        hold off
        axis([time(1) time(end) 0.5 length(spikes) + 0.5])
end