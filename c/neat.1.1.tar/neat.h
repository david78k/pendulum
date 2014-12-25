#ifndef NEAT_H
#define NEAT_H

#include <iostream>
#include <fstream>

class NEAT { 

 public:

  static const int num_trait_params=8;

  static double testvar;

  static double trait_param_mut_prob; /* Prob. of mutating a single trait param */
  static double trait_mutation_power; /* Power of mutation on a signle trait param */
  static double linktrait_mut_sig;  /* Amount that mutation_num changes for a trait
				change inside a link*/
  static double nodetrait_mut_sig; /* Amount a mutation_num changes on a link
			       connecting a node that changed its trait */
  static double weight_mut_power;  /* The power of a linkweight mutation */
  static double recur_prob;        /* OBSOLETE */
  
  /* These 3 global coefficients are used to determine the formula for
     computating the compatibility between 2 genomes.  The formula is:
     disjoint_coeff*pdg+excess_coeff*peg+mutdiff_coeff*mdmg.
     See the compatibility method in the Genome class for more info
     They can be thought of as the importance of disjoint Genes,
     excess Genes, and parametric difference between Genes of the
     same function, respectively. */
  static double disjoint_coeff;
  static double excess_coeff;
  static double mutdiff_coeff;
  
  /* This global tells compatibility threshold under which 
     two Genomes are considered the same species */
  static double compat_threshold;
  
  /* Globals involved in the epoch cycle - mating, reproduction, etc.. */
  static double age_significance;          /* How much does age matter? */
  static double survival_thresh;           /* Percent of ave fitness for survival */
  static double mutate_only_prob;          /* Prob. of a non-mating reproduction */
  static double mutate_random_trait_prob;
  static double mutate_link_trait_prob;
  static double mutate_node_trait_prob;
  static double mutate_link_weights_prob;
  static double mutate_toggle_enable_prob;
  static double mutate_gene_reenable_prob;
  static double mutate_add_node_prob;
  static double mutate_add_link_prob;
  static double interspecies_mate_rate;    /* Prob. of a mate being outside species */
  static double mate_multipoint_prob;     
  static double mate_multipoint_avg_prob;
  static double mate_singlepoint_prob;
  static double mate_only_prob;            /* Prob. of mating without mutation */
  static double recur_only_prob;  /* Probability of forcing selection of ONLY
			      links that are naturally recurrent */
  static int pop_size;  /* Size of population */
  static int dropoff_age;  /* Age where Species starts to be penalized */
  static int newlink_tries;  /* Number of tries mutate_add_link will attempt to
			 find an open link */
  static int print_every; /* Tells to print population to file every n generations */
  
  static int babies_stolen; /* The number of babies to siphen off to the champions */

  static int num_runs; /* The number of runs to average over in an experiment */
  
  //Construct NEAT object off of a NE param (.ne) file
  static bool load_neat_params(char *filename) {
    ifstream paramFile(filename,ios::in);
    char curword[30];
    
    //**********LOAD IN PARAMETERS***************//
    cout<<"NEAT READING IN "<<filename<<endl;
    paramFile>>curword;
    paramFile>>trait_param_mut_prob;
    paramFile>>curword;
    paramFile>>trait_mutation_power;
    paramFile>>curword;
    paramFile>>linktrait_mut_sig;
    paramFile>>curword;
    paramFile>>nodetrait_mut_sig;
    paramFile>>curword;
    paramFile>>weight_mut_power;
    paramFile>>curword;
    paramFile>>recur_prob;
    paramFile>>curword;
    paramFile>>disjoint_coeff;
    paramFile>>curword;
    paramFile>>excess_coeff;
    paramFile>>curword;
    paramFile>>mutdiff_coeff;
    paramFile>>curword;
    paramFile>>compat_threshold;
    paramFile>>curword;
    paramFile>>age_significance;
    paramFile>>curword;
    paramFile>>survival_thresh;
    paramFile>>curword;
    paramFile>>mutate_only_prob;
    paramFile>>curword;
    paramFile>>mutate_random_trait_prob;
    paramFile>>curword;
    paramFile>>mutate_link_trait_prob;
    paramFile>>curword;
    paramFile>>mutate_node_trait_prob;
    paramFile>>curword;
    paramFile>>mutate_link_weights_prob;
    paramFile>>curword;
    paramFile>>mutate_toggle_enable_prob;
    paramFile>>curword;
    paramFile>>mutate_gene_reenable_prob;
    paramFile>>curword;
    paramFile>>mutate_add_node_prob;
    paramFile>>curword;
    paramFile>>mutate_add_link_prob;
    paramFile>>curword;
    paramFile>>interspecies_mate_rate;
    paramFile>>curword;
    paramFile>>mate_multipoint_prob;
    paramFile>>curword;
    paramFile>>mate_multipoint_avg_prob;
    paramFile>>curword;
    paramFile>>mate_singlepoint_prob;
    paramFile>>curword;
    paramFile>>mate_only_prob;
    paramFile>>curword;
    paramFile>>recur_only_prob;  
    paramFile>>curword;
    paramFile>>pop_size;
    paramFile>>curword;
    paramFile>>dropoff_age;
    paramFile>>curword;
    paramFile>>newlink_tries;
    paramFile>>curword;
    paramFile>>print_every;
    paramFile>>curword;
    paramFile>>babies_stolen;
    paramFile>>curword;
    paramFile>>num_runs;
    paramFile.close();
    
    cout<<"trait_param_mut_prob="<<trait_param_mut_prob<<endl;
    cout<<"trait_mutation_power="<<trait_mutation_power<<endl;
    cout<<"linktrait_mut_sig="<<linktrait_mut_sig<<endl;
    cout<<"nodetrait_mut_sig="<<nodetrait_mut_sig<<endl;
    cout<<"weight_mut_power="<<weight_mut_power<<endl;
    cout<<"recur_prob="<<recur_prob<<endl;
    cout<<"disjoint_coeff="<<disjoint_coeff<<endl;
    cout<<"excess_coeff="<<excess_coeff<<endl;
    cout<<"mutdiff_coeff="<<mutdiff_coeff<<endl;
    cout<<"compat_threshold="<<compat_threshold<<endl;
    cout<<"age_significance="<<age_significance<<endl;
    cout<<"survival_thresh="<<survival_thresh<<endl;
    cout<<"mutate_only_prob="<<mutate_only_prob<<endl;
    cout<<"mutate_random_trait_prob="<<mutate_random_trait_prob<<endl;
    cout<<"mutate_link_trait_prob="<<mutate_link_trait_prob<<endl;
    cout<<"mutate_node_trait_prob="<<mutate_node_trait_prob<<endl;
    cout<<"mutate_link_weights_prob="<<mutate_link_weights_prob<<endl;
    cout<<"mutate_toggle_enable_prob="<<mutate_toggle_enable_prob<<endl;
    cout<<"mutate_gene_reenable_prob="<<mutate_gene_reenable_prob<<endl;
    cout<<"mutate_add_node_prob="<<mutate_add_node_prob<<endl;
    cout<<"mutate_add_link_prob="<<mutate_add_link_prob<<endl;
    cout<<"interspecies_mate_rate="<<interspecies_mate_rate<<endl;
    cout<<"mate_multipoint_prob="<<mate_multipoint_prob<<endl;
    cout<<"mate_multipoint_avg_prob="<<mate_multipoint_avg_prob<<endl;
    cout<<"mate_singlepoint_prob="<<mate_singlepoint_prob<<endl;
    cout<<"mate_only_prob="<<mate_only_prob<<endl;
    cout<<"recur_only_prob="<<recur_only_prob<<endl;
    cout<<"pop_size="<<pop_size<<endl;
    cout<<"dropoff_age="<<dropoff_age<<endl;
    cout<<"newlink_tries="<<newlink_tries<<endl;
    cout<<"print_every="<<print_every<<endl;
    cout<<"babies_stolen="<<babies_stolen<<endl;
    cout<<"num_runs="<<num_runs<<endl;

    return true;

  }

  NEAT() {};

  NEAT(char *filename) {
    load_neat_params(filename);
  }

}; //end class NEAT

#endif




