function dydt = izhikevich(t , y)
% IZHIKEVICH    Differential equations for Izhikevich model
% 
%   Version:    1.1
%   ----------------------------------
%   Amir Reza Saffari Azar, October 2005
%   amir@ymer.org
%   http://www.ymer.org

global net

%---setting parameters
if ischar(net.Neurons.Model.ModelParam)
    switch lower(net.Neurons.Model.ModelParam)
        case 'def'
            a   = 0.02*ones(net.Architecture.NeuronNum , 1);
            b   = 0.2*ones(net.Architecture.NeuronNum , 1);

        otherwise
            error('Unknown neuron model parameters')
    end
else
    a   = net.Neurons.Model.ModelParam(: , 1);
    b   = net.Neurons.Model.ModelParam(: , 2);

end

state_num   = net.Neurons.Model.StateNum;

I       = external_input(t , y) + internal_input(t , y);
dydt    = zeros(size(y));

for k = 1:state_num:length(y)
    v   = y(k);
    u   = y(k + 1);
    
    [dvdt dudt] = iz_de(t , v , u , I(1 + floor(k/state_num)) , a(round((k + 1)/2)) , b(round((k + 1)/2)));
    
    dydt(k)     = dvdt;
    dydt(k + 1) = dudt;
end

return

%---main DEs
function [dvdt , dudt] = iz_de(t , v , u , I , a , b)
% Izhikevich differential equations

dvdt    = 0.04*v^2 + 5*v + 140 - u + I;
dudt    = a*(b*v - u);

return
