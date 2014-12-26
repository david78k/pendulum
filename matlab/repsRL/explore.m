function sample = explore(simulator, numActions, numEpisodes, numSteps)

sample = struct(...
  'state', {}, 'action', {}, 'reward', {}, 'absorb', {}, 'end', {});
sample(1).state = feval(simulator);
sample(1).action = 0;
sample(1).reward = 0;
sample(1).absorb = 0;
sample(1).end = 0;
sample = repmat(sample, numEpisodes * numSteps, 1);

i = 1;
for e = 1 : numEpisodes
  state = feval(simulator);
  absorb = 0;
  
  t = 0;
  while ((t < numSteps) && (~absorb))
    action = ceil(rand() * numActions);
    [nstate, reward, absorb] = feval(simulator, state, action);
    sample(i).state = state;
    sample(i).action = action;
    sample(i).reward = reward;
    sample(i).absorb = absorb;
    sample(i).end = absorb | (t == numSteps - 1);
    
    state = nstate;
    t = t + 1;
    i = i + 1;
  end
end

sample = sample(1 : i - 1);
