function dydt = canonical_phase(t , y)
% CANONICAL_PHASE   Differential equations for canonical phase model
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

global net

%---DEs
I       = external_input(t , y) + internal_input(t , y);
dydt    = 1 - cos(y) + (1 + cos(y)).*I;

return