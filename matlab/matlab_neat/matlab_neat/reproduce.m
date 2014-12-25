%% Reproduction -Main Evolutionary algorithm (Mutation, crossover, speciation) 

%% Neuro_Evolution_of_Augmenting_Topologies - NEAT 
%% developed by Kenneth Stanley (kstanley@cs.utexas.edu) & Risto Miikkulainen (risto@cs.utexas.edu)
%% Coding by Christian Mayr (matlab_neat@web.de)

function [new_population,updated_species_record,updated_innovation_record]=reproduce(population, species_record, innovation_record, initial, selection, crossover, mutation, speciation, generation, population_size);

% the following 'for' loop has these three objectives:

% 1.compute matrix of existing and propagating species from species_record (first row), assign their alloted number of offspring from the shared fitness (second row), and set their actual number of offspring to zero (third row) (will be incremented as new individuals are created from this species)

% 2.copy most fit individual in every species with more than initial.number_copy individuals unchanged into new generation (elitism) (But only if species is not dying out, i.e. has at least one individual allotted to itself in the new generation)
%   utilizes data stored in species_record.generation_record (index of individual in population having the highest fitness)

% 3.erase lowest percentage (initial.kill_percentage) in species with more than initial.number_for_kill individuals to keep them from taking part in reproduction
% Matlab actually doesn't offer a facility for redirecting the pointers which link one element in a structure with the next, so essentially this individual entry in the population structure cannot be erased. Rather, it will have its species ID set to zero, 
% which has the same effect of removing it from the rest of the reproduction cycle, since all reproduction functions access the population structure through the species ID and no species has an ID of zero

% Compute sum_average_fitnesses
sum_average_fitnesses=0;
for index_species=1:size(species_record,2)
   sum_average_fitnesses=sum_average_fitnesses+species_record(index_species).generation_record(2,size(species_record(index_species).generation_record,2))*(species_record(index_species).number_individuals>0);
end
% The following two lines only initialize the new_population structure. Since its species is set to 0, the rest of the algorithm will not bother with it. It gets overwritten as soon as the first really new individual is created
new_population(1)=population(1);
new_population(1).species=0;

overflow=0;
index_individual=0;
matrix_existing_and_propagating_species=[];
for index_species=1:size(species_record,2)
   if species_record(index_species).number_individuals>0 %test if species existed in old generation
      number_offspring=species_record(index_species).generation_record(2,size(species_record(index_species).generation_record,2))/sum_average_fitnesses*population_size; %compute number of offspring in new generation
      overflow=overflow+number_offspring-floor(number_offspring);
      if overflow>=1 %Since new species sizes are fractions, overflow sums up the difference between size and floor(size), and everytime this overflow is above 1, the species gets one additional individual alotted to it  
         number_offspring=ceil(number_offspring);
         overflow=overflow-1;
      else
         number_offspring=floor(number_offspring);
      end
      if number_offspring>0 %Check to see if species is dying out, only add those species to matrix_existing_and_propagating_species which have offspring in the new generation
         matrix_existing_and_propagating_species=[matrix_existing_and_propagating_species,[species_record(index_species).ID;number_offspring;0]]; %matrix (objective 1)
         if species_record(index_species).number_individuals>=initial.number_copy %check for condition for objective 2
            index_individual=index_individual+1;
            new_population(index_individual)=population(species_record(index_species).generation_record(4,size(species_record(index_species).generation_record,2))); % Objective 2
            matrix_existing_and_propagating_species(3,size(matrix_existing_and_propagating_species,2))=1; %Update matrix_existing_and_propagating_species
         end
      end
      
      if (species_record(index_species).number_individuals>initial.kill_percentage) & (ceil(species_record(index_species).number_individuals*(1-initial.kill_percentage))>2) %check condition for objective 3
         matrix_individuals_species=[find([population(:).species]==index_species);[population(find([population(:).species]==index_species)).fitness]];
         [sorted_fitnesses_in_species,sorting_vector]=sort(matrix_individuals_species(2,:));
         matrix_individuals_species=matrix_individuals_species(:,sorting_vector);
         sorting_vector=matrix_individuals_species(1,:);
 
         for index_kill=1:floor(species_record(index_species).number_individuals*initial.kill_percentage)
            population(sorting_vector(index_kill)).species=0; %objective 3
         end         
      end
      
   end
end

% generate reference of random individuals from every species from old population 
% cycle through species ID's, and add reference individuals from old population, new species of new population will be added during reproduction
index_ref=0;
for index_species_ref=1:size(species_record,2)
   if sum([population(:).species]==index_species_ref)>0 %Check if species exists in old population 
      index_ref=index_ref+1;
      [discard,index_ref_old]=max(([population(:).species]==index_species_ref).*rand(1,size(population,2)));
      population_ref(index_ref)=population(index_ref_old);
   end
end 
matrix_existing_and_propagating_species

%% Standard-reproduction (Main)
%% Cycle through all existing species
for index_species=1:size(matrix_existing_and_propagating_species,2)
   count_individuals_species=0;
   species_ID=matrix_existing_and_propagating_species(1,index_species); %this is ID of species which will be reproduced this cycle
   %IMPORTANT: index_species only has relevance to matrix_existing_and_propagating_species, all other mechanisms using species in some way must use species_ID
   
   % Linear Ranking and stochastic universal sampling
   % Ranking with selection.pressure
   fitnesses_species=[population(find([population(:).species]==species_ID)).fitness];
   index_fitnesses_species=find([population(:).species]==species_ID);
   [discard,sorted_fitnesses]=sort(fitnesses_species);
   ranking=zeros(1,size(fitnesses_species,2));
   ranking(sorted_fitnesses)=1:size(fitnesses_species,2);
   if size(fitnesses_species,2)>1
       FitnV=(2-selection.pressure+2*(selection.pressure-1)/(size(fitnesses_species,2)-1)*(ranking-1))';  
   else
       FitnV=2;
   end
   % stochastic universal sampling
   % First compute number of individuals to be selected (two parents required for every offspring through crossover, one for mutation) 
   number_overall=matrix_existing_and_propagating_species(2,index_species)-matrix_existing_and_propagating_species(3,index_species);
   number_crossover=round(crossover.percentage*number_overall);
   number_mutate=number_overall-number_crossover;
   Nind=size(fitnesses_species,2);
   Nsel=2*number_crossover+number_mutate;
   
   if Nsel==0 %rare case, in which a species with at least initial.number_copy individuals gets individual copied, but compares poorly to new generation, which results in this copied individual being 
      %the only individual of this species in new generation, so no crossover or mutation takes place. 
      %setting Nsel to 1 will prevent division by zero error, but will have no other effect since the propagation loop is governed by matrix_existing_and_propagating_species, not by Nsel
      Nsel=1;        
   end   
   
   % Perform stochastic universal sampling (Code Snippet from Genetic Algorithm toolbox 1.2 by Chipperfield et al)
   cumfit = cumsum(FitnV);
   trials = cumfit(Nind) / Nsel * (rand + (0:Nsel-1)');
   Mf = cumfit(:, ones(1, Nsel));
   Mt = trials(:, ones(1, Nind))';   
   [NewChrIx, ans] = find(Mt < Mf & [ zeros(1, Nsel); Mf(1:Nind-1, :) ] <= Mt);
   % Shuffle selected Individuals
   [ans, shuf] = sort(rand(Nsel, 1));
   NewChrIx = NewChrIx(shuf);
   % relate to indexes of individuals in population
   NewChrIx = index_fitnesses_species(NewChrIx);

   
   while matrix_existing_and_propagating_species(3,index_species)<matrix_existing_and_propagating_species(2,index_species) %cycle until actual number of offspring has reached allotted number of offspring                                                          
      index_individual=index_individual+1;
      count_individuals_species=count_individuals_species+1;
      % Crossover 
      if count_individuals_species<=number_crossover %O.k: we are doing crossover
         %Select Parents
         %select parent1          
         parent1=population(NewChrIx(2*count_individuals_species-1));
         %select parent2
         found_parent2=0;
         %sum([species_record(:).number_individuals]>0)
         if (rand<crossover.probability_interspecies) & (size(matrix_existing_and_propagating_species,2)>1) %select parent2 from other species (can only be done if there is more than one species in old population)          
            while found_parent2==0;                
               [discard,index_parent2]=max(rand(1,size(population,2)));
               parent2=population(index_parent2);
               found_parent2=((parent2.species~=0) & (parent2.species~=parent1.species)); %check if parent2.species is not species 0 (deleted individual) or species of parent1
            end
            parent2.fitness=parent1.fitness; %set fitnesses to same to ensure that disjoint and excess genes are inherited fully from both parents (tip from ken)
         else % O.K. we take parent2 from same species as parent1
             parent2=population(NewChrIx(2*count_individuals_species));
         end
             
         %Crossover
         %inherit nodes from both parents
         new_individual.nodegenes=[];
         matrix_node_lineup=[[parent1.nodegenes(1,:);1:size(parent1.nodegenes,2);zeros(1,size(parent1.nodegenes,2))],[parent2.nodegenes(1,:);zeros(1,size(parent2.nodegenes,2));1:size(parent2.nodegenes,2)]];
         [discard,sort_node_vec]=sort(matrix_node_lineup(1,:));
         matrix_node_lineup=matrix_node_lineup(:,sort_node_vec);
         node_number=0;
         for index_node_sort=1:size(matrix_node_lineup,2)   
            if node_number~=matrix_node_lineup(1,index_node_sort)
               if matrix_node_lineup(2,index_node_sort)>0
                  new_individual.nodegenes=[new_individual.nodegenes,parent1.nodegenes(:,matrix_node_lineup(2,index_node_sort))];
               else
                  new_individual.nodegenes=[new_individual.nodegenes,parent2.nodegenes(:,matrix_node_lineup(3,index_node_sort))];
               end               
               node_number=matrix_node_lineup(1,index_node_sort);
            end
         end
         %Crossover connection genes
         %first do lineup of connection genes
         matrix_lineup=[[parent1.connectiongenes(1,:);1:size(parent1.connectiongenes,2);zeros(1,size(parent1.connectiongenes,2))],[parent2.connectiongenes(1,:);zeros(1,size(parent2.connectiongenes,2));1:size(parent2.connectiongenes,2)]];
         [discard,sort_vec]=sort(matrix_lineup(1,:));
         matrix_lineup=matrix_lineup(:,sort_vec);
         final_matrix_lineup=[];
         innovation_number=0;
         for index_sort=1:size(matrix_lineup,2)   
            if innovation_number~=matrix_lineup(1,index_sort)
               final_matrix_lineup=[final_matrix_lineup,matrix_lineup(:,index_sort)];
               innovation_number=matrix_lineup(1,index_sort);
            else
               final_matrix_lineup(2:3,size(final_matrix_lineup,2))=final_matrix_lineup(2:3,size(final_matrix_lineup,2))+matrix_lineup(2:3,index_sort);
            end
         end            
         % O.K. Connection Genes are lined up, start with crossover
         new_individual.connectiongenes=[];

         for index_lineup=1:size(final_matrix_lineup,2)
            if (final_matrix_lineup(2,index_lineup)>0) & (final_matrix_lineup(3,index_lineup)>0) %check for matching genes, do crossover
               if rand<0.5 %random crossover for matching genes
                  new_individual.connectiongenes=[new_individual.connectiongenes,parent1.connectiongenes(:,final_matrix_lineup(2,index_lineup))];
               else
                  new_individual.connectiongenes=[new_individual.connectiongenes,parent2.connectiongenes(:,final_matrix_lineup(3,index_lineup))];
               end
               if rand>crossover.probability_multipoint %weight averaging for offspring, otherwise the randomly inherited weights are left undisturbed
                  new_individual.connectiongenes(4,size(new_individual.connectiongenes,2))=(parent1.connectiongenes(4,final_matrix_lineup(2,index_lineup))+parent2.connectiongenes(4,final_matrix_lineup(3,index_lineup)))/2;
               end                              
            end                   
            parent1_flag=sum(final_matrix_lineup(2,index_lineup+1:size(final_matrix_lineup,2)));  % test if there exist further connection genes from index_lineup+1 to end of final_matrix_lineup for parent1 (to detect excess)
            parent2_flag=sum(final_matrix_lineup(3,index_lineup+1:size(final_matrix_lineup,2)));  % test if there exist further connection genes from index_lineup+1 to end of final_matrix_lineup for parent1 (to detect excess)
            % Two cases to check (excess is taken care of in the disjoint gene checks)
            if (final_matrix_lineup(2,index_lineup)>0) & (final_matrix_lineup(3,index_lineup)==0) %Disjoint parent1
               if parent1.fitness>=parent2.fitness
                  new_individual.connectiongenes=[new_individual.connectiongenes,parent1.connectiongenes(:,final_matrix_lineup(2,index_lineup))];
               end         
            end
            if (final_matrix_lineup(2,index_lineup)==0) & (final_matrix_lineup(3,index_lineup)>0) %Disjoint parent2
               if parent2.fitness>=parent1.fitness
                  new_individual.connectiongenes=[new_individual.connectiongenes,parent2.connectiongenes(:,final_matrix_lineup(3,index_lineup))];
               end              
            end             
         end
         new_individual.fitness=0; %has no impact on algorithm, only required for assignment to new population
         new_individual.species=parent1.species; %will be species hint for speciation
      else % no crossover, just copy a individual of the species and mutate in subsequent steps          
         new_individual=population(NewChrIx(number_crossover+count_individuals_species));
      end
      
      % Hidden nodes culling (remove any hidden nodes where there is no corresponding connection gene in the new individual)
      connected_nodes=[];
      for index_node_culling=1:size(new_individual.nodegenes,2)
          node_connected_flag=sum(new_individual.connectiongenes(2,:)==new_individual.nodegenes(1,index_node_culling))+sum(new_individual.connectiongenes(3,:)==new_individual.nodegenes(1,index_node_culling));
          if (node_connected_flag>0) | (new_individual.nodegenes(2,index_node_culling)~=3);
              connected_nodes=[connected_nodes,new_individual.nodegenes(:,index_node_culling)];
          end
      end
      new_individual.nodegenes=connected_nodes;
                 
      % Disabled Genes Mutation
      %run through all connection genes in a new_individual, find disabled connection genes, enable again with crossover.probability_gene_reenabled probability
      for index_connection_gene=1:size(new_individual.connectiongenes,2)
         if (new_individual.connectiongenes(5,index_connection_gene)==0)& (rand<mutation.probability_gene_reenabled)
            new_individual.connectiongenes(5,index_connection_gene)=1; 
         end
      end
            
      % Weight Mutation
      %run through all connection genes in a new_individual, decide on mutating or not
      for index_connection_gene=1:size(new_individual.connectiongenes,2)
         if rand<mutation.probability_mutate_weight %*index_connection_gene/size(new_individual.connectiongenes,2) %linearly biased towards higher probability of mutation at end of connection genes
            new_individual.connectiongenes(4,index_connection_gene)=new_individual.connectiongenes(4,index_connection_gene)+mutation.weight_range*(rand-0.5); 
         end
         % weight capping
         new_individual.connectiongenes(4,index_connection_gene)=new_individual.connectiongenes(4,index_connection_gene)*(abs(new_individual.connectiongenes(4,index_connection_gene))<=mutation.weight_cap)+(sign(new_individual.connectiongenes(4,index_connection_gene))*mutation.weight_cap)*(abs(new_individual.connectiongenes(4,index_connection_gene))>mutation.weight_cap);
      end
      
      % IMPORTANT: The checks for duplicate innovations in the following two types of mutation can only check in the current generation
      % Add Connection Mutation
      flag_recurrency_enabled=rand<mutation.probability_recurrency;
      vector_possible_connect_from_nodes=new_individual.nodegenes(1,:); %connections can run from every node
      vector_possible_connect_to_nodes=new_individual.nodegenes(1,find((new_individual.nodegenes(2,:)==2)+(new_individual.nodegenes(2,:)==3))); %connections can only run into hidden and output nodes
      number_possible_connection=length(vector_possible_connect_from_nodes)*length(vector_possible_connect_to_nodes)-size(new_individual.connectiongenes,2);
      
      flag1=(rand<mutation.probability_add_node);
      
      if (rand<mutation.probability_add_connection) & (number_possible_connection>0) & (flag1==0) %check if new connections can be added to genes (if there are any possible connections which are not already existing in genes of new individual)
         % First build matrix containing all possible new connection for nodegene of new individual 
         new_connection_matrix=[];
         for index_connect_from=1:length(vector_possible_connect_from_nodes) 
            for index_connect_to=1:length(vector_possible_connect_to_nodes) 
               possible_connection=[vector_possible_connect_from_nodes(index_connect_from);vector_possible_connect_to_nodes(index_connect_to)];
               if sum((new_individual.connectiongenes(2,:)==possible_connection(1)).*(new_individual.connectiongenes(3,:)==possible_connection(2)))==0 % Check if proposed connection is not already contained in gene
                  new_connection_matrix=[new_connection_matrix,possible_connection];
               end
            end
         end
         % Shuffle possible new connections randomly
         [discard,shuffle]=sort(rand(1,size(new_connection_matrix,2)));
         new_connection_matrix=new_connection_matrix(:,shuffle);
         
         index_new_connection=0;
         flag_connection_ok=0;         
         % check if connection is o.k. (meaning either non-recurrent or recurrent and flag_recurrency_enabled set to 1) if not connection is found which is o.k.,no connection will be added to connection genes of new individual
         while (flag_connection_ok==0) & (index_new_connection<size(new_connection_matrix,2)) 
            index_new_connection=index_new_connection+1;
            new_connection=new_connection_matrix(:,index_new_connection);
             
            % test new connection if it is recurrent (i.e. at least one of the possibles path starting from connect_to node in the network leads back to the connect_from node 
            flag_recurrent=0;            
            if new_connection(1)==new_connection(2) %trivial recurrency
               flag_recurrent=1;
            end
            nodes_current_level=new_connection(2);
            depth=0;
            while flag_recurrent==0 & depth<size(new_individual.connectiongenes,2) & ~isempty(nodes_current_level)
               depth=depth+1;
               nodes_next_level=[];
               for index_check=1:size(nodes_current_level);                  
                  nodes_next_level=[nodes_next_level,new_individual.connectiongenes(3,find(new_individual.connectiongenes(2,:)==nodes_current_level(index_check)))];
               end
               if sum(nodes_next_level(:)==new_connection(1))>0
                  flag_recurrent=1;
               end
               nodes_current_level=nodes_next_level;
            end
            if flag_recurrent==0 
               flag_connection_ok=1;
            elseif flag_recurrency_enabled 
               flag_connection_ok=1;
            end            
         end
                 
         % Now we test if it is a true innovation (i.e. hasn't already happened in this generation) we can only do this if a valid new connection has been found
         if flag_connection_ok
            index_already_happened=find((innovation_record(5,:)==generation).*(innovation_record(2,:)==new_connection(1)).*(innovation_record(3,:)==new_connection(2))); %set flag signifying new innovation (connection not contained in innovation_record of this generation)  
            new_innovation=not(sum(index_already_happened)); 
            if new_innovation==1 % O.K. is new innovation
               new_connection=[max(innovation_record(1,:))+1;new_connection]; %Update the new connection with its innovation number
               % Update connection_genes
               new_individual.connectiongenes=[new_individual.connectiongenes,[new_connection;rand*2-1;1]];
               % Update innovation_record
               innovation_record=[innovation_record,[new_connection;0;generation]];               
            else % connection gene already exists in innovation_record of this generation
               % Update connection_genes
               new_individual.connectiongenes=[new_individual.connectiongenes,[innovation_record(1:3,index_already_happened);rand*2-1;1]];         
            end
         end         
      end
            
      % Add (Insert) Node Mutation
      new_innovation=0;
      if flag1==1
         max_old_innovation_number=max((innovation_record(5,:)<generation).*innovation_record(1,:)); %highest innovation number from last generation (to ensure that only connections from from last generation or older are chosen for add node mutation, otherwise a new connection added in the last mutation might instantly be disabled)
         vector_possible_connections=[new_individual.connectiongenes(2:3,find((new_individual.connectiongenes(5,:)==1) & (new_individual.connectiongenes(1,:)<=max_old_innovation_number)));find((new_individual.connectiongenes(5,:)==1) & (new_individual.connectiongenes(1,:)<=max_old_innovation_number))];  %compute vector of connections into which a new node could be inserted and their positions in the connection_gene matrix. This vector is composed of all nondisabled connections which stem at least from the last generation or older
         insert_node_connection=vector_possible_connections(:,round(rand*size(vector_possible_connections,2)+0.5));
         new_innovation=1; %set provisionally to 1, will be checked
         exist_innovation=find((innovation_record(5,:)==generation).*(innovation_record(4,:)>0).*(innovation_record(2,:)==insert_node_connection(1))); %Beginning of check innovation record to test for real innovation. exist_innovation contains vector of index of elements in innovation record which fulfil three things: current generation, add node mutation and same connect from as current innovation
         if sum(exist_innovation)>0 %if these are fulfilled, we have to test for connect_to node to see if innovation really is the same
            for index_check=1:length(exist_innovation)
               if innovation_record(3,exist_innovation(index_check)+1)==insert_node_connection(2)
                  new_innovation=0;                   
                  index_already_existent_this_generation=exist_innovation(index_check);
               end
            end
         end
         if new_innovation==1 %O.K. is true innovation for current generation
            % Update node_genes
            new_node_number=max(innovation_record(4,:))+1;
            new_individual.nodegenes=[new_individual.nodegenes,[new_node_number;3;0;0]];
            % Update connection_genes
            new_individual.connectiongenes(5,insert_node_connection(3))=0; %disable old connection gene
            new_connections=[[max(innovation_record(1,:))+1;insert_node_connection(1);new_node_number;1;1],[max(innovation_record(1,:))+2;new_node_number;insert_node_connection(2);new_individual.connectiongenes(4,insert_node_connection(3));1]];
            new_individual.connectiongenes=[new_individual.connectiongenes,new_connections]; %extend connection_genes by the two new connections
            % Update innovation_record
            innovation_record=[innovation_record,[new_connections(1:3,:);new_node_number,0;generation,generation]];
         else %no new innovation, has already happened at least once in this generation
            % Update node_genes
            node_number=innovation_record(4,index_already_existent_this_generation);
            new_individual.nodegenes=[new_individual.nodegenes,[node_number;3;0;0]];
            % Update connection_genes
            new_individual.connectiongenes(5,insert_node_connection(3))=0; %disable old connection gene
            new_connections=[innovation_record(1:3,index_already_existent_this_generation:index_already_existent_this_generation+1);1,new_individual.connectiongenes(4,insert_node_connection(3));1,1];        
            length_con_gen=size(new_individual.connectiongenes,2); %length of the connection genes of current new_individual
            if new_individual.connectiongenes(1,length_con_gen)>new_connections(1,2) % check if there was an add_connection_mutation to current new_individual which has a higher innovation number than current add_node_mutation
                new_individual.connectiongenes=[new_individual.connectiongenes(:,1:length_con_gen-1),new_connections,new_individual.connectiongenes(:,length_con_gen)];
            else 
               new_individual.connectiongenes=[new_individual.connectiongenes,new_connections];
            end
         end                 
      end
      
      %% Speciation 
      % Loop through comparison vector
      species_assigned=0;
      index_population_ref=0;
      while species_assigned==0 & index_population_ref<size(population_ref,2)
         %extract reference_individual from reference population
         index_population_ref=index_population_ref+1;
         reference_individual=population_ref(index_population_ref);
         %run through both connection genes, compute disjoint, excess, and average weight difference
         max_num_genes=max([size(new_individual.connectiongenes,2),size(reference_individual.connectiongenes,2)]);
         max_num_innovation=max([new_individual.connectiongenes(1,:),reference_individual.connectiongenes(1,:)]); 
         vector_innovation_new=[zeros(1,max(new_individual.connectiongenes(1,:))),ones(1,max_num_innovation-max(new_individual.connectiongenes(1,:)))];
         vector_innovation_new(new_individual.connectiongenes(1,:))=2;
         vector_weight_new=zeros(1,max_num_innovation);
         vector_weight_new(new_individual.connectiongenes(1,:))=new_individual.connectiongenes(4,:);
         vector_innovation_ref=[4*ones(1,max(reference_individual.connectiongenes(1,:))),8*ones(1,max_num_innovation-max(reference_individual.connectiongenes(1,:)))];
         vector_innovation_ref(reference_individual.connectiongenes(1,:))=16;
         vector_weight_ref=zeros(1,max_num_innovation);
         vector_weight_ref(reference_individual.connectiongenes(1,:))=reference_individual.connectiongenes(4,:);
         vector_lineup=vector_innovation_new+vector_innovation_ref;
         excess=sum(vector_lineup==10)+sum(vector_lineup==17);
         disjoint=sum(vector_lineup==6)+sum(vector_lineup==16);
         vector_matching=find(vector_lineup==18);
         average_weight_difference=sum(abs(vector_weight_new(vector_matching)-vector_weight_ref(vector_matching)))/length(vector_matching);
         max_num_genes=1;
         distance=speciation.c1*excess/max_num_genes+speciation.c2*disjoint/max_num_genes+speciation.c3*average_weight_difference;
         if distance<speciation.threshold
            % assign individual to same species as current reference individual
            new_individual.species=reference_individual.species;
            species_assigned=1; %set flag indicating new_individual has been assigned to species            
         end         
      end      
      % not compatible with any? well, then create new species
      if species_assigned==0
         new_species_ID=size(species_record,2)+1;
		   % assign individual to new species
         new_individual.species=new_species_ID;
         % update species_record
         species_record(new_species_ID).ID=new_species_ID;
         species_record(new_species_ID).number_individuals=1;
         species_record(new_species_ID).generation_record=[];
         % update population reference
         population_ref(size(population_ref,2)+1)=new_individual;
      end
      
      % add new_individual to new_population
      new_population(index_individual)=new_individual;
      
      %Increment species
      matrix_existing_and_propagating_species(3,index_species)=matrix_existing_and_propagating_species(3,index_species)+1;
   end
end

% final update of species_record (can only be done now since old population sizes were needed during reproduction cycle)
for index_species=1:size(species_record,2)
   species_record(index_species).number_individuals=sum([new_population(:).species]==index_species);
end
%assign updated species_record to output
updated_species_record=species_record;
%assign updated innovation_record to output
updated_innovation_record=innovation_record;