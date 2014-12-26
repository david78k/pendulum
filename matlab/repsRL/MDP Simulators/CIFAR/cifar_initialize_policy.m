function policy = cifar_initialize_policy(explore, discount, basis)

policy.explore = explore;
policy.discount = discount;
policy.actions = cifar_actions;
policy.basis = basis;
k = feval(basis);
policy.weights = zeros(k, 1);
