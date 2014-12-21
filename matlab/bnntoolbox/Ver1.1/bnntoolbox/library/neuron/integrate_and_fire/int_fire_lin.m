function dydt = int_fire_lin(t , y)
% INT_FIRE_LIN      Differential equations for linear integrate-and-fire
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
    switch net.Neurons.Model.ModelParam
        case 'def'
            tau_m   = 10;
            C       = 1;
            R       = 10;
            
        otherwise
            error('Unknown neuron model parameters')
    end
else
    C       = net.Neurons.Model.ModelParam(: , 1);
    R       = net.Neurons.Model.ModelParam(: , 2);
    tau_m   = R.*C;
end

%---DEs
I       = external_input(t , y) + internal_input(t , y);
dydt    = (-y + R.*I)./tau_m;

return