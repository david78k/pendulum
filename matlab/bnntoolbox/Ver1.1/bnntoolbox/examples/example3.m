%%  Example 3:
%   In this example, we want to simulate a network of 2 fully connected
%   canonical phase neurons without any external stimulation.
%
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

%% Clean Up Tasks: 
%  remove this section if you don't want to lose you data and figures in
%  the workspace. This section clears command window, clears current
%  figure, and clears workspace.

clc
close all
clear all

%% Model Definitions

%% Defining Network Structure
% Note that you must specify at least one input channel, although you don't
% want to use it.
NetArch             = newarchfc(2 , 1);
NetArch.InputWeight = zeros(2 , 1);     % disconnecting input from neurons

%% Defining Model of Neurons
% Neurons will be canonical phase model.
NeuronModel     = newneuroncanon;

%% Defining Synapses Model
% There is no external synapse, but internal synapses will be of Delta
% type.
SynapseModel    = newsynapse('none' , 'def' , 'delta_fun_int');

%% Defining Adaptation Model
% No adaptation.
AdaptModel      = newadapt;

%% Defining Simulation Parameters
% Simulating with random initial conditions for 30msec.
SimParam        = newsim(30 , 0 , rand(2 , 1));

%% Defining Input/Output of the Model
% No external inputs
IOMode          = newiomode('none' , {[]} , []);

%% Creating BNN Model
network         = newbnn(NetArch , NeuronModel , SynapseModel , AdaptModel , SimParam , IOMode);

%% Simulating the Final Model
network         = simbnn(network);

%% Plotting the Results
subplot(211)
plotstates(network , 1)
ylabel('First Neuron')
title('States')

subplot(212)
plotstates(network , 2)
ylabel('Second Neuron')
xlabel('Time (msec.)')

figure
subplot(211)
plotspikes(network , 1)
ylabel('First Neuron')
title('Spikes')

subplot(212)
plotspikes(network , 2)
ylabel('Second Neuron')
xlabel('Time (msec.)')

figure
plotspikes2d(network)