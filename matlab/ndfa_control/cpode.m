function [dstate] = cpode(t, state, varargin),
% CPODE  Differential equations for the cart-pole system

% Copyright (C) 2005 Matti Tornio.
%
% This package comes with ABSOLUTELY NO WARRANTY; for details
% see License.txt in the program package.  This is free software,
% and you are welcome to redistribute it under certain conditions;
% see License.txt for details.

F = varargin{2};

% Constants
M=1.0;
m=0.1;
l=0.5;
g=9.8;
muc=0.05;
mup=0.01;

th = state(1);
dth = state(2);
x  = state(3);
dx = state(4);

ddth = (g*sin(th)+cos(th)*((-F-m*l*dth^2*sin(th)+muc*sign(dx))/(M+m))- ...
       (mup*dth)/(m*l)) / (l*(4/3-(m*cos(th)^2)/(M+m)));
ddx = (F+m*l*(dth^2*sin(th)-ddth*cos(th))-muc*sign(dx))/(M+m); 

dstate = [dth ddth dx ddx]';
