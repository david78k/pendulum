function NeuronModel = newneuronfn(model_param , spike_det_fun_param)
% NEWNEURONFN   Create FitzHugh-Nagumo model for Biological Neural
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
    case 2
        
    case 1
        spike_det_fun_param = 'def';
        
    case 0
        spike_det_fun_param = 'def';
        model_param         = 'def';
        
end

neuron_fun      = 'fitzhugh_nagumo';
model_type      = 'CDE';
state_num       = 2;
spike_det_fun   = 'fn_threshold_fun';
reset_fun       = 'none';
reset_fun_param = 'def';

NeuronModel = newneuron(neuron_fun , model_type , state_num , model_param , ...
    spike_det_fun , spike_det_fun_param , reset_fun , reset_fun_param);

return