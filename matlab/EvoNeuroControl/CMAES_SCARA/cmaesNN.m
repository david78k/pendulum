function [xmin, ...      % minimum search point of last iteration
	      fmin, ...      % function value of xmin
	      counteval, ... % number of function evaluations done
	      stopflag, ...  % stop criterion reached
	      outhist, ...   % output record with 6 columns
	      bestever ...   % struct with overall best search point
	      ] = cmaesNN( ...
                     fitfun, ...    % name of objective/fitness function
                     xstart, ...    % objective variables initial point, determines N
                     insigma, ...   % initial coordinate wise standard deviation
                     inopts, ...    % options struct, see defopts below
                     varargin )     % arguments passed to objective function 

                

                 
% cmaes.m, Version 2.24. Last change: June, 4, 2004 
% CMAES implements an Evolution Strategy with Covariance Matrix
% Adaptation (CMA-ES) for nonlinear function minimization.  For
% introductory comments and copyright see end of file (type 'type
% cmaes').
%
% OPTS = CMAES returns default options. 
% OPTS = CMAES('defaults') returns default options quietly.
% OPTS = CMAES('defaults', OPTS) supplements options OPTS with default 
% options.
%
% XMIN = CMAES(FUN, X0, SIGMA[, OPTS]) locates the minimum XMIN of
% function FUN starting from column vector X0 with the initial
% coordinate wise search standard deviation SIGMA.
%
% Input arguments: 
%
%   FUN can be a function name like 'myfun' or a function handle like
%     @myfun. FUN takes as argument a column vector of size of X0 and
%     returns a scalar.
%
%   X0 is a column vector, or a matrix, or a string. If X0 is a matrix,
%     mean(X0, 2) is taken as initial point. If X0 is a string like
%     '2*rand(10,1)-1', the string is evaluated first.
%
%   SIGMA is a scalar, or a vector of size(X0,1), or a string that can
%     be evaluated into one of these. SIGMA determines the initial
%     coordinate wise standard deviation for the search.  Setting
%     SIGMA to half of the initial search region is appropriate,
%     e.g., the initial point randomly in [-1, 3]^15 and SIGMA=2 means
%     cmaes('myfun', 4*rand(15,1)-1, 2). On the simplest quadratic
%     function f=sum(x.^2) optimal SIGMA equals roughly
%     sqrt(mean(X0'.^2)) * ParentNumber/sqrt(N) for
%     ParentNumber<sqrt(N). If SIGMA is missing and size(X0,2)>1,
%     SIGMA is set to sqrt(var(X0')'). That is, X0 is used as a sample
%     for estimating initial mean and variance of the search
%     distribution.
%
%   OPTS (an optional argument) is a struct holding additional input
%     options. Valid field names and a short documentation can be
%     discovered by looking at the default options (type 'cmaes'
%     without arguments, see above). Empty or missing fields in OPTS
%     invoke the default value, i.e. OPTS needs not to have all valid
%     field names.  Capitalization does not matter and unambiguous
%     abbreviations can be used for the field names. If a string is
%     given where a numerical value is needed, the string is evaluated
%     by eval, where 'N' expands to the problem dimension
%     (==size(X0,1)). For convenience the default values can be
%     changed by editing defopts in the source code.
%
% [X, F, COUNT, STOP, OUT, BESTEVER] = CMAES(FITFUN, X0, SIGMA)
% returns the best (minimal) point X (found in the last generation);
% function value F of X; the number of needed function evaluations
% COUNT; a STOP value as cell array, where possible entries are
% 'fitness', 'tolx', 'tolfun', 'maxfunevals', 'maxiter',
% 'stoptoresume' 'warnconditioncov', 'warnnoeffectcoord',
% 'warnnoeffectaxis', 'warnequalfunvals', 'warnequalfunvalhist', 'bug'
% (use e.g. any(strcmp(STOP, 'tolx')) or findstr(strcat(STOP, 'tolx'))
% for further processing); a history record OUT with columns (1)
% function evaluation count, (2) function value, (3) axis ratio of
% search distribution, (4) maximal coordinate wise standard deviation
% (sigma*sqrt(max(diag(C)))), (5) minimal coordinate wise standard
% deviation, (6) maximal standard deviation in covariance matrix C;
% and struct BESTEVER with the overall best evaluated point x with
% function value f evaluated at evaluation count counteval.
%
% To run the code completely quietly set Display, Plotting, and Saving
% options to 0.  When OPTS.Saving==1 (default) everything is saved in
% file OPTS.SaveFileName (default 'variablescmaes.mat') permitting to
% investigate the recent result (e.g. plot with function plotcmaes)
% even while CMAES is still running (which can be quite useful) and to
% resume the search afterwards by using the resume option. 
%
% To find the best ever evaluated point load the variables typing
% "es=load('variablescmaes')" and investigate variable es.bestever. To
% further control data sampling behavior search for the ooo tag in the
% source code.
%
% The major strategy parameter to play with is OPTS.PopSize, which can
% be increased without concern.  Increasing the population size (by
% default together with the parent number OPTS.ParentNumber) improves
% robustness and global search properties in exchange to speed. Speed
% decreases at most linearely with increasing population size.
%
% Examples: 
%
%   XMIN = cmaes('myfun', 5*ones(10,1), 2.5); starts the search at
%   10D-point 5 and initially searches mainly between 5-2.5 and 5+2.5
%   (+- one standard deviation), but this is not a strict bound.
%   'myfun' is a name of a function that returns a scalar from a 10D
%   column vector.
%
%   opts.lbounds = 0; opts.UBounds = 10; 
%   X=cmaes('myfun', 10*rand(10,1), 5, opts);
%   search within lower bound of 0 and upper bound of 10. Bounds can
%   also be given as column vectors. If the optimum is not located
%   on the boundary, use rather a penalty approach to handle bounds. 
%
%   opts=cmaes; opts.StopFitness=1e-10;
%   X=cmaes('myfun', rand(5,1), 0.5, opts); stops the search, if
%   the function value is smaller than 1e-10.
%   
%   [X, FMIN] = cmaes(@myfun2,'rand(5,1)',1,[],P1,P2); passes two
%   additional parameters to the function MYFUN2, given in function
%   handle form here.
%
% See also FMINSEARCH, FMINUNC, FMINBND.

% ----------- Set Defaults for Input Parameters and Options -------------
% These defaults may be edited for convenience

% Input Defaults
definput.fitfun = 'felli'; % frosen; fcigar; see end of file for more
definput.xstart = rand(10,1); 0.50*ones(10,1);
definput.sigma = 0.3;

% Options defaults: Stopping criteria % (value of stop flag)
defopts.StopFitness  = '-Inf  % stop if < stopfitness (minimization)';
defopts.MaxFunEvals  = '3e3*((N+5)^2+popsize)  % maximal number of fevals';
defopts.MaxIter      = 'Inf % maximal number of iterations';
defopts.StopFunEvals = 'Inf % stop after resp. evaluation to resume later';
defopts.StopIter     = 'Inf % stop after resp. iteration to resume later';
defopts.TolX         = '1e-12 % stop if x-changes smaller TolX';
defopts.TolFun       = '1e-12 % stop if fun-changes smaller TolFun';
defopts.StopOnWarnings = 'yes''   % ''no''==''off''==0, ''on''==''yes''==1 ';

% Options defaults: Other
defopts.DiffMaxChange = 'Inf % maximal variable change(s), can be Nx1-vector';
defopts.DiffMinChange = '0  % minimal variable change(s), can be Nx1-vector';  
defopts.LBounds = '-Inf % lower bounds, scalar or Nx1-vector'; 
defopts.UBounds = 'Inf  % upper bounds, scalar or Nx1-vector'; 

defopts.PopSize = '4 + floor(3*log(N))  % population size, lambda'; 
defopts.ParentNumber = 'floor(popsize/2)  % popsize equals lambda';
defopts.RecombinationWeights = 'superlinear % (decrease), or linear, or equal';
defopts.Display  = 'on   % display initial and final message';
defopts.Plotting = 'on   % plot while running';
defopts.Resume = 'no     % resume former run from SaveFile';  
defopts.Science = 'off   % off==do some additional problem capturing';
defopts.Saving = 'on     % save data to file while running';
defopts.SaveModulo = '1  % if >1 record data less frequently after gen=100';
defopts.SaveFileName = 'variablescmaes.mat'; % file name for saving

% ---------------------- Handling Input Parameters ----------------------

if nargin < 1 | isequal(fitfun, 'defaults') % pass default options
  if nargin < 1
    disp('Default options returned (type "help cmaes" for help).');
  end
  xmin = defopts;
  if nargin > 1 % supplement second argument with default options
    xmin = getoptions(xstart, defopts);
  end
  return;
end
if isempty(fitfun)
  fitfun = definput.fitfun; 
  warning(['Objective function not determined, ''' fitfun ''' used']);
end
if ischar(fitfun)
  fitfun = str2func(fitfun);
end
input.fitfun = fitfun; % record used input

if nargin < 2 | isempty(xstart)
  xstart = definput.xstart;  % objective variables initial point
  warning('Initial search point, and problem dimension, not determined');
end
input.xstart = xstart;

if nargin < 3 | isempty(insigma)
  if size(myeval(xstart),2) > 1
    insigma = std(xstart')'; 
    if any(insigma == 0)
      insigma = definput.sigma;
      warning(['Initial search volume would be zero, default ' ...
	      num2str(insigma) ' applied']);
    end
  else
    insigma = definput.sigma;
    warning(['Initial search volume (SIGMA) not determined,' ...
	     ' set to ' num2str(insigma)]);
  end
end
input.sigma = insigma;

% Compose options opts
if nargin < 4 | isempty(inopts) % no input options available
  inopts = []; 
  opts = defopts;
else
  opts = getoptions(inopts, defopts);
end

% ------------------------ Initialization -------------------------------

flgresume = myevalbool(opts.Resume);
if ~flgresume % not resuming a former run
  % Assign settings from input parameters and options for myeval...
  xmean = mean(myeval(xstart), 2); % in case of xstart is a population 
  N = size(xmean, 1); numberofvariables = N; 
  popsize = myeval(opts.PopSize); lambda = popsize;
  insigma = myeval(insigma);
else % if ~flgresume, do resume former run
  tmp = whos('-file', opts.SaveFileName);
  for i = 1:length(tmp)
    if strcmp(tmp(i).name, 'localopts');
      error('Saved variables include variable "localopts", please remove');
    end
  end
  localopts = opts; % keep stopping and display options
  load(opts.SaveFileName); 
  flgresume = 1;
  
  % Overwrite old stopping and display options
  opts.StopFitness = localopts.StopFitness; 
  %%opts.MaxFunEvals = localopts.MaxFunEvals;
  %%opts.MaxIter = localopts.MaxIter; 
  opts.StopFunEvals = localopts.StopFunEvals; 
  opts.StopIter = localopts.StopIter;  
  opts.TolX = localopts.TolX;
  opts.TolFun = localopts.TolFun;
  opts.StopOnWarnings = localopts.StopOnWarnings; 
  opts.Display = localopts.Display;
  opts.Plotting = localopts.Plotting;
  opts.Saving = localopts.Saving;
  opts.SaveModulo = localopts.SaveModulo;
  clear localopts; % otherwise localopts would be overwritten during load
end
  
% Evaluate options
stopFitness = myeval(opts.StopFitness); 
stopMaxFunEvals = myeval(opts.MaxFunEvals);  
stopMaxIter = myeval(opts.MaxIter);  
stopFunEvals = myeval(opts.StopFunEvals);  
stopIter = myeval(opts.StopIter);  
stopTolX = myeval(opts.TolX);
stopTolFun = myeval(opts.TolFun);
stopOnWarnings = myevalbool(opts.StopOnWarnings); 
flgdisplay = myevalbool(opts.Display);
flgplotting = myevalbool(opts.Plotting);
flgscience = myevalbool(opts.Science);
flgsaving = myevalbool(opts.Saving);
savemodulo = myeval(opts.SaveModulo);

if (isfinite(stopFunEvals) | isfinite(stopIter)) & ~flgsaving
  warning('To resume later the saving option needs to be set');
end

if ~flgresume
  maxdx = myeval(opts.DiffMaxChange); % maximal sensible variable change
  mindx = myeval(opts.DiffMinChange); % minimal sensible variable change 
				      % can both also be defined as Nx1 vectors
  lbounds = myeval(opts.LBounds);		     
  ubounds = myeval(opts.UBounds);

  % Check all vector sizes
  if size(xmean, 2) > 1 | size(xmean,1) ~= N
    error(['intial search point should be a column vector of size ' ...
	   num2str(N)]);
  elseif size(insigma, 2) > 1 | ~ismember(size(insigma, 1), [1 N])
    error(['sigma should be (or eval to) a scalar '...
	   'or a column vector of size ' num2str(N)] );
  elseif size(stopTolX, 2) > 1 | ~ismember(size(stopTolX, 1), [1 N])
    error(['option TolX should be (or eval to) a scalar '...
	   'or a column vector of size ' num2str(N)] );
  elseif size(maxdx, 2) > 1 | ~ismember(size(maxdx, 1), [1 N])
    error(['option DiffMaxChange should be (or eval to) a scalar '...
	   'or a column vector of size ' num2str(N)] );
  elseif size(mindx, 2) > 1 | ~ismember(size(mindx, 1), [1 N])
    error(['option DiffMinChange should be (or eval to) a scalar '...
	   'or a column vector of size ' num2str(N)] );
  elseif size(lbounds, 2) > 1 | ~ismember(size(lbounds, 1), [1 N])
    error(['option lbounds should be (or eval to) a scalar '...
	   'or a column vector of size ' num2str(N)] );
  elseif size(ubounds, 2) > 1 | ~ismember(size(ubounds, 1), [1 N])
    error(['option ubounds should be (or eval to) a scalar '...
	   'or a column vector of size ' num2str(N)] );
  end
  
  % Strategy internal parameter setting: Selection  
  lambda = myeval(opts.PopSize);  % population size, offspring number
  mu = myeval(opts.ParentNumber); % number of parents/points for recombination
  if strncmp(lower(opts.RecombinationWeights), 'equal', 3)
    weights = ones(mu,1); % (mu_I,lambda)-CMA-ES
  elseif strncmp(lower(opts.RecombinationWeights), 'linear', 3)
    weights = mu+1-(1:mu)';
  elseif strncmp(lower(opts.RecombinationWeights), 'superlinear', 3)
    weights = log(mu+1)-log(1:mu)'; % muXone array for weighted recombination
  else
    error(['Recombination weights to be "' opts.RecombinationWeights ...
	   '" is not implemented']);
  end
  mueff=sum(weights)^2/sum(weights.^2); % variance-effective size of mu
  if mueff == lambda
    error(['Combination of values for PopSize, ParentNumber and ' ...
	  ' and RecombinationWeights is not reasonable']);
  end
  
  % Strategy internal parameter setting: Adaptation
  cc = 4/(N+4);         % time constant for cumulation for covariance matrix
  cs = (mueff+2)/(N+mueff+3); % t-const for cumulation for step size control
  mucov = mueff;   % size of mu used for calculating learning rate ccov
  ccov = (1/mucov) * 2/(N+1.41)^2 ... % learning rate for covariance matrix
	 + (1-1/mucov) * min(1,(2*mueff-1)/((N+2)^2+mueff)); 
  % ||ps|| is close to sqrt(mueff/N) for mueff large on linear fitness
  damps = ... % damping for step size control, usually close to one 
      (1 + 2*max(0,sqrt((mueff-1)/(N+1))-1)) ... % limit sigma increase
      * max(0.3, ... % reduce damps, if max. iteration number is small
	  1 - N/min(stopMaxIter,stopMaxFunEvals/lambda)) + cs; 
  
  % Initialize dynamic internal strategy parameters
  if any(insigma <= 0)
    error(['Initial search volume (SIGMA) must be greater than zero']);
  end
  if max(insigma)/min(insigma) > 1e6
    error(['Initial search volume (SIGMA) badly conditioned']);
  end
  sigma = max(insigma);              % overall standard deviation
  pc = zeros(N,1); ps = zeros(N,1);  % evolution paths for C and sigma
  if length(insigma) == 1
    insigma = insigma * ones(N,1) ;
  end
  B = eye(N);                        % B defines the coordinate system
  D = diag(insigma/max(insigma));    % diagonal matrix D defines the scaling
  BD = B*D;                          % for speed up only
  C = BD*(BD)';                      % covariance matrix
  fitness.hist=NaN*ones(1,10+ceil(3*10*N/lambda)); % history of fitness values

  % Initialize boundary handling
  bnd.isactive = any(lbounds > -Inf) | any(ubounds < Inf); 
  if bnd.isactive
    if any(lbounds>ubounds)
      error('lower bound found to be greater than upper bound');
    end
    [xmean ti] = xintobounds(xmean, lbounds, ubounds); % just in case
    if any(ti)
      warning('Initial point was out of bounds, corrected');
    end
    bnd.weights = zeros(N,1);         % weights for bound penalty
    bnd.scale = diag(C)/mean(diag(C));
    bnd.isbounded = (lbounds > -Inf) | (ubounds < Inf);
    if length(bnd.isbounded) == 1
      bnd.isbounded = bnd.isbounded * ones(N,1);
    end
    maxdx = min(maxdx, (ubounds - lbounds)/2);
    if any(sigma*sqrt(diag(C)) > maxdx)
      fac = min(maxdx ./ sqrt(diag(C)))/sigma;
      sigma = min(maxdx ./ sqrt(diag(C)));
      warning(['Initial SIGMA reduced by the factor ' num2str(fac) ...
	       ', because it was larger than half' ...
	       ' of one of the boundary intervals']);
    end
    bnd.dfithist = 1;              % delta fit for setting weights
    bnd.aridxpoints = [];          % remember complete outside points
    bnd.arfitness = [];            % and their fitness
    bnd.validfitval = 0;
    bnd.iniphase = 1;
  end

  % ooo initial feval, for output only, (un-)comment if required
  counteval = 0; 
  fitness.hist(1)=feval(fitfun, xmean, varargin{:}); 
  counteval = counteval + 1;
                                         
  % Initialize further constants
  randn('state', sum(100*clock));     % random number generator state
  startseed = randn('state');         % for saving purpose
  chiN=N^0.5*(1-1/(4*N)+1/(21*N^2));  % expectation of 
				      %   ||N(0,I)|| == norm(randn(N,1))
  weights = weights/sum(weights);     % normalize recombination weights array
  
  % Initialize records and output
  outhist = [0 fitness.hist(1) max(diag(D))/min(diag(D)) ...
	     sigma*sqrt(max(diag(C))) ...
	     sigma*sqrt(min(diag(C))) sqrt(max(diag(C)))];
  outt0=clock; outetime=0.5; 
  out.x = 0;                   
  out.y1=[fitness.hist(1) sigma max(diag(D))/min(diag(D))];
  out.y2=xmean'; out.y2a=xmean';
  out.y3=sigma*sqrt(diag(C))';
  out.y4=sort(diag(D))'; 

  countiter = 0;

else
  if flgdisplay
    disp(['  resumed from ' opts.SaveFileName ]); 
  end
  if counteval >= stopMaxFunEvals 
    error(['MaxFunEvals exceeded, use StopFunEvals as stopping ' ...
	  'criterion before resume']);
  end
  if countiter >= stopMaxIter 
    error(['MaxIter exceeded, use StopIter as stopping criterion ' ...
	  'before resume']);
  end
  
end % else, if ~flgresume

% Display initial message
if flgdisplay
  if mu < 8
    disp(['  n=' num2str(N) ': (' num2str(mu) ',' ...
	    num2str(lambda) ')-CMA-ES(w=[' ...
	    num2str(100*weights(1:end-1)','%.0f ') ... 
	    	    num2str(100*weights(end)','%.0f') ']%, ' ...
	    'mu_eff=' num2str(mueff,'%.1f') ...
	    ') on function ' ...
	    func2str(fitfun) ]);
  else
    disp(['  n=' num2str(N) ': (' num2str(mu) ',' ...
	    num2str(lambda) ')-CMA-ES (w=[' ...
	    num2str(100*weights(1:2)','%.2g ') ...
	    num2str(100*weights(3)','%.2g') '...' ...
	    num2str(100*weights(end-1:end)',' %.2g') ']%, ' ...
	    'mu_eff=' num2str(mueff,'%.1f') ...
	    ') on function ' ...
	    func2str(fitfun)]);
  end
end

% -------------------- Generation Loop --------------------------------
stopflag = {};
while isempty(stopflag)
  countiter = countiter + 1; 
  
  % Generate and evaluate lambda offspring
  arz = randn(N,lambda);
  arx = xmean*ones(1, lambda) + sigma * (BD * arz);              % Eq. (13)
  % You may handle constraints here. You may either resample columns
  % of arz and/or multiply them with a factor between -1 and 1 (the
  % latter will decrease the overall step size) and recalculate arx
  % accordingly. Do not change arx or arz in any other way.
 
  for k=1:lambda,
    if ~bnd.isactive
      arxvalid(:,k) = arx(:,k);
    else
      arxvalid(:,k) = xintobounds(arx(:,k), lbounds, ubounds);
    end
      % You may handle constraints here.  You may copy and alter
      % (columns of) arxvalid(:,k) only for the evaluation of the
      % fitness function. arx and arxvalid should not be changed.
      fitness.raw(k) = feval(fitfun, arxvalid(:,k), varargin{:}); 
      counteval = counteval + 1;
  end

  % ----- handle boundaries -----
  if ~bnd.isactive
    fitness.sel = fitness.raw; 
  else % handle boundaries
    % Get delta fitness values
    val = (prctile(fitness.raw, 75) - prctile(fitness.raw, 25)) ...
	    / N / mean(diag(C)) / sigma^2;
    % Catch non-sensible values 
    if ~isfinite(val)
	warning('Non-finite fitness range');
	val = max(bnd.dfithist);  
    elseif val == 0 % happens if all points are out of bounds
	val = min(bnd.dfithist(bnd.dfithist>0)); 
    elseif bnd.validfitval == 0 % first sensible val
	bnd.dfithist = [];
	bnd.validfitval = 1;
    end

    % Keep delta fitness values
    if length(bnd.dfithist) < 20+(3*N)/lambda
	bnd.dfithist = [bnd.dfithist val];
    else
	bnd.dfithist = [bnd.dfithist(2:end) val];
    end

    % Scale weights anew, bias scaling to unity
    bnd.weights = bnd.scale .* bnd.weights;  % reset scaling
    bnd.scale = prod(diag(C))^(0.1/N) * diag(C).^0.9;
    bnd.scale = bnd.scale / mean(bnd.scale); % sum is N initially
    bnd.weights = bnd.weights ./ bnd.scale;

    [tx ti]  = xintobounds(xmean, lbounds, ubounds);

    % Set initial weights
    if bnd.iniphase 
	if any(ti) 
	  bnd.weights(find(bnd.isbounded)) = ...
	      0.5 * median(bnd.dfithist) ./ bnd.scale(find(bnd.isbounded));
	  if bnd.validfitval & countiter > 2
	    bnd.iniphase = 0;
	  end
	end
    end

    % Increase/decrease weights
    if any(ti) % any coordinate of xmean out of bounds
	idx = find(ti ~= 0);
	tx = xmean - tx;
	tx = tx(idx) ./ sqrt(diag(C(idx,idx))) / sigma;
	twnew = 0.5 * median(bnd.dfithist) ./ bnd.scale;
	% judge distance of xmean to boundary
	if norm(tx) > 10 * sqrt(N*length(idx)) | ...
	      all(sum(arx == arxvalid) == sum(bnd.isbounded))
	  bnd.weights = 1.1^(max(1, mueff/10/N)) * bnd.weights; % increase
	elseif any((log(twnew(idx))) < (log(bnd.weights(idx))))
	  bnd.weights = bnd.weights / 1.1^(1/1); % decrease
	end
    end
	
    % Assigned penalized fitness
    bnd.arpenalty = bnd.weights' * (arxvalid - arx).^2; 
    fitness.sel = fitness.raw + bnd.arpenalty;
  end % handle boundaries
  % ----- end handle boundaries -----
  
  % Sort by fitness 
  [fitness.raw, fitness.idx] = sort(fitness.raw);  
  [fitness.sel, fitness.idxsel] = sort(fitness.sel);   % minimization
  fitness.hist(2:end) = fitness.hist(1:end-1);    % record short history of
  fitness.hist(1) = fitness.raw(1);               % best fitness values

  % Calculate new xmean, this is selection and recombination 
  xold = xmean; % for speed up of Eq. (14)
  xmean = arx(:,fitness.idxsel(1:mu))*weights; 
  zmean = arz(:,fitness.idxsel(1:mu))*weights;%==D^-1*B'*(xmean-xold)/sigma
  % fmean = feval(fitfun, xintobounds(xmean, lbounds, ubounds), varargin{:});
  % counteval = counteval + 1;
  
  % Cumulation: update evolution paths
  ps = (1-cs)*ps + (sqrt(cs*(2-cs)*mueff)) * (B*zmean);          % Eq. (16)
  hsig = norm(ps)/sqrt(1-(1-cs)^(2*countiter))/chiN < 1.5 + 1/(N-0.5);
  %% hsig = 1;
  pc = (1-cc)*pc ...
        + hsig*(sqrt(cc*(2-cc)*mueff)/sigma) * (xmean-xold);     % Eq. (14)
  if hsig == 0
    %%pc = pc + (sqrt(cc*(2-cc)*mueff)/sigma) * (xmean-xold);    
    %disp([num2str(countiter) ' ' num2str(counteval) ' pc update stalled']);
  end
  % remove momentum in ps, if ps is large and fitness is getting worse.
  % this should rarely happen. 
  if ~flgscience & sum(ps.^2)/N > 1.5 + 10*(2/N)^.5 & ...
       fitness.hist(2)<fitness.hist(1) & fitness.hist(3)<fitness.hist(1)
    ps = ps * sqrt(N*(1+max(0,log(sum(ps.^2)/N))) / sum(ps.^2));
    if flgdisplay
      disp(['Momentum removed at neval=' num2str(counteval)]);
    end
  end

  % Adapt covariance matrix
  if ccov > 0                                                    % Eq. (15)
    C = (1-ccov+(1-hsig)*ccov*cc*(2-cc)/mucov) * C ... % regard old matrix 
        + ccov * (1/mucov) * pc*pc' ...                % plus rank one update
        + ccov * (1-1/mucov) ...                       % plus rank mu update
          * sigma^-2 * (arx(:,fitness.idxsel(1:mu))-xold*ones(1,mu)) ...
          * diag(weights) * (arx(:,fitness.idxsel(1:mu))-xold*ones(1,mu))';
  end
  
  % Adapt sigma

  sigma = sigma * exp((norm(ps)/chiN - 1)*cs/damps);             % Eq. (17)
  
  % Update B and D from C
  if ccov > 0 & mod(countiter, 1/ccov/N/10) < 1
    C=triu(C)+triu(C,1)'; % enforce symmetry
    [B,D] = eig(C);       % eigen decomposition, B==normalized eigenvectors
    % limit condition of C to 1e14 + 1
    if max(diag(D)) > 1e14*min(diag(D)) 
	if stopOnWarnings
	  stopflag(end+1) = {'warnconditioncov'};
	else
	  warning(['Iteration ' num2str(countiter) ': condition of C ' ...
		   'at upper limit' ]);
	  tmp = max(diag(D))/1e14 - min(diag(D));
	  C = C + tmp*eye(N); D = D + tmp*eye(N); 
	end
    end
    D = diag(sqrt(diag(D))); % D contains standard deviations now
    % D = D / prod(diag(D))^(1/N);  % C = C / prod(diag(D))^(2/N);
    BD = B*D; % for speed up only
  end % if mod

  % ----- numerical error management -----
  % Adjust maximal coordinate axis deviations
  if any(sigma*sqrt(diag(C)) > maxdx)
    sigma = min(maxdx ./ sqrt(diag(C)));
    %warning(['Iteration ' num2str(countiter) ': coordinate axis std ' ...
    %         'deviation at upper limit of ' num2str(maxdx)]);
    % stopflag=1;
  end
  % Adjust minimal coordinate axis deviations
  while any(sigma*sqrt(diag(C)) < mindx)
    sigma = max(mindx ./ sqrt(diag(C))) * exp(0.05+cs/damps); 
    %warning(['Iteration ' num2str(countiter) ': coordinate axis std ' ...
    %         'deviation at lower limit of ' num2str(mindx)]);
    % stopflag=1;
  end
  % Adjust too low coordinate axis deviations
  if any(xmean == xmean + 0.2*sigma*sqrt(diag(C))) 
    if stopOnWarnings
	stopflag(end+1) = {'warnnoeffectcoord'};
    else
      warning(['Iteration ' num2str(countiter) ': coordinate axis std ' ...
	       'deviation too low' ]);
	C = C + ccov * diag(diag(C) .* ...
			    (xmean == xmean + 0.2*sigma*sqrt(diag(C))));
	sigma = sigma * exp(0.05+cs/damps); 
    end
  end
  % Adjust step size in case of (numerical) precision problem 
  if all(xmean == xmean ...
	    + 0.1*sigma*BD(:,1+floor(mod(countiter,N))))
    i = 1+floor(mod(countiter,N));
    if stopOnWarnings
	stopflag(end+1) = {'warnnoeffectaxis'};
    else
      warning(['Iteration ' num2str(countiter) ...
	       ': main axis standard deviation ' ...
	       num2str(sigma*D(i,i)) ' has no effect' ]);
	sigma = sigma * exp(0.2+cs/damps); 
    end
  end
  % Adjust step size in case of equal function values
  if fitness.sel(1) == ...
	  fitness.sel(min(end, 1+ceil(lambda/4)))
    if stopOnWarnings
	stopflag(end+1) = {'warnequalfunvals'};
    else
      warning(['Iteration ' num2str(countiter) ...
	       ': equal function values at maximal main axis sigma ' ...
	       num2str(sigma*max(diag(D)))]);
	sigma = sigma * exp(0.2+cs/damps); 
    end
  end
  % Adjust step size in case of equal function values
  if countiter > 2 & myrange([fitness.hist fitness.sel(1)]) == 0  
    if stopOnWarnings
	stopflag(end+1) = {'warnequalfunvalhist'};
    else
      warning(['Iteration ' num2str(countiter) ...
	       ': equal function values in history at maximal main ' ...
	       'axis sigma ' num2str(sigma*max(diag(D)))]);
	sigma = sigma * exp(0.2+cs/damps); 
    end
  end
  % Align scales of sigma and C for nicer output
  if 11 < 2 & (sigma > 1e10*max(diag(D)) | 1e10*sigma < min(diag(D)))
    fac = sqrt(sigma/median(diag(D)));
    sigma = sigma/fac;
    pc = fac * pc;
    C = fac^2 * C;
    D = fac * D;
    BD = fac * BD;
  end
    
  % ----- end numerical error management -----
  
  % Keep overall best solution
  if countiter == 1
    if fitness.hist(2) < fitness.hist(1)
	bestever.x = xold;
	bestever.f = fitness.hist(2);
	bestever.counteval = 1;
    else
	bestever.x = arxvalid(:, fitness.idx(1));
	bestever.f = fitness.hist(1);
	bestever.counteval = counteval + fitness.idx(1) - lambda;
    end
  elseif fitness.hist(1) < bestever.f
    bestever.x = arxvalid(:, fitness.idx(1));
    bestever.f = fitness.hist(1);
    bestever.counteval = counteval + fitness.idx(1) - lambda;
  end

  % Set stop flag
  if fitness.raw(1) <= stopFitness stopflag(end+1) = {'fitness'}; end
  if counteval >= stopMaxFunEvals stopflag(end+1) = {'maxfunevals'}; end
  if countiter >= stopMaxIter stopflag(end+1) = {'maxiter'}; end
  if sigma*(max(max(pc), sqrt(max(diag(C))))) < stopTolX 
    stopflag(end+1) = {'tolx'};
  end
  if sigma*max(D) == 0  % should never happen
    stopflag(end+1) = {'bug'};
  end
  if countiter > 2 & myrange([fitness.sel fitness.hist]) < stopTolFun 
    stopflag(end+1) = {'tolfun'};
  end
  if counteval >= stopFunEvals | countiter >= stopIter
    stopflag(end+1) = {'stoptoresume'};
    if length(stopflag) == 1 & flgsaving == 0
      error('To resume later the saving option needs to be set');
    end
  end

  % ----- output generation -----
  % ooo mod saves time and space; remove, if *all* data needed
  if mod(countiter, 1*ceil(savemodulo)) == 0 ...
	  | countiter < 1e2 | ~isempty(stopflag)
    % Compose output argument No 5
    outhist = [outhist; [counteval fitness.hist(1) ...
		    max(diag(D))/min(diag(D)) ...
		    sigma*sqrt(max(diag(C))) sigma*sqrt(min(diag(C))) ...
		    sqrt(max(diag(C)))]];
  end
  % ooo mod just saves time and space; remove, if *all* data needed
  if flgplotting % graphic output
	  myplot(bestever,countiter,inopts.MaxIter); % outplot defined below
  end
    
  if (flgsaving | flgplotting) ...
	  & (mod(countiter, 1*ceil(savemodulo)) == 0 ...
	     | countiter < 1e2 | ~isempty(stopflag))
    out.x = [out.x counteval];
    out.y1 = [out.y1; [fitness.raw(1) sigma max(diag(D))/min(diag(D))]];
    out.y2 = [out.y2; xmean'];
    out.y2a = [out.y2a; (arx(:,fitness.idx(1)))'];
    %out.y2 = [out.y2; (arxvalid(:,fitness.idx(1)))'];
    %out.y2a = [out.y2a; xmean'];
    out.y3 = [out.y3; sigma*sqrt(diag(C))'];
    out.y4 = [out.y4; sort(diag(D))']; 
    if etime(clock, outt0) > 10*outetime | ~isempty(stopflag) | ...
	    etime(clock, outt0) <= 0 % bug fix for etime
      outt0 = clock;
	xmin = arxvalid(:, fitness.idx(1));
	fmin = fitness.raw(1);
	if flgsaving
	  save(opts.SaveFileName); % for inspection and possible restart
	end
	%if flgplotting % graphic output
	%  myplot(bestever,countiter,inopts.MaxIter); % outplot defined below
	%end
	if 0 %ooo save some data for later graphic output with outplot
	  if ~exist('sfile', 'var') 
	    spath = '';
	    sfile = [spath 'cmaN' num2str(N,'%03d') 'on' fitfun '.mat'];
	    if exist(sfile, 'file') error(['File ' sfile ' already exists.']);
	    end
	  end
	  save(sfile,'counteval','fitness.raw','arx','out');
	end
	outetime = etime(clock, outt0); 
    end % if etime(...
  end % if output
  % ----- end output generation -----
  
end % while, end generation loop

% -------------------- Final Procedures -------------------------------

% Evaluate xmean and return best recent point in xmin
fmin = fitness.raw(1);
xmin = arxvalid(:, fitness.idx(1)); % Return best point of last generation.
if length(stopflag) > sum(strcmp(stopflag, 'stoptoresume')) % final stopping
  fmean = feval(fitfun, xintobounds(xmean, lbounds, ubounds), varargin{:});
  counteval = counteval + 1;
  if fmean < fitness.raw(1)
    fmin = fmean;
    xmin = xintobounds(xmean, lbounds, ubounds); % Return xmean as best point
  end
end

% Save everything and display final message
if flgsaving
  save(opts.SaveFileName);    % for inspection and possible restart
  message = [' (Everything saved in file ' opts.SaveFileName ')'];
else
  message = [];
end

if flgdisplay
  disp(['nEvals: f(returned x) | bestever.f | stopflag' message]);
  strstop = strcat(stopflag, '.');
  disp([num2str(counteval, '%7.0f') ': ' num2str(fmin) ' | ' ...
	num2str(bestever.f) ' | ' ...
	strstop{1:end}]);
  if exist('sfile', 'var') disp(['Results saved in ' sfile]); 
  end
end

% ---------------------------------------------------------------  
% ---------------------------------------------------------------  
function [x, idx] = xintobounds(x, lbounds, ubounds)
  if ~isempty(lbounds)
    idx = x < lbounds;
    if length(lbounds) == 1
      x(idx) = lbounds;
    else
      x(idx) = lbounds(idx);
    end
  end
  if ~isempty(ubounds)
    idx2 = x > ubounds;
    if length(ubounds) == 1
      x(idx2) = ubounds;
    else
      x(idx2) = ubounds(idx2);
    end
  end
  idx = idx2-idx;
  
% ---------------------------------------------------------------  
% ---------------------------------------------------------------  
function opts=getoptions(inopts, defopts)
% OPTS = GETOPTIONS(INOPTS, DEFOPTS) handles an arbitrary number of
% optional arguments to a function. The given arguments are collected
% in the struct INOPTS.  GETOPTIONS matches INOPTS with a default
% options struct DEFOPTS and returns the merge OPTS.  Empty or missing
% fields in INOPTS invoke the default value.  Fieldnames in INOPTS can
% be abbreviated.
%
% The returned struct OPTS is first assigned to DEFOPTS. Then any
% field value in OPTS is replaced by the respective field value of
% INOPTS if (1) the field unambiguously (case-insensitive) matches
% with the fieldname in INOPTS (cut down to the length of the INOPTS
% fieldname) and (2) the field is not empty.
% 
% Example:
%   In the source-code of the function that needs optional
%   arguments, the last argument is the struct of optional
%   arguments:
%
%   function results = myfunction(mandatory_arg, inopts)
%     % Define four default options
%     defopts.PopulationSize = 200;
%     defopts.ParentNumber = 50;
%     defopts.MaxIterations = 1e6;
%     defopts.MaxSigma = 1;
%  
%     % merge default options with input options
%     opts = getoptions(inopts, defopts);
%
%     % Thats it! From now on the values in opts can be used
%     for i = 1:opts.PopulationSize
%       % do whatever
%       if sigma > opts.MaxSigma
%         % do whatever
%       end
%     end
%   
%   For calling the function myfunction with default options:
%   myfunction(argument1, []);
%   For calling the function myfunction with modified options:
%   opt.pop = 100; % redefine PopulationSize option
%   opt.PAR = 10;  % redefine ParentNumber option
%   opt.maxiter = 2; % opt.max=2 is ambiguous and would result in an error 
%   myfunction(argument1, opt);
%

if nargin < 2 | isempty(defopts) % no default options available
  opts=inopts;
  return;
elseif isempty(inopts) % empty inopts invoke default options
  opts = defopts;
  return;
elseif ~isstruct(defopts) % handle a single option value
  if isempty(inopts) 
    opts = defopts;
  elseif ~isstruct(inopts)
    opts = inopts;
  else
    error('The default options need to be a struct or empty');
  end
  return;
elseif ~isstruct(inopts) % no valid input options
  error('The options need to be a struct or empty');
end

  opts = defopts; % start from defopts 
  % if necessary overwrite opts fields by inopts values
  defnames = fieldnames(defopts);
  idxmatched = []; % indices of defopts that already matched
  for name = fieldnames(inopts)'
    name = name{1}; % name of i-th inopts-field
    idx = strncmp(lower(defnames), lower(name), length(name));
    if sum(idx) > 1
      error(['option "' name '" is not an unambigous abbreviation. ' ...
	     'Use opts=RMFIELD(opts, ''' name, ...
	     ''') to remove the field from the struct.']);
    end
    if sum(idx) == 1
      if ismember(find(idx), idxmatched)
	error(['input options match more than once with "' ...
	       defnames{find(idx)} '". ' ...
	       'Use opts=RMFIELD(opts, ''' name, ...
	       ''') to remove the field from the struct.']);
      end
      idxmatched = [idxmatched find(idx)];
      val = getfield(inopts, name);
      % next line replaces previous line from MATLAB version 6.5.0 on
      % val = inopts.(name);
      if ~isempty(val) % empty value: do nothing, i.e. stick to default
	opts = setfield(opts, defnames{find(idx)}, val);
	% next line replaces previous line from MATLAB version 6.5.0 on
	% opts.(defnames{find(idx)}) = inopts.(name); 
      end
    else
      warning(['option "' name '" disregarded (unknown field name)']);
    end
  end

% ---------------------------------------------------------------  
% ---------------------------------------------------------------  
function res=myeval(s)
  if ischar(s)
    res = evalin('caller', s);
  else
    res = s;
  end
  
% ---------------------------------------------------------------  
% ---------------------------------------------------------------  
function res=myevalbool(s)
  if ~ischar(s) % s may not and cannot be empty
    res = s;
  else % evaluation string s
    if strncmp(lower(s), 'yes', 3) | strncmp(lower(s), 'on', 2) ...
	  | strncmp(lower(s), 'true', 4) | strncmp(s, '1 ', 2)
      res = 1;
    elseif strncmp(lower(s), 'no', 2) | strncmp(lower(s), 'off', 3) ...
	  | strncmp(lower(s), 'false', 5) | strncmp(s, '0 ', 2)
      res = 0;
    else
      try res = evalin('caller', s); catch
	error(['String value "' s '" cannot be evaluated']);
      end
      try res ~= 0; catch
	error(['String value "' s '" cannot be evaluated reasonably']);
      end
    end
  end
  
% ---------------------------------------------------------------  
% ---------------------------------------------------------------  
% ---------------------------------------------------------------  
% ---------------------------------------------------------------  
function myplot(bestever,iteration,maxiteration)
     
     global f1 f2 mseArray
     mseArray(iteration)= bestever.f;
     save('cmaes_scara_mse.txt','-ascii','mseArray'); 
     subplot(f2);     
     hold on
     plot(iteration,bestever.f,'.k');
     disp(['Iteration: ',int2str(iteration),' Best Fitness ',num2str(bestever.f)])
     ylabel('fitness');          
     title([' mse: ',num2str(bestever.f),' Iteration:',num2str(iteration)])
     drawnow;     
     subplot(f1);    

 
  

  
% ---------------------------------------------------------------  
% ---------------------------------------------------------------
function outplot(o)
  figure(324);
  foffset = 1e-99;
  dfit = o.y1(:,1)-min(o.y1(:,1)); 
  dfit(find(dfit<1e-98)) = NaN;
  subplot(2,2,1);semilogy(o.x,dfit,'-c');hold on;
  idx = find(o.y1(:,1)>1e-98);  % positive values
  subplot(2,2,1);semilogy(o.x(idx), o.y1(idx,1)+foffset, '.b'); hold on; 
  idx = find(o.y1(:,1) < -1e-98);  % negative values
  subplot(2,2,1);semilogy(o.x(idx), abs(o.y1(idx,1))+foffset,'.r');hold on; 
  subplot(2,2,1);semilogy(o.x,abs(o.y1(:,1))+foffset,'-b'); hold on;
  subplot(2,2,1);semilogy(o.x,(o.y1(:,2)),'-g'); hold on;
  subplot(2,2,1);semilogy(o.x,(o.y1(:,3:end)),'-r'); hold off;
  title('abs(f) (blue), f-min(f) (cyan), Sigma (green), Axis Ratio (red)');  
  grid on; zoom on; 
  subplot(2,2,2); plot(o.x, o.y2,'-');
  title('Object Variables');grid on;zoom on; 
  subplot(2,2,3); semilogy(o.x, o.y3, '-'); 
  title('Standard Deviations in All Coordinates');grid on;zoom on; 
  subplot(2,2,4); semilogy(o.x, o.y4, '-');
  title('Scaling (All Main Axes)');grid on;zoom on; drawnow;

  
% ---------------------------------------------------------------  
% ---------------------------------------------------------------  
function res=myrange(x)
  res = max(x) - min(x);
  
% ---------------------------------------------------------------  
% --------------- OBJECTIVE TEST FUNCTIONS ----------------------  
% ---------------------------------------------------------------  

%%% Unimodal functions

function f=fsphere(x)
  f=sum(x.^2);
  
function f=frandsphere(x)
  N = size(x,1);
  idx = ceil(N*rand(7,1));
  f=sum(x(idx).^2);

function f=fspherelb0(x, M) % lbound at zero for 1:M needed
  if nargin < 2 M = 0; end
  N = size(x,1);
  % M active bounds, f_i = 1 for x = 0
  f = -M + sum((x(1:M) + 1).^2);
  f = f + sum(x(M+1:N).^2);
  
function f=fspherehull(x)
  % Patton, Dexter, Goodman, Punch
  % in -500..500
  % spherical ridge through zeros(N,1)
  % worst case start point seems x = 2*100*sqrt(N)
  % and small step size
  N = size(x,1);
  f = norm(x) + (norm(x-100*sqrt(N)) - 100*N)^2;
  
function f=fellilb0(x, M) % lbound at zero for 1:M needed
  N = size(x,1);
  scale=1.^((0:N-1)/(N-1));
  %scale(N) = 1e0;
  % M active bounds
  x(1:M) = x(1:M) + 1;
  f = scale.^2*x.^2;
  f = f - sum(scale(1:M).^2); 
  
function f=fcornersphere(x)
  w = ones(size(x,1));
  w(1) = 2.5; w(2)=2.5;
  idx = x < 0;
  f = sum(x(idx).^2);
  idx = x > 0;
  f = f + 2^2*sum(w(idx).*x(idx).^2);
  
function f=fsectorsphere(x, scal)
  if nargin < 2 | isempty (scal)
    scal = 1e3;
  end
  f=sum(x.^2);
  idx = find(x<0);
  f = f + (scal-1)^2 * sum(x(idx).^2);
  
function f=fstepsphere(x)
  f=1e-11+sum(floor(x+0.5).^2);

function f=fstep(x)
  % in -5.12..5.12 (bounded)
  N = size(x,1);
  f=1e-11+6*N+sum(floor(x));

function f=fneumaier3(x) 
  % in -n^2..n^2
  % x^*-i = i(n+1-i)
  N = size(x,1);
%  f = N*(N+4)*(N-1)/6 + sum((x-1).^2) - sum(x(1:N-1).*x(2:N));
  f = sum((x-1).^2) - sum(x(1:N-1).*x(2:N));
  
function f=fchangingsphere(x)
  N = size(x,1);
  global scale_G; global count_G; if isempty(count_G) count_G=-1; end
  count_G = count_G+1;
  if mod(count_G,10) == 0
    scale_G = 10.^(2*rand(1,N));
  end
  %disp(scale(1));
  f = scale_G*x.^2;
  
function f= flogsphere(x)
 f = 1-exp(-fsphere(x));
  
function f=fbaluja(x)
  % in [-0.16 0.16]
  y = x(1);
  for i = 2:length(x)
    y(i) = x(i) + y(i-1);
  end
  f = 1e5 - 1/(1e-5 + sum(abs(y)));

function f=fschwefel(x)
  f = 0;
  for i = 1:size(x,1),
    f = f+sum(x(1:i))^2;
  end

function f=fcigar(x)
  f = x(1)^2 + 1e6*sum(x(2:end).^2);
  
function f=fcigtab(x)
  f = x(1)^2 + 1e8*x(end)^2 + 1e4*sum(x(2:(end-1)).^2);
  
function f=ftablet(x)
  f = 1e6*x(1)^2 + sum(x(2:end).^2);
  
function f=felli(x, lgscal, expon, expon2)
  % lgscal: log10(axis ratio)
  % expon: x_i^expon, sphere==2
  N = size(x,1); if N < 2 error('dimension must be greater one'); end
  if nargin < 2 | isempty(lgscal), lgscal = 3; end
  if nargin < 3 | isempty(expon), expon = 2; end
  if nargin < 4 | isempty(expon2), expon2 = 1; end

  f=((10^(lgscal*expon)).^((0:N-1)/(N-1)) * abs(x).^expon)^(1/expon2);

function f=fellii(x, scal)
  N = size(x,1); if N < 2 error('dimension must be greater one'); end
  if nargin < 2
    scal = 1;
  end
  f= (scal*(1:N)).^2 * (x).^2;

function f=fellirot(x)
  N = size(x,1);
  global ORTHOGONALCOORSYSTEM_G
  if isempty(ORTHOGONALCOORSYSTEM_G) ...
	| length(ORTHOGONALCOORSYSTEM_G) < N
    coordinatesystem(N);
  end
  f = felli(ORTHOGONALCOORSYSTEM_G{N}*x);
  
function coordinatesystem(N)
  global ORTHOGONALCOORSYSTEM_G
  for N = 2:30
    ar = randn(N,N);
    for i = 1:N 
      for j = 1:i-1
	ar(:,i) = ar(:,i) - ar(:,i)'*ar(:,j) * ar(:,j);
      end
      ar(:,i) = ar(:,i) / norm(ar(:,i));
    end
    ORTHOGONALCOORSYSTEM_G{i} = ar; 
  end

function f=fplane(x)
  f=x(1);

function f=ftwoaxes(x)
  f = sum(x(1:floor(end/2)).^2) + 1e6*sum(x(floor(1+end/2):end).^2);

function f=fparabR(x)
  f = -x(1) + 100*sum(x(2:end).^2);

function f=fsharpR(x)
  f = -x(1) + 100*norm(x(2:end));
  
function f=frosen(x)
  if size(x,1) < 2 error('dimension must be greater one'); end
  f = 100*sum((x(1:end-1).^2 - x(2:end)).^2) + sum((x(1:end-1)-1).^2);

function f=frosenmodif(x)
  f = 74 + 100*(x(2)-x(1)^2)^2 + (1-x(1))^2 ...
      - 400*exp(-sum((x+1).^2)/2/0.05);
  
function f=fschwefelrosen1(x)
  % in [-10 10] 
  f=sum((x.^2-x(1)).^2 + (x-1).^2);
  
function f=fschwefelrosen2(x)
  % in [-10 10] 
  f=sum((x(2:end).^2-x(1)).^2 + (x(2:end)-1).^2);

function f=fdiffpow(x)
  N = size(x,1); if N < 2 error('dimension must be greater one'); end
  f=sum(abs(x).^(2+10*(0:N-1)'/(N-1)));

%%% Multimodal functions 

function f=fackley(x)
  % -32.768..32.768
  % Adding a penalty outside the interval is recommended,  
  % because for large step sizes, fackley imposes like frand
  % 
  N = size(x,1); 
  f = 20-20*exp(-0.2*sqrt(sum(x.^2)/N)); 
  f = f + (exp(1) - exp(sum(cos(2*pi*x))/N));
  % add penalty outside the search interval
  f = f + sum((x(x>32.768)-32.768).^2) + sum((x(x<-32.768)+32.768).^2);
  
function f = fbohachevsky(x)
 % -15..15
  f = sum(x(1:end-1).^2 + 2 * x(2:end).^2 - 0.3 * cos(3*pi*x(1:end-1)) ...
	  - 0.4 * cos(4*pi*x(2:end)) + 0.7);
  
function f=fconcentric(x)
  % in  +-600
  s = sum(x.^2);
  f = s^0.25 * (sin(50*s^0.1)^2 + 1);

function f=fgriewank(x)
  % in [-600 600]
  N = size(x,1);
  f = 1 - prod(cos(x'./sqrt(1:N))) + sum(x.^2)/4e3;
  
function f=frastrigin(x, scal, skewfac, skewstart, amplitude)
  if nargin < 5 | isempty(amplitude)
    amplitude = 10;
  end
  if nargin < 4 | isempty(skewstart)
    skewstart = 0;
  end
  if nargin < 3 | isempty(skewfac)
    skewfac = 1;
  end
  if nargin < 2 | isempty(scal)
    scal = 1;
  end
  N = size(x,1); 
  scale = 1;
  if N > 1
    scale=scal.^((0:N-1)'/(N-1));
  end
  f = amplitude*(N - sum(cos(2*pi*(scale.*x)))) + sum((scale.*x).^2);
  idx = find(x > skewstart);
  y = scale(idx).*x(idx);
  f = amplitude*(N - sum(cos(2*pi*y))) + (sum(y.^2));
  idx = find(x < skewstart);
  y = skewfac*scale(idx).*x(idx);
  f = f - amplitude*sum(cos(2*pi*y)) + sum(y.^2);

function f = fschaffer(x)
 % -100..100
  N = size(x,1);
  s = x(1:N-1).^2 + x(2:N).^2;
  f = sum(s.^0.25 .* (sin(50*s.^0.1).^2+1));

function f=fschwefelmult(x)
  % -500..500
  N = size(x,1); 
  f = 418.9829*N - 1.27275661e-5*N - sum(x.*sin(sqrt(abs(x))));
  f = f + sum(x(abs(x)>500).^2);
  
function f=ftwomax(x)
  % Boundaries at +/-5
  N = size(x,1); 
  f = -abs(sum(x)) + 5*N;

function f=ftwomaxtwo(x)
  % Boundaries at +/-10
  N = size(x,1); 
  f = abs(sum(x));
  if f > 30
    f = f - 30;
  end
  f = -f;

function f=frand(x)
  f=1/(1-rand) - 1;

  
% Changes: 
% 04/03: Stopflag becomes cell-array. 
% 04/04: Parameter cs depends on mueff now and damps \propto sqrt(mueff)
%        instead of \propto mueff. 
%      o Initial stall to adapt C (flginiphase) is removed and
%        adaptation of pc is stalled for large norm(ps) instead.
%      o Returned default options include documentation. 
%      o Resume part reorganized.
% 04/05: Call of function range (works with stats toolbox only) 
%        changed to myrange. 
% 04/05: Bug in boundary handling removed: A large initial SIGMA was
%        corrected not until *after* the first iteration, which could
%        lead to a complete failure.
% 04/05: damps further tuned for large mueff 
%      o Details for stall of pc-adaptation added (variable hsig 
%        introduced). 

% ---------------------------------------------------------------
% CMA-ES: Evolution Strategy with Covariance Matrix Adaptation for
% nonlinear function minimization. To be used under the terms of the
% GNU General Public License (http://www.gnu.org/copyleft/gpl.html).
% Author: Nikolaus Hansen, 2001/3. e-mail: hansen@bionik.tu-berlin.de
% URL:http://www.bionik.tu-berlin.de/user/niko
% References: See below. 
% ---------------------------------------------------------------
%
% GENERAL PURPOSE: The CMA-ES (Evolution Strategy with Covariance
% Matrix Adaptation) is a robust search method which should be
% applied, if derivative based methods, e.g. quasi-Newton BFGS or
% conjucate gradient, (supposably) fail due to a rugged search
% landscape (e.g. noise, local optima, outlier, etc.). On smooth
% landscapes CMA-ES is roughly ten times slower than BFGS. For up to
% N=10 variables even the simplex direct search method (Nelder & Mead)
% is often faster, but far less robust than CMA-ES.  To see the
% advantage of the CMA, it will usually take at least 30*N and up to
% 300*N function evaluations, where N is the search problem dimension.
% On considerably hard problems the complete search (a single run) is
% expected to take at least 30*N^2 and up to 300*N^2 function
% evaluations.
%
% SOME MORE COMMENTS: 
% The adaptation of the covariance matrix (e.g. by the CMA) is
% equivalent to a general linear transformation of the problem
% coding. Nevertheless every problem specific knowlegde about the best
% linear transformation should be exploited before starting the
% search. That is, an appropriate a priori transformation should be
% applied to the problem. This also makes the identity matrix as
% initial covariance matrix the best choice.
%
% The strategy parameter lambda (population size, opts.PopSize) is the
% preferred strategy parameter to play with.  If results with the
% default strategy are not satisfactory, increase the population
% size. (Remark that the crucial parameter mu (opts.ParentNumber) is
% increased proportionally to lambda). This will improve the
% strategies capability of handling noise and local minima. We
% recomment successively increasing lambda by a factor of about three,
% starting with initial values between 5 and 20. Casually, population
% sizes even beyond 1000+100*N can be sensible.
%
%
% ---------------------------------------------------------------
% REFERENCES: The equation numbers refer to Hansen, N. and
% A. Ostermeier (2001). Completely Derandomized Self-Adaptation in
% Evolution Strategies. Evolutionary Computation, 9(2), pp. 159-195.
% The modification of Eq. (15) corresponds to Eq. (11) in Hansen, N.,
% S.D. Mueller and P. Koumoutsakos (2003). Reducing the Time Complexity
% of the Derandomized Evolution Strategy with Covariance Matrix
% Adaptation (CMA-ES). Evolutionary Computation, 11(1).
%


