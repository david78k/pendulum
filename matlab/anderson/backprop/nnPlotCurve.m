function nnPlotCurve(bestep,errorCurve)
%plot_train :
%       plots the data returned in the errors matrix of
%       the train.m function.
%
%Usage:    plot_train(best_epoch,errorCurve)
%  Copyright (c) 1996 by Charles W. Anderson

hold on
plot(errorCurve(:,1),errorCurve(:,2),'w-')
plot(errorCurve(:,1),errorCurve(:,3),'c-.')
if any(errorCurve(:,4)>0), plot(errorCurve(:,1),errorCurve(:,4),'g--'), end;
%plot(errorCurve(:,1),errorCurve(:,5),'r:')
y_limit = get(gca,'ylim');
plot([bestep bestep],y_limit,'y:')
xlabel('Epochs')
title('Train = solid     Validate = -.    Test = --')
hold off
