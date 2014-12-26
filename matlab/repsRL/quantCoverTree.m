function [qX, qv] = quantCoverTree(X, startX, maxDepth, maxgN)
global gN gvm gpar gchild ginfreg gvreg gdepth;

N = size(X, 1);
K = size(X, 2);

% initialization
if (startX == 1)
  gN = 0;
  gvm = [];
  gpar = [];
  gchild = cell(0, maxDepth + 1);
  ginfreg = zeros(0, maxDepth + 1);
  gvreg = zeros(0, maxDepth + 1);
  gdepth = [];
end

% quantization
fprintf('Cover tree quantization');
for t = startX : N
  updateCoverTree(X(t, :), maxDepth, X);
  if (mod(t, 1000) == 0)
    fprintf('.');
  end
end
fprintf('\n');

% find the first level with at least maxgN representative vertices
depth = 0;
while ((depth < maxDepth) && (sum(gdepth(1 : gN) <= depth) <= maxgN))
  depth = depth + 1;
end
v = find(gdepth(1 : gN) <= depth);

% trim the number of representative vertices to maxgN
selu = find(gdepth(v) < depth);
sell = find(gdepth(v) == depth);
nu = length(selu);
nl = length(sell);

solver = 'greedy';
switch (solver)
  case 'greedy'
    v = v([selu, sell]);
    v = v(1 : maxgN);
    
%   case 'maxnorm'
%     % map examples to representative vertices at depth - 1
%     ud = Inf(1, gN);
%     uv = zeros(1, gN);
%     for i = selu
%       [vi, di] = getCoverTreeNN(X(gv(v(i)), :), 1 / 2 ^ (depth - 2), ...
%         maxDepth, X);
%       di = di * 2 ^ (depth - 2);
%       sel = di < ud(vi);
%       ud(vi(sel)) = di(sel);
%       uv(vi(sel)) = i;
%     end
%     
%     % map examples to representative vertices at depth
%     ld = Inf(1, gN);
%     lv = zeros(1, gN);
%     for i = 1 : nu + nl
%       [vi, di] = getCoverTreeNN(X(gv(v(i)), :), 1 / 2 ^ (depth - 1), ...
%         maxDepth, X);
%       di = di * 2 ^ (depth - 2);
%       sel = di < ld(vi);
%       ld(vi(sel)) = di(sel);
%       lv(vi(sel)) = i;
%     end
%     
%     % sort examples by their distance at depth - 1
%     [~, ndx] = sort(ud, 'descend');
%     
%     % cover the examples
%     active = false(1, nu + nl);
%     active(selu) = true;
%     i = 1;
%     while ((sum(active) < maxgN) && (i <= length(ndx)))
%       active(lv(ndx(i))) = true;
%       i = i + 1;
%     end
%     v = v(active);
end

qv = v;
qX = X(qv, :);

% printout
fprintf('%i representive vertices at depth %i (out of %i).\n', ...
  length(selu), depth - 1, length(qv));

% % printout
% d = min(pdistc(qX, X, 2), [], 1);
% errLInf = max(d);
% errL1 = mean(d);
% 
% fprintf('\n');
% fprintf('Number of representative vertices is %i (depth = %i).\n', ...
%   length(qv), depth);
% fprintf('Worst-case quantization error is %.6f.\n', errLInf);
% fprintf('Average quantization error is %.6f.\n', errL1);
% fprintf('\n');
