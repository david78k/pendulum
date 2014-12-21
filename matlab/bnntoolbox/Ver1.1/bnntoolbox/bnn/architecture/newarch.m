function NetArch = newarch(neuron_num , input_num , input_type , neuron_type , ...
    input_weight , neuron_weight , input_delay , neuron_delay)
% NEWARCH   Create a new architecture for Biological Neural Network (BNN)
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index


%---checking input arguments and creating with defaults
switch nargin
    case 8              % all arguments
        
    case 7
        neuron_delay    = zeros(neuron_num , neuron_num);
        
    case 6
        neuron_delay    = zeros(neuron_num , neuron_num);
        input_delay     = zeros(neuron_num , input_num);
        
    case 5
        neuron_delay    = zeros(neuron_num , neuron_num);
        input_delay     = zeros(neuron_num , input_num);
        neuron_weight   = rand(neuron_num , neuron_num);
        
    case 4
        neuron_delay    = zeros(neuron_num , neuron_num);
        input_delay     = zeros(neuron_num , input_num);
        neuron_weight   = rand(neuron_num , neuron_num);
        input_weight    = rand(neuron_num , input_num);
        
    case 3
        neuron_delay    = zeros(neuron_num , neuron_num);
        input_delay     = zeros(neuron_num , input_num);
        neuron_weight   = rand(neuron_num , neuron_num);
        input_weight    = rand(neuron_num , input_num);
        neuron_type     = sign(randn(neuron_num , 1));
        
    case 2
        neuron_delay    = zeros(neuron_num , neuron_num);
        input_delay     = zeros(neuron_num , input_num);
        neuron_weight   = rand(neuron_num , neuron_num);
        input_weight    = rand(neuron_num , input_num);
        neuron_type     = sign(randn(neuron_num , 1));
        input_type      = ones(input_num , 1);
        
    case 1
        input_num       = round(10*rand) + 1;
        neuron_delay    = zeros(neuron_num , neuron_num);
        input_delay     = zeros(neuron_num , input_num);
        neuron_weight   = rand(neuron_num , neuron_num);
        input_weight    = rand(neuron_num , input_num);
        input_type      = ones(input_num , 1);
        neuron_type     = sign(randn(neuron_num , 1));
        
    case 0
        neuron_num      = round(10*rand) + 1;
        input_num       = round(10*rand) + 1;
        neuron_delay    = zeros(neuron_num , neuron_num);
        input_delay     = zeros(neuron_num , input_num);
        neuron_weight   = rand(neuron_num , neuron_num);
        input_weight    = rand(neuron_num , input_num);
        input_type      = ones(input_num , 1);
        neuron_type     = sign(randn(neuron_num , 1));
        
end

NetArch = struct('NeuronNum' , neuron_num , 'InputNum' , input_num , 'InputType' , input_type , 'NeuronType' , neuron_type , ...
            'InputWeight' , input_weight , 'NeuronWeight' , neuron_weight , 'InputDelay' , input_delay , ...
            'NeuronDelay' , neuron_delay);

%---checking arch
[CheckMessage CheckFlag]    = checkarch(NetArch);
dispmessage(CheckMessage , 'text' , 'Architecture Build Status');

NetArch.BuildStatus.Message = CheckMessage;
NetArch.BuildStatus.Flag    = CheckFlag;

return