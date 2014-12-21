%%  Example 6:
%   In this example, we want to show simulation of a network of 20
%   integrate-and-fire neurons, with random threshold values. Also there is
%   2 spiking inputs and 1 analog current input.
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
NetArch         = newarchff(20 , 3 , [1 ; 1 ; -1]);

%% Defining Model of Neurons
Thresholds      = rand(20 , 1);
NeuronModel     = newneuronif('linear' , 'def' , Thresholds);

%% Defining Synapses Model
SynapseModel    = newsynapse;

%% Defining Adaptation Model
AdaptModel      = newadapt;

%% Defining Simulation Parameters
SimParam        = newsim(30 , 0 , rand(20 , 1));

%% Defining Input/Output of the Model
% Input spike times are given bellow.
IOMode          = newiomode('none' , {[2 5 7 18] ; [3 6 10 20 25]} , 1.1);

%% Creating BNN Model
network         = newbnn(NetArch , NeuronModel , SynapseModel , AdaptModel , SimParam , IOMode);

%% Simulating the Final Model
network         = simbnn(network);

%% Plotting the Results
plotspikes2d(network)