%%%%%%%%%%%%%%%% XOR experiment file  (contains experiment, receives genom, decodes it, evaluates it and returns raw fitnesses) (function)

%% Neuro_Evolution_of_Augmenting_Topologies - NEAT 
%% developed by Kenneth Stanley (kstanley@cs.utexas.edu) & Risto Miikkulainen (risto@cs.utexas.edu)
%% Coding by Christian Mayr (matlab_neat@web.de)

function population_plus_fitnesses=xor_experiment_vec(population);
population_plus_fitnesses=population;
no_change_threshold=1e-3; %threshold to judge if state of a node has changed significantly since last iteration
number_individuals=size(population,2);

input_pattern=[0 0;
               0 1; 
               1 0;
               1 1];
output_pattern=[0;
                1;
                1;
                0];
             
for index_individual=1:number_individuals   
   number_nodes=size(population(index_individual).nodegenes,2);
   number_connections=size(population(index_individual).connectiongenes,2);
   individual_fitness=0;
   output=[];
   for index_pattern=1:4
      % the following code assumes node 1 and 2 inputs, node 3 bias, node 4 output, rest arbitrary (if existent, will be hidden nodes)
      %set node output states for first timestep (depending on input states)
      population(index_individual).nodegenes(4,3)=1; %bias node output state set to 1
      population(index_individual).nodegenes(4,1:2)=input_pattern(index_pattern,:); %node output states of the two input nodes are consecutively set to the XOR input pattern  
      population(index_individual).nodegenes(4,4:number_nodes)=0.5;
      no_change_count=0;     
      index_loop=0;
      while (no_change_count<number_nodes) & index_loop<3*number_connections
         index_loop=index_loop+1;
         vector_node_state=population(index_individual).nodegenes(4,:);
         %%
         % begin Vectorisation of index_connection_loop
         % first remodel connection gene from node ID's to node index in node gene
         max_node_ID=max(population(index_individual).nodegenes(1,:));
         max_node_num=size(population(index_individual).nodegenes,2);
         vector_remodel=zeros(1,max_node_ID);
         vector_remodel(population(index_individual).nodegenes(1,:))=[1:max_node_num];
         vector_nodes_from=vector_remodel(population(index_individual).connectiongenes(2,:));
         vector_nodes_to=vector_remodel(population(index_individual).connectiongenes(3,:));
         
         matrix_compute=zeros(max_node_num,number_connections);
         % actual vectorized computation
         matrix_compute(([1:number_connections]-1)*max_node_num+vector_nodes_to(1,:))=population(index_individual).nodegenes(4,vector_nodes_from(:)).*population(index_individual).connectiongenes(4,:).*population(index_individual).connectiongenes(5,:);
         
         population(index_individual).nodegenes(3,:)=ones(1,number_connections)*matrix_compute';
         
         % end Vectorisation of index_connection_loop
         %%
         %pass on node input states to outputs for next timestep 
         population(index_individual).nodegenes(4,4:number_nodes)=1./(1+exp(-4.9*population(index_individual).nodegenes(3,4:number_nodes)));          
         no_change_count=sum(abs(population(index_individual).nodegenes(4,:)-vector_node_state)<no_change_threshold); %check for alle nodes where the node output state has changed by less than no_change_threshold since last iteration through all the connection genes
      end      
      if index_loop>=2.7*number_connections
         index_individual
         population(index_individual).connectiongenes
      end
      output=[output;population(index_individual).nodegenes(4,4)];
      individual_fitness=individual_fitness+abs(output_pattern(index_pattern,1)-population(index_individual).nodegenes(4,4)); 
   end
   population_plus_fitnesses(index_individual).fitness=(4-individual_fitness)^2; %Fitness function as defined by Kenneth Stanley    
   if sum(abs(round(output)-output_pattern))==0      
      population_plus_fitnesses(index_individual).fitness=16;
      output
   end
end
