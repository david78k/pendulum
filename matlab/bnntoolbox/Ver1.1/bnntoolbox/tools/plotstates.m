function plotstates(net , neuron_no)
% PLOTSTATES	Plot states for a BNN
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

switch nargin
    case 0
        error('You must enter a BNN object')
        
    case 1
        states  = getstates(net);
        time    = gettime(net);
        plot(time , states)
        
    case 2
        states  = getstates(net , neuron_no);
        time    = gettime(net);
        plot(time , states)
end