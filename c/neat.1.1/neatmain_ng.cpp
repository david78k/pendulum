//#include <stdlib.h>
//#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <strstream>
#include <list>
#include <vector>
#include <algorithm>
#include <math.h>
//#include <iostream.h>
//#include "networks.h"
#include "neat.h"
#include "genetics.h"
#include "experiments.h"
#include "neatmain.h"
 
//Network pointer display
//ostream& operator<<(ostream &os,Network *thenet) {
//  thenet->show_activation();
//}

double NEAT::trait_param_mut_prob=0; /* Prob. of mutating a single trait param */
double NEAT::trait_mutation_power=0; /* Power of mutation on a signle trait param */
double NEAT::linktrait_mut_sig=0;  /* Amount that mutation_num changes for a trait
			      change inside a link*/
double NEAT::nodetrait_mut_sig=0; /* Amount a mutation_num changes on a link
			     connecting a node that changed its trait */
double NEAT::weight_mut_power=0;  /* The power of a linkweight mutation */
double NEAT::recur_prob=0;        /* Prob. that a link mutation which doesn't
			     have to be recurrent will be made recurrent */

/* These 3 global coefficients are used to determine the formula for
   computating the compatibility between 2 genomes.  The formula is:
    disjoint_coeff*pdg+excess_coeff*peg+mutdiff_coeff*mdmg.
   See the compatibility method in the Genome class for more info
   They can be thought of as the importance of disjoint Genes,
   excess Genes, and parametric difference between Genes of the
   same function, respectively. */
double NEAT::disjoint_coeff=0;
double NEAT::excess_coeff=0;
double NEAT::mutdiff_coeff=0;

/* This global tells compatibility threshold under which 
   two Genomes are considered the same species */
double NEAT::compat_threshold=0;

/* Globals involved in the epoch cycle - mating, reproduction, etc.. */
double NEAT::age_significance=0;          /* How much does age matter? */
double NEAT::survival_thresh=0;           /* Percent of ave fitness for survival */
double NEAT::mutate_only_prob=0;          /* Prob. of a non-mating reproduction */
double NEAT::mutate_random_trait_prob=0;
double NEAT::mutate_link_trait_prob=0;
double NEAT::mutate_node_trait_prob=0;
double NEAT::mutate_link_weights_prob=0;
double NEAT::mutate_toggle_enable_prob=0;
double NEAT::mutate_gene_reenable_prob=0;
double NEAT::mutate_add_node_prob=0;
double NEAT::mutate_add_link_prob=0;
double NEAT::interspecies_mate_rate=0;    /* Prob. of a mate being outside species */
double NEAT::mate_multipoint_prob=0;     
double NEAT::mate_multipoint_avg_prob=0;
double NEAT::mate_singlepoint_prob=0;
double NEAT::mate_only_prob=0;            /* Prob. of mating without mutation */
double NEAT::recur_only_prob=0;  /* Probability of forcing selection of ONLY
			    links that are naturally recurrent */
int NEAT::pop_size=0;  /* Size of population */
int NEAT::dropoff_age=0;  /* Age where Species starts to be penalized */
int NEAT::newlink_tries=0;  /* Number of tries mutate_add_link will attempt to
		       find an open link */
int NEAT::print_every=0; /* Tells to print population to file every n generations */

int NEAT::babies_stolen=0; /* The number of babies to siphen off to the champions */

int NEAT::num_runs=1; /* The number of runs to average over in an experiment */

double testdoubval() {
  return *testdoub;
}

double testdoubset(double val) {
  *testdoub=val;
}

int main(int argc, char *argv[]) {

  ostream_iterator<NNode*> output(cout," ");
  list<NNode*> nodelist;

  Population *p;

  double val=1.0;

  /* GTKMM */
  myapp=new Gtk::Main(argc, argv);

  testdoub=&val;

  //***********RANDOM SETUP***************//
  /* Seed the random-number generator with current time so that
      the numbers will be different every time we run.    */
  srand( (unsigned)time( NULL ) );

  //Load in the params
  NEAT::load_neat_params(argv[1]);

  //Here is an example of how to run an experiment directly from main
  //and then visualize the speciation that took place
  p=xor_test(100);  //100 generation XOR experiment
  cout<<"Visualizing 1 to "<<p->final_gen<<endl;
  p->visualize(500,700,1,p->final_gen);
 
  return(0);
 
}














