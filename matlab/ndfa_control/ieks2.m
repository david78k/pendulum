function [s, c, x, S, S2, history] = ...
    ieks2(x, net, tnet, iters, init_s, u, R, Q),
% IEKS  Run Iterated Extended Kalman Smoother for probdist sources
%
%  [s, x] = IEKS2(data, net, tnet, iters, s, u, R, Q)
%  [s, x] = IEKS2(data, net, tnet, iters, s, u, params)
%
%  Runs the Iterated Extended Kalman Smoother for the given data x (use NaNs 
%  for missing values). Sources are initialized to s and control to
%  u (use empty u for models with no control channels). If only
%  some of the initial sources are provided, the rest are predicted
%  during the first iteration. Error covariance parameters R and Q
%  can be either be provided directly or in a params structure
%  returned by the NDFA package.
%
%  This function works with probdist sources, for mean values only,
%  use IEKS. Please also note that constraints are only supported
%  by IEKS.

% Copyright (C) 2004-2005 Matti Tornio
%
% This package comes with ABSOLUTELY NO WARRANTY; for details
% see License.txt in the program package.  This is free software,
% and you are welcome to redistribute it under certain conditions;
% see License.txt for details.

c0 = 0;
c1 = 0;
c2 = 0;
c3 = 0;
c4 = 0;
c_temp = cputime;

% Set level of debug information
debug = 2;

% Extract parameters
[Ny T] = size(x);
[Ns] = size(tnet.w1.e,2);

% Iterate for 10 sweeps by default
if nargin < 4,
  iters = 10;
end

% Check if model has control channels
if nargin < 6 | isempty(u),
  Nu = 0;
  u = acprobdist_alpha(zeros(0, T));
else
  Nu = size(u, 1);
  controls = (Ns - Nu + 1):Ns;
  Ns = Ns - Nu;
end

% By default initialize sources to all zeros
if nargin < 5 | isempty(init_s),
  init_s = acprobdist_alpha(zeros(Ns, T), .0001*ones(Ns, T));
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

% Find missing values and replace them with zeroes
missing = sparse(isnan(x));
x(find(missing)) = 0;

% Calculate llh with prediction only from the initial source(s)
if debug,
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
    S{l} = init_s.e;
    P_{1} = Q;
  end

  % Estimate the prediction for the first source
  s_(:,1) = S{l}(:,1);
  P_{1} = Q;

  % Calculate Jacobians at the current estimated states
  Fd = feedfw([init_s; u], tnet);
  %Fd = acfeedfw([init_s; u], tnet);
  %Fd(:,T) = feedfw([init_s(:,T); u(:,T)], tnet);
  Hd = feedfw([init_s; u], net);
  for k = 1:T,
    F{k} = Fd{4}.multi(1:Ns,1:Ns,k) + eye(Ns);
%    F{k} = Fd{4}.multi(1:Ns,1:Ns,k);
    H{k} = Hd{4}.multi(:,1:Ns,k);
  end

  c0 = cputime - c_temp;
  
  % Forward pass (Filter)
  for k = 1:T,
    % Calculate current error for observation and temporal mapping
    init_s.e(:,k) = S{l}(:,k);

    c_temp = cputime;
    x_ = feedfwfast([init_s(:,k); u(:,k)], net);

    %H{k} = x_{4}.multivar;

    misfit = x(:,k) - x_;
    misfit(find(missing(:,k))) = 0;
    tmisfit = s_(:,k) - S{l}(:,k);
    e{k} = misfit - H{k}*tmisfit;

    % Update error covariance
    P{k} = P_{k} - P_{k}*H{k}'*inv(H{k}*P_{k}*H{k}'+R)*H{k}*P_{k};
    
    % Correct the new estimate
    s(:,k) = s_(:,k) + P{k}*H{k}'*inv(R)*e{k};
    
    
    % Calculate new error for observation and temporal mapping
    init_s.e(:,k) = s(:,k);
    c_temp = cputime;
    x_ = feedfwfast([init_s(:,k); u(:,k)], net);
    c2 = c2 + (cputime - c_temp);
    misfit2 = x(:,k) - x_;
    misfit2(find(missing(:,k))) = 0;
    tmisfit2 = s(:,k) - S{l}(:,k);
    e2 = misfit2 - H{k}*tmisfit2;

    % Predict the next state and error covariance
    if k<T,
      tmisfit = s(1:Ns,k) - S{l}(1:Ns,k);
      init_s.e(:,k) = S{l}(:,k);
      c_temp = cputime;
      %s_temp = feedfwfast([init_s(:,k); u(:,k)], tnet);
      %s_temp = S{l}(:,k) + s_temp(1:Ns,:);

      % s_temp = acfeedfw([init_s(:,k:k+1); u(:,k:k+1)], tnet);
      %F{k} = s_temp{4}.multivar(1:Ns,1:Ns);
      %F{k} = s_temp{5}(1:Ns,1:Ns);

      s_test = acfeedfw([init_s(:,k:k+1); u(:,k:k+1)], tnet);
      c3 = c3 + (cputime - c_temp);
      %[s_test{4}.var init_s.var(:,k+1)]
      %[s_test{4}.e s_temp]
      
      %s_(:,k+1) =  s_temp + F{k}*tmisfit;
      s_(:,k+1) =  s_test{4}.e + F{k}*tmisfit;
      P_{k+1} = F{k}*P{k}*F{k}'+Q;
      P{k+1} = P{k};
    else
      %F{k} = dtnet(s(:,k), tnet);
    end
  end

  c_temp = cputime;

  % If debugging information is enabled, calculate llh after forward sweep
  if debug >= 2,
    [llh, llh_x, llh_s] = ...
        loglikelihood(x, [s; u.e], R, fQ, net, tnet, missing, []);
    fprintf('llh function after fwd sweep %d: %f=%f%f.\n', ...
            round(l), llh, llh_x, llh_s);
  end
  
  % Backward pass (Smooth)
  lambda = zeros(Ns, T);
  sigma = cell(T, 1);
  sigma{T} = zeros(Ns);
  for k=T:-1:1,
    S{l+1}(:,k) = s(:,k) - P{k}*F{k}'*lambda(:,k);
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

  % Check if iteration count has been reached
  if l == iters,
    s = S{l+1};
  end
  c4 = cputime - c_temp;

  % If debugging information is enabled, calculate llh after forward sweep
  if debug >= 2,
    [llh, llh_x, llh_s] = ...
        loglikelihood(x, [S{l+1}; u.e], R, fQ, net, tnet, missing, []);
    fprintf('llh function after bwd sweep %d: %f=%f%f.\n', ...
            round(l), llh, llh_x, llh_s);
  end
  
end

init_s.e = s;
s = [init_s; u];

%[c0 c1 c2 c3 c4 c1+c2+c3]

% Reconstruct observations
if nargout > 1,
  x = feedfw(s, net);
end



function x = feedfwfast_mean(s, net),
% FEEDFWFAST Simplified and faster version of the feedforward
% 
%  Uses probdist input, but outputs only means

filler = ones(1, size(s, 2));

x = net.w1.e*s + net.b1.e * filler;
x = feval(net.nonlin, x);
x = net.w2.e*x + net.b2.e * filler;




function x = feedfwfast(s, net),
% FEEDFWFAST Simplified and faster version of the feedforward
% 
%  Uses probdist input, but outputs only means

x = cell(1, 4);

sdim = size(s, 1);
tdim = size(s, 2);
hdim = size(net.w1, 1);
odim = size(net.w2, 1);

% Initialisation:         x{1} = s
x{1}.e = s.e;
x{1}.var = s.var;
x{1}.multi = repmat(eye(sdim), [1 1 tdim]);

% First linear layer:     x{2} = A*x{1} + a
x{2}.e = net.w1.e * x{1}.e + repmat(net.b1.e, [1, tdim]);
x{2}.extra = net.w1.var * (x{1}.e.^2 + x{1}.var) + ...
    repmat(net.b1.var, [1, tdim]);
x{2}.multi = reshape(net.w1.e * reshape(x{1}.multi, [sdim sdim*tdim]), ...
		     [hdim sdim tdim]);
x{2}.var = x{2}.extra + net.w1.e.^2 * x{1}.var;

% Nonlinear layer
x{3} = nonlin_hermite(x{2}, net.nonlin);

% Second linear layer:    x{4} = B*x{3} + b
x = net.w2.e * x{3}.e + repmat(net.b2.e, [1, tdim]);