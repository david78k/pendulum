function IntSpikeInput = alpha_fun2_int(t , y , spike_time , neuron_delay)
% ALPHA_FUN2_INT     Alpha function for internal PSPs, ver.2
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
            tau_s = 10;
            tau_m = 5;

        otherwise
            error('Unknown synapse model parameters')
    end
else
    tau_s = net.Synapses.Internal.IntPSPFunParam(1);
    tau_m = net.Synapses.Internal.IntPSPFunParam(2);

end

neuron_num      = size(neuron_delay , 1);
IntSpikeInput   = zeros(neuron_num , neuron_num);

for k = 1:neuron_num
    in_spike = spike_time{k};
    
    for m = 1:neuron_num
        IntSpikeInput(m , k) = alpha2(t - neuron_delay(m , k) , in_spike , tau_s , tau_m);
    end
end

return

%---alpha2
function spike_input = alpha2(t , in_spike , tau_s , tau_m)

spike_input = 0;

for k = 1:size(in_spike , 2)
    if (in_spike(k) <= t)&(t >= 0)
        spike_input = spike_input + (1/(tau_s - tau_m))*(exp(-(t - in_spike(k))/tau_s) - exp(-(t - in_spike(k))/tau_m));
    end
end

return