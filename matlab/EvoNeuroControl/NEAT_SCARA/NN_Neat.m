function [ output ] = NN_Neat( input,individual )
%PROC_NETWORK execute an input output operation on the network (individual)
%   Detailed explanation goes here


%threshold to judge if state of a node has changed significantly since last
%iteration
no_change_threshold=1e-5; 

%Problem dependant
input_nodes  = 2;
output_nodes = 3;

number_nodes       = size(individual.nodegenes,2);
number_connections = size(individual.connectiongenes,2);
  

      % set node input steps for first timestep
      individual.nodegenes(3,input_nodes+2:number_nodes) = 0; %set all node input states to zero
      individual.nodegenes(3,input_nodes+1) = 1; %bias node input state set to deg2rad(90)
      individual.nodegenes(3,1:input_nodes) = input; %node input states of the two input nodes are consecutively set to the XOR input pattern  
      
      %set node output states for first timestep (depending on input states)
      individual.nodegenes(4,1:input_nodes+1)=individual.nodegenes(3,1:input_nodes+1);
      n = individual.nodegenes(3,input_nodes+2:number_nodes);
      individual.nodegenes(4,input_nodes+2:number_nodes) =  tanh(n);     
      %1./(1+exp(-4.9 * n)); %old activation function
      
      no_change_count=0;     
      index_loop=0;
      
      while (no_change_count<number_nodes) && index_loop < number_connections
         index_loop        = index_loop+1;
         vector_node_state = individual.nodegenes(4,:);
         
         for index_connections = 1:number_connections             
            if individual.connectiongenes(5,index_connections)==1 %Check if Connection is enabled
                %read relevant contents of connection gene (ID of Node where connection starts, ID of Node where it ends, and connection weight)
                ID_connection_from_node = individual.connectiongenes(2,index_connections);
                ID_connection_to_node   = individual.connectiongenes(3,index_connections);
                connection_weight       = individual.connectiongenes(4,index_connections);

                %map node ID's (as extracted from single connection genes above) to index of corresponding node in node genes matrix
                index_connection_from_node = find((individual.nodegenes(1,:)==ID_connection_from_node));
                index_connection_to_node   = find((individual.nodegenes(1,:)==ID_connection_to_node));

                %take output state of connection_from node, multiply with
                %weight, add to input state of connection_to node
                individual.nodegenes(3,index_connection_to_node) = individual.nodegenes(3,index_connection_to_node) + individual.nodegenes(4,index_connection_from_node) * connection_weight; 
            end             
         end        
         
         %pass on node input states to outputs for next timestep
         n = individual.nodegenes(3,input_nodes+2:number_nodes);
         
         individual.nodegenes(4,input_nodes+2:number_nodes) = tanh(n);
         %old activation 1./(1+exp(-4.9*n));          
         
         %Re-initialize node input states for next timestep         
         %set all output and hidden node input states to zero
         individual.nodegenes(3,input_nodes+2:number_nodes)=0;
         
         %check for alle nodes where the node output state has changed by
         %less than no_change_threshold since last iteration through all the connection genes
         no_change_count = sum(abs(individual.nodegenes(4,:)-vector_node_state)<no_change_threshold); 
         
      end     
      
output = individual.nodegenes(4,input_nodes+2:input_nodes+2+output_nodes-1);
      
  