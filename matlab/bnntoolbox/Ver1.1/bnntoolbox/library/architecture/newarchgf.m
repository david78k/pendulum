function NetArch = newarchgf(neuron_num , input_num , input_type , neuron_type , weight_type , delay_type)
% NEWARCHGF     Create multilayer feedforward architecture with general
%               feedbacks for Biological Neural Network (BNN)
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

switch nargin
    case 6              % all arguments
        % nothing to do
        
    case 5
        delay_type  = 'zero';
        
    case 4
        delay_type  = 'zero';
        weight_type = 'random';
        
    case 3
        delay_type  = 'zero';
        weight_type = 'random';
        neuron_type = sign(randn(neuron_num , 1));
        
    case 2
        delay_type  = 'zero';
        weight_type = 'random';
        neuron_type = sign(randn(neuron_num , 1));
        input_type  = ones(input_num , 1);
                
    case 1
        delay_type  = 'zero';
        weight_type = 'random';
        input_num   = round(10*rand) + 1;
        neuron_type = sign(randn(neuron_num , 1));
        input_type  = ones(input_num , 1);
        
    case 0
        delay_type  = 'zero';
        weight_type = 'random';
        input_num   = round(10*rand) + 1;
        neuron_num  = round(10*rand) + 1;
        neuron_type = sign(randn(neuron_num , 1));
        input_type  = ones(input_num , 1);
        
end

total_neuron_num    = sum(neuron_num);
NetArch             = newarchfc(total_neuron_num , input_num , input_type , neuron_type , weight_type , delay_type);

%---modifying neuron weight and delay
layer_num   = length(neuron_num);

for k = 1:layer_num
    layer_neuron_num    = neuron_num(k);
    
    if k == 1
        row_index   = 1:layer_neuron_num;
        col_index   = 1:total_neuron_num - neuron_num(layer_num);
        
        NetArch.NeuronWeight(row_index , col_index) = 0;
        NetArch.NeuronDelay(row_index , col_index)  = 0;
    else
        pre_layer_neuron_num        = neuron_num(k - 1);
        total_pre_layer_neuron_num  = sum(neuron_num(1:k - 1));
        
        row_index   = total_pre_layer_neuron_num + 1:total_pre_layer_neuron_num + layer_neuron_num;
        col_index   = [1:total_pre_layer_neuron_num - pre_layer_neuron_num , ...
            total_pre_layer_neuron_num + 1:total_neuron_num];
        
        NetArch.NeuronWeight(row_index , col_index)    = 0;
        NetArch.NeuronDelay(row_index , col_index)     = 0;
    end
end

%---modifying input weight and delay
NetArch.InputWeight(neuron_num(1) + 1:end , :) = 0;
NetArch.InputDelay(neuron_num(1) + 1:end , :)  = 0;

return