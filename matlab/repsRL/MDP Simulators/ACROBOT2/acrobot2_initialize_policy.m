function policy = acrobot2_initialize_policy(explore, discount, basis)

  
  policy.explore = explore;
  
  policy.discount = discount;

  policy.actions = 3;
  
  policy.basis = basis;
  
  k = feval(basis);
  
  %%% Initial weights 
   policy.weights = zeros(k,1);  % Zeros
  %policy.weights = ones(k,1);  % Ones
%   policy.weights = rand(k,1);  % Random

  return
