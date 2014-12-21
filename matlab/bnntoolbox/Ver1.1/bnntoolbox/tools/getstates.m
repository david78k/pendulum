function states = getstates(net , neuron_no)
% GETSTATES	Get states from a BNN
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
        states  = net.InputOutput.Output.State;
        
    case 2
        state_num   = net.Neurons.Model.StateNum;
        states  = net.InputOutput.Output.State(: , (neuron_no - 1)*state_num + 1:neuron_no*state_num);
end