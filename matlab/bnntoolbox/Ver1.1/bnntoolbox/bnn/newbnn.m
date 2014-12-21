function net = newbnn(NetArch , NeuronModel , SynapseModel , AdaptModel , SimParam , IOMode)
% NEWBNN    Create a new Biological Neural Network (BNN)
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index


switch nargin
    case 0          % no input argument
        NetArch         = newarch;
        NeuronModel     = newneuron;
        SynapseModel    = newsynapse;
        AdaptModel      = newadapt;
        SimParam        = newsim;
        IOMode          = newiomode;
        
    case 1          % NetArch
        NeuronModel     = newneuron;
        SynapseModel    = newsynapse;
        AdaptModel      = newadapt;
        SimParam        = newsim;
        IOMode          = newiomode;
        
    case 2          % NetArch and NeuronModel
        SynapseModel    = newsynapse;
        AdaptModel      = newadapt;
        SimParam        = newsim;
        IOMode          = newiomode;
        
    case 3          % NetArch, NeuronModel, and SynapseModel
        AdaptModel      = newadapt;
        SimParam        = newsim;
        IOMode          = newiomode;        
        
    case 4          % NetArch, NeuronModel, SynapseModel, and AdaptModel
        SimParam        = newsim;
        IOMode          = newiomode;
               
    case 5          % NetArch, NeuronModel, SynapseModel, AdaptModel, and SimParam
        IOMode          = newiomode;
        
    case 6          % NetArch, NeuronModel, SynapseModel, AdaptModel, SimParam, and IOMode
end

net = struct('Architecture' , NetArch , 'Neurons' , NeuronModel , 'Synapses' , ...
    SynapseModel , 'Adaptation' , AdaptModel , 'Simulation' , SimParam , 'InputOutput' , IOMode);

%---checking bnn
[CheckMessage CheckFlag] = checkbnn(net);
dispmessage(CheckMessage , 'text' , 'BNN Build Status');

net.BuildStatus.Message = CheckMessage;
net.BuildStatus.Flag    = CheckFlag;

return