function NetArch = newarchfc(neuron_num , input_num , input_type , neuron_type , weight_type , delay_type)
% NEWARCHFC     Create fully connected architecture forBiological Neural
%               Network (BNN)
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
        
    case 5              % without delay type
        delay_type  = 'zero';
        
    case 4              % without weight type, delay type
        delay_type  = 'zero';
        weight_type = 'random';
        
    case 3              % without input type, weight type, delay type
        delay_type  = 'zero';
        weight_type = 'random';
        neuron_type = sign(randn(neuron_num , 1));
        
    case 2              % without input num, input type, weight type, delay type
        delay_type  = 'zero';
        weight_type = 'random';
        neuron_type = sign(randn(neuron_num , 1));
        input_type  = ones(input_num , 1);
                
    case 1              % nothing
        delay_type  = 'zero';
        weight_type = 'random';
        input_num   = round(10*rand) + 1;
        neuron_type = sign(randn(neuron_num , 1));
        input_type  = ones(input_num , 1);
        
    case 0
        neuron_num  = round(10*rand) + 1;
        delay_type  = 'zero';
        weight_type = 'random';
        input_num   = round(10*rand) + 1;
        neuron_type = sign(randn(neuron_num , 1));
        input_type  = ones(input_num , 1);
end

%---creating connection matrixes
switch lower(weight_type)       % selecting weight matrix
    case 'random'
        neuron_weight   = rand(neuron_num , neuron_num);
        input_weight    = rand(neuron_num , input_num);
        
    case 'normal'
        neuron_weight   = randn(neuron_num , neuron_num);
        input_weight    = randn(neuron_num , input_num);

    case 'one'
        neuron_weight   = ones(neuron_num , neuron_num);
        input_weight    = ones(neuron_num , input_num);

    case 'zero'
        neuron_weight   = zeros(neuron_num , neuron_num);
        input_weight    = zeros(neuron_num , input_num);
end

%---modifying excitatory ans inhibitory weights
ex_neurons      = find(neuron_type == 1);
in_neurons      = find(neuron_type == -1);

neuron_weight(: , ex_neurons) = abs(neuron_weight(: , ex_neurons));
neuron_weight(: , in_neurons) = -1*abs(neuron_weight(: , in_neurons));

switch lower(delay_type)        % selecting delay matrix
    case 'random'
        neuron_delay   = rand(neuron_num , neuron_num);
        input_delay    = rand(neuron_num , input_num);

    case 'one'
        neuron_delay   = ones(neuron_num , neuron_num);
        input_delay    = ones(neuron_num , input_num);

    case 'zero'
        neuron_delay   = zeros(neuron_num , neuron_num);
        input_delay    = zeros(neuron_num , input_num);

end

%---creating arch
NetArch = newarch(neuron_num , input_num , input_type , neuron_type , ...
    input_weight , neuron_weight , input_delay , neuron_delay);

return