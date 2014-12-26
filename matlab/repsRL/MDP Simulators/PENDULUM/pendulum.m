function xdot = pendulum(t, x) 
  
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Copyright 2000-2002 
%
% Michail G. Lagoudakis (mgl@cs.duke.edu)
% Ronald Parr (parr@cs.duke.edu)
%
% Department of Computer Science
% Box 90129
% Duke University
% Durham, NC 27708
% 
%
% xdot = pendulum(t, x) 
%
% Equation of the pendulum
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 
  
  m=2.0;		% Mass of the pendulum
  M=8.0; 		% Mass of the cart
  l=1.0/2.0; 	        % Length of the pendulum
  g=9.8; 		% Gravity constant
  a=1.0/(m+M); 

  
  % Nonlinear model 

  u = x(3); 
  xdot(1)=x(2); 
  xdot(2)=( g*sin(x(1)) - a*m*l*x(2)^2*sin(2*x(1))/2 - a*cos(x(1))*u ) / ...
	  ( 4/3*l - a*m*l*cos(x(1))^2 ); 
  xdot(3)=0;

  xdot=xdot(:); 

  
  return
