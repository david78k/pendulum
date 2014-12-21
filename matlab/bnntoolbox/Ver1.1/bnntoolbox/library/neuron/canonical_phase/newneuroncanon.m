function NeuronModel = newneuroncanon
% NEWNEURONCANON    Create canonical phase model for Biological Neural
%                   Network (BNN)
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

neuron_fun          = 'canonical_phase';
model_type          = 'CDE';
state_num           = 1;
model_param         = 'def';
spike_det_fun       = 'canon_threshold_fun';
spike_det_fun_param = 'def';
reset_fun           = 'canon_reset_fun';
reset_fun_param     = 'def';

NeuronModel = newneuron(neuron_fun , model_type , state_num , model_param , ...
    spike_det_fun , spike_det_fun_param , reset_fun , reset_fun_param);

return