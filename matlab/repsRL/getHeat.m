function heat = getHeat(maxgN)
global gN gdepth;

% find the first level with at least maxgN representative vertices
depth = 0;
while (sum(gdepth(1 : gN) <= depth) <= maxgN)
  depth = depth + 1;
end
v = find(gdepth(1 : gN) <= depth);

selu = find(gdepth(v) < depth);
selb = find(gdepth(v) == depth);
nu = length(selu);
nb = length(selb);

% % heat parameter
% d = (log(nu + nb) - log(nu)) / log(2);
% logA = log(nu + nb) + d * log(1 / (2 ^ (depth + 1)));
% heat = exp((logA - log(maxgN)) / d);
% 
% % printout
% fprintf('Space dimensionality:  %.3f\n', d);

% heat parameter
eps = 1 / (2 ^ (depth - 1));
heat = eps * (0.5 * (maxgN - nu) + (nu + nb - maxgN)) / nb;
