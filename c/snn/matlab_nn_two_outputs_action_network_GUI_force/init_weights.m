function [a,b,c,d,e,f] = init_weights()

% Initialize action and heuristic critic weights and traces 
% to random values between -0.1 and 0.1
for i = 1: 5,
    for j = 1:5,
        a(i,j) = randomdef * 0.2 - 0.1; % evaluation network I-H
        d(i,j) = randomdef * 0.2 - 0.1; % action network I-H
    end
%     b(i) = randomdef;   % evaluation network I-O
    b(i) = randomdef * 0.2 - 0.1;   % evaluation network I-O
    c(i) = randomdef * 0.2 - 0.1;   % evaluation network H-O
    for j = 1:2
        e(i,j) = randomdef * 0.2 - 0.1;   % action network I-O
        f(i,j) = randomdef * 0.2 - 0.1;   % action network H-O
    end
end
