function [value , isterminal , direction] = ml_threshold_fun(t , y)
% ML_THRESHOLD_FUN      Threshold function for Morris-Lecar model
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

global net

state_num   = net.Neurons.Model.StateNum;

%---setting parameters
if ischar(net.Neurons.SpikeDet.SpikeDetFunParam)
    switch lower(net.Neurons.SpikeDet.SpikeDetFunParam)
        case 'def'
            threshold   = 40*ones(net.Architecture.NeuronNum , 1);
        otherwise
            error('Unknown threshold parameters')
    end
else
    if length(net.Neurons.SpikeDet.SpikeDetFunParam) == 1
        threshold   = net.Neurons.SpikeDet.SpikeDetFunParam*ones(net.Architecture.NeuronNum , 1);
    elseif length(y) ~= state_num*length(net.Neurons.SpikeDet.SpikeDetFunParam)
        error('Threshold vector size does not match with the number of neurons')
    else
        threshold   = net.Neurons.SpikeDet.SpikeDetFunParam;
    end
end

%---checking for threshold crossing
y2check     = y(1:state_num:end);
value2check = y2check - threshold;

value                   = ones(size(y));
value(1:state_num:end)  = value2check;

isterminal  = ones(size(y));
direction   = ones(size(y));

return