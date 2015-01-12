function [s, kls] = findsources(data, result, iters, init_s, control),
% FINDSOURCES Finds sources fitting the given data
%
%  [s, kls, llh] = findsources(data, result, iters, init_s, control)
%
%  2004-08-04 Matti Tornio


Ns = size(result.net.w1, 2);
[Nx, T] = size(data);
Nu = result.status.controlchannels;

if nargin < 5,
  control = nan * ones(Nu, T);
end

if nargin < 4 | isempty(init_s),
  init_s = zeros(Ns, T);
end

if nargin < 3,
  iters = nan;
end

result.status.kls = [];

% Improve estimate with NDFA
result.status.kls = [];
result.sources = result.sources(:, 1:T);
if isa(init_s, 'acprobdist_alpha'),
  result.sources = init_s;
else
  result.sources.e = init_s;
end

result = ndfa(data, result, 'initclamped', control, ...
	      'nolearning', 1, 'iters', iters, 'debug', 0);
s = result.sources;

if iters > 0,
  kls = result.status.kls(end);
else
  kls = nan;
end
  
%llh = result.status.llh(end);

% Find suitable initial value for the first source
%result_init = ndfa(data(:,1:2), result, 'initclamped', control(:,1:2), ...
%	      'nolearning', 1, 'iters', nan, 'debug', 0);
%init_s = result_init.sources.e;
%init_s = ieks(data(:,1), result);
%result.sources = result.sources(:, 1:T);
%result.sources.e = predict(init_s(:,end), result, T-size(init_s, 2));
%result = ndfa(data, result, 'initclamped', control, ...
%	      'nolearning', 1, 'iters', 100);
%init_s = result.sources.e;
% Do some IEKS iterations to get a good estimate
%if nargin < 3,
%  s = ieks(data, result, 500, zeros(7,1));
%else 
%  s = ieks(data, result, 500, zeros(7,1), 1, 1, control);
%end


