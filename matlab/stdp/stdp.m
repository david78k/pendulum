%stdp net----------------------------------------------------------------
%This is the main program for running stdp net simulations as used in
%mauscript.
%It creates an initial weight matrix and applies input (regular synchronous, 50Hz)
%for one hour while modifying weights according to STDP learning rule.
%Displays a rastergram, weight distribution, and input matrix during each second.
%Main output is a multidim matrix 'W', which contains the weight matrix at each
% minute of simulation.
%
%Code for the STDP implementation was inspired by Izhikevich, 2005
%and can be found at:
%http://senselab.med.yale.edu
%titled: spnet.m
%
%Written by David B. Stone, 2010 - 2012
%--------------------------------------------------------------------------
%-----Parameters-----------------------------------------------------------
N = 500; %number of neurons
E = round(.8*N); %# of excitatory
In = round(.2*N); % In = N - E; # of inhibitory
num_conn = round(.1*N); %# of synapses
a=[0.02*ones(E,1); .1*ones(In,1)]; %} a,b,c,d are neuron dynamical params
b = .2; %
c = -65; %
d=[8*ones(E,1); 2*ones(In,1)]; %
max_w = 8; %maximum weight per synapse
v = c; %voltage (initially a scalar, becomes an N-by-1 vector)
u = 0.2.*v; %relaxation variable
ftimes = zeros(N,1002); %a neuro-by-timepoint matrix of current stdp value(voltage)
dw = zeros(N); %differential weight matrix (updated ea ms according to stdp)
%--------------------------------------------------------------------------
%----Create an initial network--------------------------------------------
A = zeros(N); %Adjacency (binary) matrix
for i = 1:N
 p = randperm(N); %shuffle neurons
 q = round((5*randn)+num_conn); %pick #ofconnects from normdistrib(mean
%num_conn,var 25)
 h = p(1:q); %pick post syn neuros
 A(i,h)=1;
end;
%make the diagonal zeros (no self-connecting neurons)
diagonal = diag(A);
diagonal = diag(diagonal);
A = A-diagonal;
w = zeros(N); %weight matrix
ind = find(A);
for i = 1:length(ind)
 w(ind(i)) = max_w*rand; % uniform distr of weights (range from 0 to max_w)
end;

w(E+1:N,:) = -1 .* w(E+1:N,:); %make the inhib neuros negative
%-------------------------------------------------------------------------
%index of pre- and post-synaptic neurons for each neuron
pre = cell(N,1);
post = cell(N,1);
for i = 1:N
 pre{i} = find(w(:,i));
 post{i} = find(w(i,:));
end;
%multidim matrix of weights each minute
W = zeros(N,N,61);
W(:,:,1) = w; %Initial weight matrix
%--------Simulation (one hour)--------------------------------------------
for mins = 2:61 %minutes. min(1) is initial weight matrix

 for s = 1:60 %seconds

 %Input (Regular Synchronous)---------------------------------------
 P = zeros(N,1000); %input matrix (neurons-by-time)
 pt = linspace(1,981,50); %50 = every 20 spaces (50Hz)
 for i = 1:50 %length of pt
 z= randperm(500);
 e = uint16(100+randn);
 z = z(1:e); %pick ~100 random neurons
 P(z,pt(i)) = 16; %16 mV input
 end;
 %------------------------------------------------------------------

 for t = 1:1000 %millisecs

 I = .5*randn(N,1)+1.3; %weak internal noise term
 I = I + P(:,t); %external input
 F = find(v>=30); %index of neurons that fired at t-1 (at last ms)
 v(F)=c;
 u(F)=u(F)+d(F);
 ftimes(F,t+1) = .044; %= learning rate

 for i = 1:length(F) %for ea fired neuro
 I(post{F(i)}) = I(post{F(i)}) + w(F(i),post{F(i)})'; %synaptic
 %input
 %pre->post stdp
 dw(pre{F(i)},F(i)) = dw(pre{F(i)},F(i)) + ftimes(pre{F(i)},t);
 %post->pre stdp
%  dw(F(i),post{F(i)})=dw(F(i),post{F(i)})1.05*ftimes(post{F(i)},t)';
 dw(F(i),post{F(i)})=dw(F(i),post{F(i)}) + 1.05*ftimes(post{F(i)},t)';
 end;

 ftimes(:,t+2) = 0.95*ftimes(:,t+1); %reduce potentiation
%[(A+)e^(-t/20)=stdp rule]

 %Runge Kutta ------------------------
 h = .5;

 v1 = v;
 k1 = h*((.04*v1+5).*v1+140-u+I);
 v2 = (v +.5.*k1);
 k2 = h*((.04*v2+5).*v2+140-u+I);
 
v3 = (v+.5.*k2);
 k3 = h*((.04*v3+5).*v3+140-u+I);
 v4 = (v + k3);
 k4 = h*((.04*v4+5).*v4+140-u+I);
 v = v + (1/6).*(k1 + 2*k2 + 2*k3 + k4); %update voltage (4th order

 v1 = v;
 k1 = h*((.04*v1+5).*v1+140-u+I);
 v2 = (v +.5.*k1);
 k2 = h*((.04*v2+5).*v2+140-u+I);
 v3 = (v+.5.*k2);
 k3 = h*((.04*v3+5).*v3+140-u+I);
 v4 = (v + k3);
 k4 = h*((.04*v4+5).*v4+140-u+I);
 v = v + (1/6).*(k1 + 2*k2 + 2*k3 + k4); %do it again

 if max(v) > 30
 v_ind = find(v>=30);
 v(v_ind) = 30; %bound max voltage to 30 mV
 end;

 u1 = u;
 ku1 = a.*(b.*v-u1);
 u2 = u + .5.*ku1;
 ku2 = a.*(b.*v-u2);
 u3 = u + .5.*ku2;
 ku3 = a.*(b.*v-u3);
 u4 = u + ku3;
 ku4 = a.*(b.*v-u4);
 u = u + (1/6).*(ku1 + 2*ku2 + 2*ku3 + ku4); %update relaxation term
 %-----------------------------------------------

 end;

 %----- Plot ----------------------------------%
 [x,y] = find(ftimes==.044);
 ind = (w ~= 0);
 wt = w(ind);
 subplot(3,1,1);
 plot(y,x,'k.'); %rastergram
 title(s);
 axis([0 1000 0 N]); drawnow;
 subplot(3,1,2);
 hist(wt,100); %weight distribution
 title(mins-1);
 axis([0 8 0 16500]);drawnow;
 subplot(3,1,3);
 image(P);drawnow %external input
 %--------------------------------------------------%

 ftimes(:,1:2) = ftimes(:,1001:1002); %wrap around
 w(1:E,1:E) = max(0,min(max_w,w(1:E,1:E)+dw(1:E,1:E))); %update weights
 dw = .9*dw; %reduction term for stability

 end; %seconds

 W(:,:,mins) = w; %weight matrix at each minute

end; %minutes
