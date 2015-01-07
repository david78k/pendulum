function [handles] = nnPlotRuns(results,nparms,axes)
%nnPlotRuns(nnRuns(results,parms,values),length(parms),axes)
% Plots runs.  Each row has parameter values, mean, then samples.
%  nparms is number of parameters before mean.
%  axes are limits for x and  y axes, as accepted by axis command
%
%       Copyright (c) 1996 by Charles W. Anderson

if nargin < 2,
 nparms = 4;
end

hold on

clf;
handles = [];
[num,ncols] = size(results);
for i=1:num
  handles = [handles subplot(num*2,1,i*2)];
  r = results(i,nparms+2:ncols);
  r = r(~isnan(r));
  hist(r);
  if exist('axes')==1
    axis(axes);
  end
  avg = results(i,nparms+1);
  limits = axis;
  h = line([avg;avg],limits(3:4)');
  set(h,'LineWidth',2,'Color',[0 1 1]);
  which = [];
  for n=results(i,[1:nparms])
    which = [which ' ' num2str(n)];
  end
  title(which);
end

%ns = size(results,2)-nparms-1;
% plot(results(i,1)*ones(ns),results(i,nparms+2:ns+nparms+1),'y*')
%end

%plot(results(:,1),results(:,nparms+1),'r');
