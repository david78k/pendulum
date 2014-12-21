%%  Example 5:
%   In this example, we want to simulate a network of 2 coupled Izhikevich
%   neurons with an analog current stimulation. One of the neurons are
%   inhibitory and the other is excitatory. We will use very strong
%   synapses to see the inhibitatory effect. We also prevent the
%   self-feedback.
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
NetArch         = newarchfc(2 , 1 , -1 , [1 ; -1]);
NeuronWeight    = 800*NetArch.NeuronWeight;

NeuronWeight([1 , 4])   = 0;
NetArch.NeuronWeight    = NeuronWeight;

%% Defining Model of Neurons
NeuronModel     = newneuroniz;

%% Defining Synapses Model
SynapseModel    = newsynapse;

%% Defining Adaptation Model
AdaptModel      = newadapt;

%% Defining Simulation Parameters
SimParam        = newsim(10 , 0 , rand(2 , 1));

%% Defining Input/Output of the Model
IOMode          = newiomode('none' , {[]} , 100);

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