% tras.m
%
% $Id: tras.m,v 1.2 2006/04/22 15:48:38 jdlope Exp $

% Devuelve la matriz correspondiente a una traslación en los ejes X, Y y Z

function ret = tras(tx,ty,tz)

  ret = [ 1  0  0  tx ;
	  0  1  0  ty ;
	  0  0  1  tz ;
	  0  0  0  1
	];
