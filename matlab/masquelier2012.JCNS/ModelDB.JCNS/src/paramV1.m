% This V1 paremeter file was used in:
% Masquelier T (2012) Relative spike time coding and STDP-based orientation
% selectivity in the early visual system in natural continuous and saccadic
% vision: a computational model. J Comput Neurosc.
% timothee.masquelier@alum.mit.edu
% The current parameter values correspond the baseline simulation in the
% paper.

global PARAM

PARAM.goOn = 0;

if PARAM.goOn
    PARAM.nRun = 1;
%     PARAM.stdp_a_pos = 2^-4; % avoid > 2^-4
% %     PARAM.stdp_a_neg = -.9* PARAM.stdp_a_pos; % at least .85
%     PARAM.stdp_a_neg = -.875* PARAM.stdp_a_pos; % at least .85
% %     PARAM.threshold = Inf;
% %     neuron.nextFiring = Inf;
% %     PARAM.fixedFiringMode = false;
else % new computation
    clear all
    global PARAM
    PARAM.goOn = false;
    
    PARAM.dump=false;
    PARAM.beSmart=true; % save time by not computing the potential when it is estimated that the threshold cannot be reached. Rigorous only when no inhibition, and no PSP Kernel. Otherwise use at your own risks...
 
    PARAM.fixedFiringMode = false;
    PARAM.fixedFiringLatency = 10e-3;
    PARAM.fixedFiringPeriod = 150e-3;

    n=0; % spike train number
    PARAM.nRun = 1; % number of times spike train is propagated
    
    % Random generators
    PARAM.randomState = 300;
%     d = dir('../mat/rand*.mat');
%     if isempty(d)
%         PARAM.randomState=0;
%     else
%         last = d(end).name;
%         PARAM.randomState = str2num(last(5:7))+1;
%     end
%     % just to warn other threads that this random is done
%     tmp=0;
%     save(['../mat/rand' sprintf('%03d',PARAM.randomState) '.mat'],'tmp');

    PARAM.useSavedWeight = true;

    %********
    %* STDP *
    %********
    PARAM.stdp_t_pos = 17e-3; %(source: Bi & Poo 2001)
    PARAM.stdp_t_neg = 34e-3; %(source: Bi & Poo 2001)
    PARAM.stdp_a_pos = 1e-2; % avoid > 1e-2 (Tim, Jan 2011)
    PARAM.stdp_a_neg = -  0.85 * PARAM.stdp_a_pos; % 
    PARAM.stdp_cut = 7;
    PARAM.minWeight = 0.0;

    %***************
    %* EPSP Kernel *
    %***************
    PARAM.tm = 20e-3; % membrane time constant (typically 10-30ms)
    PARAM.ts = PARAM.tm/4; % synapse time constant
    PARAM.epspCut = 5;% specifies after how many ms we neglect the epsp
    PARAM.tmpResolution = 1e-3;
    
    PARAM.memristor = false;
    PARAM.t_op = .0005; 
    PARAM.t_on = .0005;        
    if PARAM.memristor     % Bernabe Linares: EPSP suitable for STDP with memristors:
        tn = 3e-3;
        tp = 40e-3;
        t1 = 5e-3;
        t2 = 75e-3;
        An = 1;
        Ap = An/4;
        current = [ -An/(exp(t1/tn)-1)*(exp([PARAM.tmpResolution:PARAM.tmpResolution:t1]/tn)-1) Ap/(exp(t2/tp)-1)*(exp((t1+t2-[t1+PARAM.tmpResolution:PARAM.tmpResolution:t1+t2])/tp)-1) zeros(1,(PARAM.epspCut*PARAM.tm)/PARAM.tmpResolution) ];
    %     figure; plot(PARAM.tmpResolution*(1:length(current)),current)    
        f = exp(-[0:PARAM.tmpResolution:(t1+PARAM.epspCut*PARAM.tm)]/PARAM.tm); % filter it (~LIF)
        PARAM.epspKernel=filter(f,sum(f),current);
        PARAM.refractoryPeriod = t1+t2;     
    else   
        % Double exp (Gerstner 2002)
        PARAM.epspKernel = pspKernel(0:PARAM.tmpResolution:PARAM.epspCut*PARAM.tm,PARAM.ts,PARAM.tm);
        PARAM.refractoryPeriod = 3e-3;
    end
    
    [m idx] = max(PARAM.epspKernel);
    PARAM.epspKernel = PARAM.epspKernel/m;
    %     figure; plot(PARAM.tmpResolution*(1:length(PARAM.epspKernel)),PARAM.epspKernel)
    PARAM.epspMaxTime = (idx-1)*PARAM.tmpResolution;
    
    % post synaptic spike kernel
    PARAM.usePssKernel = false;
    PARAM.pssKernel = [];
%     % time constant: tm
%     PARAM.pssKernel =   0*pspKernel(0:PARAM.tmpResolution:PARAM.epspCut*PARAM.tm,PARAM.ts/10,PARAM.tm/10) ...
%                     -   3*pspKernel(0:PARAM.tmpResolution:PARAM.epspCut*PARAM.tm,PARAM.ts,PARAM.tm) ...
%                     +   2*exp(-[0:PARAM.tmpResolution:PARAM.epspCut*PARAM.tm]/PARAM.tm);
%     % time constant: tm/2
%     PARAM.pssKernel =   -3*pspKernel(0:PARAM.tmpResolution:PARAM.epspCut*PARAM.tm/2,PARAM.ts/2,PARAM.tm/2) ...
%                     +   2*exp(-[0:PARAM.tmpResolution:PARAM.epspCut*PARAM.tm/2]/(PARAM.tm/2));
%     PARAM.pssKernel =   0*PARAM.epspKernel;

    % inhibitory postsynaptic potential (positive by convention, scaled so that max is 1)
    PARAM.ipspKernel = pspKernel(0:PARAM.tmpResolution:PARAM.epspCut*PARAM.tm,2*PARAM.ts,PARAM.tm);
    PARAM.ipspKernel = PARAM.ipspKernel / (max(PARAM.ipspKernel));
    PARAM.inhibStrength = 0.5; % inhibition strength (in fraction of threshold)
    
%     % Simple exp
%     PARAM.epspKernel = exp(-[0:PARAM.tmpResolution:PARAM.epspCut*PARAM.tm]/PARAM.tm);
%     PARAM.epspMaxTime = 0;
%     disp(['Neglecting EPSP when below ' num2str(PARAM.epspKernel(end))])

    % figure;plot(0:PARAM.tmpResolution:PARAM.epspCut*PARAM.tm,PARAM.epspKernel);
    % return


%     %***************
%     %* Spike Train *
%     %***************
%     PARAM.nPattern = 3;
    PARAM.nAfferent = 2*11^2;
%     PARAM.oscillations = false;
%     PARAM.nCopyPasteAfferent = round( .5 * PARAM.nAfferent );
%     PARAM.dt = 1e-3;
%     PARAM.maxFiringRate = 90;
%     PARAM.spontaneousActivity = 10;
%     PARAM.copyPasteDuration = 50e-3;
%     PARAM.jitter=1e-3;
%     PARAM.spikeDeletion=.0;
%     PARAM.maxTimeWithoutSpike = PARAM.copyPasteDuration;
%     PARAM.patternFreq = 1/3;
%     for idx = 1:PARAM.nPattern
%         PARAM.posCopyPaste{idx} = [];
%     end
%     PARAM.T = PARAM.nPattern*(500/PARAM.patternFreq)*PARAM.copyPasteDuration;
%     if PARAM.patternFreq>0
%         rand('state',PARAM.randomState);
%         skip = false;
%         for p = 1:round( PARAM.T / PARAM.copyPasteDuration )
%             if skip
%                 skip = false;
%             else
%                 if rand<1/(1/PARAM.patternFreq-1)
%                     idx = ceil(rand * PARAM.nPattern);
%                     PARAM.posCopyPaste{idx} = [PARAM.posCopyPaste{idx} p];
%                     skip = true; % skip next
%                 end
%             end
%         end
%     end
% %     PARAM.nCopyPaste = length(PARAM.posCopyPaste);
    
    %**********
    %* Neuron *
    %**********
    % The threshold corresponds roughly to the number of coincindent spikes we want to detect
    % Should be as big as possible (avoids FA), avoiding to get "stuck" in low
    % spike density zone of the copy-pasted pattern.
    % Appropriate value depends on number of afferent. Eg around 125 for 500
    % afferents.
    % Then initial weights should be tuned so as to reach threshold.
    
    PARAM.nNeuron = 2^5;
%     PARAM.threshold = 65;
% 	ratio=900/2000;
%    PARAM.threshold = ratio*PARAM.nCopyPasteAfferent;
%     PARAM.threshold = Inf;
    PARAM.threshold = 30;
%     PARAM.threshold = .55*(1-PARAM.spikeDeletion)*PARAM.nCopyPasteAfferent;
%    PARAM.nuThr = round( 2.0 * PARAM.maxFiringRate/2*PARAM.nAfferent*PARAM.tm); % trace parameter for thr computation use inf not to use
    PARAM.nuThr = Inf;
%     if PARAM.goOn && PARAM.threshold==Inf
%         warning('"Going on" in infinite threshold mode has no sense. Setting goOn = false');
%         PARAM.goOn = false;
%     end
    %****************
    %* Neural codes *
    %****************
%     PARAM.realValuedPattern = false;
%     PARAM.codingMethod = 4; % 1 for poisson, 2 for LIF, 3 for intensity to phase, 4 for LIF with oscillatory drive
%     PARAM.gammaFreq = 50; % freq of oscillatory drive
%     PARAM.oscillMagnitude = 1;% magnitude of oscillatory drive
%     PARAM.oscillPhase = rand*2*pi;% phase of oscillatory drive
%     PARAM.resetPeriod = Inf;%100e-3;
%     PARAM.resetStd = 25e-3;
%     if PARAM.resetPeriod<Inf
%         if PARAM.resetStd==0
%             PARAM.resetTimes = PARAM.resetPeriod * [1:ceil(PARAM.T/PARAM.resetPeriod)];
%         else
%             PARAM.resetTimes = cumsum( PARAM.resetStd * randn(1,round(1.1*PARAM.T/PARAM.resetPeriod)) + PARAM.resetPeriod );
%         end
%     else
%         PARAM.resetTimes = [];
%     end
%     % LIF model for afferents
%     PARAM.R = 1;
%     PARAM.Vthr = 1.72;
%     PARAM.Vreset = 0;
%     PARAM.Vrest = 0;
% 	PARAM.Imin = 0;
% 	PARAM.Imax = 0.8;
%     
%     PARAM.interStimuliInterval = 1 / 16;
%     PARAM.interPatternInterval = 1 / 2;

    
    %*************
    %* Reporting *
    %*************
%     PARAM.plottingPeriod = [ [0 1]  [5 6] PARAM.nRun*PARAM.T+[-1 0] ];
%     PARAM.plottingPeriod = [ [-1 -1] [-1 -1] [-1 -1] ];
end
