function [nexts, reward, endsim] = acrobot2_simulator(state, action)
%%%%%%%%%%%%%%%%%%%%%
% Acrobot simulator %
%%%%%%%%%%%%%%%%%%%%%

global theta1;
global theta2;
global theta1_p;
global theta2_p;

torquetable = [1, -1, 0];
dt = 0.05;

if (nargin < 1)
  % initialize
  theta1 = (rand() - 0.5) * (pi / 4);
  theta2 = (rand() - 0.5) * (pi / 4);
  theta1_p = 0;
  theta2_p = 0;
  
  nexts = [theta1, theta2, theta1_p, theta2_p];  
  reward = 0;
  endsim = 0;
elseif (nargin < 2)
  % set state
  nexts = state;
  reward = 0;
  endsim = 0;
else
  % next state
  theta1 = state(1);
  theta2 = state(2);
  theta1_p = state(3);
  theta2_p = state(4);
  
  endsim = 0;
  reward = 0;
  i = 1;
  while ((i <= 4) && (endsim == 0))
    endsim = acrobot2_change(torquetable(action), dt);
    if (endsim == 0)
      reward = reward - 1;
    else
      reward = 0;
    end
    i = i + 1;
  end
  
  nexts = [theta1, theta2, theta1_p, theta2_p];
end
