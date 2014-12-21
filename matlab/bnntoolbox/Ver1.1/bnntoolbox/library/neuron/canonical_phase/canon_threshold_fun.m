function [value , isterminal , direction] = canon_threshold_fun(t , y)
% CANON_THRESHOLD_FUN   Threshold function for canonical phase model spike
%                       detection
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

global net

threshold   = pi;

%---checking for threshold crossing
value       = y - threshold;
isterminal  = ones(size(y));
direction   = ones(size(y));

return