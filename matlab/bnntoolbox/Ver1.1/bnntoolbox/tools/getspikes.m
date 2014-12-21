function spike_time = getspikes(net , neuron_no)
% GETSPIKES	Get spike times from a BNN
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
        spike_time  = net.InputOutput.Output.SpikeTimes;
        
    case 2
        spike_time  = net.InputOutput.Output.SpikeTimes{neuron_no};
end