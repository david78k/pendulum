function dydt = fitzhugh_nagumo(t , y)
% FITZHUGH_NAGUMO   Differential equations for FitzHugh-Nagumo model
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

global net

%---setting parameters
if ischar(net.Neurons.Model.ModelParam)
    switch lower(net.Neurons.Model.ModelParam)
        case 'def'
            alpha   = 0.1*ones(net.Architecture.NeuronNum , 1);
            epsilon = 0.01*ones(net.Architecture.NeuronNum , 1);
            gamma   = 0.5*ones(net.Architecture.NeuronNum , 1);

        otherwise
            error('Unknown neuron model parameters')
    end
else
    alpha   = net.Neurons.Model.ModelParam(: , 1);
    epsilon = net.Neurons.Model.ModelParam(: , 2);
    gamma   = net.Neurons.Model.ModelParam(: , 3);
end

state_num       = net.Neurons.Model.StateNum;

I       = external_input(t , y) + internal_input(t , y);
dydt    = zeros(size(y));

for k = 1:state_num:length(y)
    v       = y(k);
    w       = y(k + 1);
    
    [dvdt dwdt] = fn_de(t , v , w , I(1 + floor(k/state_num)) , alpha(round((k + 1)/2)) , epsilon(round((k + 1)/2)) , gamma(round((k + 1)/2)));
    
    dydt(k)     = dvdt;
    dydt(k + 1) = dwdt;
end

return

%---main DEs
function [dvdt , dwdt] = fn_de(t , v , w , I , alpha , epsilon , gamma)
% FitzHugh-Nagumo differential equations

dvdt    = (-v*(v - alpha)*(v - 1) - w + I)/epsilon;
dwdt    = v - gamma*w;

return