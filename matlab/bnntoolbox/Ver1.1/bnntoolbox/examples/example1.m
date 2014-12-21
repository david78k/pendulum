%%  Example 1:
%   Simulating a Network of 3 Fully Cennected Integrate-and-Fire
%   Neurons
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
NetArch         = newarchfc(3 , 1 , -1);

%% Defining Neurons Model
NeuronModel     = newneuronif;

%% Defining Synapses Model
SynapseModel    = newsynapse;

%% Defining Adaptation Model
AdaptModel      = newadapt;

%% Defining Simulation Parameters
SimParam        = newsim(20);

%% Defining Input/Output Model
IOMode          = newiomode('none' , {[]} , 1);

%% Creating BNN Model
network         = newbnn(NetArch , NeuronModel , SynapseModel , AdaptModel , SimParam , IOMode);

%% Simulating the Model
network         = simbnn(network);

%% Plotting the Results
subplot(311)
plotstates(network , 1)
subplot(312)
plotstates(network , 2)
subplot(313)
plotstates(network , 3)