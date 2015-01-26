function [a,b,c,d,e,f] = updateWeights(BETA, RHO, BETAH, RHOH, rhat, ...
    unusualness, xold, yold, a, b, c, d, e, f, z)

% backpropagation
for i = 1: 5,  
     % for each hidden unit
    factor1 = BETAH * rhat * yold(i) * (1 - yold(i)) * sign(c(i));
    factor2 = RHOH * rhat * z(i) * (1 - z(i)) * unusualness; % * sign(f(i,1)) * sign(f(i,2));
    for j = 1:5,    
        % for each weight I-H
        a(i,j) = a(i, j) + factor1 * xold(j); % evaluation network I-H
        d(i,j) = d(i, j) + factor2 * xold(j); % action network I-H
    end
    % for each weight H-O of evaluation network
    b(i) = b(i) + BETA * rhat * xold(i);   % evaluation network I-O
    c(i) = c(i) + BETA * rhat * yold(i);   % evaluation network H-O
    
    % for each output of action network
    for j = 1:2,
        e(i,j) = e(i,j) + RHO * rhat * xold(i) * unusualness; % I-O
        f(i,j) = f(i,j) + RHO * rhat * z(i) * unusualness;   % H-O
    end
end
