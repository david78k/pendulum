%%%%%%%%%%%%%%%% ArmExperiment  (contains experiment, receives genom, decodes it, evaluates it and returns raw fitnesses) (function)

%% Neuro_Evolution_of_Augmenting_Topologies - NEAT 
%% developed by Kenneth Stanley (kstanley@cs.utexas.edu) & Risto Miikkulainen (risto@cs.utexas.edu)
%% Coding by Christian Mayr (matlab_neat@web.de)

function [population_plus_fitnesses mseArray] = Experiment(population)
population_plus_fitnesses = population;
number_individuals        = size(population,2);
mseArray=[];
for index_individual = 1:number_individuals 
   [f mse] =  Procces_Individual(population(index_individual));
   population_plus_fitnesses(index_individual).fitness = f;
   mseArray(index_individual)     = mse;
   %display(['Indivividual: ',int2str(index_individual),' Fitness: ',num2str(f),' mse: ',num2str(mse)])
end
