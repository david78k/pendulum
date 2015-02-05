clear all;

% initialization of the random number generator
rng('default');
rng('shuffle');

% domain
% domain = 'acrobot2';
%domain = 'mcar';
domain = 'pendulum';
% domain = 'cifar';

% parameterization of the MDP
simulator = [domain, '_simulator'];
switch (domain)
  case 'acrobot2'
    numStateVars = 4;
    numActions = 3;
    discount = 1;
    absorbSteps = 1000;
    maxNumSteps = 10000;
    maxNumEpisodes = 100;
    numExps = 300;
    rescaling = true;
    maxNumStates = 1024;
  case 'mcar'
    numStateVars = 2;
    numActions = 3;
    discount = 0.99;
    absorbSteps = 500;
    maxNumSteps = 300;
    maxNumEpisodes = 500;
    numExps = 300;
    rescaling = true;
    maxNumStates = 1024;
  case 'pendulum'
    numStateVars = 2;
    numActions = 3;
    discount = 0.95;
    absorbSteps = 3000;
    maxNumSteps = 100;
    maxNumEpisodes = 500;
    numExps = 10;
    rescaling = true;
    maxNumStates = 128;
  case 'cifar'
    numStateVars = 64;
    numActions = 2;
    discount = 0.99;
    absorbSteps = 300;
    maxNumSteps = 100;
    maxNumEpisodes = 100;
    numExps = 300;
    rescaling = false;
    maxNumStates = 1024;
end
maxTreeDepth = 10;

numTrials = 10;
results = struct(...
  'absorb', {}, 'steps', {}, 'reward', {}, 'times', {}, ...
  'errLInf', {}, 'errL1', {}, 'N', {}, 'numStates', {}, 'heat', {});

% experiments
for trial = 1 : numTrials
  % sampling
  fprintf('Sampling...\n');
  sample = ...
    explore(simulator, numActions, maxNumEpisodes(end), maxNumSteps);
  epiend = find([sample.end] == 1);
  N = length(sample);
  
  % statistics for state-space rescaling
  S = reshape([sample.state], numStateVars, N)';
  if (rescaling)
    addf = - min(S);
    mulf = 1 ./ (sqrt(numStateVars) * (max(S) - min(S)));
  else
    addf = zeros(1, numStateVars);
    mulf = 1 ./ repmat(2 * sqrt(max(sum(S .^ 2, 2))), 1, numStateVars);
  end
  
  % preprocessing
  fprintf('Preprocessing...\n');
  tic;
  
  S = reshape([sample.state], numStateVars, N)';
  a = [sample.action];
  r = [sample.reward];
  absorb = [sample.absorb];
  fprintf('%i absorbing samples out of %i.\n', sum(absorb), N);
  results(1).times(trial, 1) = toc;
  
  % rescaling
  S = (S + repmat(addf, N, 1)) .* repmat(mulf, N, 1);
  
  % quantization
  fprintf('Quantization...\n');
  tic;
  qS = quantCoverTree(S, 1, maxTreeDepth, maxNumStates);
  numStates = size(qS, 1);
  results(1).N(trial, 1) = N;
  results(1).numStates(trial, 1) = numStates;
  results(1).times(trial, 2) = toc;
  
  % building of a discrete-state MDP
  fprintf('Building model...\n');
  fprintf('Number of states:      %i / %i\n', ...
    numStates, maxNumStates);
  tic;
  
  % cover errors
  mind = Inf(N, 1);
  mini = Inf(N, 1);
  for i = 1 : numStates
    d = pdistc(qS(i, :), S, 2)';
    sel = (d < mind);
    mini(sel) = i;
    mind(sel) = d(sel);
  end
  errLInf = max(mind);
  errL1 = mean(mind);
  
  % heat parameter
  heat = getHeat(numStates) / 3;
  fprintf('Heat parameter:        %.6f\n', heat);
  
  % discretized model
  k0 = 1e-9;
  [P, R, G, Nza] = ...
    buildMDP(S, qS, a, r, absorb, numActions, heat, k0, epiend);
  results(1).errLInf(trial, 1) = errLInf;
  results(1).errL1(trial, 1) = errL1;
  results(1).heat(trial, 1) = heat;
  results(1).times(trial, 3) = toc;
  
  % policy iteration
  fprintf('Policy computation...\n');
  tic;
  [V, Q] = solveMDP(P, R, G, discount, numStates, numActions);
  Qz = zeros(numStates, numActions);
  for i = 1 : numActions
    Qz(:, i) = R(:, i) + discount * squeeze(P(:, i, :)) * V;
  end
  results(1).times(trial, 4) = toc;
  
  % policy evaluation
  fprintf('Policy evaluation...\n');
  for e = 1 : numExps
    state = feval(simulator);
    absorb = 0;
    reward = 0;
    
    t = 0;
    while ((t < absorbSteps) && (~absorb))
      nstate = (state + addf) .* mulf;
      g = kernel(nstate, qS, Nza, heat);
      [maxQ, action] = max(sum(g .* Qz, 1));
      
      [state, rt, absorb] = feval(simulator, state, action);
      reward = reward + rt * (discount ^ t);
      t = t + 1;
    end
    
    results(1).absorb(trial, e) = (absorb == 1);
    results(1).steps(trial, e) = t;
    results(1).reward(trial, e) = reward;
  end
  
  fprintf('Number of trials:      %i\n', trial);
  fprintf('Worst-case quantization error: %.6f \\pm %.6f\n', ...
    mean(results(1).errLInf), std(results(1).errLInf));
  fprintf('Average quantization error:    %.6f \\pm %.6f\n', ...
    mean(results(1).errL1), std(results(1).errL1));
  fprintf('Absorbing probability: %.3f \\pm %.3f\n', ...
    mean(mean(results(1).absorb, 2)), ...
    std(mean(results(1).absorb, 2)));
  fprintf('Steps to absorption:   %.3f \\pm %.3f\n', ...
    mean(mean(results(1).steps, 2)), ...
    std(mean(results(1).steps, 2)));
  fprintf('Reward:                %.3f \\pm %.3f\n', ...
    mean(mean(results(1).reward, 2)), ...
    std(mean(results(1).reward, 2)));
  fprintf('Computation time:      %.3fs\n', ...
    mean(sum(results(1).times, 2)));
  fprintf('\n');
  
  pause(1);
end

% averaging
results(1).absorb = mean(results(1).absorb, 2);
results(1).steps = mean(results(1).steps, 2);
results(1).reward = mean(results(1).reward, 2);
