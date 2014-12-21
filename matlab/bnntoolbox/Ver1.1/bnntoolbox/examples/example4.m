%%  Example 4:
%   In this example, we want to simulate a network of 3 independent
%   Hodgkin-Huxley neurons with an analog current stimulation. Note that we
%   chose input weights to be random. This will cause a random input
%   current stimulation, although the external current is constant.
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
NetArch                 = newarchff(3 , 1 , -1);
NetArch.NeuronWeight    = zeros(3 , 3);     % disconnecting neurons from each other

%% Defining Model of Neurons
NeuronModel     = newneuronhh;

%% Defining Synapses Model
SynapseModel    = newsynapse;

%% Defining Adaptation Model
AdaptModel      = newadapt;

%% Defining Simulation Parameters
% Note that initial condition is for a single neuron, since HH model has 4
% states. The simulator porgram will generalize it to all other neurons.
SimParam        = newsim(100 , 0 , [-65 ; 0.05 ; 0.317 ; 0.6]);

%% Defining Input/Output of the Model
IOMode          = newiomode('none' , {[]} , 50);

%% Creating BNN Model
network         = newbnn(NetArch , NeuronModel , SynapseModel , AdaptModel , SimParam , IOMode);

%% Simulating the Final Model
network         = simbnn(network);

%% Plotting the Results
subplot(311)
plotstates(network , 1)
ylabel('First Neuron')
title('States')

subplot(312)
plotstates(network , 2)
ylabel('Second Neuron')

subplot(313)
plotstates(network , 3)
ylabel('Third Neuron')
xlabel('Time (msec.)')

figure
subplot(311)
plotspikes(network , 1)
ylabel('First Neuron')
title('Spikes')

subplot(312)
plotspikes(network , 2)
ylabel('Second Neuron')

subplot(313)
plotspikes(network , 3)
ylabel('Third Neuron')
xlabel('Time (msec.)')

figure
plotspikes2d(network)