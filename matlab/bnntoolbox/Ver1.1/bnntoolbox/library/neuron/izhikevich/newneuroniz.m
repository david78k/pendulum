function NeuronModel = newneuroniz(model_param , spike_det_fun_param , reset_fun_param)
% NEWNEURONIF   Create Izhikevich model for Biological Neural Network (BNN)
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

%---checking input arguments and creating with defaults
switch nargin
    case 3
        
    case 2
        reset_fun_param     = 'def';
        
    case 1
        spike_det_fun_param = 'def';
        reset_fun_param     = 'def';
        
    case 0
        model_param         = 'def';
        spike_det_fun_param = 'def';
        reset_fun_param     = 'def';
        
end

neuron_fun      = 'izhikevich';
model_type      = 'CDE';
state_num       = 2;
spike_det_fun   = 'iz_threshold_fun';
reset_fun       = 'iz_reset_fun';

NeuronModel = newneuron(neuron_fun , model_type , state_num , model_param , ...
    spike_det_fun , spike_det_fun_param , reset_fun , reset_fun_param);

return