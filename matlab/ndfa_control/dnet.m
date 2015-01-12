function H = dnet(s, net),
% DNET  Calculate the Jacobian of the observation mapping MLP
%
%  H = dnet(s, net)
%
%  Calculates the Jacobian for the given MLP in the point s. If s is a matrix,
%  Jacobians for all the points are returned in a 3-D matrix with different 
%  samples along the third dimension.
%
%  DNET ignores variance information, for more accurate results, use
%  the multivar returned by FEEDFW.

% Copyright (C) 2004-2005 Matti Tornio
%
% This package comes with ABSOLUTELY NO WARRANTY; for details
% see License.txt in the program package.  This is free software,
% and you are welcome to redistribute it under certain conditions;

% Extract parameters
if isa(s, 'probdist'),
  W1 = net.w1;
  b1 = net.b1;
  W2 = net.w2;
  b2 = net.b2;
else
  W1 = net.w1.e;
  b1 = net.b1.e;
  W2 = net.w2.e;
  b2 = net.b2.e;
end

[Nn, Ns] = size(W1);
Nx = size(W2, 1);
T = size(s, 2);

if net.nonlin ~= 'tanh', error('Unknown nonlinearity'); end

% Calculate the Jacobian(s)
if isfield(net, 'identity') & net.identity,
  % Identity mapping
  H = repmat([eye(Nx) zeros(Nx, Ns-Nx)], [1 1 T]);
elseif T == 1,
  % Single sample
  H = W2 * (repmat(dtanh(W1*s+b1), 1, Ns) .* W1);
else
  % Multiple samples
  H2 = dtanh(W1*s+repmat(b1,1,T));
  H2 = reshape(H2,Nn,1,T);
  H2 = repmat(H2,[1 Ns 1]);
  H2 = H2 .* repmat(W1, [1 1 T]);

  H3 = reshape(H2, [Nn Ns*T 1]);
  H3 = W2 * H3;

  H = reshape(H3, [Nx Ns T]);
end