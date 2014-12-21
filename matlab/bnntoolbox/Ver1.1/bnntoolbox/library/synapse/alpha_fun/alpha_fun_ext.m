function ExtSpikeInput = alpha_fun_ext(t , y , input_spike_time , spike_input_delay)
% ALPHA_FUN_EXT     Alpha function for external PSPs
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

global net

%---setting parameters
if ischar(net.Synapses.External.ExtPSPFunParam)
    switch lower(net.Synapses.External.ExtPSPFunParam)
        case 'def'
            tau = 10;

        otherwise
            error('Unknown synapse model parameters')
    end
else
    tau = net.Synapses.External.ExtPSPFunParam;
end

input_spike_num = size(spike_input_delay , 2);
neuron_num      = size(spike_input_delay , 1);

ExtSpikeInput   = zeros(neuron_num , input_spike_num);

for k = 1:input_spike_num
    in_spike = input_spike_time{k};
    
    for m = 1:neuron_num
        ExtSpikeInput(m , k) = alpha(t - spike_input_delay(m , k) , in_spike , tau);
    end
end

return

%---alpha
function spike_input = alpha(t , in_spike , tau)

spike_input = 0;

for k = 1:size(in_spike , 2)
    if (in_spike(k) <= t)&(t >= 0)
        spike_input = spike_input + (1/tau)*exp(-(t - in_spike(k))/tau);
    end
end

return