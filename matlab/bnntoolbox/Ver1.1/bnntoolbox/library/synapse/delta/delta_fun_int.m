function IntSpikeInput = delta_fun_int(t , y , input_spike_time , spike_input_delay)
% DELTA_FUN_INT    Delta function for internal PSPs
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
            validation_period = 2;

        otherwise
            error('Unknown synapse model parameters')
    end
else
    validation_period = net.Synapses.Internal.IntPSPFunParam;

end

neuron_num      = size(spike_input_delay , 1);
IntSpikeInput   = zeros(neuron_num , neuron_num);

for k = 1:neuron_num
    in_spike = input_spike_time{k};
    
    for m = 1:neuron_num
        delayed_spikes      = in_spike + spike_input_delay(m , k);
        most_recent_spikes  = find(delayed_spikes <= t);
        if length(most_recent_spikes) ~= 0
            last_spike          = delayed_spikes(most_recent_spikes(end));

            if (last_spike >= 0)&(t - last_spike <= validation_period)
                IntSpikeInput(m , k) = 1;
            end
        end
    end
end

return