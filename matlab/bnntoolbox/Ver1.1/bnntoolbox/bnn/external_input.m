function ExtInput = external_input(t , y)
% EXTERNAL_INPUT    Compute external input value for all neurons in the BNN
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

global net

input_num       = net.Architecture.InputNum;
neuron_num      = net.Architecture.NeuronNum;
input_type      = net.Architecture.InputType;
input_weight    = net.Architecture.InputWeight;
input_delay     = net.Architecture.InputDelay;

state_num       = net.Neurons.Model.StateNum;

psp_fun         = net.Synapses.External.ExtPSPFun;

analog_loc  = find(input_type == -1);
spike_loc   = find(input_type == 1);

switch lower(net.InputOutput.Input.InputFunName)
    case 'none'
        analog_input        = net.InputOutput.Input.AnalogInput;
        input_spike_time    = net.InputOutput.Input.InputSpikeTime;
    
    otherwise
        [analog_input , input_spike_time]       = feval(net.InputOutput.Input.InputFunName , t , y);
        net.InputOutput.Input.InputSpikeTime    = input_spike_time;
        net.InputOutput.Input.AnalogInput       = [net.InputOutput.Input.AnalogInput analog_input];     
end

if (length(input_spike_time) == 1)&(length(spike_loc) ~= 1)
    temp = cell(length(spike_loc) , 1);
    for k = 1:length(spike_loc)
        temp{k} = input_spike_time;
    end
    input_spike_time = temp;
end

if length(input_spike_time) ~= length(spike_loc)
    error('Input spike time cell size does not match with number of spiking inputs')
end

if (length(analog_input) == 1)&(length(analog_loc) ~= 1)
    analog_input = analog_input*ones(length(analog_loc) , 1);
end

if length(analog_input) ~= length(analog_loc)
    error('Analog input vector size does not match with number of analog inputs')
end

analog_input_delay  = input_delay(: , analog_loc);
spike_input_delay   = input_delay(: , spike_loc);

switch lower(psp_fun)
    case 'none'
        spike_net_input     = zeros(neuron_num , length(spike_loc));
        
    otherwise
        spike_net_input     = feval(psp_fun , t , y , input_spike_time , spike_input_delay);
end

analog_net_input = repmat(analog_input' , neuron_num , 1);

net_input                   = zeros(neuron_num , input_num);

if length(analog_loc) ~= 0
    net_input(: , analog_loc)   = analog_net_input;
end

if length(spike_loc) ~= 0
    net_input(: , spike_loc)    = spike_net_input;
end

ExtInput = (input_weight.*net_input)*ones(input_num , 1);

return