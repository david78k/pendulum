function updated_state_vector = reset_fun(state_vector , event_index)
% RESET_FUN     Reset function (general)
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

global net

%---setting parameters
if ischar(net.Neurons.Reset.ResetFunParam)
    switch lower(net.Neurons.Reset.ResetFunParam)
        case 'def'
            reset_value   = zeros(net.Architecture.NeuronNum , 1);
        otherwise
            error('Unknown reset parameters')
    end
else
    if length(net.Neurons.Reset.ResetFunParam) == 1
        reset_value = net.Neurons.Reset.ResetFunParam*ones(net.Architecture.NeuronNum , 1);
    elseif length(y) ~= length(net.Neurons.Reset.ResetFunParam)
        error('Reset vector size does not match with the number of neurons')
    else
        reset_value = net.Neurons.Reset.ResetFunParam;
    end
end

%---updating states
updated_state_vector                = state_vector(end , :);
updated_state_vector(event_index)   = reset_value(event_index);

return