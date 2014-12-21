function NeuronModel = newneuron(neuron_fun , model_type , state_num , model_param , ...
    spike_det_fun , spike_det_fun_param , reset_fun , reset_fun_param)
% NEWNEURON     Create new neuron model for Biological Neural Network (BNN)
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

%---checking input arguments and creating with defaults
switch nargin
    case 8          % all arguments
        neuron_prop = struct('NeuronFun' , neuron_fun , 'ModelType' , model_type , 'StateNum' , state_num , 'ModelParam' , model_param);
        spike_det   = struct('SpikeDetFun' , spike_det_fun , 'SpikeDetFunParam' , spike_det_fun_param);
        reset_fun   = struct('ResetFun' , reset_fun , 'ResetFunParam' , reset_fun_param);
                
    case 7
        neuron_prop = struct('NeuronFun' , neuron_fun , 'ModelType' , model_type , 'StateNum' , state_num , 'ModelParam' , model_param);
        spike_det   = struct('SpikeDetFun' , spike_det_fun , 'SpikeDetFunParam' , spike_det_fun_param);
        reset_fun   = struct('ResetFun' , reset_fun , 'ResetFunParam' , 'def');
        
    case 6
        neuron_prop = struct('NeuronFun' , neuron_fun , 'ModelType' , model_type , 'StateNum' , state_num , 'ModelParam' , model_param);
        spike_det   = struct('SpikeDetFun' , spike_det_fun , 'SpikeDetFunParam' , spike_det_fun_param);
        reset_fun   = struct('ResetFun' , 'reset_fun' , 'ResetFunParam' , 'def');
        
    case 5
        neuron_prop = struct('NeuronFun' , neuron_fun , 'ModelType' , model_type , 'StateNum' , state_num , 'ModelParam' , model_param);
        spike_det   = struct('SpikeDetFun' , spike_det_fun , 'SpikeDetFunParam' , 'def');
        reset_fun   = struct('ResetFun' , 'reset_fun' , 'ResetFunParam' , 'def');
        
    case 4
        neuron_prop = struct('NeuronFun' , neuron_fun , 'ModelType' , model_type , 'StateNum' , state_num , 'ModelParam' , model_param);
        spike_det   = struct('SpikeDetFun' , 'threshold_fun' , 'SpikeDetFunParam' , 'def');
        reset_fun   = struct('ResetFun' , 'reset_fun' , 'ResetFunParam' , 'def');
        
    case 3
        neuron_prop = struct('NeuronFun' , neuron_fun , 'ModelType' , model_type , 'StateNum' , state_num , 'ModelParam' , 'def');
        spike_det   = struct('SpikeDetFun' , 'threshold_fun' , 'SpikeDetFunParam' , 'def');
        reset_fun   = struct('ResetFun' , 'reset_fun' , 'ResetFunParam' , 'def');
        
    case 2
        neuron_prop = struct('NeuronFun' , neuron_fun , 'ModelType' , model_type , 'StateNum' , 1 , 'ModelParam' , 'def');
        spike_det   = struct('SpikeDetFun' , 'threshold_fun' , 'SpikeDetFunParam' , 'def');
        reset_fun   = struct('ResetFun' , 'reset_fun' , 'ResetFunParam' , 'def');
        
    case 1
        neuron_prop = struct('NeuronFun' , neuron_fun , 'ModelType' , 'CDE' , 'StateNum' , 1 , 'ModelParam' , 'def');
        spike_det   = struct('SpikeDetFun' , 'threshold_fun' , 'SpikeDetFunParam' , 'def');
        reset_fun   = struct('ResetFun' , 'reset_fun' , 'ResetFunParam' , 'def');
        
    case 0
        neuron_prop = struct('NeuronFun' , 'int_fire_lin' , 'ModelType' , 'CDE' , 'StateNum' , 1 , 'ModelParam' , 'def');
        spike_det   = struct('SpikeDetFun' , 'threshold_fun' , 'SpikeDetFunParam' , 'def');
        reset_fun   = struct('ResetFun' , 'reset_fun' , 'ResetFunParam' , 'def');
end

NeuronModel = struct('Model' , neuron_prop , 'SpikeDet' , spike_det , 'Reset' , reset_fun);

%---checking model
[CheckMessage CheckFlag]    = checkneuron(NeuronModel);
dispmessage(CheckMessage , 'text' , 'Neuron Build Status');

NeuronModel.BuildStatus.Message = CheckMessage;
NeuronModel.BuildStatus.Flag    = CheckFlag;

return