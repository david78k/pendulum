function nnDrawBoxes(data,max_m,min_m)
%nnDrawBoxes(data)   or   nnDrawBoxes(data,10,0)  (max and min abs values)
%  Copyright (c) 1996 by Charles W. Anderson

global nnColor

if nargin == 1
  mags = abs(data);
  mags = mags ./ max(max(mags));
else
  mags = (abs(data)-min_m)/max_m;
end

% Vectors of x centers and y centers as we go from top left down.

xcenters = ones(size(data,1),1) * [1:size(data,2)];
ycenters = [1:size(data,1)]' * ones(1,size(data,2));

% Duplicate into four more rows to be five corners of each box.

xcorners = ones(5,1) * xcenters(:)';
ycorners = ones(5,1) * ycenters(:)';

% Widths of each box determined by data magnitude. Use to set increment from
% center of each box to get corners.

magsVector = mags(:)';
xincs = 0.5 * [-1;1;1;-1;-1] * magsVector;
yincs = 0.5 * [-1;-1;1;1;-1] * magsVector;

% Add these incs to the x and y coordinates

xcorners = xcorners + xincs;
ycorners = ycorners + yincs;

% Do positive and negative ones indendently.

posMask = data(:)' >= 0;

if exist('nnColor') ~= 1
  nnColor = 0;
end
if nnColor
  fill(xcorners(:,posMask),ycorners(:,posMask),'g');
  hold on
  fill(xcorners(:,~posMask),ycorners(:,~posMask),'r');
else
  fill(xcorners(:,posMask),ycorners(:,posMask),'w');
  hold on
  plot(xcorners(:,~posMask),ycorners(:,~posMask),'w');
end

hold off
end

