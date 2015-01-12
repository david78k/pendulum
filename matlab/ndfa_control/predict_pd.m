function [s, x] = predict_pd(init_s, result, n, control),
% PREDICT Predict the future state with probdist
%
%  [s, x] = predict(init_s, result, n, control)
%
%  Predict the future state using the mapping g from the result. State is
%  predicted n time steps to the future. If provided, control is forced to the
%  given values.
%
%  2004-07-15 Matti Tornio

if isa(init_s, 'acprobdist_alpha'),
  s = probdist(init_s.e, init_s.var);
elseif isa(init_s, 'probdist'),
  s = init_s;
else
  s = probdist(init_s, ones(size(init_s)) * .0001);
end

% Initialize control if necessary
if nargin >= 4 & ~isempty(control),
  nsources = size(init_s, 1);
  controls = (nsources - size(control, 1) + 1):nsources;
  s(controls) = probdist(control(:,1), 0.1);
end

% Do the actual prediction
for t=size(s, 2)+1:n+1,
  h = feedfw(s(:,t-1), result.tnet);   
  h = h{4};
  s(:,t) = s(:,t-1) + h;

  if nargin >= 4 & ~isempty(control),
    s(controls,t) = probdist(control(:,t), 0.1);
  end
end

if nargout > 1,
  x = feedfw(s, result.net);
  x = x{4};
end
