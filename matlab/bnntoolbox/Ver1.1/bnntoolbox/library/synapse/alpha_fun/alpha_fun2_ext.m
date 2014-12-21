function ExtSpikeInput = alpha_fun2_ext(t , y , input_spike_time , spike_input_delay)
% ALPHA_FUN2_EXT     Alpha function for external PSPs, ver.2
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
            tau_s = 10;
            tau_m = 5;

        otherwise
            error('Unknown synapse model parameters')
    end
else
    tau_s = net.Synapses.PSP.External.ExtPSPFunParam(1);
    tau_m = net.Synapses.PSP.External.ExtPSPFunParam(2);

end

input_spike_num = size(spike_input_delay , 2);
neuron_num      = size(spike_input_delay , 1);

ExtSpikeInput   = zeros(neuron_num , input_spike_num);

for k = 1:input_spike_num
    in_spike = input_spike_time{k};
    
    for m = 1:neuron_num
        ExtSpikeInput(m , k) = alpha2(t - spike_input_delay(m , k) , in_spike , tau_s , tau_m);
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