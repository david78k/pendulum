function [action, actionphi] = policy_function(policy, state)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Copyright 2000-2002
%
% Michail G. Lagoudakis (mgl@cs.duke.edu)
% Ronald Parr (parr@cs.duke.edu)
%
% Department of Computer Science
% Box 90129
% Duke University, NC 27708
%
%
% [action, actionphi] = policy_function(policy, state)
%
% Computes the "policy" at the given "state".
%
% Returns the "action" that the policy picks at that "state" and the
% evaluation ("actionphi") of the basis at the pair (state, action).
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%%% Exploration or not?
if (rand < policy.explore)

    %%% Pick one action in random
    action = randint(policy.actions);
    actionphi = feval(policy.basis, state, action);

else

    %%% Pick the action with maximum Q-value
    bestq = -inf*ones(1,size(state,1));
    besta = [];

    %%% Find first all actions with maximum Q-value
    for i=1:policy.actions
        % Compute the basis functions at all the states, and action i
        phi = feval(policy.basis, state, i);
        % Compute the the state-action q-value
        q = phi' * policy.weights;
        % Update the best action, for each state
        for k = length(q):-1:1,
            if (q(k) > bestq(k))
                bestq(k) = q(k);
                besta{k} = [i];
                actionphi{k} = [phi(:,k)];
            elseif (q(k) == bestq(k))           % MM: This should really be an \epsilon-comparison.
                besta{k} = [besta{k}; i];
                actionphi{k} = [actionphi{k}, phi(:,k)];
            end;
        end;
    end;
    
    % Now, pick one of the best actions
    for k = length(q):-1:1,
        which = 1;                         % Pick the first (deterministic)
%         which = randint(length(besta{k}));    % Pick randomly

        action(k) = besta{k}(which);
        actionphi{k} = actionphi{k}(:,which);
    end;
    actionphi=cat(2,actionphi{:});
end;


return
