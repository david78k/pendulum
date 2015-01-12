function G = costgrad(init_s, u, goal, result),
% COSTGRAD  Calculates the gradient of the cart-pole system cost function
%
%  G = COSTGRAD(init_s, u, goal, result)
%
%  2004-07-16 Matti Tornio

% Extract parameters
Ns = size(init_s);
[Nu T] = size(u');
Nx = size(goal, 1);

[s_, x_] = predict(init_s(:,1), result, T-1, u');
c = cost(x_, goal, u');

if size(goal, 2)==1,
  goal = repmat(goal, 1, T);
end

missing = sparse(isnan(goal));

% Gradient of the cost function
dJdx = 2 * reshape((x_ - goal) .* ~missing, 1, Nx*T);

% Gradient of the penalty term for F
F = u;
F = sign(F) .* max([abs(F)-10 zeros(T, Nu)],[],2);
dJ2 = 2 * F';

% Gradient of the penalty term for position
xc = [x_(1,:); zeros(3, T)];
xc = reshape(xc, 1, Nx*T);
xc = sign(xc) .* max([abs(xc)-3; zeros(1, Nx*T)]);
dJdx2 = 2 * xc;

% Gradient of the complete cost function
G = (dJdx + dJdx2) * dnetu(s_, result) + dJ2;
