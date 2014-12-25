% rotz.m
%
% $Id: rotz.m,v 1.2 2006/04/22 15:48:38 jdlope Exp $

% Devuelve la matriz correspondiente a una rotación alrededor del eje Z un
% ángulo theta

function ret = rotz(theta)

  c = cos(theta);
  s = sin(theta);

  ret = [ c -s  0  0 ;
	  s  c  0  0 ;
	  0  0  1  0 ;
	  0  0  0  1
	];
