function net = simbnn(network)
% SIMBNN     Simulate a Biological Neural Network (BNN)
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

global net

switch nargin
    case 0
        error('Nothing to simulate')
end

net = network;
clear network

%---checking bnn
[CheckMessage CheckFlag] = checkbnn(net);
if ~CheckFlag
    error(['Invalid BNN: ' , CheckMessage])
end

%---parameters
neuron_num      = net.Architecture.NeuronNum;

model_type      = net.Neurons.Model.ModelType;
state_num       = net.Neurons.Model.StateNum;
neuron_function = net.Neurons.Model.NeuronFun;
event_function  = net.Neurons.SpikeDet.SpikeDetFun;
reset_function  = net.Neurons.Reset.ResetFun;

weight_adapt    = net.Adaptation.Weight.AdaptFun;
delay_adapt     = net.Adaptation.Delay.AdaptFun;
threshold_adapt = net.Adaptation.Threshold.AdaptFun;
model_adapt     = net.Adaptation.Model.AdaptFun;

solver_function = net.Simulation.Solver;
initial_cond    = net.Simulation.InitialCond;
stop_function   = net.Simulation.StopFun;
user_function   = net.Simulation.UserFun;
start_time      = net.Simulation.StartTime;
stop_time       = net.Simulation.StopTime;
current_time    = start_time;
stop_flag       = 0;
spike_flag      = 0;

auto_save       = net.InputOutput.Output.AutoSave;

%---creating reser function
switch upper(model_type)
    case 'CDE'
        solver_option   = odeset('Events' , str2func(event_function));
        
    case 'CTE'
        
end

%---checking initial conditions
if (length(initial_cond) ~= neuron_num*state_num)
    if length(initial_cond) == 1
        initial_cond = initial_cond*ones(neuron_num*state_num , 1);
    elseif length(initial_cond) == state_num
        initial_cond = repmat(initial_cond , neuron_num , 1);
    else
        error('Initial condition size does not match with the number of neurons')
    end
end

time_vector      = [];   state_vector        = [];   event_time      = [];   event_states        = [];   event_index      = [];
time_vector_temp = [];   state_vector_temp   = [];   event_time_temp = [];   event_states_temp   = [];   event_index_temp = [];

net.InputOutput.Output.SpikeTimes   = cell(neuron_num , 1);
spike_times                         = cell(neuron_num , 1);

disp('Current time:')

while ~stop_flag
    
    %---simulating
    switch upper(model_type)
        case 'CDE'
            [time_vector_temp , state_vector_temp , event_time_temp , event_states_temp , event_index_temp] = feval(solver_function , ...
                str2func(neuron_function) , [current_time stop_time] , initial_cond , solver_option);
            
        otherwise
            error('Unknown model type')
    end
    
    %---storing new information
    time_vector     = [time_vector  ; time_vector_temp];
    state_vector    = [state_vector ; state_vector_temp];
    current_time    = time_vector(end);
    
    for k = 1:size(event_index_temp , 1)
        if event_time_temp(k) == current_time
            spiker  = 1 + floor((event_index_temp(k) - 1)/state_num);
            spike_times{spiker} = [spike_times{spiker} event_time_temp(k)];
            spike_flag = 1;
        end
    end
    
    net.InputOutput.Output.SpikeTimes    = spike_times;
    net.InputOutput.Output.State         = state_vector;
    net.InputOutput.Output.Time          = time_vector; 
       
    disp(num2str(current_time))
    
    %---reset
    switch lower(reset_function)
        case 'none'
            initial_cond = state_vector_temp(end , :);
        otherwise
            if spike_flag
                initial_cond = feval(reset_function , state_vector_temp , event_index_temp);
            end
    end
    
    %---adaptation
    switch lower(weight_adapt)
        case 'none'
            
        otherwise
            net = feval(weight_adapt , net);
    end
    
    switch lower(delay_adapt)
        case 'none'
            
        otherwise
            net = feval(delay_adapt , net);
    end
    
    switch lower(threshold_adapt)
        case 'none'
            
        otherwise
            net = feval(threshold_adapt , net);
    end
    
    switch lower(model_adapt)
        case 'none'
            
        otherwise
            net = feval(model_adapt , net);
    end
    
    %---calling user function
    switch lower(user_function)
        case 'none'
            
            
        otherwise
            net = feval(user_function , net);
            
    end
      
    %---initializing next run
    spike_flag = 0;
    
    time_vector_temp    = [];
    state_vector_temp   = [];
    event_states_temp   = [];
    
    
    %---stop function
    switch lower(stop_function)
        case 'none'
            
            
        otherwise
            stop_flag = feval(stop_function , net);
            
    end
    
    if current_time >= stop_time
        stop_flag = 1;
        break
    end
end

%---auto save
if auto_save
    savebnn(net)
end

return