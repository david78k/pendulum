function [nexts, reward, endsim] = cifar_simulator(state, action)
  %%%%%%%%%%%%%%%%%%%%%%%%%%%
  % (c) 2012 Branislav Kveton
  %%%%%%%%%%%%%%%%%%%%%%%%%%%
  persistent Psim Pdiff X y minX maxX hkey;
  
  if (isempty(Psim))
    load -mat cifar.mdp;
    
    % hashing
    n = size(X, 1);
    minX = min(X);
    maxX = max(X);
    hkey = zeros(n, 1);
    for i = 1 : size(X, 2)
      c = round(255 * (X(:, i) - minX(i)) / (maxX(i) - minX(i)));
      hkey = bitxor(hkey, c);
    end
  end
  
  n = size(X, 1);
  if (nargin < 1)
    nexti = ceil(n * rand());
    nexts = X(nexti, :);
    reward = y(nexti) == 0;
    endsim = 0;
  elseif (nargin < 2)
    nexti = state2index(state);
    nexts = X(nexti, :);
    reward = y(nexti) == 0;
    endsim = 0;
  else
    statei = state2index(state);
    if (action == 1)
      nexti = Psim(statei, ceil(size(Psim, 2) * rand()));
    else
      nexti = Pdiff(statei, ceil(size(Pdiff, 2) * rand()));
    end
    nexts = X(nexti, :);
    reward = y(nexti) == 0;
    endsim = 0;
  end
  return
  
  function ndx = state2index(x)
    % hashing key
    h = 0;
    c = round(255 * (x - minX) ./ (maxX - minX));
    for i = 1 : size(X, 2)
      h = bitxor(h, c(i));
    end
    
    % index
    sub = find(hkey == h);
    d = sum((repmat(x, length(sub), 1) - X(sub, :)) .^ 2, 2);
    [~, ndx] = min(d);
    ndx = sub(ndx);
  end
end
