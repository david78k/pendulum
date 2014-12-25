function nnPlotOutsScat(testouts)
%nnPlotsOutsScat([targ1 out1; targ2 out2; targ2 out2; ...])
%  Copyright (c) 1996 by Charles W. Anderson

hold on
numout = size(testouts,2)/2; 
minx = 1.;
maxx = 0.;
miny = 1.;
maxy = 0.;
%This loop over numout doesn't make sense!  Works for numout=1.
for i=1:numout
  x = testouts(:,i);
  y = testouts(:,numout+i);
  plot(x,y,'c*')
end
minx = min(x);
maxx = max(x);
miny = min(y);
maxy = max(y);
first = max([minx miny]);
last = min([maxx maxy]);
plot([first last], [first last],'y--');

xlabel('Target')
ylabel('Output')
title('Pattern Scatter Plot')
x_limit = get(gca,'xlim');
offset = (x_limit(2) - x_limit(1))/10;
x_limit = [(x_limit(1) - offset) (x_limit(2) + offset)];
set(gca,'xlim',x_limit);

hold off
