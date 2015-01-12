function [s, c, x, S, S2, history] = ...
    ieks(x, net, tnet, iters, init_s, u, R, Q, constraints, debug),
% IEKS  Run Iterated Extended Kalman Smoother
%
%  [s, c] = IEKS(data, net, tnet, iters, s, u, R, Q)
%  [s, c] = IEKS(data, net, tnet, iters, s, u, R, Q, constraints)
%  [s, c] = IEKS(data, net, tnet, iters, s, u, params)
%  [s, c, x, S, S2, h] = IEKS(data, net, tnet, iters, s, u, R, Q, constraints)
%
%  Runs the Iterated Extended Kalman Smoother for the given data x (use NaNs 
%  for missing values). Sources are initialized to s and control to
%  u (use empty u for models with no control channels). If only
%  some of the initial sources are provided, the rest are predicted
%  during the first iteration. Error covariance parameters R and Q
%  can be either be provided directly or in a params structure
%  returned by the NDFA package. If provided, min-max constraints
%  will be forced after each sweep.
%
%  Return values are the sources s, LLH cost c and reconstructed
%  observations x. For debugging reasons, cell arrays S and S2 are
%  also available containing the sources after each
%  forward/backward sweep.
%
%  This function only uses the mean values of the sources, for
%  probdist sources use IEKS2.

% Copyright (C) 2004-2005 Matti Tornio
%
% This package comes with ABSOLUTELY NO WARRANTY; for details
% see License.txt in the program package.  This is free software,
% and you are welcome to redistribute it under certain conditions;
% see License.txt for details.

% Extract parameters
[Ny T] = size(x);
[Ns] = size(tnet.w1.e,2);

if nargin == 2,
  r = net;
  net = r.net;
  tnet = r.tnet;
  iters = 1;
  Nu = r.status.controlchannels;
  init_s = r.sources.e(1:end-Nu,:);
  u = r.sources.e(end-Nu+1:end,:);
  R = r.params;
end

% Iterate for 10 sweeps by default
if nargin < 4,
  iters = 10;
end

% Check if model has control channels
if nargin < 6 | isempty(u),
  Nu = 0;
  u = zeros(0, T);
else
  Nu = size(u, 1);
  controls = (Ns - Nu + 1):Ns;
  Ns = Ns - Nu;
end

% By default initialize sources to all zeros
if nargin < 5 | isempty(init_s),
  init_s = zeros(Ns, T);
end

% Use the provided noise parameters
if nargin == 7,
  params = R;
  R = diag(normalvar(params.noise));
  Q = diag(normalvar(params.src(1:Ns)));
  fQ = diag(normalvar(params.src));
else
  fQ = Q;
  Q = Q(1:Ns, 1:Ns);
end

if nargin < 9,
  constraints = [];
else
  constraints(find(isnan(constraints(:,1))), 1) = 10e100;
  constraints(find(isnan(constraints(:,2))), 2) = -10e100;
end

if nargin < 10,
  debug = 0;
end

% Find missing values and replace them with zeroes
missing = sparse(isnan(x));
x(find(missing)) = 0;

% Calculate llh with prediction only from the initial source(s)
if debug >= 2,
  [llh_pred, llh_pred_x, llh_pred_s] = loglikelihood(...
      x, [init_s; u], R, fQ, net, tnet, missing, []);
  fprintf('Initial llh function:       %f=%f%f.\n', ...
	  llh_pred, llh_pred_x, llh_pred_s);
  history(1) = llh_pred;
end

% Start actual IEKS
for l=1:iters,
  % Initialization for the first iteration
  if l == 1,
    S{l} = init_s;
    P_{1} = Q;
  end

  % Estimate the prediction for the first source
  s_(:,1) = S{l}(:,1); % Should use this unless initial estimate is good
%  s_(:,1) = s_pred(:,1);
  P_{1} = Q;

  % Calculate Jacobians at the current estimated states
  Fd = dtnet([S{l}; u], tnet);
  Hd = dnet([S{l}; u], net);
  for k = 1:T,
    F{k} = Fd(1:Ns,1:Ns,k);
    H{k} = Hd(:,1:Ns,k);
  end
  
  % Forward pass (Filter)
  
  talpha = ones(Ns, 1);
  
  for k = 1:T,
    % Calculate current error for observation and temporal mapping
    misfit = x(:,k) - feedfwfast([S{l}(:,k); u(:,k)], net);
    misfit(find(missing(:,k))) = 0;
    tmisfit = s_(:,k) - S{l}(:,k);

    % Combined misfit
    e{k} = misfit - H{k}*tmisfit;
   
    % Update error covariance
    P{k} = P_{k} - P_{k}*H{k}'*inv(H{k}*P_{k}*H{k}'+R)*H{k}*P_{k};

    % Correct the new estimate
    s(:,k) = s_(:,k) + P{k}*H{k}'*inv(R)*e{k};

    % Calculate new error for observation and temporal mapping
    %misfit2 = x(:,k) - feedfwfast([s(:,k); u(:,k)], net);
    %misfit2(find(missing(:,k))) = 0;
    %tmisfit2 = s(:,k) - S{l}(:,k);
    %e2 = misfit2 - H{k}*tmisfit2;

    tmisfit = s(1:Ns,k) - S{l}(1:Ns,k);
    if k<T,
      % Predict the next state and error covariance
      s_temp = feedfwfast([S{l}(:,k); u(:,k)], tnet);

      s_temp = S{l}(:,k) + s_temp(1:Ns);
      s_(:,k+1) =  s_temp(1:Ns,:) + F{k}*tmisfit;
      P_{k+1} = F{k}*P{k}*F{k}'+Q;
      % Force constraints
      if ~isempty(constraints),
	s_(:,k+1) = cut(s_(:,k+1), constraints(:,1), constraints(:,2));
      end
    end
    
    % Store history for debugging
    if debug >= 4,
      misfit_hist(:,k) = misfit - misfit2;
      tmisfit_hist(:,k) = tmisfit - tmisfit2;;
      emisfit_hist(:,k) = e{k} - e2;
    end
  end

  if debug >= 4,
    figure(10+l), clf;
    subplot(3,1,1), plot(misfit_hist');
    subplot(3,1,2), plot(tmisfit_hist');
    subplot(3,1,3), plot(emisfit_hist');
  end

    
  S2{l+1} = s;
  % If debugging information is enabled, compute and display llh
  % after forward sweep
  if debug >= 3,
    [llh, llh_x, llh_s] = ...
        loglikelihood(x, [S2{l+1}; u], R, fQ, net, tnet, missing, []);
    fprintf('llh function after fwd sweep %d: %f=%f%f.\n', ...
            round(l), llh, llh_x, llh_s);
  end

  % Backward pass (Smooth)
  lambda = zeros(Ns, T);
  sigma = cell(T, 1);
  sigma{T} = zeros(Ns);
  for k=T:-1:1,
    S{l+1}(:,k) = s(:,k) - P{k}*F{k}'*lambda(:,k);

    if ~isempty(constraints),
      S{l+1}(:,k) = cut(S{l+1}(:,k), constraints(:,1), constraints(:,2));
    end
    
    S_{k} = H{k}'*inv(R)*H{k};
    if k>1, 
      lambda(:,k-1) = (eye(Ns) - P{k}*S_{k})' * ...
	  	      (F{k}'*lambda(:,k)-H{k}'*inv(R)*e{k});
    end
    P_{k} = P{k}-P{k}*F{k}'*sigma{k}*F{k}*P{k};
    if k>1,
      sigma{k-1} = (eye(Ns)-P{k}*S_{k})'*F{k}'*sigma{k}*F{k}* ...
		   (eye(Ns)-P{k}*S_{k})+S_{k}*(eye(Ns)-P{k}*S_{k});
    end
  end

  % Calculate difference between the latest two samples
  change = max(sum((S{l+1} - S{l}).^2));
  if l>1,
    change2 = max(sum((S{l+1} - S{l-1}).^2));
    change = min(change, change2);
  end

  % Check for convergence
  if l < iters && change < 1E-5,
    llh = loglikelihood(x, [S{l+1}; u], R, fQ, net, tnet, missing, []);
    fprintf('Converged at iteration %d: %f.\n', round(l), llh);
    s = S{l+1};
    c = llh;
    break;
  end

  % Check if iteration count has been reached
  if l == iters,
    s = S{l+1};
    
%    [llh, llh_x, llh_s] = loglikelihood(...
%	x, [S{l+1}; u], R, fQ, net, tnet, missing, []);
%    c = llh;
%    if debug,
%      fprintf('Finally after %d iterations:     %f.\n', round(l), llh);
%    end
%    history(l+1) = llh;
     % Convergence failure
     llh = nan;
  elseif debug,
%    [llh, llh_x, llh_s] = loglikelihood(...
%	x, [S{l+1}; u], R, fQ, net, tnet, missing, []);
%    fprintf('llh function after bwd sweep %d: %f=%f%f.\n', ...
%            round(l), llh, llh_x, llh_s);
%    history(l+1) = llh;
  end
end

s = [s; u];

c = llh;

% Reconstruct observations
if nargout > 2,
  x = feedfwfast(s, net);
end


function x = feedfwfast(s, net),
% FEEDFWFAST Do feedforward with mean values only
 
filler = ones(1, size(s, 2));

x = net.w1.e*s + net.b1.e * filler;

x = feval(net.nonlin, x);
x = net.w2.e*x + net.b2.e * filler;