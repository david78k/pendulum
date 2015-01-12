function A = dtnet(s, tnet)
% DTNET  Calculate the Jacobian of the dynamic mapping MLP
%
%  A = dtnet(s, tnet)
%
%  Calculates the Jacobian for the given temporal MLP in the point s. If s i a
%  matrix, Jacobians for all the points are returned in a 3-D matrix with
%  different samples along the third dimension.
%
%  DTNET ignores variance information, for more accurate results, use
%  the multivar returned by acfeedforward.

% Copyright (C) 2004-2005 Matti Tornio
%
% This package comes with ABSOLUTELY NO WARRANTY; for details
% see License.txt in the program package.  This is free software,
% and you are welcome to redistribute it under certain conditions;
	
% Extract parameters
W1 = tnet.w1.e;
b1 = tnet.b1.e;
W2 = tnet.w2.e;
b2 = tnet.b2.e;
[Nn, Ns] = size(W1);
Nx = size(W2, 1);
T = size(s, 2);

if tnet.nonlin ~= 'tanh', error('Unknown nonlinearity'); end

% Calculate the Jacobian(s)
if T == 1,
  % Single sample
  A = W2 * (repmat(dtanh(W1*s+b1), 1, Ns) .* W1);
  A = eye(Ns) + A;
else
  % Multiple samples
  H2 = dtanh(W1*s+repmat(b1,1,T));
  H2 = reshape(H2,Nn,1,T);
  H2 = repmat(H2,[1 Ns 1]);
  H2 = H2 .* repmat(W1, [1 1 T]);

  H3 = reshape(H2, [Nn Ns*T 1]);
  H3 = W2 * H3;
  A = reshape(H3, [Nx Ns T]);

  % Temporal mapping is s[t+1] = s[t] + g(s[t]), so I is added
  A = repmat(eye(Ns), [1 1 T]) + A;
end
