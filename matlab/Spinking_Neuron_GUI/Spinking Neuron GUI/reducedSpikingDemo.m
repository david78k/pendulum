function [] = reducedSpikingDemo_mod_input(input_index);

%% Input_index has one of four vaules: 
% 0 = load a custom image
% n = load the n-th test image (n is a positive integer)

if input_index ~= 0
        input_image = imread(['image_' num2str(input_index) '.jpg']);
        input_image = double(input_image(:,:,1));
else
        % Load image
        [filename, pathname] = uigetfile('*.*', 'Load an image file'); %Load the image file from directory
        if isequal(filename,0) %If no path is selected then display 'User Selected Cancel'
            disp('User selected Cancel')
        else % If a image file is selected display the path to the image
            disp(['User selected ', fullfile(pathname, filename)])
        end
        temp = imread(strcat(pathname, filename)); temp = double(temp); % Convert the image file onto numeric array
        input_image = temp(:,:,1); clear temp % Load only one matrix entry in case of color images
end

%% Parameters
int_time_step_v_EXC =0.2500;   % integration rate of variable v
int_time_step_u_EXC = 0.0200;  % integration rate of variable u
inp_ratio = 0.2500;  % coefficient that multiplies the input RGB value (in this case, it is a single value between 0 and 255)
resting_potential = -65;  % the resting potential of the cell
simulation_length = 200;  % the simulated time steps

%% Size population
N_Rec = size(input_image);  % Receptors; total # of receptor neurons

%% Receptors variables
Rec_record = resting_potential*ones(N_Rec(1)*N_Rec(2),simulation_length); %Initialize Receptors' cell voltages
Rec_v = resting_potential*ones(N_Rec);  % Initialise Receptors' membrane potential
Rec_u = .2*Rec_v;                       % Initialise Receptors' recovery variable

% Initialize figure for plots
h = figure; set(h, 'DoubleBuffer', 'on', 'color', 'white', 'Name', 'Reduced spiking equation demo');

%% Simulate for t time steps
for t = 1:simulation_length; %there will be simulation_length # of time steps
       
    %%%%%%%%%%%%% Receptors %%%%%%%%%%%%%
    Rec_fired = find(Rec_v >= 30);             % Search the indices of receptor neurons for spikes by seeing whether it has crossed the threshohold of 30mV
    if ~isempty(Rec_fired)                     % Reset F1 neurons after firing if Rec_v has crossed the spiking threshhold
        Rec_v(Rec_fired) = resting_potential;  % The receptor's membrane potencial is reset to its resting potencial
        Rec_u(Rec_fired) = Rec_u(Rec_fired)+ 8;  % The receptor's recovery variable is increased by a parameter value equal to 8
    end
    
    % Perform numerical integration using Euler's method 
    Rec_v = Rec_v + int_time_step_v_EXC*((0.04*Rec_v + 5) .* Rec_v + 140 - Rec_u + input_image * inp_ratio);    
    Rec_u = Rec_u + int_time_step_u_EXC .*(0.2 * Rec_v - Rec_u);                   
    
    % Store Receptors variables
    Rec_record(:,t) = reshape(Rec_v, 1, N_Rec(1)*N_Rec(2));
    Rec_record(Rec_record >= 30)=30; % Normalize spikes: in real neurons they cannot be above 30mV
        
    % Create PLOT
    
    subplot(2,2,1), imagesc(input_image), title(['Input image, cycle ', num2str(t), ' of ', num2str(simulation_length)]), axis off,...
        colormap gray, drawnow % display the loaded image and t time step
    subplot(2,2,2), imagesc(Rec_v>30, [0 1]), drawnow,  title('Receptor spikes'), colormap gray, drawnow % Display the neurion that are spiking at time t

    subplot(2,2,3), drawnow, plot(Rec_record(10,1:t), 'linewidth', 1), xlim([0 200]), ylim([-75 40]), title('Voltage of receptor #10') %Plot the voltage change of receptor #10
       subplot(2,2,4), drawnow, plot(Rec_record(200,1:t), 'red', 'linewidth',1), xlim([0 200]), ylim([-75 40]), title('Voltage of receptor #200') %Plot the voltage change of receptor #200
end;