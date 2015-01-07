function answer = nnRuns(runs,parms,values,bestparms,good)
%result = nnRuns(runs,parms,values,bestparms,good)
% runs  is matrix with one row per run with columns of parameters followed
%   by values.
% parms is vector of column indices to summarize by
% values is vector of column indices to collect for each run
% bestparms  If present, only return runs with best average value over values
%            of this parameter.
% good   is 'max' or 'min'
%
% Returns rows of best runs, columns are parameters, mean, then samples.
%
% Example:  Each line in runs has
%   20. 10. 0.5000  0.0500 0  0.0208 57.0000 0.1680    0.1668    0.9420
% Hid1 Hid2  Hidr   Outr  mom  trE    ep      valE       testE     testFrac
%
% r = nnRuns(runs,[1 2 3 4],[10],[1 2],'max')
%
%     to find best learning rates for each net arch.
%
%  Copyright (c) 1996 by Charles W. Anderson

result = [];

v = runs(:,values);  %matrix of values
p = runs(:,parms);  %matrix of parms

while ~ isempty(v)
 %row mask for all equal to first row
 mask = all((p == ones(size(p,1),1)*p(1,:))')';

 new = v(mask,:)';
 num = length(new);
 currentNum = size(result,2)-length(parms);

 if num > currentNum
   result = [result NaN*ones(size(result,1),num-currentNum)];
 elseif num < currentNum
   new = [new NaN*ones(currentNum-num)];
 end

 result = [result; p(1,:) new];

 v = v(~mask,:);
 p = p(~mask,:);
end

vs = length(parms)+1:size(result,2);
result = [result(:,1:length(parms)) meanNoNaN(result(:,vs)')' result(:,vs)];

answer = result;

% Now find best ones across value of bestparms values.

if exist('bestparms') == 1

 if exist('good') == 0
   good = 'min';
 end
 bestOnes = [];
 %bestparms might be more than one
 [bpi,junk] = find((ones(length(parms),1)*bestparms)...
     == (parms'*ones(1,length(bestparms))));

 while ~ isempty(result)

  %row mask for all rows with bestparms equal to one in first row
  mask = (result(:,bpi) == ones(size(result,1),1)*result(1,bpi));
  mask = all(mask')';
  sub = result(mask,:);
  if findstr(good,'max')
   [best,i] = max(sub(:,length(parms)+1));
  else
   [best,i] = min(sub(:,length(parms)+1));
  end

  bestOnes = [bestOnes;  sub(i,:)];

  result = result(~mask,:);
 end

 answer = bestOnes;
 
 
 % If answer is all positive, negation will sort in descending order.
 
 [junk,order] = sort(-answer(:,length(parms)+1));
 answer = answer(order,:);
 
end


