%module glist
%{
#include <list>
//#include "networks.h"
#include "genetics.h"
#include "visual.h"
#include "experiments.h"
#include "neatmain.h"
%}

//Interface functions
extern double testdoubval();

extern double testdoubset(double val);

extern NNode *new_NNode3arg(int ntype,int nodeid, int placement);

 
 /* ---------                                                     --------- */
 /* ----------------------------------------------------------------------- */

 /* ----------------------------------------------------------------------- */
 /* A NETWORK is a LIST of input NODEs and a LIST of output NODEs           
    The point of the network is to define a single entity which can evolve
    or learn on its own, even though it may be part of a larger framework */
 class Network {

  public:

   //Genome *genotype;  //Allows Network to be matched with its Genome

   char *name; //Every Network or subNetwork can have a name

   int net_id; //Allow for a network id

   ~Network();

   void flush();  //Puts the network back into an inactive state
   //This preserves recurrent inputs 
   bool activate(); //Activates the net such that all outputs are active

   void show_activation(); //Prints the values of its outputs
   void add_input(NNode*);  //Add a new input node
   void add_output(NNode*);  //Add a new output node
   void load_sensors(double*); //Takes an array of sensor values and loads it into SENSOR inputs ONLY
   void give_name(char*); //Name the network

   int nodecount(); //Counts the number of nodes in the net if not yet counted

   int linkcount(); //Counts the number of links in the net if not yet counted

   /* Guile input loading helpers */
   int input_start();	
   int load_in(double d);

   /* A Network Graphing Routine using GTK+ and GDK */
   /* Width and height give the size of the drawing window */
   void graph(int width,int height);

  };

const int NEWNODE=0;
const int NEWLINK=1;

/* ----------------------------------------------------------------------- */  
/* A Genome is the primary source of genotype information used to create   */
/* a phenotype.  It contains 3 major constituents:                         */
/*   1) A Vector of Traits                                                 */
/*   2) A List of NNodes pointing to a Trait from (1)                      */
/*   3) A List of Genes with Links that point to Traits from (1)           */
/* (1) Reserved parameter space for future use
/* (2) NNode specifications                                                */
/* (3) Is the primary source of innovation in the evolutionary Genome.     */
/*     Each Gene in (3) has a marker telling when it arose historically.   */
/*     Thus, these Genes can be used to speciate the population, and the   */
/*     list of Genes provide an evolutionary history of innovation and     */
class Genome {         
 public:             

  int genome_id;

  Network *phenotype;  //Allows Genome to be matched with its Network

  //Special constructor which spawns off an input file
  //This constructor assumes that some routine has already read in GENOMESTART
  Genome(int id,ifstream &iFile);

  //Destructor kills off all lists (including the trait vector)
  ~Genome();

  Network *genesis(int);  //Generate a network phenotype from this Genome with specified id

  void print_to_file(ofstream &outFile); //Dump this genome to specified file

  Genome *duplicate(int new_id);  /* Duplicate this Genome to create a new one
				     with the specified id */
  
  /* ******* MUTATORS ******* */

  void mutate_random_trait(); /* Perturb params in one trait */

  void mutate_link_trait(int times);  /* Change random link's trait. 
					 Repeat times times */

  void mutate_node_trait(int times); /* Change random node's trait times 
					times */

  void mutate_link_weights(double power,double rate, int mut_type);  /* Add Gaussian noise to 
					      linkweights either GAUSSIAN
					      or COLDGAUSSIAN (from zero) */

  void mutate_toggle_enable(int times); /* toggle genes on or off */

  void mutate_gene_reenable();  /* Find first disabled gene and enable it */

  /* These last kinds of mutations return false if they fail
     They can fail under certain conditions,  being unable
     to find a suitable place to make the mutation.
     Generally, if they fail, they can be called again if desired. */

  int mutate_add_node(list<Innovation*> &innovs,int &curnode_id,double &curinnov); /* Mutate genome by adding a node respresentation */

  /* Mutate the genome by adding a new link between 2 random NNodes */
  int mutate_add_link(list<Innovation*> &innovs,double &curinnov,int tries); 

  /* ****** MATING METHODS ***** */

  /* This method mates this Genome with another Genome g.  
     For every point in each Genome, where each Genome shares
     the innovation number, the Gene is chosen randomly from 
     either parent.  If one parent has an innovation absent in 
     the other, the baby will inherit the innovation */
  Genome *mate_multipoint(Genome *g,int genomeid,double fitness1,double fitness2);

  /* This method mates like multipoint but instead of selecting one
     or the other when the innovation numbers match, it averages their
     weights */
  Genome *mate_multipoint_avg(Genome *g,int genomeid,double fitness1,double fitness2);

  /* This method is similar to a standard single point CROSSOVER
     operator.  Traits are averaged as in the previous 2 mating
     methods.  A point is chosen in the smaller Genome for crossing
     with the bigger one.  */
  Genome *mate_singlepoint(Genome *g,int genomeid);


  /* ******** COMPATIBILITY CHECKING METHODS * ********/
  
  /* This function gives a measure of compatibility between
     two Genomes by computing a linear combination of 3
     characterizing variables of their compatibilty.
     The 3 variables represent PERCENT DISJOINT GENES, 
     PERCENT EXCESS GENES, MUTATIONAL DIFFERENCE WITHIN
     MATCHING GENES.  So the formula for compatibility 
     is:  disjoint_coeff*pdg+excess_coeff*peg+mutdiff_coeff*mdmg.
     The 3 coefficients are global system parameters */
  double compatibility(Genome *g);

};


extern Genome *new_Genome_fromfile(char *filename, int id);

//Display a graph of a Genome's resultant Network directly from a file
Genome *display_Genome(char *filename, int width, int height);

//Load in a genome without knowing its id
extern Genome *new_Genome_load(char *filename);

extern int print_Genome_tofile(Genome *g,char *filename);

extern int print_Genome(Genome *g);

/* Population loading and manipulation methods */

extern Population *new_Pop_fromfile(char *filename);

extern Population *new_Pop_fromGenomefile(char *filename);

class Population {
	public:
	 bool epoch(int generation);

	//Visualize the Population's Speciation using a graphic depiction
	//width and height denote the size of the window
	 void visualize(int width,int height,int start_gen,int stop_gen);		
}

extern int xor_epoch(Population *pop,int generation,char *filename,int &winnernum, int &winnergenes,int &winnernodes);

extern list<Innovation*> *new_innov_list();

//Main functions
extern Population *xor_test(int gens);
extern bool xor_evaluate(Organism *org);

extern Population *pole1_test(int gens);
extern bool pole1_evaluate(Organism *org);

Population *pole2_test(int gens,int velocity);
bool pole2_evaluate(Organism *org,bool velocity,CartPole *thecart);
















