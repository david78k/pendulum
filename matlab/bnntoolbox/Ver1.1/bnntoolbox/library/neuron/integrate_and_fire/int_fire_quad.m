function dydt = int_fire_quad(t , y)
% INT_FIRE_QUAD     Differential equations for quadratic nonlinear
%                   integrate-and-fire
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
            a0      = 1;
            u_eq    = 0;
            u_c     = 0.5;

        otherwise
            error('Unknown neuron model parameters')
    end
else
    C       = net.Neurons.Model.ModelParam(: , 1);
    R       = net.Neurons.Model.ModelParam(: , 2);
    tau_m   = R.*C;
    a0      = net.Neurons.Model.ModelParam(: , 3);
    u_eq    = net.Neurons.Model.ModelParam(: , 4);
    u_c     = net.Neurons.Model.ModelParam(: , 5);

end

%---DEs
I       = external_input(t , y) + internal_input(t , y);
dydt    = (a0.*(y - u_eq).*(y - u_c) + R.*I)./tau_m;

return