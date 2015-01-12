function history = control(init_data, result, goal, varargin),
% CONTROL  Run control with a NDFA model
%
%  history = CONTROL(init_data, result, goal, otherargs)
%
%  Runs nonlinear control with different strategies using a NDFA
%  model. Initial values for data are taken from init_data,
%  mappings from the result. Goal is the state to be
%  stabilized. Additional arguments can be used to set the control
%  problem and to tune control strategies.
%
% --------------------------------------------------------------------
%  ACCEPTED ARGUMENTS
%
%  'strategy'           Control strategy to use, see available
%                       control strategies
%  'noise'              Level of observation noise to use
%  'silent'             Run control in silent mode
%  'g'                  Function used to simulate the system 
%  'g_in'               Extra parameters for the simulation
%  'Tc'                 Lenght of the control horizon
%  'iters'              Max number of iterations during each step
%  'steps'              Number of steps the system is controlled
%  'constraints'        Constraints for the system
%
% --------------------------------------------------------------------
%  AVAILABLE CONTROL STRATEGIES
%
%  1 - Optimistic Inference Control (slow)
%  2 - Classic NMPC with quadratic cost function (quite fast, default)
%  3 - Direct Control (real-time)
%  4 - OIC initialized with NMPC (slow)
%  5 - Direct control with a single NMPC iteration (real-time)

% Copyright (C) 2004-2005 Matti Tornio
%
% This package comes with ABSOLUTELY NO WARRANTY; for details
% see License.txt in the program package.  This is free software,
% and you are welcome to redistribute it under certain conditions;
% see License.txt for details.


% Optional arguments (defaults are for the cartpole simulation)
args = readargs(varargin);
[strategy, args]    = getargdef(args, 'strategy', 2);
[noise, args]       = getargdef(args, 'noise', 0.001);
[silent, args]      = getargdef(args, 'silent', 0);
[g, args]           = getargdef(args, 'g', 'cartpole_f');
[g_in, args]        = getargdef(args, 'g_in', 0.05);
[Tc, args]          = getargdef(args, 'Tc', 40);
[iters, args]       = getargdef(args, 'iters', nan);
[steps, args]       = getargdef(args, 'steps', 60);
[samples, args]     = getargdef(args, 'samples', 0);
[constraints, args] = getargdef(args, 'constraints', []);

% Check that the strategy is valid
switch strategy,
  case {1,2,3,4,5},
    fprintf('Using strategy %d.\n', strategy);
  otherwise,
    error('Invalid strategy!');
end

% Extract model parameters
Ns = size(result.net.w1, 2);
Nx = size(result.net.w2, 1);
Nu = result.status.controlchannels;
Nu = size(result.status.controlchannels, 1);
controls = Ns - 1 + (1:Nu);

% Initialize sources to zero
zs = ones(Ns, Tc + 1);
result.sources = acprobdist_alpha(zs * 0, zs * (.5 * noise) ^ 2);
result.sources.var(controls) = .1;
result.clamped = sparse(0 * zs);
result.notimedep = [];

% Initialize cost functions
result.status.kls = [];
result.status.llh = [];

% Initialize control
switch strategy
  case 1,
    control = [nan*ones(Nu, Tc) zeros(Nu, 1)];
  case {2, 3, 4, 5},
    control = [nan*ones(Nu, Tc+1)];
end

% Initialize data
switch strategy
  case {1, 2, 4, 5},
    data = [init_data ones(Nx, Tc - 1)*nan goal];
  case 3,
    data = [init_data ones(Nx, Tc)*nan];
end

% Do some ndfa iterations to find suitable initial values for the sources 
% and then improve the estimate with IEKS
if strategy == 1,
  result.status.kls = [];
  result = ndfa(data, result, 'initclamped', control, ...
		'nolearning', 1, 'iters', 300, 'constraints', constraints);
  % TODO Add constraints
  s = ieks(data, result.net, result.tnet, 500, result.sources.e, [], ...
	   result.params);
  result.sources.e = s;
  fprintf('Source initialisation done.\n');
end

% Store the true state of the system
truesample = init_data;

% Start the actual NMPC
history.data = [init_data];
%history.state = [truesample];
history.control = [];
j = 0;

for i=1:steps,
  j = j + 1;
  sources = result.sources;
  % Do extra iterations for the first round (OIC)
  if (i == 1) & (strategy == 1),
    result = ndfa(data, result, 'initclamped', control, 'nolearning', 1, ...
		  'iters', iters, 'constraints', constraints);
  end
  
  % Find suitable control based on strategy
  switch (strategy),
    case 1,
      % Do some IEKS iterations to get a good initial estimate
      result = ndfa(data, result, 'nolearning', 1, 'iters', 0);
      result.sources.e = ieks(data, result.net, result.tnet, 500, ...
			      result.sources.e, [], result.params);
      result.sources.e(controls, end) = zeros(Nu, 1);
      % Improve IEKS estimate and calculate variances with NDFA
      result = ndfa(data, result, 'nolearning', 1, 'iters', iters, ...
		    'constraints', constraints);
    case {2, 3, 4, 5},
      % Estimate the current state with IEKS and predict the future state
      c = [sources.e(controls,1:j-1) nan*ones(Nu,1)];

      if result.net.identity,
	init_s = [data(:,1:j); sources.e(controls,1:j)];
	llh = 0;
      else
	[init_s, llh] = ...
	    ieks(data(:,1:j), result.net, result.tnet, 50, ...
		 sources.e(1:Ns-Nu,1:j), sources.e(controls,1:j), ...
		 result.params);
      end
      if isnan(llh),
	% If IEKS failed to converge, estimate the state with NDFA
        temp_s = findsources(data(:,1:j), result, 50, sources(:,1:j), c);
        temp_s = findsources(data(:,1:j), result, 50);
        init_s = temp_s.e;
      end
      first_s = init_s(:,end);

      switch strategy,

        case {2, 4},
 %          s = predict(first_s, result, Tc, sources.e(controls,j:end));
        otherwise,  
          s = predict(first_s, result, Tc);
      end
  
      % Initialize control
      init_u = sources.e(controls,j:end);
      
      % Optimize the cost function
      switch strategy,
       case {2, 4},
	[s, c] = optimize(first_s, init_u, ...  
			  data(:,j:end), result, 100, 3);
       case 5,
	[s, c] = optimize(first_s, init_u, ...  
			  data(:,j:end), result, 1, 3);
      end

      % Store the new control signal and reconstruct observations
      result.sources.e = [init_s(:,1:end-1) s];
      if strategy == 4,
        result.sources = findsources(data, result, 1000, result.sources, ...
				     result.sources(controls,:));
      end

  end


  % Fetch the first control input and simulate the system for one
  % time step
  newcontrol = result.sources.e(controls, j);
  if i == 1,
    % Special case for the first step to initialize the simulator
    [newcontrol, truesample] = feval(g, newcontrol, truesample, g_in);
  else
    [newcontrol, truesample] = feval(g, newcontrol);
  end
  newsample = truesample + randn(Nx, 1) * noise;

  % Store (possibly saturated) control back to sources
  result.sources.e(controls, j) = newcontrol;

  % Reconstruct data and predict future sources
  x = feedfw(result.sources, result.net);
  x = x{4};

  % Store the new sample to observations
  switch strategy,
   case 1,
    data = [data(:,1:j) newsample data(:,j+1:end)];
   case {2, 3, 4, 5},
    data = [data(:,1:j) newsample data(:,j+2:end) goal(:,end)];
  end
  
  % Save the new observations and control signal to history
  history.data = [history.data data(:,j+1)];
  history.control = [history.control newcontrol];

  % Predict the next source after the horizon
  newsource = result.sources(:,end);
  newsource.e = newsource.e + feedfwfast(newsource.e, result.tnet);
  newsource.e(controls) = 0;
  sources = [result.sources newsource];

  % Current control is no longer missing
  clamped = result.clamped;
  clamped = [clamped(:,1:j-1) ...
	     [zeros(Ns-Nu, 1); ones(Nu, 1)] ...
	     clamped(:,j:end)];
  result.clamped = sparse(clamped);

  % Drop old history (older than five time steps)
  if size(data, 2) > (Tc + 6),
    data = data(:, 2:end);
    sources = sources(:, 2:end);
    result.clamped = sparse(result.clamped(:, 2:end));
    j = j - 1;
  end

  % Store new sources
  result.sources = sources;

  fprintf('****************************************************************************\n')
%  fprintf('Optimization round %d done with cost N(%f,%f)).\n', i,
%  c.e, c.var);
  fprintf('Optimization round %d done.\n', i);
  fprintf('****************************************************************************\n\n')

  % Plotting code for the cart-pole system
  if ~silent,
    figure(10),clf,hold on;

    % Plot observed history
    plot(1-i:0, [history.data(:,1:end-1); history.control(:,1:end)]');

    % Plot estimated and predicted state with confidence intervals
    x = x(:,1:end);
    pred_start = Tc - size(x.e, 2) + 1;
    plot(pred_start:Tc, x.e, '--');
    plot(pred_start:Tc, x.e + 2 * sqrt(x.var), ':');
    plot(pred_start:Tc, x.e - 2 * sqrt(x.var), ':');

    % Plot control signals
    c = result.sources(controls, j:end-1);
    if Nx == 2,
      plot(0:Tc, c.e, 'r');
      plot(0:Tc, c.e + 2 * sqrt(c.var), 'r:');
      plot(0:Tc, c.e - 2 * sqrt(c.var), 'r:');
    else,
      plot(0:Tc, c.e, 'm');
      plot(0:Tc, c.e + 2 * sqrt(c.var), 'm:');
      plot(0:Tc, c.e - 2 * sqrt(c.var), 'm:');
    end

    % Plot desired state
    plot((Tc - size(goal, 2) + 1):Tc, goal);
    
    % Plot lines for the current time and the control horizon
    axis tight,
    scale = axis;
    scale = scale(3:4) + [-.5 .5];
    plot([0 0], scale, 'k--');
    plot([Tc Tc], scale, '--');
    axis tight,

    % Add legend for the cart-pole simulation
    if strcmp(g, 'cartpole_f'),
      switch(Nx),
        case 2,
          legend('x', '\theta', 'F', 0);
        case 4,
          legend('x', 'dx', '\theta', 'd\theta', 'F', 0);
      end
      ylabel('observed state')
      xlabel('time steps')
    end
    
    % Force the figure to be displayed
    hold off;
    figure(10);
    pause(.2);
  end
end

% Store the history. The last control input is set to all
% zeros, since it is left indetermined during optimization
history.control = [history.control zeros(Nu, 1)];
history.state = feval(g);
history.sources = sources;
history.lastdata = data;



function u = predict_u(s, result, n),
% PREDICT_U  Use feedforward to generate control

% Do the actual prediction
for t=size(s, 2)+1:n+1,
  s(:,t) = s(:,t-1) + feedfwfast(s(:,t-1), result.tnet);
  if s(end,t) > 10,
    s(end,t) = 10;
  end
  if s(end,t) < -10,
    s(end,t) = -10;
  end
end

u = s(end,:);