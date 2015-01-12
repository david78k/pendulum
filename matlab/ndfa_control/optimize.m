function [s, c, S, X, C, G, G_] = ...
    optimize(init_s, init_u, goal, result, N, method),
% OPTIMIZE  Minimize the cost function of the cart-pole system
%
%  s = optimize(init_s, init_u, goal, result, N, method)
%  [s, c, S, X, C, G] = optimize(init_s, init_u, goal, result, N, method)
%
%  Cost function of the cart-pole is minimized with a combination
%  of gradient-descent and a quasi-Newton method. Backtracking is
%  used to determine step size. 
%
%  Goal can either be a vector to be used for all time steps or a
%  matrix containing the reference signal. Method is the
%  optimization method used. Please note, that the used backtracking
%  algorithm DOES NOT guarantee that the Hessian in BFGS or Broyden
%  stays positive definitive. Unless the initial guess is really
%  poor, this should not be a problem, however.
%
% --------------------------------------------------------------------
%  AVAILABLE OPTIMIZATION METHODS (first 10 steps are always gradient-descent)
%
%  0 - Pure gradient-descent (slower convergence, safe)
%  1 - DFP (fast, Hessian should remain positive definitive)
%  2 - Broyden method (faster, Hessian may become non-positive definitive)
%  3 - BFGS (default, fastest, Hessian may become non-positive definitive)

% Copyright (C) 2004-2005 Matti Tornio
%
% This package comes with ABSOLUTELY NO WARRANTY; for details
% see License.txt in the program package.  This is free software,
% and you are welcome to redistribute it under certain conditions;


% Default quasi-Newton scheme is BFGS
if nargin < 6,
  method = 3;
end

% By default stop after 50 iterations
if nargin < 5,
  N = 50;
end

% Extract model parameters
Ns = size(init_s);
[Nu T] = size(init_u);
Nx = size(goal, 1);
net = result.net;
tnet = result.tnet;
status = result.status;

% Level of debug information
debug = 0;

% Copy reference signal to all samples if only one sample is given
if size(goal, 2) == 1,
  goal = repmat(goal, 1, T);
end

% Initialization
s = predict(init_s, result, T-1, init_u);
S{1} = s;
X{1} = feedfwfast(s, net);
U{1} = s(end, :)';
B{1} = eye(T);
C(1) = cost(X{1}, goal, U{1}');
fprintf('Initial cost: %f\n', C(1));

% Find missing values in the reference signal
missing = sparse(isnan(goal));

for i=1:N,
  % Predict future state and evaluate cost function
  s_ = predict(s(:,1), result, T - 1, U{i}');
  x_ = feedfwfast(s_, net);
  c = cost(x_, goal, U{i}');

  % Gradient of the cost function
  dJdx = 2 * reshape((x_ - goal) .* ~missing, 1, Nx*T);

  % Gradient of the penalty term for F
  F = U{i};
  F = sign(F) .* max([abs(F)-10 zeros(T, Nu)],[],2);
  dJ2 = 2 * F';

  % Gradient of the penalty term for location of the cart x
  xc = [x_(1,:); zeros(Nx-1, T)];
  xc = reshape(xc, 1, Nx*T);
  xc = sign(xc) .* max([abs(xc)-2.5; zeros(1, Nx*T)]);
  dJdx2 = 2 * xc;

  % Gradient of the complete cost function
  G = dnetu(s, dJdx + dJdx2, net, tnet, status) + dJ2;
  
  % Choose search direction based on current approximation of the Hessian
  G_{i} = G;
  f = - B{i}*G';

  % Initialize line search
  step = 10;
  U{i+1} = U{i} + step*f;
  new_c = c;
  s_old = s;

  % Line search based on backtracking
  while true,
    % Get new candidate
    U{i+1} = U{i} + step*f;
    s = predict(s_old(:,1), result, T-1, U{i+1}');
    x_ = feedfwfast(s, net);
    new_c = cost(x_, goal, U{i+1}');

    % Check for the curvature condition (to ensure that Hessian remains 
    % positive definitive with BFGS). This slows down the computation
    % and can be safely ignored in almost all cases. If you have
    % convergence problems, uncomment the following lines
    %G_new = costgrad(s(:,1), U{i+1}, goal, result);
    %while G_new, goal, result)*f < .9*G*f,
    %  step = step * 2;
    %  U{i+1} = U{i} + step*f;
    %end

    % Check for Armijo condition (with c1=.1)
    if new_c < c + 0.1*step*G*f,
      break
    end;
  
    % If no improved value for the cost function is found in search direction,
    % assume this was a (local) minimum and stop the search
    if step < 1E-6,
      s = s_old;
      U{i+1} = U{i};       
      new_c = c;
      break;
    end

    % Use quadratic interpolation with safeguards to determine next step-size
    alpha = -(norm(G))/(2 * (new_c - c - norm(G) * step));
    step = step * min([max([alpha .1]) .5]);
  end

  % Check if the iteration has converged
  if (i>5) & ((C(i - 1) - new_c) < 1E-4),
    S{i+1} = s;
    C(i) = new_c;
    fprintf('Converged at iteration %d with cost %f.\n', round(i), C(i - 1));
    break;
  end

  % Print current cost
  C(i) = new_c;
  if debug | mod(i, 10) == 1,
    fprintf('Iteration %d: %f (Step %f, |grad| %f)\n', round(i), C(i), ...
            step, norm(G));
  end

  % After 10 iterations, start updating the approximation of the Hessian 
  % using a quasi-Newton scheme
  if i > 10
    d = U{i} - U{i-1};
    l = (G_{i} - G_{i-1})';
    % Select the Hessian update method to use
    switch method,
      case 0,
        % Pure gradient (no Hessian update)
        B{i+1} = B{i};      
      case 1,
        % DFP
        B{i+1} = B{i} + (d*d')/(d'*l)-(B{i}*l*l'*B{i})/((B{i}*l)'*l);
      case 2,
        % Broyden
        B{i+1} = B{i} + ((d-B{i}*l)*(d-B{i}*l)')/((d-B{i}*l)'*l);
      case 3,
        % BFGS
        B{i+1} = B{i} + (1+(l'*B{i}*l)/(d'*l))*(d*d')/(d'*l) - ...
                        (d*l'*B{i}+B{i}*l*d')/(d'*l);
    end
  else
    B{i+1} = B{i};
  end

  % Store history
  S{i+1} = s;
  X{i} = x_;

end

% Get final observations and the cost function
X{i} = feedfwfast(s, net);

c = new_c;