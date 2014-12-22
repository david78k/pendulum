% This code was used in:
% Masquelier T (2012) Relative spike time coding and STDP-based orientation
% selectivity in the early visual system in natural continuous and saccadic
% vision: a computational model. J Comput Neurosc.
% timothee.masquelier@alum.mit.edu
% The current parameter values correspond the baseline simulation in the
% paper.
%
% This code simulates the LGN stage. In fact it calls the same mex file as
% the V1 stage, but with PARAM.stdp_a_pos=PARAM.stdp_a_neg=0 (no STDP) and PARAM.inhibStrength=0 (no lateral inhibition)
% The scripts loads all the afferent.rand###.###.###.mat files (corresponding to RGC output) and produces
% new ones (corresponding to LGN output).

timedLogLn('LGN')

clear all

PARAM.randomState = 300;

disp(['Loading ../mat/afferent.rand' sprintf('%03d',PARAM.randomState) '.000.000.mat'])
load(['../mat/afferent.rand' sprintf('%03d',PARAM.randomState) '.000.000.mat'])

% N = round(sqrt(double(max(afferentList))/2));
% polarity = afferentList > N^2;
% i = floor( (double(afferentList) - polarity * N^2 -1)/N ) + 1;
% j = mod( (double(afferentList) - polarity * N^2 -1), N ) + 1;
% newN = 2*32;
% idx = (i<=newN & j<=newN);
% spikeList = spikeList(idx);
% afferentList = uint16( j(idx) + (i(idx)-1)*newN + polarity(idx)*newN^2 );

PARAM.stdp_t_pos = 1; %(double): tau^+ STDP time constant in s (for LTP)
PARAM.stdp_t_neg = 1; %(double): tau^- STDP time constant in s (for LTD)
PARAM.stdp_a_pos = 0; %(double): a^+ STDP constant (for LTP)
PARAM.stdp_a_neg = 0; %(double): a^- STDP constant (for LTD). Should be <0
PARAM.stdp_cut = 0; %(double): time delay (in number of time constants) for STDP modifications to be considered negligible (eg 7)
PARAM.minWeight = 0; %(double): lower bound for weight (usually 0, however when using memristors, max restistance / min resistance ~ 10, therefore minWeight ~0.1).
PARAM.memristor = false; %(logical): use or not memristor-based STDP with quadratic weight dependance, and linear region when t_pre - t_post is in [-t_op, t_on]
PARAM.t_op = 0; %(double) : limit of the linear LTP region
PARAM.t_on = 0; % (double): limit of the linear LTD region
PARAM.tmpResolution = 1e-3; % (double): temporal resolution in s

PARAM.tm = 20e-3; % membrane time constant (typically 10-30ms)
PARAM.ts = PARAM.tm/4; % synapse time constant
PARAM.epspCut = 5;% specifies after how many ms we neglect the epsp
PARAM.epspKernel = pspKernel(0:PARAM.tmpResolution:PARAM.epspCut*PARAM.tm,PARAM.ts,PARAM.tm);
[m idx] = max(PARAM.epspKernel);
PARAM.epspKernel = PARAM.epspKernel/m;
%     figure; plot(PARAM.tmpResolution*(1:length(PARAM.epspKernel)),PARAM.epspKernel)
PARAM.epspMaxTime = (idx-1)*PARAM.tmpResolution;
PARAM.refractoryPeriod = 3e-3;

PARAM.usePssKernel = false; % (logical): use or not the post synaptic spike kernel (negative spike after potential that follows the pulse)
PARAM.pssKernel = []; %(double): array containing the PSS kernel
PARAM.ipspKernel = [];
PARAM.inhibStrength = 0; % inhibition strength (in fraction of threshold)

PARAM.nAfferent = double(max(afferentList)+1); %(double): number of afferents
PARAM.threshold = 1.25; %(double): neurons' threshold (arbitrary units)
PARAM.fixedFiringMode = false; %(logical): use or not the fixed firing mode, in which periodic firing is imposed
PARAM.fixedFiringLatency = false; % (double): in fixedFiringMode specify the latency of the first firing
PARAM.fixedFiringPeriod = false; % (double): in fixedFiringMode specify the period of the first firing

N = sqrt(PARAM.nAfferent/2);
% N = 313;
disp(['Estimated N = ' num2str(N)])

arraySize = 20;


fileList = dir(['../mat/afferent.rand' sprintf('%03d',PARAM.randomState) '.*.*.mat']);
disp([int2str(length(fileList)) ' files found']);

for f=1:length(fileList)
    
    load(['../mat/' fileList(f).name])

    % create neurons
    for polarity = 1:-1:0
        for i=N:-1:1
    %         disp(int2str(i))
            for j=N:-1:1
                neuron(j+(i-1)*N+polarity*N^2).weight = zeros(1,2*N^2);
                neuron(j+(i-1)*N+polarity*N^2).weight(j+(i-1)*N+polarity*N^2) = 1;
                neuron(j+(i-1)*N+polarity*N^2).epspAmplitude = zeros(1,arraySize);
                neuron(j+(i-1)*N+polarity*N^2).epspTime = zeros(1,arraySize);
                neuron(j+(i-1)*N+polarity*N^2).epspAfferent = uint16(zeros(1,arraySize));
                neuron(j+(i-1)*N+polarity*N^2).nEpsp = 0;
                neuron(j+(i-1)*N+polarity*N^2).nextFiring = Inf;
                neuron(j+(i-1)*N+polarity*N^2).firingTime = zeros(1,round(20*spikeList(end)));
                neuron(j+(i-1)*N+polarity*N^2).nFiring = 0;
                neuron(j+(i-1)*N+polarity*N^2).alreadyDepressed = false(1,PARAM.nAfferent);
                neuron(j+(i-1)*N+polarity*N^2).maxPotential = 0;
                neuron(j+(i-1)*N+polarity*N^2).trPot = 0;%PARAM.initialTr;
                neuron(j+(i-1)*N+polarity*N^2).ipspTime = [];
                neuron(j+(i-1)*N+polarity*N^2).nIpsp = 0;
            end
        end
    end

    % run
    timedLogLn(['Running (' int2str(length(spikeList)) ' iterations ~ ' int2str(2e-9*length(spikeList)*length(neuron)*(ceil( PARAM.epspMaxTime / PARAM.tmpResolution )+1)) ' min )'])
    neuron=STDPContinuous(neuron,spikeList',afferentList',false,true,PARAM); % C indexes start at 0
    timedLog(['Done'])

    disp(['Avg firing rate =' num2str(mean([neuron(:).nFiring])/(spikeList(end)-spikeList(1))) ])

    if sum([neuron.nFiring]) == 0
        warning('Neurons do not fire')
%         break;
    end
    if max([neuron.nFiring])>length(neuron(1).firingTime)
        warning('Increase firingTime array size')
    end

    spikeList = zeros(1,sum([neuron.nFiring]));
    afferentList = uint16(spikeList);

    cursor = 0;
    for n=1:length(neuron)
        spikeList(cursor+1:cursor+neuron(n).nFiring) = neuron(n).firingTime(1:neuron(n).nFiring);
        afferentList(cursor+1:cursor+neuron(n).nFiring) = uint16(n);   
        cursor = cursor + neuron(n).nFiring;
    end
    % clear neuron
    [spikeList idx] = sort(spikeList);
    afferentList = afferentList(idx);

    disp(['Saving in ../mat/' fileList(f).name ])
    save(['../mat/' fileList(f).name],'spikeList','afferentList')

    % brian format
%     spikeList = [double(afferentList')-1, spikeList'];
end
