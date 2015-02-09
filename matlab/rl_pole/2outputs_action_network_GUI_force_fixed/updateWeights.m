function [a,b,c,d,e,f] = updateWeights(BETA, RHO, BETAH, RHOH, rhat, ...
    unusualness, xold, yold, a, b, c, d, e, f, z)

for i = 1: 5,  
     % for each hidden unit
    factor1 = BETAH * rhat * yold(i) * (1 - yold(i)) * sign(c(i));
    factor2 = RHOH * rhat * z(i) * (1 - z(i)) * unusualness;% * sign(f(i)); % unusualness required, sign(f(i)) optional
    for j = 1:5,    
        % for each weight I-H
        a(i,j) = a(i, j) + factor1 * xold(j); % evaluation network I-H
        d(i,j) = d(i, j) + factor2 * xold(j); % action network I-H
    end
    % for each weight H-O
    b(i) = b(i) + BETA * rhat * xold(i);   % evaluation network I-O
    c(i) = c(i) + BETA * rhat * yold(i);   % evaluation network H-O

    for j = 1:2
        e(i,j) = e(i,j) + RHO * rhat * xold(i) * unusualness;  % action network I-O
        f(i,j) = f(i,j) + RHO * rhat * z(i) * unusualness;  % action network H-O
    end
%     e(i) = e(i) + RHO * rhat * xold(i) * unusualness;  % action network I-O
%     f(i) = f(i) + RHO * rhat * z(i) * unusualness;  % action network H-O
end
