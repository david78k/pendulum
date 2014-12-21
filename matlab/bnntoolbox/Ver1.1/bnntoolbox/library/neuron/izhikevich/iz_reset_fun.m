function updated_state_vector = iz_reset_fun(state_vector , event_index)
% IZ_RESET_FUN     Reset function for Izhikevich model
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
if ischar(net.Neurons.Reset.ResetFunParam)
    switch lower(net.Neurons.Reset.ResetFunParam)
        case 'def'
            c   = -65*ones(net.Architecture.NeuronNum , 1);
            d   = 8*ones(net.Architecture.NeuronNum , 1);

        otherwise
            error('Unknown reser parameters')
    end
else
    if length(net.Neurons.Reset.ResetFunParam) == 2
        c   = net.Neurons.Reset.ResetFunParam(1)*ones(net.Architecture.NeuronNum , 1);
        d   = net.Neurons.Reset.ResetFunParam(2)*ones(net.Architecture.NeuronNum , 1);

    elseif length(y) ~= state_num*length(net.Neurons.Reset.ResetFunParam)
        error('Reset vector size does not match with the number of neurons')
    else
        c   = net.Neurons.Reset.ResetFunParam(: , 1);
        d   = net.Neurons.Reset.ResetFunParam(: , 2);
    end
end

%---updating states
updated_state_vector                    = state_vector(end , :);
updated_state_vector(event_index)       = c(1 + floor((event_index - 1)/state_num));
updated_state_vector(event_index + 1)   = d(1 + floor((event_index - 1)/state_num))' + updated_state_vector(event_index + 1);

return