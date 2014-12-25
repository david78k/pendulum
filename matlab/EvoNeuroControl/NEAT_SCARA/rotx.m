% rotx.m
%
% $Id: rotx.m,v 1.2 2006/04/22 15:48:38 jdlope Exp $

% Devuelve la matriz correspondiente a una rotación alrededor del eje X un
% ángulo theta

function ret = rotx(theta)

  c = cos(theta);
  s = sin(theta);

  ret = [ 1  0  0  0 ;
	  0  c -s  0 ;
	  0  s  c  0 ;
	  0  0  0  1
	];
