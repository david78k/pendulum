function [V, Q, pi] = solveMDP(P, R, G, gamma, numStates, numActions)

V = zeros(numStates, 1);
Vp = Inf(numStates, 1);
Q = zeros(numStates, numActions);
pi = ones(numStates, 1);
pip = zeros(numStates, 1);

% precomputing GR and GP
GR = cell(numActions, 1);
for u = 1 : numActions
  GR{u} = squeeze(G(:, :, u)) * R(:, u);
end
GP = cell(numActions, 1);
for u = 1 : numActions
  GP{u} = squeeze(G(:, :, u)) * squeeze(P(:, u, :));
end

% policy iteration
numBackups = 0;
while (any(pi ~= pip) && (numBackups < 100))
  % evaluate policy
  Ppi = zeros(numStates, numStates);
  Rpi = zeros(numStates, 1);
  for u = 1 : numActions
    sel = (pi == u);
    Ppi(sel, :) = GP{u}(sel, :);
    Rpi(sel) = GR{u}(sel);
  end
  V = (eye(numStates) - gamma * Ppi) \ Rpi;
  
  % improve policy
  pip = pi;
  for u = 1 : numActions
    Q(:, u) = GR{u} + gamma * GP{u} * V;
  end
  [V, pi] = max(Q, [], 2);
  
  numBackups = numBackups + 1;
end
fprintf('%i policy iteration steps.\n', numBackups);
