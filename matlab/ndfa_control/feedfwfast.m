function x = feedfwfast(s, net),
% FEEDFWFAST Do feedforward with mean values only

% Copyright (C) 2002-2005 Harri Valpola, Antti Honkela and Matti Tornio.
%
% This package comes with ABSOLUTELY NO WARRANTY; for details
% see License.txt in the program package.  This is free software,
% and you are welcome to redistribute it under certain conditions;
% see License.txt for details.

if isfield(net, 'identity') & net.identity,
  x = s(1:end-1,:);
  return;
end

filler = ones(1, size(s, 2));

x = net.w1.e*s + net.b1.e * filler;
x = feval(net.nonlin, x);
x = net.w2.e*x + net.b2.e * filler;