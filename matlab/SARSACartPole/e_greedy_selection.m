function [ a ] = e_greedy_selection( Q , s, epsilon )
% e_greedy_selection selects an action using Epsilon-greedy strategy
% Q: the Qtable
% s: the current state

actions = size(Q,2);
	
if (rand()>epsilon) 
    a = GetBestAction(Q,s);    
else
    % selects a random action based on a uniform distribution
    % +1 because randint goes from 0 to N-1 and matlab matrices goes from
    % 1 to N
    actions
%     a = randint(1,1,actions)+1; % original code, not working on 2014a,
%     2014b
    a = randi(actions); % working on 2014a
%     a = randi(actions) + 1; % index out of bounds
    a
end

