function [P, R, G, Nza] = ...
  buildMDP(S, Z, a, r, absorb, numActions, heat, k0, epiend)

N = size(S, 1);
numStates = size(Z, 1);

% transition and reward models
P = zeros(numStates, numActions, numStates);
Px = cell(1, numActions);
for k = 1 : numActions
  Px{k} = zeros(numStates, numStates);
end
Pa = zeros(numStates, numActions);
R = zeros(numStates, numActions);
Nza = zeros(numStates, numActions);

nnzKsz = 0;
for e = 1 : length(epiend)
  t = epiend(max(e - 1, 1)) * (e > 1) + 1;
  sub = t : epiend(e);
  n = length(sub);
  
  % distance to representative states
  D = pdistc(S(sub, :), Z, 2);
  
  % similarity to representative states
  smoothing = true;
  if (smoothing)
    Ksz = max(exp(- D .^ 2 / (2 * heat ^ 2)) - k0, 0);
    sel0 = find(all(Ksz == 0, 2));
    if (~isempty(sel0))
      [~, ndx] = min(D(sel0, :), [], 2);
      Ksz(sel0 + (ndx - 1) * n) = 1;
    end
    Ksz = Ksz ./ repmat(sum(Ksz, 2), 1, numStates);
    if (k0 > 0)
      Ksz = sparse(Ksz);
    end
  else
    [~, ndx] = min(D, [], 2);
    Ksz = zeros(size(D));
    Ksz((1 : n)' + (ndx - 1) * n) = 1;
    Ksz = sparse(Ksz);
  end
  nnzKsz = nnzKsz + nnz(Ksz);
  
  % estimation
  for k = 1 : numActions
    selk = find(a(sub(1 : end - 1)) == k);
    if (~isempty(selk))
      Px{k} = Px{k} + Ksz(selk, :)' * Ksz(selk + 1, :);
      R(:, k) = R(:, k) + (r(sub(selk)) * Ksz(selk, :))';
      Nza(:, k) = Nza(:, k) + sum(Ksz(selk, :), 1)';
    end
  end
  k = a(sub(end));
  if (absorb(sub(end)))
    Pa(:, k) = Pa(:, k) + Ksz(end, :)';
    R(:, k) = R(:, k) + r(sub(end)) * Ksz(end, :)';
    Nza(:, k) = Nza(:, k) + Ksz(end, :)';
  end
end

for k = 1 : numActions
  P(:, k, :) = Px{k};
end

% normalization
for k = 1 : numActions
  for i = 1 : numStates
    mu = sum(P(i, k, :)) + Pa(i, k);
    P(i, k, :) = P(i, k, :) / mu;
    Pa(i, k) = Pa(i, k) / mu;
    R(i, k) = R(i, k) / Nza(i, k);
  end
end
P(isnan(P)) = 0;
Pa(isnan(Pa)) = 0;
R(isnan(R)) = 0;

% similarity between representative vertices
G = zeros(numStates, numStates, numActions);
for i = 1 : numStates
  G(i, :, :) = kernel(Z(i, :), Z, Nza, heat);
end
if (min(sum(sum(G, 3), 2)) < numActions - 0.5)
  error('An all-zero row in the kernel matrix.');
end

% for k = 1 : numActions
%   G(:, :, k) = eye(numStates);
% end
% Nza(:, :) = 1;

fprintf(['Non-zero entries:      ', ...
  'P %.2f%% | G %.2f%% | Nza %.2f%% | Ksz %.2f%%\n'], ...
  100 * [nnz(P) / numel(P), nnz(G) / numel(G), ...
  nnz(Nza) / numel(Nza), nnzKsz / (N * numStates)]);
if (any(isnan(P(:))) || any(isnan(Pa(:))) || any(isnan(R(:))) || ...
    any(isnan(G(:))) || any(isnan(Nza(:))))
  error('NaN entries in the model.');
end
