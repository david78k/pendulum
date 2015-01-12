function [data, F] = cartpole(init, F, steps, dt),
% CARTPOLE  Simulates a cart-pole system
%
% [data, F] = cartpole(init, F, steps, dt)
%
% Simulates the cart-pole system starting from the given state. If no control
% force F is provided, a suitable control is generated. dt is the time step to
% use (default 0.05 s).
%
%  2004-06-18 Matti Tornio

% Initialize control
if (nargin < 2) | isempty(F),
  F_missing = 1;
  F = zeros(1, steps+1);
else
  steps = size(F, 2);
  F_missing = 0;
  F = [F zeros(1,1)];
end

% Use default time step if none is specified
if nargin < 7,
 dt = 0.05;
end

data = [init zeros(4, steps)];

% Start simulation
for i=1:steps,
  % Calculate next data sample
  [F(i) data(:,i+1)] = cartpole_f(F(i), data(:,i), dt);

  x = data(1,i+1);
  dx = data(2,i+1);
  th = data(3,i+1);
  dth = data(4,i+1);

  % If no control is provided, generate it
  if F_missing,
    F(i+1) = F(i) + randn * .02;
    if (abs(dx)>.8) & (sign(dx)==sign(F(i+1))),
      if (i*2)>steps,
        F(i+1) = -F(i+1)*rand;
      else
        F(i+1) = -F(i+1)*rand*.5;
      end
    end
    if (abs(x)>.3) & (sign(x)==sign(F(i+1))),
      F(i+1) = -(1+rand) * sign(x);
    end
    if (abs(x)>1.2)
      F(i+1) = F(i+1) - sign(x)*rand;
    end
    F(i+1) = min([abs(F(i+1)) 10]) * sign(F(i+1));
    if rand > 0.99,
      F(i+1) = 8 - rand * 16;
    end
    if (abs(dx) > 4),
      F(i+1) = F(i+1) - dx / 5;
    end
    F(i+1) = F(i+1) - dx / 100;
  end
end

% Drop initial state and return data with observation error added
%F = F(2:size(data, 2));
%data = data(:,2:end);
%data = data + randn(size(data)) * .01;
