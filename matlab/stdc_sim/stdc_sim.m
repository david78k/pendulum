function stdc_sim() 
%stdc_sim() - Spike timing-dependent construction simulation
%
%  function [firings, growth, SNN, inputParam] = stdc_sim() 
%  Uncomment alternative function and comment out 'function stdc_sim()' to 
%  get optional simulation outputs.
%
%  This function is an example implementation of the 'spike timing-
%  dependent construction' (STDC) learning algorithm; a constructive 
%  approach to spike timing-dependent plasticity (STDP).  Starting with a 
%  minimal network (only input neurons), neurons and connection are 
%  constructed in response to the detection of temporally correlated input 
%  spikes.
%
%  In this example STDC is applied to a 2-dimensional random neural field 
%  that is excited by a vertical strip that travels cyclically across the 
%  field in both horizontal directions.  The output of this simulation are 
%  plots of the spatial location and the growth and firing of neurons.
%
%  Optional simulation outputs:
%   firings - matrix of spike time and neuron index
%   growth - matrix of growth time and neuron index
%   SNN - a data structure containing the spiking neural network
%     parameters (Izhikevich model)
%   inputParam - a data structure containing the input 

%  Author: Toby Lightheart
%  Date Last Modified: 09/12/2011
%  Contact: toby.lightheart@adelaide.edu.au
%  Institution: School of Mechanical Engineering,
%        University of Adelaide, Australia

%% Initialize the simulation and algorithm variables

nMax = 100;         % Maximum number of neurons, default nMax = 100;
Ne = 20;            % Initial excitatory neurons, default Ne = 20;
n = Ne;             % Number of neurons

inputNeurons = [ones(n,1); zeros(nMax-n,1)];  % input neurons

firings = [];       % record of all spikes
growth = [];        % record growth of neurons

% Initialize spiking neural network
SNN = spiking_net_init();

% Initialize input parameters
inputParam = input_init(n);

% Initialize constructive algorithm
Tc = 10;            % time threshold for construction, default Tc = 10;

f = false(nMax,1);              % current active neurons
ft = -Tc*ones(nMax,1);          % time of last spike
C = get_binary_connections();   % binary matrix of network connections


%% Simulate spiking network with spike timing dependent construction (STDC)

TS = 1;             % time step of 1 ms - also used for input update
for t=1:TS:1000     % simulation of 1000 ms
    
    % update network
    spiking_net_update();
    
    % get neuron spike vector
    f = get_spikes(inputNeurons);       % binary vector of spikes
    
    % spike timing-dependent construction search
    ft = ft - f.*ft + f*t;      % update neuron spike time vector
    r = ft > (t-Tc);            % binary vector of 'recent' input spikes
    x = sum(1*((C*r - C*~r)==sum(r)));  % find connection combination
    
    if ~x
        % add the neuron to the network 
        create_neuron(r);
        
        % add to binary matrix of network connections
        C = get_binary_connections();
    end
end

% plot the neural field and the network growth and spikes
display_results();


%% Spiking neural network simulation and interface functions

% Spiking neural network initialisation
    function SNN = spiking_net_init()
        % SNN = spiking_net_init() - initialise the simulated 'SNN'
        %  Initialise a spiking neural network (SNN) using the 'simple 
        %  model' created by E. M. Izhikevich (2003)
        
        % 'regular spiking' neurons 
        %  default parameter values: a=0.02, b=0.2, c=-65, d=8        
        a=[0.02*ones(Ne,1);     zeros(nMax-n, 1)];  % Recovery rate
        b=[0.2*ones(Ne,1);      zeros(nMax-n, 1)];  % Recovery sensitivity
        c=[-65*ones(Ne,1);      zeros(nMax-n, 1)];  % Refractory reset
        d=[8*ones(Ne,1);        zeros(nMax-n, 1)];  % Refractory recovery

        S=[zeros(nMax, n)       zeros(nMax, nMax-n)]; % Synapse weights
        
        % initial membrane potential, default v=-65
        v=[-65*ones(n,1);       zeros(nMax-n,1)];   % Initial values of v
        u=b.*v;                                     % Initial values of u
        
        % Return the spiking neural network data structure 
        SNN = struct('a',a, 'b',b, 'c',c, 'd',d, 'S',S, 'v',v, 'u',u);
    end

% Spiking neural network input
    function in = spiking_net_input()
        % in = spiking_net_input() - generate spiking network input 'in'
        %  Interface function between spiking neural network and the 
        %  an input signal generation function: 
        %    in = update_input()
        
        in = update_input();
    end

% Spiking neural network update
    function spiking_net_update()
        % spiking_net_update() - update the SNN for a single time step
        %  Update spiking neural network data (stored in 'SNN') using the 
        %  'simple model' created by E. M. Izhikevich (2003)
        
        % neuron spike threshold potential
        SP_T = 30;  % default SP_T = 30;
        
        % update post-spike potentials
        s = (SNN.v >= SP_T);        % find neurons that have spiked
        SNN.v(s) = SNN.c(s);        % reduce potential to post-spike value
        SNN.u(s) = SNN.u(s) + SNN.d(s); % add refractoriness
        
        % update spiking network inputs
        I = spiking_net_input();
        I = [I; zeros(nMax-size(I,1),1)]+sum(SNN.S(:,s),2);
        
        % standard differential equation for neuron potential update
        %  (Izhikevich 2003): v' = 0.04*v^2 + 5*v + 140 - u + I
        %  Izhikevich implements 0.5 ms step for numerical stability 
        SNN.v = SNN.v+0.5*(0.04*SNN.v.^2+5*SNN.v+140-SNN.u+I);    
        SNN.v = SNN.v+0.5*(0.04*SNN.v.^2+5*SNN.v+140-SNN.u+I);    
        
        % prevent unusual behaviour in empty network matrix locations
        SNN.v = SNN.v.*[ones(n,1); zeros(nMax-n,1)];
        
        % update refractoriness of neurons
        SNN.u = SNN.u+SNN.a.*(SNN.b.*SNN.v-SNN.u);      
        
        % store spikes for graphical output
        firings = [firings; t+0*find(s>0),find(s>0)];
    end

% Get network spikes from network
    function spikes = get_spikes(neurons)
        % spikes = get_spikes(neurons) - test if 'neurons' have spiked
        %  Return a binary vector 'spikes' indicating whether 
        %  input parameter 'neurons' have spiked

        % neuron spike threshold potential
        SP_T = 30;  % default SP_T = 30;

        spikes = (SNN.v).*neurons >= SP_T;  % return spikes
    end

% Get binary connections of the neural network
    function bin = get_binary_connections()
        % bin = get_binary_connections() - create a binary matrix of
        % SNN connections
        %  Find all network connections that are above the threshold and
        %  return them as a binary matrix 'bin'
        
        % synapse weight threshold
        SW_T = 0;   % default SW_T = 0;
        
        bin = 1*(SNN.S > SW_T);    % create a binary matrix of synapses > 0
    end
    
% Create a new spiking neuron with given connections
    function create_neuron(req)
        % create_neuron(req) - create a neuron with 'req' connections
        %  Create a neuron with connections from 'req' in spiking neural
        %  network 'SNN'
        
        % Initial weight constant to control total input to new neuron
        INIT_W = 40;    % default INIT_W = 40;
        
        if n<nMax        
            % increment the number of neurons
            n=n+1;      

            % add new attributes to neural network property vectors
            %  'regular spiking' neuron 
            %  default a=0.02, b=0.2, c=-65, d=8   
            SNN.a(n) = 0.02;    % Recovery rate
            SNN.b(n) = 0.2;     % Recovery sensitivity
            SNN.c(n) = -65;     % Refractory reset
            SNN.d(n) = 8;       % Refractory recovery
            
            % initial neuron potential and refractoriness
            %  set SNN.v(n)=0 for immediate spiking after creation
            SNN.v(n)= -65;      % Initial value of v, default SNN.v(n)=-65;
            SNN.u(n)= 0; %SNN.b(n).*SNN.v(n);   % Initial value of u

            % add new neuron connection weights to synapse matrix
            SNN.S(n,:) = INIT_W/sum(req).*req';
            
            % record growth for graphical output
            growth=[growth; t+0*n,n];
         end
     end
        
%% Neuron field with spatial input

 % initialize neural network input
    function [param] = input_init(numIn)
        % [param] = input_init(numIn) - initialise network input
        %  Initialise the spiking neural network input parameters 'param'
        %  for a given number of inputs 'numIn'
        
        % neuron field parameters
        field = gen_neuron_field(numIn);
        
        % line input parameters
        line = gen_input_line();
        
        % return field and line
        param.f = field;
        param.l = line;        
    end
    
 % generate a spatial representation of input neurons
    function field = gen_neuron_field(numIn)
        % field = gen_neuron_field(numIn) - generate a 2D field of neurons
        %  Generate a 2D spatial field of a total of 'numIn' neurons and
        %  return the 'field' parameters
        
        field.NUM = numIn;  % number of neurons
        field.SIZE = 1;     % size of the distribution area
        field.loc = field.SIZE*rand(numIn,2);   % randomise locations
    end

 % generate an activation pattern (line) for the neuron field
    function line = gen_input_line()
        % line = gen_input_line() - generate an excitatory input line 
        %  Create a finite width 'line' to operate as an activation region
   
        line.WI = 0.1;      % width of the input line, default 0.1
        line.SP = 0.01;     % speed line travels, default 0.01
        line.SL = [0; 0.1]; % slope of line [x; y], default [0; 0.1]
        line.DI = [1; 0];   % direction of movement [x; y], default [1; 0]

        line.ST = [0; 0];   % start/restart line location, default [0; 0]
        line.loc = line.ST; % initialise the location of the line [x; y]
    end

 % update the input neuron activation
    function [in] = update_input()
        % [in, inputParam] = update_input(inputParam) - Update network
        % input line position
        %  Update the input parameters 'inputParam' and neuron activation 
        %  'in'
        
        INC = 10;   % potential increase from input, default 10
        
        % update the location of the input line
        inputParam.l.loc = inputParam.l.loc ... 
                                + TS * inputParam.l.DI * inputParam.l.SP;

        % change input properties at t=500 - uncomment to observe effect of
        % changing input region orientation and direction
%         if t==500    
%             inputParam.l.SL = [0.1; 0];    % change line orientation
%             inputParam.l.DI = [0; 1];      % change line direction
%             inputParam.l.loc = inputParam.l.ST;    % change line location
%         end
        
        % reset line if exceeding field boundaries
        if any(inputParam.l.loc>inputParam.f.SIZE | inputParam.l.loc<[0;0])
            inputParam.l.DI = -inputParam.l.DI; % reverse line direction        
        end
        
        % calculate distance using two point description of line
        p1 = inputParam.l.loc;
        p2 = inputParam.l.loc + inputParam.l.SL;
        
        % calculate the distance of the neuron to the line
        dist = abs((p2(1)-p1(1))*(p1(2)-inputParam.f.loc(:,2)) ...
                        - (p2(2)-p1(2))*(p1(1)-inputParam.f.loc(:,1)));
        dist = dist/sqrt((p2(1)-p1(1))^2 + (p2(2)-p1(2))^2);
        
        % input to neurons that are within activation distance
        in = INC*(dist < inputParam.l.WI/2);
    end
        
%% Plot output from the operation of the constructive algorithm

    function display_results()
        % display_results() - Display results of the simulation
        %  Plot the spatial location of input neurons and the pattern of
        %  growth and activation in the network population
        
        % plot the spatial location of input neurons
        figure(1);
        cla;
        plot(inputParam.f.loc(:,1),inputParam.f.loc(:,2), 'o');
        title('Neural Field');
        ylabel('y Position');
        xlabel('x Position');
        legend('Neuron');
        
        % plot the timing and index of neuron spikes
        figure(2);
        hold off;
        if ~isempty(firings)
            plot(firings(:,1),firings(:,2),'.');
        end
        
        % plot the timing and index of neuron growth
        hold on;
        if ~isempty(growth)
            plot(growth(:,1),growth(:,2),'b+', 'MarkerSize', 5);
        end
        title('Neuron Spikes and Growth');
        ylabel('Neuron');
        xlabel('Time (ms)');
        legend('Spike', 'Growth');
    end

%% End of STDC
% Reference:
%   E. M. Izhikevich, “Simple model of spiking neurons,” IEEE Trans.
%   Neural Netw., vol. 14, no. 6, pp. 1569–1572, Nov. 2003.
end
