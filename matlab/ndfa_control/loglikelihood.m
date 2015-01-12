function [c, cx, ct] = loglikelihood(x, s, R, Q, net, tnet, missing, notimedep),
% LOGLIKELIHOOD  Calculates the log likelihood function
%
%  c = LOGLIKELIHOOD(x, s, R, Q, net, tnet, missing)
%
%  Calculates the log likelihood function between the estimated state s and
%  the actual observations x given covariance matrices R and Q (constant
%  terms are discarded)

% Copyright (C) 2004-2005 Matti Tornio
%
% This package comes with ABSOLUTELY NO WARRANTY; for details
% see License.txt in the program package.  This is free software,
% and you are welcome to redistribute it under certain conditions;
% see License.txt for details.

if ~isempty(notimedep),
  ignoretimedep = [zeros(size(s, 1), 1) notimedep];
else
  ignoretimedep = [];
end

if isa(s, 'probdist'),
  s = s.e;
end

% Calculate estimates for observations and future state
x_ = feedfwfast(s, net) .* ~missing + x .* missing;
s_ = s + feedfwfast(s, tnet);
s_ = [s(:,1) s_(:,1:end-1)];

% R and Q are diagonal for NDFA
R = repmat(diag(R), 1, size(x, 2));
Q = repmat(diag(Q), 1, size(s, 2));
dx = x - x_;
ds = s - s_;

if ~isempty(ignoretimedep),
  ds = ds .* ~ignoretimedep;
end

cx = -.5 * (sum(sum(dx .* dx ./ R)));
ct = -.5 * (sum(sum(ds .* ds ./ Q)));
c = cx + ct;