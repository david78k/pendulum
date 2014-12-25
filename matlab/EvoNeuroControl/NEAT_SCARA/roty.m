% roty.m
%
% $Id: roty.m,v 1.2 2006/04/22 15:48:38 jdlope Exp $

% Devuelve la matriz correspondiente a una rotación alrededor del eje Y un
% ángulo theta

function ret = roty(theta)

  c = cos(theta);
  s = sin(theta);

  ret = [  c  0  s  0 ;
	   0  1  0  0 ;
	  -s  0  c  0 ;
	   0  0  0  1
	];
