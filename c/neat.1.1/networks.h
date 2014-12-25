#ifndef NETWORKS_H
#define NETWORKS_H

#include <assert.h>
#include <iostream>
#include <fstream.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


#include <gtk--/main.h>
#include <gtk--/style.h>
#include <gtk--/window.h>
#include <gtk--/button.h>
#include <gtk--/box.h>
#include <gtk--/drawingarea.h>

#include <list>
#include <vector>
#include <algorithm>

#include "neat.h"

/* These are NNode types */
typedef int nodetype;
extern const int NEURON;
extern const int SENSOR;

/* These have to do with genetic usage */
typedef int nodeplace;
extern const int HIDDEN;
extern const int INPUT;
extern const int OUTPUT;
extern const int BIAS;

//Typedef used in the Innovation class
typedef int innovtype;
extern const int NEWNODE;
extern const int NEWLINK;

/* Mutators are variables that specify a kind of mutation */
typedef int mutator;
extern const mutator GAUSSIAN;  //This adds Gaussian noise to the weights
extern const mutator COLDGAUSSIAN;  //This sets weights to numbers chosen from a Gaussian distribution


/* Bools */
typedef int boolean;

/* Activation Types */
typedef int functype;
extern const int SIGMOID;


class NNode;
class Link;
class Trait;
class Network;

/* Inline Random Functions */
inline bool randbit() {return rand()%2;}
inline int randposneg() {
  if (randbit()) return 1; 
  else return -1;
}
inline int randint(int x,int y) {return rand()%(y-x+1)+x;}
inline double randfloat() {return (rand())/(RAND_MAX+1.0);}

//Returns a normally distributed deviate with 0 mean and unit variance
//Algorithm is from Numerical Recipes in C, Second Edition
inline double gaussrand() {
  static int iset=0;
  static double gset;
  double fac,rsq,v1,v2;

  if (iset==0) {
    do {
      v1=2.0*((rand())/(RAND_MAX+1.0))-1.0;
      v2=2.0*((rand())/(RAND_MAX+1.0))-1.0;
      rsq=v1*v1+v2*v2;
    } while (rsq>=1.0 || rsq==0.0);
    fac=sqrt(-2.0*log(rsq)/rsq);
    gset=v1*fac;
    iset=1;
    return v2*fac;
  }
  else {
    iset=0;
    return gset;
  }
}

//This is an incorrect gassian distribution...but it is faster than gaussrand (maybe it's good enough?)
inline double gaussrand_wrong() {return (randposneg())*(sqrt(-log((rand()*1.0)/RAND_MAX)));}

//Conversion of types for graphics
void reverse(char *s);
char *itoa(int n,char s[]);



/* ------------------------------------------------------------------ */
/* TRAIT: A Trait is a group of parameters that can be expressed     */
/*        as a group more than one time.  Traits save a genetic      */
/*        algorithm from having to search vast parameter landscapes  */
/*        on every node.  Instead, each node can simply point to a trait */
/*        and those traits can evolve on their own */
class Trait : public NEAT {

  /* ************ LEARNING PARAMETERS *********** */
  /* The following parameters are for use in    
     neurons that learn through habituation,
     sensitization, or Hebbian-type processes  */

 public:

  int trait_id;  //Used in file saving and loading

  //static const int num_params=8; //Now defined in neat.h instead

  double params[NEAT::num_trait_params];  //Keep traits in an array

  Trait () {
    for (int count=0;count<NEAT::num_trait_params;count++)
      params[count]=0;

    trait_id=0;
  }

  Trait(int id,double p1,double p2,double p3,double p4,double p5,double p6,double p7,double p8,double p9) {
    trait_id=id;

    params[0]=p1;
    params[1]=p2;
    params[2]=p3;
    params[3]=p4;
    params[4]=p5;
    params[5]=p6;
    params[6]=p7;
    params[7]=0;

  }

  //Create a trait exactly like another trait
  Trait(Trait *t) {
    for(int count=0;count<NEAT::num_trait_params;count++)
      params[count]=(t->params)[count];

    trait_id=t->trait_id;
  }


  //Special constructor off a file assume word "trait" has been read in
  Trait(ifstream &iFile) {

    //Read in trait id
    iFile>>trait_id;

    //IS THE STOPPING CONDITION CORRECT?  ALERT
    for(int count=0;count<NEAT::num_trait_params;count++)
      iFile>>params[count];

  }

  //Special Constructor creates a new Trait which is the average
  //of 2 existing traits passed in
  Trait(Trait *t1,Trait *t2) {
    for(int count=0;count<NEAT::num_trait_params;count++)
      params[count]=(((t1->params)[count])+((t2->params)[count]))/2.0;
    trait_id=t1->trait_id;
  }

  ~Trait() {
  }

  void print_to_file(ofstream &outFile);  //Dump trait to a file
  void mutate();  //Perturb the trait parameters slightly
  
  friend ostream& operator<< (ostream& os, const Trait *thetrait);
  
};


/* ----------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */
/* A LINK is a connection from one node to another with an associated weight */
/* It can be marked as recurrent */
/* Its parameters are made public for efficiency */
class Link : public NEAT {
 public: 
  double weight; //Weight of connection
  NNode *in_node; //NNode inputting into the link
  NNode *out_node; //NNode that the link affects
  bool is_recurrent; //TRUE or FALSE
  bool time_delay; //TRUE or FALSE

  Trait *linktrait; //Points to a trait of parameters for genetic creation
  
  /* ************ LEARNING PARAMETERS *********** */
  /* These are link-related parameters that change
     during Hebbian type learning */
  double added_weight;  /* The amount of weight adjustment */

  double params[NEAT::num_trait_params];

  Link(double w,NNode *inode,NNode *onode,bool recur) {
    weight=w;
    in_node=inode;
    out_node=onode;
    is_recurrent=recur;
    added_weight=0;
    linktrait=0;
    time_delay=false;
  }

  //Including a trait pointer in the Link creation
  Link(Trait *lt,double w,NNode *inode,NNode *onode,bool recur) {
    weight=w;
    in_node=inode;
    out_node=onode;
    is_recurrent=recur;
    added_weight=0;
    linktrait=lt;
    time_delay=false;
  }	

  Link(double w) {  //For when you don't know the connections yet
    weight=w;
    in_node=out_node=0;  
    is_recurrent=FALSE;
    linktrait=0;
    time_delay=false;
  }

  ~Link() {
    //if (linktrait!=0) delete linktrait;
  }

  void derive_trait(Trait *curtrait);  //Derive a trait into link params

};


/* ----------------------------------------------------------------------- */
/* A NODE is either a NEURON or a SENSOR.  
   If it's a sensor, it can be loaded with a value for output
   If it's a neuron, it has a list of its incoming input signals (List<Link> is used) */
//Use an activation count to avoid flushing
class NNode : public NEAT {
  friend class Network;
  friend class Genome;
  //friend class Population;

 protected:

  int activation_count;  //keeps track of which activation the node is currently in
  double last_activation; //Holds the previous step's activation for recurrency
  double last_activation2; //Holds the activation BEFORE the prevous step's
  //This is necessary for a special recurrent case when the innode 
  // of a recurrent link is one time step ahead of the outnode.
  // The innode then needs to send from TWO time steps ago

  Trait *nodetrait; //Points to a trait of parameters

  NNode *analogue;  //Used for Gene decoding 
  NNode *dup;       //Used for Genome duplication

 public:
  functype ftype; //type is either SIGMOID ..or others that can be added
  nodetype type; //type is either NEURON or SENSOR 

  double activesum;  //The incoming activity before being processed 
  double activation; //The total activation entering the NNode 
  bool active_flag;  //To make sure outputs are active

  //NOT USED IN NEAT- covered by "activation" above
  double output;  //Output of the NNode- the value in the NNode 

  /* ************ LEARNING PARAMETERS *********** */
  /* The following parameters are for use in    
     neurons that learn through habituation,
     sensitization, or Hebbian-type processes  */

  double params[NEAT::num_trait_params];

  list<Link*> incoming; //A list of pointers to incoming weighted signals from other nodes
  list<Link*> outgoing;  //A list of pointers to links carrying this node's signal

  //These members are used for graphing with GTK+/GDK
  list<double> rowlevels;  //Depths from output where this node appears
  int row;  //Final row decided upon for drawing this NNode in
  int ypos;
  int xpos;

  friend ostream& operator<< (ostream& os, const NNode &thenode);
  friend ostream& operator<< (ostream& os, const NNode *thenode); 

  int node_id;  //A node can be given an identification number for saving in files

  nodeplace gen_node_label;  //Used for genetic marking of nodes

  NNode(nodetype ntype,int nodeid) {
    active_flag=false;
    activesum=0;
    activation=0;
    output=0;
    last_activation=0;
    last_activation2=0;
    type=ntype; //NEURON or SENSOR type
    activation_count=0; //Inactive upon creation
    node_id=nodeid;
    ftype=SIGMOID;
    nodetrait=0;
    gen_node_label=HIDDEN;
    dup=0;
    analogue=0;
  }

  NNode(nodetype ntype,int nodeid, nodeplace placement) {
    active_flag=false;
    activesum=0;
    activation=0;
    output=0;
    last_activation=0;
    last_activation2=0;
    type=ntype; //NEURON or SENSOR type
    activation_count=0; //Inactive upon creation
    node_id=nodeid;
    ftype=SIGMOID;
    nodetrait=0;
    gen_node_label=placement;
    dup=0;
    analogue=0;
  }

  //Construct a NNode off another NNode for genome purposes
  NNode(NNode *n,Trait *t) {
    active_flag=false;
    activation=0;
    output=0;
    last_activation=0;
    last_activation2=0;
    type=n->type; //NEURON or SENSOR type
    activation_count=0; //Inactive upon creation
    node_id=n->node_id;
    ftype=SIGMOID;
    nodetrait=0;
    gen_node_label=n->gen_node_label;
    dup=0;
    analogue=0;
    nodetrait=t;
  }

  //Construct the node out of a file specification using given list of traits
  NNode (ifstream &iFile,vector<Trait*> &traits) {
    int traitnum;
    vector<Trait*>::iterator curtrait;

    activesum=0;

    //Get the node parameters
    iFile>>node_id;
    iFile>>traitnum;
    iFile>>type;
    iFile>>gen_node_label;
    
    //Get a pointer to the trait this node points to
    if (traitnum==0) nodetrait=0;
    else {
      curtrait=traits.begin();
      while(((*curtrait)->trait_id)!=traitnum)
	++curtrait;
      nodetrait=(*curtrait);
    }
  }

  ~NNode() {
    list<Link*>::iterator curlink;

    //Kill off all incoming links
    for(curlink=incoming.begin();curlink!=incoming.end();++curlink) {
      delete (*curlink);
    }

    //if (nodetrait!=0) delete nodetrait;
    
  }

  double get_active_out(); //Just return activation for step
  double get_active_out_td(); //Return activation from PREVIOUS time step

  const nodetype get_type(); //Returns the type of the node, NEURON or SENSOR
  nodetype set_type(nodetype);  //Allows alteration between NEURON and SENSOR.  Returns its argument
  bool sensor_load(double); //If the node is a SENSOR, returns TRUE and loads the value
  void add_incoming(NNode*,double); //Adds a NONRECURRENT Link to a new NNode in the incoming List
  void add_incoming(NNode*,double,bool); //Adds a Link to a new NNode in the incoming List
  void flushback();  //Recursively deactivate backwards through the network
  void flushback_check(list<NNode*> &seenlist);  //Verify flushing for debuginh

  void  print_to_file(ofstream &outFile); //Print the node to a file

  void derive_trait(Trait *curtrait);//Have NNode gain its properties from the trait

  //Find the greatest depth starting from this neuron at depth d
  int depth(int d,Network *mynet); 

};

inline double fsigmoid(double,double,double); /* Sigmoid activation function- helper for NNode */


/* ---------                                                     --------- */
/* ----------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */
/* A NETWORK is a LIST of input NODEs and a LIST of output NODEs           
   The point of the network is to define a single entity which can evolve
   or learn on its own, even though it may be part of a larger framework */
class Network : public NEAT {

  friend class Genome;
  //friend class Population;
  friend ostream& operator<< (ostream& os, const NNode *thenode);

 protected:

  int numnodes; //The number of nodes in the net (-1 means not yet counted)
  int numlinks; //The number of links in the net (-1 means not yet counted)

  list<NNode*> all_nodes;  //A list of all the nodes

  list<NNode*>::iterator input_iter;  //For GUILE network inputting

  void destroy();  //Kills all nodes and links within
  void destroy_helper(NNode *curnode,list<NNode*> &seenlist); //helper for above

  void nodecounthelper(NNode *curnode,int &counter,list<NNode*> &seenlist);
  void linkcounthelper(NNode *curnode,int &counter,list<NNode*> &seenlist);

 public:

  Genome *genotype;  //Allows Network to be matched with its Genome

  char *name; //Every Network or subNetwork can have a name
  list<NNode*> inputs;  //NNodes that input into the network
  list<NNode*> outputs; //Values output by the network

  int net_id; //Allow for a network id
  
  friend ostream& operator<< (ostream& os, const Network &thenet);

  //This constructor allows the input and output lists to be supplied
  Network(list<NNode*> in,list<NNode*> out,list<NNode*> all,int netid) {
    inputs=in;
    outputs=out;
    all_nodes=all;
    name=0;   //Defaults to no name  ..NOTE: TRYING TO PRINT AN EMPTY NAME CAN CAUSE A CRASH
    numnodes=-1;
    numlinks=-1;
    net_id=netid;
  }

  //This constructs a net with empty input and output lists
  Network(int netid) {
    name=0; //Defaults to no name
    numnodes=-1;
    numlinks=-1;
    net_id=netid;
  }

  ~Network() {
    if (name!=0)
      delete [] name;

    destroy();  //Kill off all the nodes and links
                                                                         
   }

  void flush();  //Puts the network back into an inactive state
  void flush_check();  //Verify flushedness for debugging
 
  bool activate(); //Activates the net such that all outputs are active

  void show_activation(); //Prints the values of its outputs
  void show_input();
  void add_input(NNode*);  //Add a new input node
  void add_output(NNode*);  //Add a new output node
  void load_sensors(double*); //Takes an array of sensor values and loads it into SENSOR inputs ONLY
  void give_name(char*); //Name the network

  int nodecount(); //Counts the number of nodes in the net if not yet counted


  int linkcount(); //Counts the number of links in the net if not yet counted

  /* This checks a POTENTIAL link between a potential in_node
     and potential out_node to see if it must be recurrent */
  /* Use count and thresh to jump out in the case of an infinite loop */
  bool is_recur(NNode *potin_node,NNode *potout_node,int &count,int thresh); 

  /* Some functions to help GUILE input into Networks */
  int input_start();
  int load_in(double d);

  /* A Network Graphing Routine using GTK+ and GDK */
  /* Width and height give the size of the drawing window */
  void graph(int width,int height);
  //This collects a record of which row every NNode appears in
  //counting from the top (outputs) down
  //Path contains a pointer to a list of all nodes visited
  //on the way to this node, so we can avoid inifnite loops
  void findrows(NNode *curnode,double row,list<NNode*> &drawlist,list<NNode*> *path);

  bool outputsoff(); //If all output are not active then return true

  //Find the maximum number of neurons between an ouput and an input
  int max_depth();

 };


//WRAPPERS FOR GUILE
NNode *new_NNode3arg(nodetype ntype,int nodeid, nodeplace placement);

#endif






