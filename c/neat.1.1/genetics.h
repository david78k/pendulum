#ifndef GENETICS_H
#define GENETICS_H

#include <assert.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <list>
#include <vector>
#include <algorithm>

#include "networks.h"
#include "visual.h"
#include "neat.h"

/* These are NNode types */
typedef int nodetype;
extern const int NEURON;
extern const int SENSOR;

typedef int nodeplace;
extern const int HIDDEN;
extern const int INPUT;
extern const int OUTPUT;

/* Mutators are variables that specify a kind of mutation */
typedef int mutator;
extern const mutator GAUSSIAN;  //This adds Gaussian noise to the weights
extern const mutator COLDGAUSSIAN;  //This sets weights to numbers chosen from a Gaussian distribution

/* Bools */
typedef int boolean;
//extern const int TRUE;
//extern const int FALSE;

/* Activation Types */
typedef int functype;
extern const int SIGMOID;

class Innovation;

class Trait;
class Link;
class NNode;
class Network;

//Typedef used in the Innovation class
typedef int innovtype;
extern const int NEWNODE;
extern const int NEWLINK;


/* ----------------------------------------------------------------------- */
/* The Gene class in this system specifies a "Connection Gene." 
   Nodes are represented using the Node class, which serves as both
   a genotypic and phenotypic representation of nodes.
   Genetic Representation of connections uses this special class because
   it calls for special operations better served by a specific genetic 
   representation  */
/* A Gene object in this system specifies a link      */
/* between two nodes along with an "innovation number" which tells when    */
/* in the history of a population the gene first arose.   This allows      */
/* the system to track innovations and use those to determine which        */
/* organisms are compatible.  (i.e. in the same species)                   */
/* A mutation_num gives a rough sense of how much mutation the gene has    */
/* experienced since it originally appeared.  (Since it was first          */
/* innovated.) In the current implenetation the mutation number is the
   same as the weight */

class Gene : public NEAT {
 public:

  Link *lnk;
  double innovation_num;
  double mutation_num;  //Used to see how much mutation has changed the link

  bool enable;  //When this is off the Gene is disabled

  //Construct a gene with no trait
  Gene(double w,NNode *inode,NNode *onode,bool recur,double innov,double mnum) {
    lnk=new Link(w,inode,onode,recur);
    innovation_num=innov;
    mutation_num=mnum;

    enable=true;
  }

  //Construct a gene with a trait
  Gene(Trait *tp,double w,NNode *inode,NNode *onode,bool recur,double innov,double mnum) {
    lnk=new Link(tp,w,inode,onode,recur);
    innovation_num=innov;
    mutation_num=mnum;

    enable=true;
  }

  //Construct a gene off of another gene as a duplicate
  Gene(Gene *g,Trait *tp,NNode *inode,NNode *onode) {
    //cout<<"Trying to attach nodes: "<<inode<<" "<<onode<<endl;
    lnk=new Link(tp,(g->lnk)->weight,inode,onode,(g->lnk)->is_recurrent);
    innovation_num=g->innovation_num;
    mutation_num=g->mutation_num;
    enable=g->enable;
  }

  //Construct a gene from a file spec given traits and nodes
  Gene(ifstream &iFile,vector<Trait*> &traits,list<NNode*> &nodes) {
    //Gene parameter holders
    int traitnum;
    int inodenum;
    int onodenum;
    NNode *inode;
    NNode *onode;
    double weight;
    int recur;
    Trait *traitptr;

    vector<Trait*>::iterator curtrait;
    list<NNode*>::iterator curnode;

    //Get the gene parameters
    iFile>>traitnum;
    iFile>>inodenum;
    iFile>>onodenum;
    iFile>>weight;
    iFile>>recur;
    iFile>>innovation_num;
    iFile>>mutation_num;
    iFile>>enable;
    
    //Get a pointer to the linktrait
    if (traitnum==0) traitptr=0;
    else {
      curtrait=traits.begin();
      while(((*curtrait)->trait_id)!=traitnum)
	++curtrait;
      traitptr=(*curtrait);
    }
    
    //Get a pointer to the input node
    curnode=nodes.begin();
    while(((*curnode)->node_id)!=inodenum)
      ++curnode;
    inode=(*curnode);
    
    //Get a pointer to the output node
    curnode=nodes.begin();
    while(((*curnode)->node_id)!=onodenum)
      ++curnode;
    onode=(*curnode);

    lnk=new Link(traitptr,weight,inode,onode,recur);
      
    
  }

  
  ~Gene() {
    delete lnk;
  }

  void print_to_file(ofstream &outFile); //Print gene to a file- called from Genome

  friend ostream& operator<< (ostream& os, const Gene *thegene);


};



/* ----------------------------------------------------------------------- */  
/* A Genome is the primary source of genotype information used to create   */
/* a phenotype.  It contains 3 major constituents:                         */
/*   1) A Vector of Traits                                                 */
/*   2) A List of NNodes pointing to a Trait from (1)                      */
/*   3) A List of Genes with Links that point to Traits from (1)           */
/* (1) Reserved parameter space for future use                             */
/* (2) NNode specifications                                                */
/* (3) Is the primary source of innovation in the evolutionary Genome.     */
/*     Each Gene in (3) has a marker telling when it arose historically.   */
/*     Thus, these Genes can be used to speciate the population, and the   */
/*     list of Genes provide an evolutionary history of innovation and     */
/*     link-building. */
class Genome : public NEAT {         

 protected:
  //Inserts a NNode into a given ordered list of NNodes in order
  void node_insert(list<NNode*> &nlist,NNode *n);

 public:             

  friend ostream& operator<< (ostream& os, const Genome *thegenome);

  int genome_id;

  vector<Trait*> traits; //parameter conglomerations
  list<NNode*> nodes;     //List of NNodes for the Network
  list<Gene*> genes;     //List of innovation-tracking genes

  Network *phenotype;  //Allows Genome to be matched with its Network

  int get_last_node_id();  //Return id of final NNode in Genome
  double get_last_gene_innovnum();  //Return last innovation number in Genome

  void print_genome();  //Displays Genome on screen

  //Constructor which takes full genome specs and puts them into the new one
  Genome(int id,vector<Trait*> t, list<NNode*> n, list<Gene*> g) {
    genome_id=id;
    traits=t;
    nodes=n; 
    genes=g;
  }

  //Constructor which takes in links (not genes) and creates a Genome
  Genome(int id,vector<Trait*> t, list<NNode*> n, list<Link*> links) {
    list<Link*>::iterator curlink;
    Gene *tempgene;
    traits=t;
    nodes=n;

    genome_id=id;
    
    //We go through the links and turn them into original genes
    for(curlink=links.begin();curlink!=links.end();++curlink) {
      //Create genes one at a time
      tempgene=new Gene((*curlink)->linktrait, (*curlink)->weight,(*curlink)->in_node,(*curlink)->out_node,(*curlink)->is_recurrent,1.0,0.0);
      genes.push_back(tempgene);
    }

  }

  //Special constructor which spawns off an input file
  //This constructor assumes that some routine has already read in GENOMESTART
  Genome(int id,ifstream &iFile) {
    char curword[20];  //max word size of 20 characters

    int done=0;

    genome_id=id;

    //Loop until file is finished, parsing each line
    while (!done) {
      
      iFile>>curword;

      //Check for end of Genome
      if (strcmp(curword,"genomeend")==0) {
	int idcheck;
	iFile>>idcheck;
	if (idcheck!=genome_id) cout<<"ERROR: id mismatch in genome"<<endl;
	done=1;
      }

      //Ignore comments surrounded by /* */ - they get printed to screen
      else if (strcmp(curword,"/*")==0) {
	iFile>>curword;
	while (strcmp(curword,"*/")!=0) {
	  cout<<curword<<" ";
	  iFile>>curword;
	}
	cout<<endl;
      }

      //Read in a trait
      else if (strcmp(curword,"trait")==0) {
	Trait *newtrait;

	//Allocate the new trait
	newtrait=new Trait(iFile);

	//Add trait to vector of traits
	traits.push_back(newtrait);
      }

      //Read in a node
      else if (strcmp(curword,"node")==0) {
	NNode *newnode;

	//Allocate the new node
	newnode=new NNode(iFile,traits);
	
	//Add the node to the list of nodes
	nodes.push_back(newnode);
      }

      //Read in a Gene
      else if (strcmp(curword,"gene")==0) {
	Gene *newgene;

	//Allocate the new Gene
	newgene=new Gene(iFile,traits,nodes);

	//Add the gene to the genome
	genes.push_back(newgene);
	
	//cout<<"Added gene "<<newgene<<endl;
      }
      
    }
    
  }

    /* This special constructor creates a Genome
     with i inputs, o outputs, n out of nmax hidden units, and random
     connectivity.  If r is true then recurrent connections will
     be included. 
     The last input is a bias
     Linkprob is the probability of a link  */
  Genome(int new_id,int i, int o, int n,int nmax, bool r, double linkprob) {
    int totalnodes;
    bool *cm; //The connection matrix which will be randomized
    bool *cmp; //Connection matrix pointer
    int matrixdim;
    int count;
    
    int ncount; //Node and connection counters
    int ccount;

    int row;  //For navigating the matrix
    int col;

    double new_weight;

    int maxnode; //No nodes above this number for this genome

    int first_output; //Number of first output node

    totalnodes=i+o+nmax;
    matrixdim=totalnodes*totalnodes;
    cm=new bool[matrixdim];  //Dimension the connection matrix
    maxnode=i+n;

    first_output=totalnodes-o+1;

    //For creating the new genes
    NNode *newnode;
    Gene *newgene;
    Trait *newtrait;
    NNode *in_node;
    NNode *out_node;

    //Retrieves the nodes pointed to by connection genes
    list<NNode*>::iterator node_iter;

    //Assign the id
    genome_id=new_id;

    cout<<"Assigned id "<<genome_id<<endl;

    //Step through the connection matrix, randomly assigning bits
    cmp=cm;
    for(count=0;count<matrixdim;count++) {
      if (randfloat()<linkprob)
	*cmp=true;
      else *cmp=false;
      cmp++;
    }

    //Create a dummy trait (this is for future expansion of the system)
    newtrait=new Trait(1,0,0,0,0,0,0,0,0,0);
    traits.push_back(newtrait);

    //Build the input nodes
    for(ncount=1;ncount<=i;ncount++) {
      if (ncount<i)
	newnode=new NNode(SENSOR,ncount,INPUT);
      else newnode=new NNode(SENSOR,ncount,BIAS);
      
      newnode->nodetrait=newtrait;

      //Add the node to the list of nodes
      nodes.push_back(newnode);
    }

    //Build the hidden nodes
    for(ncount=i+1;ncount<=i+n;ncount++) {
      newnode=new NNode(NEURON,ncount,HIDDEN);
      newnode->nodetrait=newtrait;
      //Add the node to the list of nodes
      nodes.push_back(newnode);
    }
    
    //Build the output nodes
    for(ncount=first_output;ncount<=totalnodes;ncount++) {
      newnode=new NNode(NEURON,ncount,OUTPUT);
      newnode->nodetrait=newtrait;
      //Add the node to the list of nodes
      nodes.push_back(newnode);
    }

    cout<<"Built nodes"<<endl;

    //Connect the nodes 
    ccount=1;  //Start the connection counter
    
    //Step through the connection matrix, creating connection genes
    cmp=cm;
    count=0;
    for(col=1;col<=totalnodes;col++)
      for(row=1;row<=totalnodes;row++) {
	//Only try to create a link if it is in the matrix
	//and not leading into a sensor
		
	if ((*cmp==true)&&(col>i)&&
	    ((col<=maxnode)||(col>=first_output))&&
	    ((row<=maxnode)||(row>=first_output))) {
	  //If it isn't recurrent, create the connection no matter what
	  if (col>row) {

	    //Retrieve the in_node
	    node_iter=nodes.begin();
	    while((*node_iter)->node_id!=row)
	      node_iter++;

	    in_node=(*node_iter);

	    //Retrieve the out_node
	    node_iter=nodes.begin();
	    while((*node_iter)->node_id!=col)
	      node_iter++;

	    out_node=(*node_iter);

	    //Create the gene
	    new_weight=randposneg()*randfloat();
	    newgene=new Gene(newtrait,new_weight, in_node, out_node,false,count,new_weight);
  
	    //Add the gene to the genome
	    genes.push_back(newgene);
	  }
	  else if (r) {
	    //Create a recurrent connection
	    	    
	    //Retrieve the in_node
	    node_iter=nodes.begin();
	    while((*node_iter)->node_id!=row)
	      node_iter++;
	    
	    in_node=(*node_iter);
	    
	    //Retrieve the out_node
	    node_iter=nodes.begin();
	    while((*node_iter)->node_id!=col)
	      node_iter++;
	    
	    out_node=(*node_iter);
	    
	    //Create the gene
	    new_weight=randposneg()*randfloat();
	    newgene=new Gene(newtrait,new_weight, in_node, out_node,true,count,new_weight);
  
	    //Add the gene to the genome
	    genes.push_back(newgene);

	  }

	}

	count++; //increment gene counter	    
	cmp++;
      }

    delete [] cm;

  }

  //Destructor kills off all lists (including the trait vector)
  ~Genome() {
    vector<Trait*>::iterator curtrait;
    list<NNode*>::iterator curnode;
    list<Gene*>::iterator curgene;

    for(curtrait=traits.begin();curtrait!=traits.end();++curtrait) {
      delete (*curtrait);
    }

    for(curnode=nodes.begin();curnode!=nodes.end();++curnode) {
      delete (*curnode);
    }
    
    for(curgene=genes.begin();curgene!=genes.end();++curgene) {
      delete (*curgene);
    }

  }

  Network *genesis(int);  //Generate a network phenotype from this Genome with specified id

  void print_to_file(ofstream &outFile); //Dump this genome to specified file

  void print_to_filename(char *filename); //Dump genome to file

  Genome *duplicate(int new_id);  /* Duplicate this Genome to create a new one
				     with the specified id */
  
  bool verify();  /* For debugging- tests node integrity */

  /* ******* MUTATORS ******* */

  void mutate_random_trait(); /* Perturb params in one trait */

  void mutate_link_trait(int times);  /* Change random link's trait. 
					 Repeat times times */

  void mutate_node_trait(int times); /* Change random node's trait times 
					times */

  void mutate_link_weights(double power,double rate,mutator mut_type);  /* Add Gaussian noise to 
					      linkweights either GAUSSIAN
					      or COLDGAUSSIAN (from zero) */

  void mutate_toggle_enable(int times); /* toggle genes on or off */

  void mutate_gene_reenable();  /* Find first disabled gene and enable it */

  /* These last kinds of mutations return false if they fail
     They can fail under certain conditions,  being unable
     to find a suitable place to make the mutation.
     Generally, if they fail, they can be called again if desired. */

  bool mutate_add_node(list<Innovation*> &innovs,int &curnode_id,double &curinnov); /* Mutate genome by adding a node respresentation */

  /* Mutate the genome by adding a new link between 2 random NNodes */
  bool mutate_add_link(list<Innovation*> &innovs,double &curinnov,int tries); 

  /* ****** MATING METHODS ***** */

  /* This method mates this Genome with another Genome g.  
     For every point in each Genome, where each Genome shares
     the innovation number, the Gene is chosen randomly from 
     either parent.  If one parent has an innovation absent in 
     the other, the baby will inherit the innovation */
  Genome *mate_multipoint(Genome *g,int genomeid,double fitness1, double fitness2);

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

  /* Return number of non-disabled genes */
  int extrons();

};

class Population;

//Guile Wrappers
Genome *new_Genome_fromfile(char *filename, int id);
Genome *new_Genome_load(char *filename);
Genome *display_Genome(char *filename, int width, int height);
int print_Genome_tofile(Genome *g,char *filename);
int print_Genome(Genome *g);
Population *new_Pop_fromfile(char *filename);
Population *new_Pop_fromGenomefile(char *filename);


/* ------------------------------------------------------------ */
/* This Innovation class serves as a way to record innovations
   specifically, so that an innovation in one genome can be 
   compared with other innovations in the same epoch, and if they
   are the same innovation, they can both be assigned the same
   innovation number.

   This class can encode innovations that represent a new link
   forming, or a new node being added.  In each case, two 
   nodes fully specify the innovation and where it must have
   occured.  (Between them)                                     */
/* ------------------------------------------------------------ */
class Innovation : public NEAT {
 public:

  innovtype innovation_type;  //Either NEWNODE or NEWLINK

  int node_in_id;     //Two nodes specify where the innovation took place
  int node_out_id;
 
  double innovation_num1;  //The number assigned to the innovation
  double innovation_num2;  /* If this is a new node innovation, 
			      then there are 2 innovations (links)
			      added for the new node */

  double new_weight;   /*  If a link is added, this is its weight */
  int new_traitnum; /* If a link is added, this is its connected trait */
  
  int newnode_id;  /* If a new node was created, this is its node_id */

  double old_innov_num;  /* If a new node was created, this is  
			    the innovnum of the gene's link it is being
			    stuck inside */
  
  bool recur_flag;

  //Constructor for the new node case
  Innovation(int nin,int nout,double num1,double num2,int newid,double oldinnov) {
    innovation_type=NEWNODE;
    node_in_id=nin;
    node_out_id=nout;
    innovation_num1=num1;
    innovation_num2=num2;
    newnode_id=newid;
    old_innov_num=oldinnov;

    //Unused parameters set to zero
    new_weight=0;
    new_traitnum=0;
    recur_flag=false;
  }

  //Constructor for new link case
  Innovation(int nin,int nout,double num1,double w,int t) {
    innovation_type=NEWLINK;
    node_in_id=nin;
    node_out_id=nout;
    innovation_num1=num1;
    new_weight=w;
    new_traitnum=t;

    //Unused parameters set to zero
    innovation_num2=0;
    newnode_id=0;
    recur_flag=false;
  }

  //Constructor for a recur link
  Innovation(int nin,int nout,double num1,double w,int t,bool recur) {
    innovation_type=NEWLINK;
    node_in_id=nin;
    node_out_id=nout;
    innovation_num1=num1;
    new_weight=w;
    new_traitnum=t;
    
    //Unused parameters set to zero
    innovation_num2=0;
    newnode_id=0;
    recur_flag=recur;
  }

};

list<Innovation*> *new_innov_list(); //Retrns a brand new empty list of innovations (for Guile)

class Species;

/* ---------------------------------------------  */
/* ORGANISM CLASS:
   Organisms are Genomes and Networks with fitness
   information 
   i.e. The genotype and phenotype together
                                                  */
/* ---------------------------------------------  */
class Organism : public NEAT {

 public:
  double fitness;  //A measure of fitness for the Organism

  double orig_fitness;  //A fitness measure that won't change during adjustments

  double error;  //Used just for reporting purposes

  bool winner;  //Win marker (if needed for a particular task)

  Network *net;  //The Organism's phenotype

  Genome *gnome; //The Organism's genotype 

  Species *species;  //The Organism's Species 

  double expected_offspring; //Number of children this Organism may have

  int generation;  //Tells which generation this Organism is from

  bool eliminate;  //Marker for destruction of inferior Organisms

  bool champion; //Marks the species champ

  int super_champ_offspring;  //Number of reserved offspring for a population leader

  bool pop_champ;  //Marks the best in population
  
  bool pop_champ_child; //Marks the duplicate child of a champion (for tracking purposes)

  double high_fit; //DEBUG variable- high fitness of champ

  void update_phenotype(); /* Regenerate the network based on a change
			      in the genotype */
           
  //Track its origin- for debugging or analysis- we can tell how the organism was born
  bool mut_struct_baby;
  bool mate_baby;
                
  Organism(double fit, Genome *g,int gen) {
    fitness=fit;
    orig_fitness=fitness;
    gnome=g;
    net=gnome->genesis(gnome->genome_id);
    species=0;  //Start it in no Species
    expected_offspring=0;
    generation=gen;
    eliminate=false;
    error=0;
    winner=false;
    champion=false;
    super_champ_offspring=0;

    //DEBUG vars
    pop_champ=false;
    pop_champ_child=false;
    high_fit=0;
    mut_struct_baby=0;
    mate_baby=0;
  }

  ~Organism() {
    delete net;
    delete gnome;

  }

};

/* ---------------------------------------------  */
/* SPECIES CLASS:
   A Species is a group of similar Organisms      
   Reproduction takes place mostly within a
   single species, so that compatible organisms
   can mate.                                      */
/* ---------------------------------------------  */
class Species : public NEAT {

 public:

  int id;

  int age;  //The age of the Species 
  
  double ave_fitness;  //The average fitness of the Species
  double max_fitness;  //Max fitness of the Species
  double max_fitness_ever;  //The max it ever had

  int expected_offspring;

  bool novel;

  bool checked;

  list<Organism*> organisms; //The organisms in the Species

  //list<Organism*> reproduction_pool;  //The organisms for reproduction- NOT NEEDED

  int age_of_last_improvement;  //If this is too long ago, the Species will goes extinct
  
  bool add_Organism(Organism *o);

  Organism *first();

  bool print_to_file(ofstream &outFile);

  //Change the fitness of all the organisms in the species 
  //to possibly depend slightly on the age of the species
  //and then divide it by the size of the species so that the
  //organisms in the species "share" the fitness
  void adjust_fitness();

  double compute_average_fitness(); 

  double compute_max_fitness();

  //Counts the number of offspring expected from all its members
  //skim is for keeping track of remaining fractional parts of offspring
  //and distributing them among species
  double count_offspring(double skim);

  //Compute generations since last improvement
  int last_improved() {
    return age-age_of_last_improvement;
  }

  //Remove an organism from Species
  bool remove_org(Organism *org);

  double size() {
    return organisms.size();
  }

  Organism *get_champ();

  //Perform mating and mutation to form next generation
  bool reproduce(int generation, Population *pop,list<Species*> &sorted_species);

  Species(int i) {
    id=i;
    age=1;
    ave_fitness=0.0;
    expected_offspring=0;
    novel=false;
    age_of_last_improvement=0;
    max_fitness=0;
    max_fitness_ever=0;
  }

  //Allows the creation of a Species that won't age (a novel one)
  //This protects new Species from aging inside their first generation
  Species(int i,bool n) {
    id=i;
    age=1;
    ave_fitness=0.0;
    expected_offspring=0;
    novel=n;
    age_of_last_improvement=0;
    max_fitness=0;
    max_fitness_ever=0;
  }


  ~Species() {
    
    list<Organism*>::iterator curorg;

    for(curorg=organisms.begin();curorg!=organisms.end();++curorg) {
      delete (*curorg);
    }

  }

};


/* Special Structures for Speciation visualization */
class Species_viz : public NEAT {
 public:
  int id;
  int size;
  double fitness;   //The average fitness
  double max_fitness;  //Max fitness for Species
  
  int startx;   //Starting x position on window
  int endx;     //Ending x position on window
  
  //The program can orient species along the x or y axis
  int starty;
  int endy;

  bool firstgen;  //Is it the Species' first generation?

  //A visualization of a Species is constructed off of that Species s
  Species_viz(Species *s) {
    id=s->id;
    size=(s->organisms).size();
    fitness=s->ave_fitness;
    max_fitness=s->max_fitness;
    startx=0;  //These positions will be computed later
    endx=0;
    if (s->age==1)
      firstgen=true; 
    else firstgen=false;
  }

};

/* The visualization of a generation is a list of all the Species visualizations
   of that generation */
struct Generation_viz {
  int gen_num;
  list<Species_viz*> species_list;
};

/* ---------------------------------------------  */
/* POPULATION CLASS:
   A Population is a group of Organisms   
   including their species                        */
/* ---------------------------------------------  */
class Population : public NEAT {

 protected: 

  /* A Population can be spawned off of a single Genome */
  /* There will be size Genomes added to the Population */
  /* The Population does not have to be empty to add Genomes */
  bool spawn(Genome *g,int size);

 public:

  list<Organism*> organisms; //The organisms in the Population

  list<Species*> species;  /* Species in the Population
				   Note that the species should comprise
				   all the genomes */

  /* Member variables used during reproduction */

  list<Innovation*> innovations;  /* For holding the genetic innovations
				     of the newest generation */
  int cur_node_id;  //Current label number available
  double cur_innov_num;

  int last_species;  //The highest species number

  int final_gen;  //The last generation played

  list<Generation_viz*> generation_snapshots; /* Visualization of Speciation History */

  /* Fitness Statistics */
  double mean_fitness;
  double variance;
  double standard_deviation;

  //An integer that when above zero tells when the first winner appeared
  int winnergen;
  
  /*  When do we need to delta code?  */
  double highest_fitness;  //Stagnation detector
  int highest_last_changed; //If too high, leads to delta coding

  bool speciate(); //Separate the Organisms into species

  //Print Population to a file specified by a string 
  bool print_to_file(char *filename);

  //Print Population to a file in speciated order with comments
  //separating each species
  bool print_to_file_by_species(char *filename);

  //Run verify on all Genomes in this Population (Debugging)
  bool verify();

  //Turnover the population to a new generation using fitness 
  //The generation argument is the next generation
  bool epoch(int generation);

  //Take a snapshot of the current generation for visualization purpose
  void snapshot();

  //Visualize the Population's Speciation using a graphic depiction
  //width and height denote the size of the window
  void visualize(int width,int height,int start_gen,int stop_gen);

  /* Construct off of a single spawning Genome */
  Population(Genome *g,int size) {
    winnergen=0;
    highest_fitness=0.0;
    highest_last_changed=0;
    spawn(g,size);
  }

  /* Special constructor to create a population of random topologies */
  // uses Genome(int i, int o, int n,int nmax, bool r, double linkprob) 
  //See the Genome constructor above for the argument specifications
  Population(int size,int i,int o, int nmax, bool r, double linkprob) {
    int count;
    Genome *new_genome; 

    cout<<"Making a random pop"<<endl;

    winnergen=0;
    highest_fitness=0.0;
    highest_last_changed=0;

    for(count=0;count<size;count++) {
      new_genome=new Genome(count,i,o,randint(0,nmax),nmax,r,linkprob);
      organisms.push_back(new Organism(0,new_genome,1));
    }

    cur_node_id=i+o+nmax+1;;
    cur_innov_num=(i+o+nmax)*(i+o+nmax)+1;

    cout<<"Calling speciate"<<endl;
    speciate(); 

  }

  /* Construct off of a file of Genomes */
  Population(char *filename) {

    char curword[20];  //max word size of 20 characters

    ifstream iFile(filename,ios::in);

    Genome *new_genome;

    winnergen=0;

    highest_fitness=0.0;
    highest_last_changed=0;

    cur_node_id=0;
    cur_innov_num=0.0;

    //Make sure it worked
    if (!iFile) {
      cerr<<"Can't open "<<filename<<" for input"<<endl;
    }
   
    else {

      //Loop until file is finished, parsing each line
      while (iFile>>curword) {
	
	//Check for next
	if (strcmp(curword,"genomestart")==0) {
	  int idcheck;
	  iFile>>idcheck;
	  new_genome=new Genome(idcheck,iFile);
	  organisms.push_back(new Organism(0,new_genome,1));
	  if (cur_node_id<(new_genome->get_last_node_id()))
	    cur_node_id=new_genome->get_last_node_id();
	  
	  if (cur_innov_num<(new_genome->get_last_gene_innovnum()))
	    cur_innov_num=new_genome->get_last_gene_innovnum();
	}
	
	//Ignore comments surrounded by /* */ - they get printed to screen
	else if (strcmp(curword,"/*")==0) {
	  iFile>>curword;
	  while (strcmp(curword,"*/")!=0) {
	    cout<<curword<<" ";
	    iFile>>curword;
	  }
	  cout<<endl;
	  
	}
      }
      
      iFile.close();
      
      speciate();
     
    }
  }


  //It can delete a Population in two ways:
  //-delete by killing off the species
  //-delete by killing off the organisms themselves (if not speciated)
  //It does the latter if it sees the species list is empty
  ~Population() {
    
    list<Species*>::iterator curspec;
    list<Organism*>::iterator curorg;
    list<Generation_viz*>::iterator cursnap;

    if (species.begin()!=species.end()) {
      for(curspec=species.begin();curspec!=species.end();++curspec) {
	delete (*curspec);
      }
    }
    else {
      for(curorg=organisms.begin();curorg!=organisms.end();++curorg) {
	delete (*curorg);
      }


    }

    //Delete the snapshots
    for(cursnap=generation_snapshots.begin();cursnap!=generation_snapshots.end();++cursnap) {
      delete (*cursnap);
    }

  }


};




#endif
















