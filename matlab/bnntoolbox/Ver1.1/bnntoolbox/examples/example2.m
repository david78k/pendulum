%%  Example 2:
%   In this example, we want to show how to simulate neurons with different
%   parameters values. We create a network of 2 linear integrate-and-fire
%   neurons. One of them (neuron number 1) receives an analog input current
%   from outside. This neuron has a small a membrane time constant compared
%   to the other one (neuron number 2). The output of the neuron#1 is
%   connected to the input of the other (neuron number 2) with a strong
%   excitatory connection with a delay of 2msec.
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
% The architecture of this network is a feedforward one: 2 layers with 1
% neuron in each layer. We will first create a FF model.

NetArch         = newarchff([1 1] , 1 , -1 , [1 1] , 'one');
NeuronWeight    = NetArch.NeuronWeight;
NeuronDelay     = NetArch.NeuronDelay;

%% Modifying Weight and Delay Matrixes
% Now we will modify the weights and delay matrixes. To have a strong
% excitatory connection from neuron#1 to neuron#2, we have to insert a
% large weight in the proper position. Then we will modify the delay of
% connection to be 2msec. After these steps we will save the matrixes back.

NeuronWeight(2 , 1) = 10;
NeuronDelay(2 , 1)  = 2;

NetArch.NeuronWeight    = NeuronWeight;
NetArch.NeuronDelay     = NeuronDelay;

%% Defining Model of Neurons
% Now we will define the neuron model to be an integrate-and-fire with
% following parameters: neuron#1: C = 1, R = 1, neuron#1: C = 4, R = 5.

%                  C1  R1  C2  R2
ModelParam      = [1 , 1 ; 4 , 5];
NeuronModel     = newneuronif('linear' , ModelParam);

%% Defining Synapses Model
% We will set the PSP functions to be alpha-function with time constant of
% 20msec.

SynapseModel    = newsynapse('alpha_fun_ext' , 'def' , 'alpha_fun_int' , 20);

%% Defining Adaptation Model
% We have no adaptation mechanism.

AdaptModel      = newadapt;

%% Defining Simulation Parameters
% We will simulate the model for 30msec with a random initial conditions.

SimParam        = newsim(30 , 0 , rand(2 , 1));

%% Defining Input/Output of the Model
% Suppose the analog current value is 1.5 and we also select the auto save
% option to save the results in 'mysim' file.

IOMode          = newiomode('none' , {[]} , 1.5 , 1 , 'mysim');

%% Creating BNN Model
% We are ready to create the BNN model.

network         = newbnn(NetArch , NeuronModel , SynapseModel , AdaptModel , SimParam , IOMode);

%% Simulating the Final Model
% This section will simulate the model.

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