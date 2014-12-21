function [CheckMessage , CheckFlag] = checkneuron(NeuronModel)
% CHECKNEURON   Check neuron model for errors
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

CheckMessage    = 'OK';
CheckFlag       = 1;

switch nargin
    case 0
        CheckMessage    = 'Nothing to check';
        CheckFlag       = 0;
        
    case 1
        %---neuron_prop
        if exist(NeuronModel.Model.NeuronFun) ~= 2
            CheckMessage    = 'Neuron function is not available';
            CheckFlag       = 2;
        end
        
        if NeuronModel.Model.StateNum == 0
            CheckMessage    = 'Number of neuron states is zero';
            CheckFlag       = 3;
        end
                
        %---spike_det
        if exist(NeuronModel.SpikeDet.SpikeDetFun) ~= 2
            CheckMessage    = 'Spike detection function is not available';
            CheckFlag       = 4;
        end
        
        %---reset_fun
        switch lower(NeuronModel.Reset.ResetFun)
            case 'none'
                
            otherwise
                
                if exist(NeuronModel.Reset.ResetFun) ~= 2
                    CheckMessage    = 'Reset function is not available';
                    CheckFlag       = 5;
                end
        end
end

return