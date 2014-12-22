function neuron = createNewNeuron(PARAM,N)
%         N = round(1.75*PARAM.epspCut*PARAM.tm*length(spikeList)/spikeList(end));
neuron.epspAmplitude = zeros(1,N);
neuron.epspTime = zeros(1,N);
neuron.epspAfferent = uint16(zeros(1,N));
neuron.nEpsp = 0;
if PARAM.fixedFiringMode
    neuron.nextFiring = PARAM.fixedFiringLatency;
else
    neuron.nextFiring = Inf;
end
neuron.firingTime = zeros(1,1000000);
neuron.nFiring = 0;
neuron.alreadyDepressed = false(1,PARAM.nAfferent);
%         neuron.nInefficient = 0;
neuron.maxPotential = 0;
%         neuron.currentPotential = NaN;
neuron.trPot = 0;%PARAM.initialTr;


%         neuron(nn).initialPot = 0;
%         neuron(nn).lastInhib = NaN;

neuron.ipspTime = zeros(1,1000);
neuron.nIpsp = 0;

    if PARAM.fixedFiringMode
        neuron.weight = .5*ones(1,PARAM.nAfferent);
    else
        if PARAM.threshold == Inf % must be a post-fixedFiringMode computatio
            load('../mat/weight.mat');
            neuron.weight = weight;
    %         neuron.weight = double([neuron.weight>.5]);
        else
%             randn('state',1+PARAM.randomState); % +1 is there to make sure that pseudo-random spike trains and weights are not correlated
%             rand('state',1+PARAM.randomState);  % +1 is there to make sure that pseudo-random spike trains and weights are not correlated
           	meanW = 1.9*PARAM.threshold/PARAM.nAfferent;
           	stdW = 1.0*meanW;
           	maxW = 1.0;%2*1.4*PARAM.threshold/PARAM.nAfferent;
%             for nn=1:PARAM.nNeuron
%                 neuron(nn).weight = meanW + stdW * randn(1,PARAM.nAfferent);
%                 neuron.weight = (1-rand(1,PARAM.nAfferent).^1.0);
                neuron.weight = .9+.1*rand(1,PARAM.nAfferent) ;                
                neuron.weight = max(0,min(1,neuron.weight));
%             end
        end
    end
