function NeuronModel = newneuronif(if_model , model_param , spike_det_fun_param , reset_fun_param)
% NEWNEURONIF   Create integrate-and-fire model for Biological Neural
%               Network (BNN)
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

%---checking input arguments and creating with defaults
switch nargin
    case 4
        
    case 3
        reset_fun_param     = 'def';
        
    case 2
        spike_det_fun_param = 'def';
        reset_fun_param     = 'def';
        
    case 1
        model_param         = 'def';
        spike_det_fun_param = 'def';
        reset_fun_param     = 'def';
        
    case 0
        if_model            = 'linear';
        model_param         = 'def';
        spike_det_fun_param = 'def';
        reset_fun_param     = 'def';
        
end

switch lower(if_model)
    case 'linear'
        neuron_fun      = 'int_fire_lin';
        model_type      = 'CDE';
        state_num       = 1;
        spike_det_fun   = 'threshold_fun';
        reset_fun       = 'reset_fun';
        
    case 'quadratic'
        neuron_fun      = 'int_fire_quad';
        model_type      = 'CDE';
        state_num       = 1;
        spike_det_fun   = 'threshold_fun';
        reset_fun       = 'reset_fun';
end

NeuronModel = newneuron(neuron_fun , model_type , state_num , model_param , ...
    spike_det_fun , spike_det_fun_param , reset_fun , reset_fun_param);

return