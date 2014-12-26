function g = kernel(x, Z, Nza, heat)

numActions = size(Nza, 2);
d = pdistc(x, Z, 2)';
g = exp(- d .^ 2 / (2 * heat ^ 2));
g = repmat(g, 1, numActions) .* Nza;

for k = 1 : numActions
  if (all(g(:, k) == 0))
    [~, ndx] = sort(d);
    nn = find(Nza(ndx, k) > 0, 1, 'first');
    g(ndx(nn), k) = 1;
  else
    g(:, k) = g(:, k) / sum(g(:, k));
  end
end
