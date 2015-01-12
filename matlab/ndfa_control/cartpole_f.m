function [F, newsample] = cartpole_f(F, newsample, param),
% CARTPOLE_F  Runs a single step of cart-pole simulation
%
%  [F, sample] = cartpole_f(F, state, prm) Start a new simulation
%  [F, sample] = cartpole_f(F)             Continue a previous simulation
%  history = cartpole_f()                  End simulation and return history
%
%  Cart-pole system is simulated for one time step and the new observation is 
%  returned with the (possibly saturated) force. Additional parameters can
%  be passed in the struct param, currently field dt for setting time step and
%  angletransform for enabling the sin transformation of the angle are 
%  supported. STD of the process noise is fixed at 0.001. The function stores
%  the current state in persistent variables and continue a previous 
%  simulation if new control is provided.

% Copyright (C) 2004-2005 Matti Tornio.
%
% This package comes with ABSOLUTELY NO WARRANTY; for details
% see License.txt in the program package.  This is free software,
% and you are welcome to redistribute it under certain conditions;
% see License.txt for details.


% The current state of the simulation is stored in persistent variables
persistent sample Nx dt angletransform history;

% Return the simulation run, if no parameters are given
if nargin == 0,
  F = history;
  return;
end

% Initialize a new simulation with default options
if nargin >= 2,
  angletransform = false;
  dt = 0.05;
  sample = newsample;
  Nx = size(sample, 1);
  % Generate actual sample if state is not in the native format
  switch(Nx);
    case 2,
      sample = [sample(1); 0; sample(2); 0];
  end
  history = sample;
end

% Extract additional parameters
if nargin >= 3,
  if ~isstruct(param),
    dt = param;
  else
    if isfield(param, 'dt'),
      dt = param.dt;
    end
    if isfield(param, 'angletransform'), 
      angletransform = param.angletransform;
    end
  end
end

% Add process noise
%sample = sample + randn(4, 1) * 0.001;

% Extract samples
x = sample(1);
dx = sample(2);
if angletransform,
  th = real(asin(sample(3))) + pi;
else
  th = sample(3) + pi;
end
dth = sample(4);

% Check if control is saturated
if abs(F) > 10,
  F=sign(F)*10;
end

init = [th dth x dx];



% Simulate the system with Matlab ODE solver
[t y] = ode23('cpode', [0 dt], init, [], F);

th = y(end, 1);
dth = y(end, 2);
x = y(end, 3);
dx = y(end, 4);

% Check if cart has collided
if abs(x)>3,
  warning('***BUMB***');
  x=sign(x)*3;
  dx=-0.2*dx;
end;

% Remap angle back to the observation space
th = th - pi;
if th > 2*pi,
  th = th - 2*pi;
elseif th < - 2*pi,
  th = th + 2*pi;
end
if angletransform,
  th = sin(th);
end

% Collect sample
sample(1) = x;
sample(2) = dx;
sample(3) = th;
sample(4) = dth;

% Store new sample to history
history = [history sample];

% Return the state in the format it was originally provided
switch(Nx);
  case 2,
    newsample = [sample(1); sample(3)];
  case 4,
    newsample = sample;
end