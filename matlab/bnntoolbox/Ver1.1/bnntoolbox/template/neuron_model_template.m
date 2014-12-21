function dydt = neuron_model_template(t , y)
% NEURON_MODEL_TEMPLATE     Template file for user defined neuron models
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

% First of all take a copy of this file, rename it to 'my_model_name' (for
% example 'int_fire2'). Also change the name of function at the first
% line from 'neuron_model_template' to 'my_model_name'.

%Defining BNN model as a global variable. DON'T CHANGE THIS SECTION.
global net

%Setting parameters for the model
if ischar(net.Neurons.Model.ModelParam)
    switch net.Neurons.Model.ModelParam
        case 'def'
            % Put default parameters for your model here.
            tau_m   = 10;
            C       = 1;
            R       = 10;
            
        otherwise
            error('Unknown neuron model parameters')
    end
else
    % If you want to get other parameters rather than defaults from
    % outside, change these codes according to your model. Note that you
    % must preserve the parameters order here and outside to get the
    % correct results. Another point is that you can specify different
    % parameters for each neuron by setting model parameter in neuron
    % object to a matrix containing parameters for each neuron in a row
    % (i.e. each column corresponds to one of parameters and each row has
    % the parameters for that neuron).
    C       = net.Neurons.Model.ModelParam(: , 1);
    R       = net.Neurons.Model.ModelParam(: , 2);
    tau_m   = R.*C;
end

% This is line calculates the total dendritic current. 'external_input' and
% 'internal_input' are associated functions of this toolbox.
I       = external_input(t , y) + internal_input(t , y);

% Put the DEs here. Use dotted arithmetic operations if you have different
% parameters for different neurons.
dydt    = (-y + R.*I)./tau_m;

return