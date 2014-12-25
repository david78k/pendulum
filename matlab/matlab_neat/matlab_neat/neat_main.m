%%%%%%%%%%%%%%%%%%% Main NEAT file  (calls all other functions) (script file)

%% Neuro_Evolution_of_Augmenting_Topologies - NEAT 
%% developed by Kenneth Stanley (kstanley@cs.utexas.edu) & Risto Miikkulainen (risto@cs.utexas.edu)
%% Coding by Christian Mayr (matlab_neat@web.de)

clear;
tic;
%list of parameters
  
  %parameters of main file
    maxgeneration=200; %maximum number of generations for generational loop    
    load_flag=0; %if set to 1, will load population, generation, innovation_record and species_record from neatsave.mat at start of algorithm, if set to 0, algorithm will start with initial population, new species record and new innovation record, at generation=1 (default option)
    save_flag=1; %if set to 1, will save population, generation, innovation_record and species_record to neatsave.mat at every generation (default option)
    % upshot of this is: the settings above will start with initial population (of your specification) and save all important structures at every generation, so if your workstation crashes or you have to interrupt evolution, you can, at next startup, simply set the load flag to 1 and continue where you have left off. 
    % Please note, however, that only changing structures are saved, the parameters in this section will not be saved, so you have to ensure that you use the same parameters when using a saved version of evolution as when you created this saved version!
    % Also note that all variables are saved in binary matlab format, so file is only readable by matlab. If you want to look at some of the values stored in this file, load it in matlab, then you can access the saved values
    average_number_non_disabled_connections=[];
    average_number_hidden_nodes=[];
    max_overall_fitness=[];
    
  %parameters initial population
    population_size=150;
    number_input_nodes=2;
    number_output_nodes=1;
    vector_connected_input_nodes=[1 2]; %vector of initially connected input nodes out of complete number of input nodes 
                                          %(if you want to start with a subset and let evolution decide which ones are necessary)
                                          %for a fully connected initial population, uncomment the following:
    %vector_connected_input_nodes=1:number_input_nodes;
                                          
                                          
  %speciation parameters  
    % The following structure will contain various information on single species
    % This data will be used for fitness sharing, reproduction, and for visualisation purposes                       
    species_record(1).ID=0;%consecutive species ID's
    species_record(1).number_individuals=0;%number of individuals in species
    species_record(1).generation_record=[]; %matrix will be 4 rows by (number of generations existent) columns, will contain (from top to bottom) number of generation, mean raw fitness, max raw fitness, and index of individual in population which has produced max raw fitness 
    
    speciation.c1=1.0; %Speciation parameters as published by Ken Stanley
    speciation.c2=1.0;
    speciation.c3=0.4;
    speciation.threshold=3;
    
  %reproduction parameters
    %stagnation+refocuse
    stagnation.threshold=1e-2; %threshold to judge if a species is in stagnation (max fitness of species varies below threshold) this threshold is of course dependent on your fitness function, if you have a fitness function which has a large spread, you might want to increase this threshold 
    stagnation.number_generation=15; %if max fitness of species has stayed within stagnation.threshold in the last stagnation.number_generation generations, all its fitnesses will be reduced to 0, so it will die out
                                    %Computation is done the following way: the absolute difference between the average max fitness of the last stagnation.number_generation generations and the max fitness of each of these generations is computed and compared to stagnation.threshold. 
                                    %if it stays within this threshold for the indicated number of generations, the species is eliminated 
    refocus.threshold=1e-2;                                
    refocus.number_generation=20; %if maximum overall fitness of population doesn't change within threhold for this number of generations, only the top two species are allowed to reproduce                                
    
    
    %initial setup
    initial.kill_percentage=0.2; %the percentage of each species which will be eliminated (lowest performing individuals)
    initial.number_for_kill=5; % the above percentage for eliminating individuals will only be used in species which have more individuals than min_number_for_kill 
                               % Please note that whatever the above settings, the code always ensures that at least 2 individuals are kept to be able to cross over, or at least the single individual in a species with only one individual
    initial.number_copy=5; % species which have equal or greater than number_copy individuals will have their best individual copied unchanged into the next generation
    
    %selection (ranking and stochastic universal sampling)
    selection.pressure=2; % Number between 1.1 and 2.0, determines selective pressure towards most fit individual of species
    
    %crossover
    crossover.percentage=0.8; %percentage governs the way in which new population will be composed from old population.  exception: species with just one individual can only use mutation
    crossover.probability_interspecies=0.0 ; %if crossover has been selected, this probability governs the intra/interspecies parent composition being used for the
    crossover.probability_multipoint=0.6; %standard-crossover in which matching connection genes are inherited randomly from both parents. In the (1-crossover.probability_multipoint) cases, weights of the new connection genes are the mean of the corresponding parent genes
    

    
    %mutation
    mutation.probability_add_node=0.03;
    mutation.probability_add_connection=0.05;
    mutation.probability_recurrency=0.0; %if we are in add_connection_mutation, this governs if a recurrent connection is allowed. Note: this will only activate if the random connection is a recurrent one, otherwise the connection is simply accepted. If no possible non-recurrent connections exist for the current node genes, then for e.g. a probability of 0.1, 9 times out of 10 no connection is added. 
    mutation.probability_mutate_weight=0.9;
    mutation.weight_cap=8; % weights will be restricted from -mutation.weight_cap to mutation.weight_cap
    mutation.weight_range=5; % random distribution with width mutation.weight_range, centered on 0. mutation range of 5 will give random distribution from -2.5 to 2.5
    mutation.probability_gene_reenabled=0.25; % Probability of a connection gene being reenabled in offspring if it was inherited disabled 
     
    
%%%%%%%%%%%%%%%%main algorithm%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

if load_flag==0
   
   %call function to create initial population
   %for information about the make-up of the population structure and the innovation_record, look at initial_population.m
   [population,innovation_record]=initial_population(population_size, number_input_nodes, number_output_nodes, vector_connected_input_nodes);

   %initial speciation
   number_connections=(length(vector_connected_input_nodes)+1)*number_output_nodes;
   %put first individual in species one and update species_record
   population(1).species=1;
   matrix_reference_individuals=population(1).connectiongenes(4,:); %species reference matrix (abbreviated, only weights, since there are no topology differences in initial population)
   species_record(1).ID=1;
   species_record(1).number_individuals=1;

   %Loop through rest of individuals and either assign to existing species or create new species and use first individual of new species as reference
   for index_individual=2:size(population,2);
      assigned_existing_species_flag=0;
      new_species_flag=0;
      index_species=1;
      while assigned_existing_species_flag==0 & new_species_flag==0 %loops through the existing species, terminates when either the individual is assigned to existing species or there are no more species to test it against, which means it is a new species 
         distance=speciation.c3*sum(abs(population(index_individual).connectiongenes(4,:)-matrix_reference_individuals(index_species,:)))/number_connections; %computes compatibility distance, abbreviated, only average weight distance considered
         if distance<speciation.threshold %If within threshold, assign to the existing species 
            population(index_individual).species=index_species;
            assigned_existing_species_flag=1;
            species_record(index_species).number_individuals=species_record(index_species).number_individuals+1;
         end
         index_species=index_species+1;
         if index_species>size(matrix_reference_individuals,1) & assigned_existing_species_flag==0 %Outside of species references, must be new species
            new_species_flag=1;
         end
      end
      if new_species_flag==1 %check for new species, if it is, update the species_record and use individual as reference for new species
         population(index_individual).species=index_species;
         matrix_reference_individuals=[matrix_reference_individuals;population(index_individual).connectiongenes(4,:)];
         species_record(index_species).ID=index_species;
         species_record(index_species).number_individuals=1; %if number individuals in a species is zero, that species is extinct
      
      end    
   end
   generation=1;
else % start with saved version of evolution
   load 'neatsave'
end

%%%
%%%
%%%  Generational Loop
%%%
%%%
flag_solution=0;
while generation<maxgeneration & flag_solution==0
   
   if save_flag==1 % Backup copies of current generation
      save 'neatsave' population generation innovation_record species_record
   end
      
   % call evaluation function (in this case XOR), fitnesses of individuals will be stored in population(:).fitness
   % IMPORTANT reproduction assumes an (all positive!) evaluation function where a higher value means better fitness (in other words, the algorithm is geared towards maximizing a fitness function which can only assume values between 0 and +Inf) 
   
   population=xor_experiment(population);
   %population=fulladder_experiment(population);
   
   generation
   %compute mean and max raw fitnesses in each species and store in species_record.generation_record
   max_fitnesses_current_generation=zeros(1,size(species_record,2));

   for index_species=1:size(species_record,2)
      if species_record(index_species).number_individuals>0
         [max_fitness,index_individual_max]=max(([population(:).species]==index_species).*[population(:).fitness]);
         mean_fitness=sum(([population(:).species]==index_species).*[population(:).fitness])/species_record(index_species).number_individuals;  
         % Compute stagnation vector (last stagnation.number_generation-1 max fitnesses plus current fitness
         if size(species_record(index_species).generation_record,2)>stagnation.number_generation-2
            stagnation_vector=[species_record(index_species).generation_record(3,size(species_record(index_species).generation_record,2)-stagnation.number_generation+2:size(species_record(index_species).generation_record,2)),max_fitness];
            if sum(abs(stagnation_vector-mean(stagnation_vector))<stagnation.threshold)==stagnation.number_generation %Check for stagnation
               mean_fitness=0.01; %set mean fitness to small value to eliminate species (cannot be set to 0, if only one species is present, we would have divide by zero in fitness sharing. anyways, with only one species present, we have to keep it)
            end
         end         
         species_record(index_species).generation_record=[species_record(index_species).generation_record,[generation;mean_fitness;max_fitness;index_individual_max]];
         max_fitnesses_current_generation(1,index_species)=max_fitness;
      end
   end
   %check for refocus  
   [top_fitness,index_top_species]=max(max_fitnesses_current_generation);
   if size(species_record(index_top_species).generation_record,2)>refocus.number_generation
      index1=size(species_record(index_top_species).generation_record,2)-refocus.number_generation;
      index2=size(species_record(index_top_species).generation_record,2);
      if sum(abs(species_record(index_top_species).generation_record(3,index1:index2)-mean(species_record(index_top_species).generation_record(3,index1:index2)))<refocus.threshold)==refocus.number_generation
         [discard,vector_cull]=sort(-max_fitnesses_current_generation);         
         vector_cull=vector_cull(1,3:sum(max_fitnesses_current_generation>0));
         for index_species=1:size(vector_cull,2)
            index_cull=vector_cull(1,index_species);
            species_record(index_cull).generation_record(2,size(species_record(index_cull).generation_record,2))=0.01;
         end
      end
   end
       
   %visualisation fitness & species
   a=0;
   b=0;
   for index_individual=1:size(population,2)
      a=a+sum(population(index_individual).connectiongenes(5,:)==1);
      b=b+sum(population(index_individual).nodegenes(2,:)==3);
   end
   average_number_non_disabled_connections=[average_number_non_disabled_connections,[a/population_size;generation]];
   average_number_hidden_nodes=[average_number_hidden_nodes,[b/population_size;generation]];
   c=[];
   for index_species=1:size(species_record,2)
      c=[c,species_record(index_species).generation_record(1:3,size(species_record(index_species).generation_record,2))];
   end
   max_overall_fitness=[max_overall_fitness,[max(c(3,:).*(c(1,:)==generation));generation]];
   maximale_fitness=max(c(3,:).*(c(1,:)==generation))
   if maximale_fitness>15.6025
      flag_solution=1;
   end
       
   subplot(2,2,1);
   plot(average_number_non_disabled_connections(2,:),average_number_non_disabled_connections(1,:));
   ylabel('non disabled con');
   subplot(2,2,2);
   plot(average_number_hidden_nodes(2,:),average_number_hidden_nodes(1,:));
   ylabel('num hidden nodes');
   subplot(2,2,3);
   plot(max_overall_fitness(2,:),max_overall_fitness(1,:));
   ylabel('max fitness');
   drawnow;   
    
     
   
   if flag_solution==0
   %call reproduction function with parameters, current population and species record, returns new population, new species record and new innovation record
   [population,species_record,innovation_record]=reproduce(population, species_record, innovation_record, initial, selection, crossover, mutation, speciation, generation, population_size);
   toc;
   end
   %increment generational counter
   generation=generation+1; 
end