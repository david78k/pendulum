% scara.m
%
% $Id: scara.m,v 1.1 2006/04/22 15:51:53 jdlope Exp $

% Devuelve la matriz de transformación entre el sistema de referencia
% base y la pinza o efector final de un manipulador de tipo SCARA

function ret = scara(t1,t2,t3,d4)

    a1 = 20;  % Distancia del hombro al codo
    a2 = 20;  % Distancia del codo a la muñeca

    % Parametros Denavit-Hartenberg del manipulador
    dh = [ t1  0   a1  0  ; ...
           t2  0   a2  pi ; ...
	       t3  0   0   0  ; ...
	       0   d4  0   0  ];

    ret = diag(ones(1,4));

    for i = 1:size(dh,1)
        ret = ret * amat(dh(i,:));
    end
