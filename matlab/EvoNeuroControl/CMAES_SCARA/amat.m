% amat.m
%
% $Id: amat.m,v 1.2 2006/04/22 15:48:38 jdlope Exp $

% Devuelve la matriz de transformación entre dos sistemas de referencia
% consecutivos a partir de los parámetros de Denavit-Hartenberg

function ret = amat(Theta)

    % ret = rotz(theta) * tras(0,0,d) * tras(a,0,0) * rotx(alpha);

    if (nargin == 1)  % Los parametros vienen en un vector
        alpha = Theta(4);
    	a     = Theta(3);
    	d     = Theta(2);
    	theta = Theta(1);
    end

    sa = sin(alpha); ca = cos(alpha);
    st = sin(theta); ct = cos(theta);

    ret = [ ct   -ca*st    sa*st   a*ct ; ...
	        st    ca*ct   -sa*ct   a*st ; ...
	        0     sa       ca      d    ; ...
	        0     0        0       1    ];

%ret = [ ...
%  cos(theta) -cos(alpha)*sin(theta)  sin(alpha)*sin(theta) a*cos(theta); ...
%  sin(theta)  cos(alpha)*cos(theta) -sin(alpha)*cos(theta) a*sin(theta); ...
%  0           sin(alpha)             cos(alpha)            d           ; ...
%  0           0                       0                    1             ...
%      ];
