function IntSpikeInput = alpha_fun_int(t , y , spike_time , neuron_delay)
% ALPHA_FUN_INT     Alpha function for internal PSPs
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

global net

%---setting parameters
if ischar(net.Synapses.Internal.IntPSPFunParam)
    switch lower(net.Synapses.Internal.IntPSPFunParam)
        case 'def'
            tau = 10;

        otherwise
            error('Unknown synapse model parameters')
    end
else
    tau = net.Synapses.Internal.IntPSPFunParam;
end

neuron_num      = size(neuron_delay , 1);
IntSpikeInput   = zeros(neuron_num , neuron_num);

for k = 1:neuron_num
    in_spike = spike_time{k};
    
    for m = 1:neuron_num
        IntSpikeInput(m , k) = alpha(t - neuron_delay(m , k) , in_spike , tau);
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