function [CheckMessage , CheckFlag] = checkbnn(net)
% CHECKBNN   Check BNN for errors
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
        NetArch         = net.Architecture;
        NeuronModel     = net.Neurons;
        SynapseModel    = net.Synapses;
        AdaptModel      = net.Adaptation;
        SimParam        = net.Simulation;
        IOMode          = net.InputOutput;
        
        %---checking NetArch
        [CheckMessage CheckFlag] = checkarch(NetArch);
        if (CheckFlag ~= 1)
            error(['Invalid architecture: ' , CheckMessage])
        end
        
        %---checking NeuronModel
        [CheckMessage CheckFlag] = checkneuron(NeuronModel);
        if (CheckFlag ~= 1)
            error(['Invalid neuron model: ' , CheckMessage])
        end
        
        %---checking SyanpseModel
        [CheckMessage CheckFlag] = checksynapse(SynapseModel);
        if (CheckFlag ~= 1)
            error(['Invalid synapse model: ' , CheckMessage])
        end
        
        %---checking AdaptModel
        [CheckMessage CheckFlag] = checkadapt(AdaptModel);
        if (CheckFlag ~= 1)
            error(['Invalid adaptation model: ' , CheckMessage])
        end
        
        %---checking SimParam
        [CheckMessage CheckFlag] = checksim(SimParam);
        if (CheckFlag ~= 1)
            error(['Invalid simulation parameters: ' , CheckMessage])
        end
        
        %---checking IOMode
        [CheckMessage CheckFlag] = checkiomode(IOMode);
        if (CheckFlag ~= 1)
            error(['Invalid I/O parameters: ' , CheckMessage])
        end        
        
end

return