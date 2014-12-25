%%%%%%%%%%%%%%%%%%%%%%% generate minimal initial population 

%% Neuro_Evolution_of_Augmenting_Topologies - NEAT 
%% developed by Kenneth Stanley (kstanley@cs.utexas.edu) & Risto Miikkulainen (risto@cs.utexas.edu)
%% Coding by Christian Mayr (matlab_neat@web.de)

function [population,innovation_record]=initial_population(number_individuals,number_input_nodes,number_output_nodes,vector_connected_input_nodes);

% nodegenes is array 4rows * (number_input_nodes+number_output_nodes+hidden-nodes(not existent in initial population) +1 (bias-node))columns
% nodegenes contains consecutive node ID's (upper row), node type (lower row) 1=input 2=output 3=hidden 4=bias, node input state, and node output state (used for evaluation, all input states zero initially, except bias node, which is always 1)
% connectiongenes is array 5 rows * number_connections columns
% from top to bottom, those five rows contain: innovation number, connection from, connection to, weight, enable bit 
% the rest of the elements in the structure for an individual should be self-explanatory

% innovation_record tracks innovations in a 5rows by (number of innovations) columns matrix, contains innovation number, connect_from_node as well as connect_to_node for this innovation)
% the new node (if it is a new node mutation, then this node will appear in the 4th row when it is first connected. There will always be two innovations with one node mutation, since there is a connection to and from the new node. 
% In the initial population, this will be abbreviated to the Node with the highest number appearing in the last column of the record, since only this is needed as starting point for the rest of the algorithm), 
% and 5th row is generation this innovation occured (generation is assumed to be zero for the innovations in the initial population)
   
%compute number and matrix of initial connections (all connections between output nodes and the nodes listed in vector_connected_input_nodes)
number_connections=(length(vector_connected_input_nodes)+1)*number_output_nodes;
vector_connection_from=rep([vector_connected_input_nodes,number_input_nodes+1],[1 number_output_nodes]);
vector_connection_to=[];

for index_output_node=(number_input_nodes+2):(number_input_nodes+1+number_output_nodes)
   vector_connection_to=[vector_connection_to,index_output_node*ones(1,length(vector_connected_input_nodes)+1)];
end
connection_matrix=[vector_connection_from;
                   vector_connection_to];

for index_individual=1:number_individuals;
   population(index_individual).nodegenes=[1:(number_input_nodes+1+number_output_nodes);
                                           ones(1,number_input_nodes),4,2*ones(1,number_output_nodes);
                                           zeros(1,number_input_nodes),1,zeros(1,number_output_nodes);
                                           zeros(1,number_input_nodes+1+number_output_nodes)];
   population(index_individual).connectiongenes=[1:number_connections;
                                                 connection_matrix;
                                                 rand(1,number_connections)*2-1;
                                                 ones(1,number_connections)]; %all weights uniformly distributed in [-1 +1], all connections enabled
   population(index_individual).fitness=0;
   population(index_individual).species=0;
end
innovation_record=[population(index_individual).connectiongenes(1:3,:);zeros(size(population(index_individual).connectiongenes(1:2,:)))];
innovation_record(4,size(innovation_record,2))=max(population(1).nodegenes(1,:)); %highest node ID for initial population