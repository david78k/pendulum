% This code was used in:
% Masquelier T (2012) Relative spike time coding and STDP-based orientation
% selectivity in the early visual system in natural continuous and saccadic
% vision: a computational model. J Comput Neurosc.
% timothee.masquelier@alum.mit.edu
%
% This code simulates the V1 stage.
% The scripts successively loads all the afferent.rand###.###.###.mat files (corresponding to LGN output), and each time launches the STDPContinuous.c mex file (see comments in its header).
% All the parameters are gathered in paramV1.m

if ~exist('PARAM','var')
    global PARAM
end

paramV1

rand('state',PARAM.randomState);

timedLogLn('V1')
timedLog(['RANDOM STATE = ' int2str(PARAM.randomState) ]);

% if ~PARAM.goOn
%     % generate spike train
%     %     rand('state',PARAM.randomState);
%     %     randn('state',PARAM.randomState);
%     if exist(['../mat/afferent.rand' sprintf('%03d',PARAM.randomState) '.mat'],'file')
%         load(['../mat/afferent.rand' sprintf('%03d',PARAM.randomState) '.mat'])
%         %         if ~exist('spikeList','var') % 'false' file
%         %             [spikeList afferentList] = generateSpikeTrain;
%         %             save(['../mat/afferent.rand' sprintf('%03d',PARAM.randomState) '.mat'],'spikeList','afferentList')
%         %         end
%     else
%         timedLogLn('Generating spike train...');
%         if PARAM.realValuedPattern
%             [spikeList afferentList patternPeriod, values, times] = generateSpikeTrainWithRealValuedPattern2;
%             save(['../mat/afferent.rand' sprintf('%03d',PARAM.randomState) '.mat'],'spikeList','afferentList','patternPeriod','values','times')
%         else
%             [spikeList afferentList] = generateSpikeTrain;
%             save('-v7.3',['../mat/afferent.rand' sprintf('%03d',PARAM.randomState) '.mat'],'spikeList','afferentList')
%         end
%     end
% end


filePath = '../mat/';
% filePath = '../../../data/';
fileList = dir([filePath 'afferent.rand' sprintf('%03d',PARAM.randomState) '.*.*.mat']);
disp([int2str(length(fileList)) ' files found']);

for f=1:length(fileList)

    load([filePath fileList(f).name])
    
    if isempty(spikeList)
        continue
    end

    % init neuron
    N = round(2^6.5*length(PARAM.epspKernel)*PARAM.tmpResolution*length(spikeList)/spikeList(end));
    for nn=1:PARAM.nNeuron %neuron loop
        neuron(nn) = createNewNeuron(PARAM,N);
    end %neuron loop

    if PARAM.useSavedWeight
        dirlist = dir([filePath 'weight.t=*s.txt']);
        % dirlist = dir(['../mat/sav.weight.24.txt']);
        % dirlist = dir(['../mat/sav.weight.t=0000080.479s.txt']);
        if ~isempty(dirlist)
            disp(['Used weights saved in ' filePath dirlist(end).name ])
            weight = load([filePath dirlist(end).name]);
            for nn=1:PARAM.nNeuron
                neuron(nn).weight = weight(nn,:);
            end
        end
        delete([filePath 'weight*.txt'])
    end
    % spikeList(1e4+1:end) =[];
    % afferentList(1e4+1:end) =[];



    % PARAM.threshold = Inf;
    % stop = 0;
    for r=1:PARAM.nRun
        tic

        n=n+1;

        timedLogLn(['Run ' int2str(n) ' (' int2str(length(spikeList)) ' iterations ~ ' int2str(2e-9*length(spikeList)*length(neuron)*(ceil( PARAM.epspMaxTime / PARAM.tmpResolution )+1)) ' min )'])

        %     t=(n-1)*PARAM.T; % simulation time
%         if n>1
%             spikeList = spikeList+PARAM.T;% shift spike list
% 
%             maxLastFiring = 0;
%             for nn=1:length(neuron)
%                 if neuron(nn).nFiring>0 && neuron(nn).firingTime(neuron(nn).nFiring)>maxLastFiring
%                     maxLastFiring = neuron(nn).firingTime(neuron(nn).nFiring);
%                 end
%             end
%             spikeList = max(spikeList,maxLastFiring); % this is to avoid inserting epsp before last firing
%             %         for n=1:length(neuron) %neuron loop
%             %             neuron(n).nextFiring = Inf;
%             %         end
%             for nn=1:length(neuron) % make sure epsp are chronologic
%                 neuron(nn).nEpsp=0;
%                 neuron(nn).maxPotential=0;
%                 if PARAM.fixedFiringMode
%                     neuron(nn).nextFiring = (n-1)*PARAM.T + PARAM.fixedFiringLatency;
%                 else
%                     neuron(nn).nextFiring = Inf;
%                 end
%             end
%         end

        if PARAM.dump


            %         % beginning
            %         nSpike = round(1/3/PARAM.T*length(spikeList));
            %         delete('dump.txt');
            %         neuron=STDPContinuous(neuron,spikeList(1:nSpike),afferentList(1:nSpike)-1,true,false,PARAM);
            %         copyfile('dump.txt','dump.beginning.txt')
            %         break % no need to go further

            % end
            if r==PARAM.nRun
                nSpike = round((PARAM.T-1)/PARAM.T*length(spikeList));
                neuron=STDPContinuous(neuron,spikeList(1:nSpike),afferentList(1:nSpike)-1,false,PARAM.beSmart,PARAM);
                delete('dump.txt');
                neuron=STDPContinuous(neuron,spikeList(nSpike+1:end),afferentList(nSpike+1:end)-1,true,false,PARAM);
                copyfile('dump.txt','dump.end.txt')
            else
                neuron=STDPContinuous(neuron,spikeList,afferentList-1,false,PARAM.beSmart,PARAM); % C indexes start at 0
            end

        else
%             neuron=STDPContinuous(neuron,spikeList',afferentList',false,PARAM.beSmart,PARAM); % C indexes start at 0
            neuron=STDPContinuous(neuron,spikeList,afferentList-1,false,PARAM.beSmart,PARAM); % C indexes start at 0
        end

        %     % add new 'virgin' neurons
        %     if r<PARAM.nRun-1
        %         for nn=1:PARAM.nNeuron %neuron loop
        %             neuron = [ neuron createNewNeuron(PARAM,N) ];
        %         end
        %     end

        disp(' ');
        toc

        if sum([neuron.nFiring]) == 0
            warning('Neurons do not fire')
            break;
        end

    end % run loop

    if max([neuron.nFiring])>length(neuron(1).firingTime)
        warning('Increase firingTime array size')
    end

    % % save
    % if PARAM.fixedFiringMode
    %     weight = neuron.weight;
    %     save('../mat/weight.mat','weight');
    % end

    % save all
    c = clock;
    timeTag = [sprintf('%02.0f',c(2)) '.' sprintf('%02.0f',c(3)) '.' sprintf('%02.0f',c(4)) '.' sprintf('%02.0f',c(5)) '.' sprintf('%02.0f',c(6)) ];

    % perf;
    % for nn=1:length(neuron)
    %     timeTag = [timeTag '.n' sprintf('%02.0f',nn) '.HR' sprintf('%03.0f',100*neuron(nn).HR) '.FA' sprintf('%03.2f',neuron(nn).FA)  ];
    % end

    % if stop
    %     timeTag = [timeTag '.S'];
    % end
    % timeTag = [timeTag '.nF' int2str(neuron.nFiring)];

    % % dispMultiPattern
    % multiPattern
    % writeLatencies


    % % tags useful for multi patterns
    % hr = -sort(-max(HR,[],2));
    % timeTag = [timeTag '.pat'];
    % for pat=1:PARAM.nPattern
    %     timeTag = [timeTag '.' sprintf('%03.0f',100*hr(pat)) ];
    % end
    % hr = -sort(-max(HR,[],1));
    % timeTag = [timeTag '.neur'];
    % for neur=1:length(neuron)
    %     timeTag = [timeTag '.' sprintf('%03.0f',100*hr(neur)) ];
    % end

    % tags useful for mono pattern
    % timeTag = [timeTag '.HR.' sprintf('%03.0f',100*HR) '.FA.' sprintf('%f',FA) ];

end

clear spikeList
clear afferentList
clear patternPeriod
clear values
clear times

disp(['Saving results in ' filePath 'matlab.rand' sprintf('%03d',PARAM.randomState) '.' timeTag '.mat']);
save([filePath 'matlab.rand' sprintf('%03d',PARAM.randomState) '.' timeTag '.mat']);
% disp(timeTag)
% save

if PARAM.fixedFiringMode
    weight = neuron.weight;
    save([filePath 'weight.mat'],'weight');
end

if PARAM.dump
    showPot
end

% dispMultiPattern

% displayResult
% perf

