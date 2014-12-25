function nnShowWeights(w1,w2,w3,max_m,min_m)
% nnShowWeights(wh1,wh2,wo,max_m,min_m);
%    Plots a hinton diagram of the weight and output matricies.
%
% Input: w1,w2,w3  are up to three layers of weights.
%        w2 and w3 can be [].
%        max_m  largest box corresponds to this magnitude
%        min_m  smallest box (point) corresponds to this magnitude
%
%       See also NNPLOT, HINTONW.
%
%nnShowWeights(wo);
%nnShowWeights(wh,wo);
%nnShowWeights(wh1,wh2,wo);
%nnShowWeights(wh1,wh2,wo,wmax,wmin);
%  Copyright (c) 1996 by Charles W. Anderson

max_m = [];  min_m = [];  
if nargin == 3
  if isempty(w2) & isempty(w3)
    %one layer, max and min given
    wo = w1; wh1 = []; wh2 = []; numLayers = 1;
    max_m = w2;  min_m = w3;
  else
    wh1 = w1; wh2 = w2; wo = w3; numLayers = 3;
  end
elseif nargin == 1     %one layer
  wo = w1;  wh1 = []; wh2 = []; numLayers = 1;
elseif nargin == 2     %Two layer net
  wh1 = w1; wh2 = []; wo = w2; numLayers = 2;
else                   % Three layer net  
  wh1 = w1; wh2 = w2; wo = w3; numLayers = 3;
end

[ih1,uh1] = size(wh1);
[ih2,uh2] = size(wh2);
[io,uo] = size(wo);

%How many weights wide and high is whole figure? Leave one weight space
%between layers.  No text for now.
if numLayers == 1
  nwide = uo;
  nhigh = io+1;
elseif numLayers == 2
  nwide = uh1 + 1;
  nhigh = ih1 + 1 + uo + 1;
else
  nwide = uh1 + 1 + uo + 1;
  nhigh = ih1 + 1 + uh2 + 1;
end

%Set aspect ratio to get square weights.
figPos = get(gcf,'position');
if nhigh/nwide > 1
	figPos(3) = figPos(4) * nwide/nhigh;
else
	figPos(4) = figPos(3) * nhigh/nwide;
end


set(gcf,'position',figPos,...
    'paperposition',[0.25 0.25 8*min(1,nwide/nhigh) 10*min(1,nhigh/nwide)] )



figw = 0.85;
figh = 0.9;
boxw = figw / nwide;
boxh = (figh-0.1) / nhigh;


if numLayers == 1
  x1 = 0.15; y1 = 0.1; dx1 = figw; dy1 = figh;
  axis1 = axes('position',[x1 y1 dx1 dy1]);
  hold on;
  set(gca,'xlim',[0 uo]+0.5);  set(gca,'ylim',[0 io]+0.5);
  set(gca,'xlimmode','manual');  set(gca,'ylimmode','manual');
  xticks = get(gca,'xtick');
  set(gca,'xtick',xticks(find(xticks == floor(xticks))))
  yticks = get(gca,'ytick');
  set(gca,'ytick',yticks(find(yticks == floor(yticks))))
  set(gca,'ydir','reverse');

  if isempty(max_m)
    maxl = max(max(abs(wo)));
  else
    maxl = max_m;
  end
  if isempty(min_m)
    minl = 0.;
  else
    minl = min_m;
  end
  
  nnDrawBoxes(wo,maxl,minl)

  xlabel('Output Units'); ylabel('Inputs');

elseif numLayers == 2

  dx1 = boxw * uh1;  dy1 = boxh * ih1;
  dx2 = boxw * io;   dy2 = boxh * uo ;

  x1 = 0.15; y1 = 0.1 + dy2 + 0.1;
  x2 = 0.15; y2 = 0.1;

  axes('position',[x1 y1 dx1 dy1]);
  hold on;
  set(gca,'xlim',[0 uh1]+0.5);  set(gca,'ylim',[0 ih1]+0.5);
  set(gca,'xlimmode','manual');  set(gca,'ylimmode','manual');
  xticks = get(gca,'xtick');
  set(gca,'xtick',xticks(find(xticks == floor(xticks))))
  yticks = get(gca,'ytick');
  set(gca,'ytick',yticks(find(yticks == floor(yticks))))
  set(gca,'ydir','reverse');
  xlabel('Hidden Units');  ylabel('Inputs');

  if isempty(max_m)
    maxl = max(max(abs(wh1)));
  else
    maxl = max_m;
  end
  if isempty(min_m)
    minl = 0.;
  else
    minl = min_m;
  end

  nnDrawBoxes(wh1,maxl,minl)

  axes('position',[x2 y2 dx2 dy2]);
  hold on;
  set(gca,'xlim',[0 io]+0.5);  set(gca,'ylim',[0 uo]+0.5);
  set(gca,'xlimmode','manual');  set(gca,'ylimmode','manual');
  xticks = get(gca,'xtick');
  set(gca,'xtick',xticks(find(xticks == floor(xticks))))
  yticks = get(gca,'ytick');
  set(gca,'ytick',yticks(find(yticks == floor(yticks))))
  set(gca,'ydir','reverse');
  xlabel('Hidden Units');  ylabel('Output Units');

  if isempty(max_m)
    maxl = max(max(abs(wo)));
  else
    maxl = max_m;
  end
  if isempty(min_m)
    minl = 0.;
  else
    minl = min_m;
  end
  
  nnDrawBoxes(wo',maxl,minl)

elseif numLayers == 3

  dx1 = boxw * uh1;  dy1 = boxh * ih1;
  dx2 = boxw * ih2;   dy2 = boxh * uh2;
  dx3 = boxw * uo;   dy3 = boxh * io;

  x1 = 0.15; y1 = 0.1 + dy3 + boxh;
  x2 = 0.15; y2 = 0.1;
  x3 = 0.15 + dx2; y3 = 0.1;
  
  axes('position',[x1 y1 dx1 dy1]);
  hold on;
  set(gca,'xlim',[0 uh1]+0.5);  set(gca,'ylim',[0 ih1]+0.5);
  set(gca,'xlimmode','manual');  set(gca,'ylimmode','manual');
  xticks = get(gca,'xtick');
  set(gca,'xtick',xticks(find(xticks == floor(xticks))))
  yticks = get(gca,'ytick');
  set(gca,'ytick',yticks(find(yticks == floor(yticks))))
  set(gca,'ydir','reverse');
  xlabel('Hidden Units');  ylabel('Inputs');

  if isempty(max_m)
    maxl = max(max(abs(wh1)));
  else
    maxl = max_m;
  end
  if isempty(min_m)
    minl = 0.;
  else
    minl = min_m;
  end
  
  nnDrawBoxes(wh1,maxl,minl)

  axes('position',[x2 y2 dx2 dy2]);
  hold on;
  set(gca,'xlim',[0 ih2]+0.5);  set(gca,'ylim',[0 uh2]+0.5);
  set(gca,'xlimmode','manual');  set(gca,'ylimmode','manual');
  xticks = get(gca,'xtick');
  set(gca,'xtick',xticks(find(xticks == floor(xticks))))
  yticks = get(gca,'ytick');
  set(gca,'ytick',yticks(find(yticks == floor(yticks))))
  set(gca,'ydir','reverse');
  xlabel('Hidden Units');  ylabel('Hidden Units 2');
  if isempty(max_m)
    maxl = max(max(abs(wh2)));
  else
    maxl = max_m;
  end
  if isempty(min_m)
    minl = 0.;
  else
    minl = min_m;
  end
  
  nnDrawBoxes(wh2',maxl,minl)

  axes('position',[x3 y3 dx3 dy3]);
  hold on;
  set(gca,'xlim',[0 uo]+0.5);  set(gca,'ylim',[0 io]+0.5);
  set(gca,'xlimmode','manual');  set(gca,'ylimmode','manual');
  xticks = get(gca,'xtick');
  set(gca,'xtick',xticks(find(xticks == floor(xticks))))
  yticks = get(gca,'ytick');
  set(gca,'ytick',yticks(find(yticks == floor(yticks))))
  set(gca,'ydir','reverse');
  xlabel('Output Units');  ylabel('Hidden Units 2');
  if isempty(max_m)
    maxl = max(max(abs(wo)));
  else
    maxl = max_m;
  end
  if isempty(min_m)
    minl = 0.;
  else
    minl = min_m;
  end
  
  nnDrawBoxes(wo,maxl,minl)

end

