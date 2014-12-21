function dydt = morris_lecar(t , y)
% MORRIS_LECAR  Differential equations for Morris-Lecar model
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
            g_ca    = 4.4*ones(net.Architecture.NeuronNum , 1);
            g_k     = 8*ones(net.Architecture.NeuronNum , 1);
            g_l     = 2*ones(net.Architecture.NeuronNum , 1);

            v_ca    = 120*ones(net.Architecture.NeuronNum , 1);
            v_k     = -84*ones(net.Architecture.NeuronNum , 1);
            v_l     = -60*ones(net.Architecture.NeuronNum , 1);

            v1      = -1.2*ones(net.Architecture.NeuronNum , 1);
            v2      = 18*ones(net.Architecture.NeuronNum , 1);
            v3      = 2*ones(net.Architecture.NeuronNum , 1);
            v4      = 30*ones(net.Architecture.NeuronNum , 1);

            phi     = 0.04*ones(net.Architecture.NeuronNum , 1);

        otherwise
            error('Unknown neuron model parameters')
    end
else
    g_ca    = net.Neurons.Model.ModelParam(: , 1);
    g_k     = net.Neurons.Model.ModelParam(: , 2);
    g_l     = net.Neurons.Model.ModelParam(: , 3);

    v_ca    = net.Neurons.Model.ModelParam(: , 4);
    v_k     = net.Neurons.Model.ModelParam(: , 5);
    v_l     = net.Neurons.Model.ModelParam(: , 6);

    v1      = net.Neurons.Model.ModelParam(: , 7);
    v2      = net.Neurons.Model.ModelParam(: , 8);
    v3      = net.Neurons.Model.ModelParam(: , 9);
    v4      = net.Neurons.Model.ModelParam(: , 10);

    phi     = net.Neurons.Model.ModelParam(: , 11);

end

state_num       = net.Neurons.Model.StateNum;

I       = external_input(t , y) + internal_input(t , y);
dydt    = zeros(size(y));

for k = 1:state_num:length(y)
    v       = y(k);
    w       = y(k + 1);
    
    [dvdt dwdt] = ml_de(t , v , w , I(1 + floor(k/state_num)) , g_ca(round((k + 1)/2)) , g_k(round((k + 1)/2)) , ...
        g_l(round((k + 1)/2)) , v_ca(round((k + 1)/2)) , v_k(round((k + 1)/2)) , v_l(round((k + 1)/2)) , ...
        v1(round((k + 1)/2)) , v2(round((k + 1)/2)) , v3(round((k + 1)/2)) , v4(round((k + 1)/2)) , phi(round((k + 1)/2)));
    
    dydt(k)     = dvdt;
    dydt(k + 1) = dwdt;
end

return

%---main DEs
function [dvdt , dwdt] = ml_de(t , v , w , I , g_ca , g_k , g_l , v_ca , v_k , v_l , v1 , v2 , v3 , v4 , phi)
% Morris-Lecar differential equations

m_inf   = 0.5*(1 + tanh((v - v1)/v2));
w_inf   = 0.5*(1 + tanh((v - v3)/v4));
tau_w   = 1/(cosh((v - v3)/(2*v4)));

dvdt    = I - g_ca*m_inf*(v - v_ca) - g_k*w*(v - v_k) - g_l*(v - v_l);
dwdt    = phi*(w_inf - w)/tau_w;

return
