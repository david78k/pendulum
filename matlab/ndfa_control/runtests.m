function stats = runtests(N, result, Tc, strategy, init, noise, opt_iter),
% RUNTESTS  Automatically runs the cart-pole task multiple times.
%
%  stats = RUNTESTS(N, result, Tc, strategy, init_data, noise)
%
%  Automatically runs the cart-pole swing-up task simulation multiple times.
%  Statistics collected from the runs are saved to a Matlab-file and returned 
%  as a single structure.

% Copyright (C) 2004-2005 Matti Tornio.
%
% This package comes with ABSOLUTELY NO WARRANTY; for details
% see License.txt in the program package.  This is free software,
% and you are welcome to redistribute it under certain conditions;
% see License.txt for details.


% Default horizon length
if nargin < 3,
  Tc = 35;
end

% Extra model parameters
Nx = size(result.net.w2, 1);
Ns = size(result.net.w1, 2);

% Use strategy 2 (classic NMPC) by default
if nargin < 4,
  strategy = 2;
end

% Use silent mode for more than five simulation runs
silent = (N > 5);

if nargin < 7,
  opt_iter = 50;
end


% Initialize statistics
[success{1}(1:N)] = deal(0);
[partialsuccess{1}(1:N)] = deal(0);
[crash{1}(1:N)] = deal(0);
hcost = cell(1);
htime = cell(1);
history = cell(1);

% Start simulations
for i=1:N,
  % If no initial state is provided a random initialization is used
  if (nargin < 5) | isempty(init),
    init = 2 * [1 2 1 3]' .* (rand(Nx, 1) - .5)
    %init = .1 * randn(Nx, 1);
  end

  % Set goal for the swing-up task
  switch Nx
    case 4,
      goal = [nan;nan;pi;0];
      constraints = [3 -3; ones(3,2)*nan; 10 -10];
    case 2,
      goal = [nan;pi];
      constraints = [3 -3; ones(1,2)*nan; 10 -10];
    case 5,
      goal = [nan;nan;0;-1;0];
      constraints = [3 -3; ones(3,2)*nan; 10 -10];
    otherwise,
      error('Invalid number of observation channels.');
  end

  % Do the actual swing-up
  tic;
  if nargin >= 6,
    h = control(init, result, goal, 'strategy', strategy, ...
		'steps', 61, 'Tc', Tc, 'silent', silent, 'noise', noise, ...
		'constraints', constraints, 'opt_iter', opt_iter);
  else
    h = control(init, result, goal, 'strategy', strategy, ...
		'steps', 61, 'Tc', Tc, 'silent', silent, ...
		'constraints', constraints, 'opt_iter', opt_iter);
  end  

  % Check if the swing-up succeeded
  if max(abs(h.state(1,:))) > 2.999,
    crash{1}(i) = true;
  elseif (abs(h.state(4,end)) < 2) & (abs(abs(h.state(3,end)) - pi) < .133*pi),
    success{1}(i) = true;
  end
 
  % Check when the desired state was first reached
  for j = 1:size(h.state, 2),
    if (abs(h.state(4,j)) < 2) & (abs(abs(h.state(3,j)) - pi) < .133*pi),
      partialsuccess{1}(i) = j;
      break;
    end
  end

  % Collect statistics
  htime{1}(i) = toc;
  switch size(goal, 1),
    case 2,
      goal = [goal(1); nan; goal(2); nan];
    case 5,
      goal = angletransformback(goal, 0);
  end

  hcost{1}(i) = cost(h.state(:,Tc+1:end), goal, h.control(Tc+1:end));
  history{1, i} = h;
  % Save current results
  save runtests success partialsuccess crash hcost htime history;
end

% Pack all return values to a single structure
stats.success = success;
stats.partialsuccess = partialsuccess;
stats.crash = crash;
stats.cost = hcost;
stats.time = htime;
stats.history = history;
