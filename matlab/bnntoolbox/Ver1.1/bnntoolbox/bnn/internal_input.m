function IntInput = internal_input(t , y)
% INTERNAL_INPUT    Compute internal input value for all neurons in the BNN
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

global net

neuron_num      = net.Architecture.NeuronNum;
neuron_weight   = net.Architecture.NeuronWeight;
neuron_delay    = net.Architecture.NeuronDelay;

psp_fun         = net.Synapses.Internal.IntPSPFun;

spike_times     = net.InputOutput.Output.SpikeTimes;

switch lower(psp_fun)
    case 'none'
        net_feedback    = zeros(neuron_num , neuron_num);
        
    otherwise
        net_feedback    = feval(psp_fun , t , y , spike_times , neuron_delay);
end

IntInput = (neuron_weight.*net_feedback)*ones(neuron_num , 1);

return