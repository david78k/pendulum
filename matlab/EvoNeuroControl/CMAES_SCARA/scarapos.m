% scarapos.m
%
% $Id: scarapos.m,v 1.1 2006/05/04 17:19:28 jdlope Exp $

% Devuelve un matriz cuyas filas son las posiciones de cada uno de los
% referenciales asociados a los joints desde el sistema de referencia base

function ret = scarapos(t1,t2,t3,d4)

    a1 = 20;  % Distancia del hombro al codo
    a2 = 20;  % Distancia del codo a la muñeca

    % Parametros Denavit-Hartenberg del manipulador
    dh = [ t1  0   a1  0  ; ...
           t2  0   a2  pi ; ...
           t3  0   0   0  ; ...
           0   d4  0   0  ];

    dia = diag(ones(1,4));
    ret = zeros(size(dh,1)+1,3);

    for i = 1:size(dh,1)
        dia = dia * amat(dh(i,:));
        ret(i+1,1:3) = dia(1:3,4)';        
    end

    % displacement of 4 unit in the Z axis
    ret(:,3)=ret(:,3)+4;