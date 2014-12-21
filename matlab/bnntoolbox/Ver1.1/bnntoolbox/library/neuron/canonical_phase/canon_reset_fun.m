function updated_state_vector = canon_reset_fun(state_vector , event_index)
% CANON_RESET_FUN   Reset function for canonical phase model
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

global net

%---updating states
neuron_weight           = net.Architecture.NeuronWeight;
neuron_num              = net.Architecture.NeuronNum;
spikers                 = zeros(neuron_num , 1);
spikers(event_index)    = 1;

state_vector            = state_vector(end , :)';

updated_state_vector    = mod(state_vector + (1 + cos(state_vector)).*(neuron_weight*spikers) , 2*pi);

return