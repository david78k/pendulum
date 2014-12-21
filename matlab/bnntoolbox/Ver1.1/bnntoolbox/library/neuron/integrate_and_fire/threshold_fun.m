function [value , isterminal , direction] = threshold_fun(t , y)
% THRESHOLD_FUN     Threshold function for spike detection (general)
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

global net

%---setting parameters
if ischar(net.Neurons.SpikeDet.SpikeDetFunParam)
    switch lower(net.Neurons.SpikeDet.SpikeDetFunParam)
        case 'def'
            threshold   = ones(net.Architecture.NeuronNum , 1);
        otherwise
            error('Unknown threshold parameters')
    end
else
    if length(net.Neurons.SpikeDet.SpikeDetFunParam) == 1
        threshold   = net.Neurons.SpikeDet.SpikeDetFunParam*ones(net.Architecture.NeuronNum , 1);
    elseif length(y) ~= length(net.Neurons.SpikeDet.SpikeDetFunParam)
        error('Threshold vector size does not match with the number of neurons')
    else
        threshold   = net.Neurons.SpikeDet.SpikeDetFunParam;
    end
end

%---checking for threshold crossing
value       = y - threshold;
isterminal  = ones(size(y));
direction   = ones(size(y));

return