function dydt = hodgkin_huxley(t , y)
% HODGKIN_HUXLEY    Differential equations for Hodgkin-Huxley model
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
            g_na    = 120*ones(net.Architecture.NeuronNum , 1);
            g_k     = 36*ones(net.Architecture.NeuronNum , 1);
            g_l     = 0.3*ones(net.Architecture.NeuronNum , 1);

            v_na    = 50*ones(net.Architecture.NeuronNum , 1);
            v_k     = -77*ones(net.Architecture.NeuronNum , 1);
            v_l     = -54.4*ones(net.Architecture.NeuronNum , 1);

            C       = 1*ones(net.Architecture.NeuronNum , 1);

        otherwise
            error('Unknown neuron model parameters')
    end
else
    g_na    = net.Neurons.Model.ModelParam(: , 1);
    g_k     = net.Neurons.Model.ModelParam(: , 2);
    g_l     = net.Neurons.Model.ModelParam(: , 3);

    v_na    = net.Neurons.Model.ModelParam(: , 4);
    v_k     = net.Neurons.Model.ModelParam(: , 5);
    v_l     = net.Neurons.Model.ModelParam(: , 6);

    C       = net.Neurons.Model.ModelParam(: , 7);
end

state_num       = net.Neurons.Model.StateNum;

I       = external_input(t , y) + internal_input(t , y);
dydt    = zeros(size(y));

for k = 1:state_num:length(y)
    u       = y(k);
    m       = y(k + 1);
    n       = y(k + 2);
    h       = y(k + 3);
    
    [dudt dmdt dndt dhdt] = hh_de(t , u , m , n , h , I(1 + floor(k/state_num)) , g_na(round((k + 3)/4)) , ...
        g_k(round((k + 3)/4)) , g_l(round((k + 3)/4)) , v_na(round((k + 3)/4)) , v_k(round((k + 3)/4)) , ...
        v_l(round((k + 3)/4)) , C(round((k + 3)/4)));
    
    dydt(k)     = dudt;
    dydt(k + 1) = dmdt;
    dydt(k + 2) = dndt;
    dydt(k + 3) = dhdt;
end

return

%---main DEs
function [dudt , dmdt , dndt , dhdt] = hh_de(t , u , m , n , h , I , g_na , g_k , g_l , v_na , v_k , v_l , C)
% Hodgkin-Huxley differential equations

alpha_m = 0.1*(u + 40)/(1 - exp(-(u + 40)/10));
beta_m  = 4*exp(-(u + 65)/18);

alpha_n = 0.01*(u + 55)/(1 - exp(-(u + 55)/10));
beta_n  = 0.125*exp(-(u + 65)/80);

alpha_h = 0.07*exp(-(u + 65)/20);
beta_h  = 1/(1 + exp(-(u + 35)/10));

dudt    = (I - (g_na*m^3*h*(u - v_na) + g_k*n^4*(u - v_k) + g_l*(u - v_l)))/C;
dmdt    = alpha_m*(1 - m) - beta_m*m;
dndt    = alpha_n*(1 - n) - beta_n*n;
dhdt    = alpha_h*(1 - h) - beta_h*h;

return
