function IntSpikeInput = synapse_model_template(t , y , spike_time , neuron_delay)
% SYNAPSE_MODEL_TEMPLATE    Template file for user defined synapse models
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

% First of all take a copy of this file, rename it to 'my_model_name' (for
% example 'beta2'). Also change the name of function at the first
% line from 'synapse_model_template' to 'my_model_name'.

%Defining BNN model as a global variable. DON'T CHANGE THIS SECTION.
global net

%Setting parameters for the model
if ischar(net.Synapses.Internal.IntPSPFunParam)
    switch lower(net.Synapses.Internal.IntPSPFunParam)
        case 'def'
            % Put default parameters for your model here.
            tau = 10;

        otherwise
            error('Unknown synapse model parameters')
    end
else
    % If you want to get other parameters rather than defaults from
    % outside, change these codes according to your model. Note that you
    % must preserve the parameters order here and outside to get the
    % correct results.
    tau = net.Synapses.Internal.IntPSPFunParam;
end

neuron_num      = size(neuron_delay , 1);
IntSpikeInput   = zeros(neuron_num , neuron_num);

% Loop sections for each input and neuron
for k = 1:neuron_num
    in_spike = spike_time{k};
    
    for m = 1:neuron_num
        % Put your PSP function here.
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