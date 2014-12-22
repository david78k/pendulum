% This code was used in:
% Masquelier T (2012) Relative spike time coding and STDP-based orientation
% selectivity in the early visual system in natural continuous and saccadic
% vision: a computational model. J Comput Neurosc.
% timothee.masquelier@alum.mit.edu
% The current parameter values correspond the baseline simulation in the
% paper.
%
% This script loads the spikes.spk text file produced by Virtual Retina,
% does multiple n x n crops (shifted by delta, thus if delta<n, there is
% overlap), and stores them in files ../mat/afferent.rand###.###.###.mat.
% This is a way to get more training data for STDP-based learning.

randState = 300; %seed for random generator (if any) and ref number for the computation
n = uint16(11); % crop size
delta = uint16(n);% crop shift

% %--------------------------------------
timedLogLn('concatSL')

timedLog('Loading spikes...')

load('../mat/spikes.spk')
spikeList = spikes;
clear spikes;

% 
% % spikeList_ = load('../spikes.ref.spk');
% spikeList_ = load('../paper/no-noise/spikes.spk');
% afferentList_ = uint32(spikeList_(:,1));
% spikeList_(:,1) = [];
% 
% % save('../paper/spikes.mat','spikeList_','afferentList_')
% 
% % load('../paper/spikeList.mat');
afferentList_ = uint32(spikeList(:,1));
spikeList_ = spikeList(:,2);
clear spikeList
% 
% %load('../paper/spikes.mat')
% 
% timedLog('Done...')
% %--------------------------------------


N = sqrt((double(max(afferentList_))+1)/2);
disp(['Estimated N = ' num2str(N)])
disp(['Avg firing rate = ' num2str(size(spikeList_,1)/N/N/2/(spikeList_(end)-spikeList_(1))) ' Hz'])

% n = N;


afferentList_ = double(afferentList_);
i=uint16(mod(afferentList_,N));
j=uint16(mod(floor(afferentList_/N),N));
k=afferentList_ >= N^2;
clear afferentList_ % not useful anymore


K = floor((N-double(n))/delta);

IJ = uint16(zeros(2,(K+1)^2));
IJ(1,:)=uint16(floor( (0:size(IJ,2)-1)/double(K+1) ));
IJ(2,:)=uint16(mod( (0:size(IJ,2)-1), double(K+1) ));

% % use this if you want to shuffle the crops
% rand('state',randState);
% IJ = IJ(:,randperm((K+1)^2));

for r=1:size(IJ,2)

    timedLog(['RF = [' int2str(IJ(1,r)*delta) ',' int2str(IJ(1,r)*delta+n) '[ , [' int2str(IJ(2,r)*delta) ',' int2str(IJ(2,r)*delta+n) '['])
    % crop
    idx = i>=IJ(1,r)*delta & i<IJ(1,r)*delta+n & j>=IJ(2,r)*delta & j<IJ(2,r)*delta+n;
    spikeList = spikeList_(idx);
    
    % new ref
    afferentList = (i(idx)-IJ(1,r)*delta) + n*(j(idx)-IJ(2,r)*delta) + n^2 * uint16(k(idx));

    %         return
    
    fileName = ['afferent.rand' sprintf('%03d',randState) '.' sprintf('%03d',IJ(1,r)) '.' sprintf('%03d',IJ(2,r)) '.mat'];
    disp(['Saving in ../mat/' fileName])
    save(['../mat/' fileName],'spikeList','afferentList')
end
