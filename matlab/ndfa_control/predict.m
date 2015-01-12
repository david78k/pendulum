function [s, x] = predict(init_s, result, n, control),
% PREDICT Predict the future state
%
%  [s, x] = predict(init_s, result, n, control)
%
%  Predict the future state using the temporal mapping from the result. State 
%  is predicted n time steps to the future. If provided, control is forced to 
%  the given values. For probdist values, use predict_pd.
%

% Copyright (C) 2004-2005 Matti Tornio
%
% This package comes with ABSOLUTELY NO WARRANTY; for details
% see License.txt in the program package.  This is free software,
% and you are welcome to redistribute it under certain conditions;
% see License.txt for details.

% Initialize control if necessary
if nargin >= 4 & ~isempty(control),
  Ns = size(init_s, 1);
  Nu = size(control, 1);
  controls = (Ns - Nu + 1):Ns;
  init_s(controls) = control(:,1);
end

s = init_s;

% Do the actual prediction
for t=size(s, 2)+1:n+1,
  s(:,t) = s(:,t-1) + feedfwfast(s(:,t-1), result.tnet);
  if nargin >= 4 & ~isempty(control),
    s(controls,t) = control(:,t);
  end
end

% Do feedforward to reconstruct observations if necessary
if nargout > 1,
  x = feedfwfast(s, result.net);
end
