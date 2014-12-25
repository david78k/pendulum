% rpy.m
%
% $Id: rpy.m,v 1.2 2006/04/22 15:48:38 jdlope Exp $

% Devuelve los ángulos RPY de la matriz de transformación m

function ret = rpy(m)

  r = atan2(m(2,1), m(1,1));  % Y también r + pi
  p = atan2(-m(3,1), cos(r)*m(1,1)+sin(r)*m(2,1));
  y = atan2(sin(r)*m(1,3)-cos(r)*m(2,3), -sin(r)*m(1,2)+cos(r)*m(2,2));

  ret = [ y p r ];
