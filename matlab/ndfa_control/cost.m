function c = cost(x, goal, F),
% COST  Evaluate the cartpole system cost function
%
%  c = cost(x, goal, F)
%
%  Evaluates the quadratic cost function between the desired reference state
%  and the actual predicted state for the cartpole system. Out of bounds 
%  values for force and position are penalized with a quadratic term.
%
%  If no force is provided, it's assumed to be within allowed
%  limits.
%
%  For probdist sources use COSTP.


% Copyright (C) 2004-2005 Matti Tornio
%
% This package comes with ABSOLUTELY NO WARRANTY; for details
% see License.txt in the program package.  This is free software,
% and you are welcome to redistribute it under certain conditions;

[Nx T] = size(x);

% Use zeroes for force if none is provided (no penalty)
if nargin < 3,
  F = zeros(1,T);
end

% Find missing values and replace them with zeroes
missing = sparse(isnan(goal));
goal(find(missing)) = 0;

% If only single reference sample is provided, use it for all time steps
if size(goal, 2) == 1,
  goal = repmat(goal, 1, T);
  missing = repmat(missing, 1, T);
end

% Calculate quadratic error between predicted state and reference signal
c = sum((~missing .* (x - goal)) .^ 2, 1);

% Force becomes saturated at 10 N, larger forces are penalized with a quadratic
% term
F = abs(F);
F = F - 10;
F(find(F < 0)) = 0;
c = c + F .^ 2;

% Position of the cart must be between -3 and 3, quadratic penalty
% starts to apply when mean is outside [-2.5, 2.5] range
x = abs(x(1,:));
x = x - 2.5;
x(find(x < 0)) = 0;
c = c + x .^ 2;

% Collect costs from all time steps
c = full(sum(c));