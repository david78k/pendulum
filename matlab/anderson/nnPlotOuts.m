function nnPlotOuts(testouts,corr)
%nnPlotOuts([targ1 out1; targ2 out2; ...],80)   80 is percent correct.
%  Copyright (c) 1996 by Charles W. Anderson

hold on
if size(testouts,2) == 2
  % Can plot target and actual as a 2 line graph
  plot(testouts(:,1),'y--')
  plot(testouts(:,2),'c-')
  title(['Target = --  Output = Solid  ' num2str(corr*100) ' Percent Correct'])
else
  % More than one output.  Can't draw.  Just count.
  numout = size(testouts,2)/2;
  targs = testouts(:,1:numout);
  outs = testouts(:,numout+1:numout*2);
  trueHigh = sum(sum(targs>=0.5 & outs>=0.5));
  trueLow =  sum(sum(targs<0.5 & outs<0.5));
  falseLow =  sum(sum(targs>=0.5 & outs<0.5));
  falseHigh =  sum(sum(targs<0.5 & outs>=0.5));
  s = sprintf(...
      ['       High %5d%5d\n' ...
       'Network\n' ...
       '       Low  %5d %5d\n\n' ...
       '             Low  High\n' ...
       '              Target'], falseHigh,trueHigh,trueLow,falseLow);
 disp('Confusion matrix:'),disp(s)
 
 %Now draw bar graph of counts
 denHigh = sum(targs>=0.5); denHigh(find(denHigh==0.)) = inf*ones(1,sum(denHigh==0));
 denLow = sum(targs<0.5); denLow(find(denLow==0.)) = inf*ones(1,sum(denLow==0));

 trueHighs = sum(targs>=0.5 & outs>=0.5) ; % ./ denHigh * 100;
 trueLows =  sum(targs<0.5 & outs<0.5) ; % ./ denLow * 100;
 falseLows =  sum(targs>=0.5 & outs<0.5) ; %./ denHigh * 100;
 falseHighs =  sum(targs<0.5 & outs>=0.5) ; %./ denLow * 100;

 subplot(3,2,3);
 stackedbar([trueHighs; trueHighs+falseLows]);
 title('High Targets'); 
 set(gca,'XLim',[1 numout]);
 %axis([1 55 0 100]);
 subplot(3,2,4);  
 stackedbar([trueLows; trueLows+falseHighs]);
 title('Low Targets'); 
 set(gca,'XLim',[1 numout]);
 %axis([1 55 0 100]);
 
% subplot(3,4,5); bar(trueHighs); title('True Highs'); axis([1 55 0 100]);
% subplot(3,4,6); bar(trueLows);  title('True Lows');axis([1 55 0 100]);
% subplot(3,4,7); bar(falseHighs); title('False Highs');axis([1 55 0 100]);
% subplot(3,4,8); bar(falseLows);  title('False Lows');axis([1 55 0 100]);

% counts = [trueHighs; trueLows; falseHighs; falseLows; zeros(1,numout)];
% counts = counts(:);
% bar(counts);
end


       
hold off
