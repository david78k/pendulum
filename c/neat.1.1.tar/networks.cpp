#include <iostream>
#include <list>
//#include <algorithm>
#include <math.h>
#include "networks.h"
#include "visual.h"

//Global NEAT class
class NEAT;

//Drawing Classes
class ScribbleWindow;
class ScribbleDrawingArea;

double *testdoub;


/* These are NNode types */
typedef int nodetype;
const int NEURON=0;
const int SENSOR=1;

typedef int nodeplace;
const int HIDDEN=0;
const int INPUT=1;
const int OUTPUT=2;
const int BIAS=3;

/* Mutators are variables that specify a kind of mutation */
typedef int mutator;
const mutator GAUSSIAN=0;  //This adds Gaussian noise to the weights
const mutator COLDGAUSSIAN=1;  //This sets weights to numbers chosen from a Gaussian distribution

/* Bools */
typedef int boolean;

/* Activation Types */
typedef int functype;
const int SIGMOID=0;

//Typedef used in the Innovation class
typedef int innovtype;
const int NEWNODE=0;
const int NEWLINK=1;

/* ************************************************************** */
/* *                       NODE METHODS HERE                    * */
/* ************************************************************** */

//Returns the type of the node, NEURON or SENSOR
const nodetype NNode::get_type() {
  return type;
}

//Allows alteration between NEURON and SENSOR.  Returns its argument
nodetype NNode::set_type(nodetype newtype) {
  type=newtype;
  return newtype;
}

//If the node is a SENSOR, returns TRUE and loads the value
bool NNode::sensor_load(double value) {
  if (type==SENSOR) {

    //Time delay memory
    last_activation2=last_activation;
    last_activation=activation;

    activation_count++;  //Puts sensor into next time-step
    activation=value;
    return true;
  }
  else return false;
}

// SIGMOID FUNCTION ********************************
//This is a signmoidal activation function, which is an S-shaped squashing function
//It smoothly limits the amplitude of the output of a neuron to between 0 and 1
//It is a helper to the neural-activation function get_active_out
//It is made inline so it can execute quickly since it is at every non-sensor 
// node in a network.
//NOTE:  In order to make node insertion in the middle of a link possible,
// the signmoid can be shifted to the right and more steeply sloped:
// slope=4.924273
// constant= 2.4621365
// These parameters optimize mean squared error between the old output,
// and an output of a node inserted in the middle of a link between
// the old output and some other node. 
// When not right-shifted, the steepened slope is closest to a linear
// ascent as possible between -0.5 and 0.5
inline double fsigmoid(double activesum,double slope,double constant) {
  //RIGHT SHIFTED ---------------------------------------------------------
  //return (1/(1+(exp(-(slope*activesum-constant))))); //ave 3213 clean on 40 runs of p2m and 3468 on another 40 
  //41394 with 1 failure on 8 runs

  //LEFT SHIFTED ----------------------------------------------------------
  //return (1/(1+(exp(-(slope*activesum+constant))))); //original setting ave 3423 on 40 runs of p2m, 3729 and 1 failure also

  //PLAIN SIGMOID ---------------------------------------------------------
  //return (1/(1+(exp(-activesum)))); //3511 and 1 failure

  //LEFT SHIFTED NON-STEEPENED---------------------------------------------
  //return (1/(1+(exp(-activesum-constant)))); //simple left shifted

  //NON-SHIFTED STEEPENED
  return (1/(1+(exp(-(slope*activesum))))); //Compressed
}


//NNode Display
ostream& operator<<(ostream &os,const NNode &thenode) {
  if (thenode.type==SENSOR)
    cout<<"(S"<<thenode.node_id<<", step "<<thenode.activation_count<<":"<<thenode.activation<<")";
  else if (thenode.type==NEURON)
    cout<<"(N"<<thenode.node_id<<", step "<<thenode.activation_count<<":"<<thenode.activation<<")";
  return os;
}

//NNode Pointer Display
ostream& operator<<(ostream &os,const NNode *thenode) {
  if ((*thenode).type==SENSOR)
    cout<<"(S"<<(*thenode).node_id<<","<<" step "<<(*thenode).activation_count<<":"<<(*thenode).activation<<")";
  else if ((*thenode).type==NEURON)
    cout<<"(N"<<(*thenode).node_id<<","<<" step "<<(*thenode).activation_count<<":"<<(*thenode).activation<<")";
  return os;
}

//Note: NEAT keeps track of which links are recurrent and which
//are not even though this is unnecessary for activation.
//It is useful to do so for 2 other reasons: 
//1. It makes networks visualization of recurrent networks possible
//2. It allows genetic control of the proportion of connections
//    that may become recurrent

//Add an incoming connection a node
void NNode::add_incoming(NNode *feednode,double weight,bool recur) {
  Link *newlink=new Link(weight,feednode,this,recur);
  incoming.push_back(newlink);
  (feednode->outgoing).push_back(newlink);
}

//Nonrecurrent version
void NNode::add_incoming(NNode *feednode,double weight) {
  Link *newlink=new Link(weight,feednode,this,FALSE);
  incoming.push_back(newlink);
  (feednode->outgoing).push_back(newlink);
}

//Return activation currently in node, if it has been activated
double NNode::get_active_out() {
  if (activation_count>0)
    return activation;
  else return 0.0;
}

//Return activation currently in node from PREVIOUS (time-delayed) time step,
// if there is one
double NNode::get_active_out_td() {
  if (activation_count>1)
    return last_activation;
  else return 0.0;
}

//This recursively flushes everything leading into and including this NNode, including recurrencies
void NNode::flushback() {
  list<Link*>::iterator curlink;

  //A sensor should not flush black
  if (type!=SENSOR) {
    
    if (activation_count>0) {
      activation_count=0;
      activation=0;
      last_activation=0;
      last_activation2=0;
    }

    //Flush back recursively
    for(curlink=incoming.begin();curlink!=incoming.end();++curlink) {
      //Flush the link itself (For future learning parameters possibility) 
      (*curlink)->added_weight=0;
      if ((((*curlink)->in_node)->activation_count>0))
	  ((*curlink)->in_node)->flushback();
    }
  }
  else {
    //Flush the SENSOR
    activation_count=0;
    activation=0;
    last_activation=0;
    last_activation2=0;
    
  }

}

//This recursively checks everything leading into and including this NNode, 
// including recurrencies
// Useful for debugging
void NNode::flushback_check(list<NNode*> &seenlist) {
  list<Link*>::iterator curlink;
  int pause;
  list<Link*> innodes=incoming;
  list<NNode*>::iterator location;

  if (!(type==SENSOR)) {
    

      cout<<"ALERT: "<<this<<" has activation count "<<activation_count<<endl;
      cout<<"ALERT: "<<this<<" has activation  "<<activation<<endl;
      cout<<"ALERT: "<<this<<" has last_activation  "<<last_activation<<endl;
      cout<<"ALERT: "<<this<<" has last_activation2  "<<last_activation2<<endl;

    if (activation_count>0) {
      cout<<"ALERT: "<<this<<" has activation count "<<activation_count<<endl;
      cin>>pause;
    }

    if (activation>0) {
      cout<<"ALERT: "<<this<<" has activation  "<<activation<<endl;
      cin>>pause;
    }

    if (last_activation>0) {
      cout<<"ALERT: "<<this<<" has last_activation  "<<last_activation<<endl;
      cin>>pause;
    }

    if (last_activation2>0) {
      cout<<"ALERT: "<<this<<" has last_activation2  "<<last_activation2<<endl;
      cin>>pause;
    }

    for(curlink=innodes.begin();curlink!=innodes.end();++curlink) {
      location=find(seenlist.begin(),seenlist.end(),((*curlink)->in_node));
      if (location==seenlist.end()) {
	seenlist.push_back((*curlink)->in_node);
	((*curlink)->in_node)->flushback_check(seenlist);
      }
    }

  }
  else {
    //Flush_check the SENSOR

    
      cout<<"sALERT: "<<this<<" has activation count "<<activation_count<<endl;
      cout<<"sALERT: "<<this<<" has activation  "<<activation<<endl;
      cout<<"sALERT: "<<this<<" has last_activation  "<<last_activation<<endl;
      cout<<"sALERT: "<<this<<" has last_activation2  "<<last_activation2<<endl;
    

    if (activation_count>0) {
      cout<<"ALERT: "<<this<<" has activation count "<<activation_count<<endl;
      cin>>pause;
    }
    
    if (activation>0) {
      cout<<"ALERT: "<<this<<" has activation  "<<activation<<endl;
      cin>>pause;
    }
    
    if (last_activation>0) {
      cout<<"ALERT: "<<this<<" has last_activation  "<<last_activation<<endl;
      cin>>pause;
    }
    
    if (last_activation2>0) {
      cout<<"ALERT: "<<this<<" has last_activation2  "<<last_activation2<<endl;
      cin>>pause;
    }
    
  }
  
}

//Reserved for future system expansion
void NNode::derive_trait(Trait *curtrait) {

  if (curtrait!=0) {
    for (int count=0;count<NEAT::num_trait_params;count++)
      params[count]=(curtrait->params)[count];
  }
  else {
    for (int count=0;count<NEAT::num_trait_params;count++)
      params[count]=0;
  }

}


/* ************************************************************** */
/* *                       LINK METHODS HERE                    * */
/* ************************************************************** */
//Reserved for future system expansion
void Link::derive_trait(Trait *curtrait) {
 
  if (curtrait!=0) {
    for (int count=0;count<NEAT::num_trait_params;count++)
      params[count]=(curtrait->params)[count];
  }
  else {
    for (int count=0;count<NEAT::num_trait_params;count++)
      params[count]=0;
  }

}


/* ************************************************************** */
/* *                      NETWORK METHODS HERE                  * */
/* ************************************************************** */

//Network pointer display
ostream& operator<<(ostream &os,Network *thenet) {
  thenet->show_activation();
}
 
//Puts the network back into an initial state
void Network::flush() {
  list<NNode*>::iterator curnode;

  for(curnode=outputs.begin();curnode!=outputs.end();++curnode) {
    (*curnode)->flushback();
  }
}

//Debugger: Checks network state
void Network::flush_check() {
  list<NNode*>::iterator curnode;
  list<NNode*>::iterator location;
  list<NNode*> seenlist;  //List of nodes not to doublecount

  for(curnode=outputs.begin();curnode!=outputs.end();++curnode) {    
    location=find(seenlist.begin(),seenlist.end(),(*curnode));
    if (location==seenlist.end()) {
      seenlist.push_back(*curnode);
      (*curnode)->flushback_check(seenlist);
    }
  }
}

//If all output are not active then return true
bool Network::outputsoff() {
  list<NNode*>::iterator curnode;

  for(curnode=outputs.begin();curnode!=outputs.end();++curnode) {
    if (((*curnode)->activation_count)==0) return true;
  }

  return false;
}

//Activates the net such that all outputs are active
//Returns true on success;
bool Network::activate() {
  list<NNode*>::iterator curnode;
  list<Link*>::iterator curlink;
  double add_amount;  //For adding to the activesum
  bool onetime; //Make sure we at least activate once
  int abortcount=0;  //Used in case the output is somehow truncated from the network

  //cout<<"Activating network: "<<this->genotype<<endl;

  //Keep activating until all the outputs have become active 
  //(This only happens on the first activation, because after that they
  // are always active)

  onetime=false;

  while(outputsoff()||!onetime) {

    ++abortcount;

    if (abortcount==20) {
      return false;
      cout<<"Inputs disconnected from output!"<<endl;
    }
    //cout<<"Outputs are off"<<endl;

    /* For each node, compute the sum of its incoming activation */
    for(curnode=all_nodes.begin();curnode!=all_nodes.end();++curnode) {
      //Ignore SENSORS

      //cout<<"On node "<<(*curnode)->node_id<<endl;

      if (((*curnode)->type)!=SENSOR) {
	(*curnode)->activesum=0;
	(*curnode)->active_flag=false;  //This will tell us if it has any active inputs

	/* For each incoming connection, add the activity from the connection
	   to the activesum */
	for(curlink=((*curnode)->incoming).begin();curlink!=((*curnode)->incoming).end();++curlink) {
	  //Handle possible time delays
	  if (!((*curlink)->time_delay)) {
	    add_amount=((*curlink)->weight)*(((*curlink)->in_node)->get_active_out());
	    if ((((*curlink)->in_node)->active_flag)||
		(((*curlink)->in_node)->type==SENSOR)) (*curnode)->active_flag=true;
	    (*curnode)->activesum+=add_amount;
	    //cout<<"Node "<<(*curnode)->node_id<<" adding "<<add_amount<<" from node "<<((*curlink)->in_node)->node_id<<endl;
	  }
	  else {
	    //Input over a time delayed connection
	    add_amount=((*curlink)->weight)*(((*curlink)->in_node)->get_active_out_td());
	    (*curnode)->activesum+=add_amount;
	  }
	  
	} //End for over incoming links
	
      } //End if (((*curnode)->type)!=SENSOR) 
	
    } //End for over all nodes
    
    /* Now activate all the non-sensor nodes off their incoming activation */
    for(curnode=all_nodes.begin();curnode!=all_nodes.end();++curnode) {

      if (((*curnode)->type)!=SENSOR) {
	//Only activate if some active input came in
	if ((*curnode)->active_flag) {
	  //cout<<"Activating "<<(*curnode)->node_id<<" with "<<(*curnode)->activesum<<": ";
	  
	  //Keep a memory of activations for potential time delayed connections
	  (*curnode)->last_activation2=(*curnode)->last_activation;
	  (*curnode)->last_activation=(*curnode)->activation;
	  //Now run the net activation through an activation function
	  if ((*curnode)->ftype==SIGMOID)
	    (*curnode)->activation=fsigmoid((*curnode)->activesum,4.924273,2.4621365);  //Sigmoidal activation- see comments under fsigmoid
	  
	  //cout<<(*curnode)->activation<<endl;
	  
	  //Increment the activation_count
	  //First activation cannot be from nothing!!
	  (*curnode)->activation_count++;
	}
      }
    }

    onetime=true;
  }

  return true;  
}


//THIS WAS NOT USED IN THE FINAL VERSION, AND NOT FULLY IMPLEMENTED,
//BUT IT SHOWS HOW SOMETHING LIKE THIS COULD BE INITIATED
//Note that checking networks for loops in general in not necessary
// and therefore I stopped writing this function
//Check Network for loops.  Return true if its ok, false if there is a loop.
//bool Network::integrity() {
//  list<NNode*>::iterator curnode;
//  list<list<NNode*>*> paths;
//  int count;
//  list<NNode*> *newpath;
//  list<list<NNode*>*>::iterator curpath;

//  for(curnode=outputs.begin();curnode!=outputs.end();++curnode) {
//    newpath=new list<NNode*>();
//    paths.push_back(newpath);
//    if (!((*curnode)->integrity(newpath))) return false;
//  }

  //Delete the paths now that we are done
//  curpath=paths.begin();
//  for(count=0;count<paths.size();count++) {
//    delete (*curpath);
//    curpath++;
//  }

//  return true;
//}

//Prints the values of its outputs
void Network::show_activation() {
  list<NNode*>::iterator curnode;
  int count;

  if (name!=0)
    cout<<"Network "<<name<<" with id "<<net_id<<" outputs: (";
  else cout<<"Network id "<<net_id<<" outputs: (";

  count=1;
  for(curnode=outputs.begin();curnode!=outputs.end();++curnode) {
    cout<<"[Output #"<<count<<": "<<(*curnode)<<"] ";
    count++;
  }

  cout<<")"<<endl;
}

void Network::show_input() {
  list<NNode*>::iterator curnode;
  int count;

  if (name!=0)
    cout<<"Network "<<name<<" with id "<<net_id<<" inputs: (";
  else cout<<"Network id "<<net_id<<" outputs: (";

  count=1;
  for(curnode=inputs.begin();curnode!=inputs.end();++curnode) {
    cout<<"[Input #"<<count<<": "<<(*curnode)<<"] ";
    count++;
  }

  cout<<")"<<endl;
}

//Add an input
void Network::add_input(NNode *in_node) {
  inputs.push_back(in_node);
}

//Add an output
void Network::add_output(NNode *out_node) {
  outputs.push_back(out_node);
}

//Takes an array of sensor values and loads it into SENSOR inputs ONLY
void Network::load_sensors(double *sensvals) {
  int counter=0;  //counter to move through array
  list<NNode*>::iterator sensPtr;

  for(sensPtr=inputs.begin();sensPtr!=inputs.end();++sensPtr) {
    //only load values into SENSORS (not BIASes)
    if (((*sensPtr)->type)==SENSOR) {
     (*sensPtr)->sensor_load(*sensvals);
      sensvals++;
    }
  }
}

void Network::give_name(char *newname) {
  char *temp;
  char *temp2;
  temp=new char[strlen(newname)+1];
  strcpy(temp,newname);
  if (name==0) name=temp;
  else {
    temp2=name;
    delete temp2;
    name=temp;
  }
}

//The following two methods recurse through a network from outputs
//down in order to count the number of nodes and links in the network.
//This can be useful for debugging genotype->phenotype spawning 
//(to make sure their counts correspond)

int Network::nodecount() {
  int counter=0;
  list<NNode*>::iterator curnode;
  list<NNode*>::iterator location;
  list<NNode*> seenlist;  //List of nodes not to doublecount

  for(curnode=outputs.begin();curnode!=outputs.end();++curnode) {
    
    location=find(seenlist.begin(),seenlist.end(),(*curnode));
    if (location==seenlist.end()) {
      counter++;
      seenlist.push_back(*curnode);
      nodecounthelper((*curnode),counter,seenlist);
    }
  }

  numnodes=counter;

  return counter;

}

void Network::nodecounthelper(NNode *curnode,int &counter,list<NNode*> &seenlist) {
  list<Link*> innodes=curnode->incoming;
  list<Link*>::iterator curlink;
  list<NNode*>::iterator location;

  if (!((curnode->type)==SENSOR)) {
    for(curlink=innodes.begin();curlink!=innodes.end();++curlink) {
      location=find(seenlist.begin(),seenlist.end(),((*curlink)->in_node));
      if (location==seenlist.end()) {
	counter++;
	seenlist.push_back((*curlink)->in_node);
	nodecounthelper((*curlink)->in_node,counter,seenlist);
      }
    }

  }

}

int Network::linkcount() {
  int counter=0;
  list<NNode*>::iterator curnode;
  list<NNode*> seenlist;  //List of nodes not to doublecount

  for(curnode=outputs.begin();curnode!=outputs.end();++curnode) {
    linkcounthelper((*curnode),counter,seenlist);
  }

  numlinks=counter;

  return counter;

}

void Network::linkcounthelper(NNode *curnode,int &counter,list<NNode*> &seenlist) {
  list<Link*> inlinks=curnode->incoming;
  list<Link*>::iterator curlink;
  list<NNode*>::iterator location;

  location=find(seenlist.begin(),seenlist.end(),curnode);
  if ((!((curnode->type)==SENSOR))&&(location==seenlist.end())) {
    seenlist.push_back(curnode);

    for(curlink=inlinks.begin();curlink!=inlinks.end();++curlink) {
      counter++;
      linkcounthelper((*curlink)->in_node,counter,seenlist);
    }

  }

}


//Destroy will find every node in the network and subsequently
//delete them one by one.  Since deleting a node deletes its incoming
//links, all nodes and links associated with a network will be destructed
//Note: Traits are parts of genomes and not networks, so they are not
//      deleted here
void Network::destroy() {
  list<NNode*>::iterator curnode;
  list<NNode*>::iterator location;
  list<NNode*> seenlist;  //List of nodes not to doublecount

  /* Erase all nodes from all_nodes list */

  for(curnode=all_nodes.begin();curnode!=all_nodes.end();++curnode) {
    delete (*curnode);
  }


  /* ----------------------------------- 

    OLD WAY-the old way collected the nodes together and then deleted them

  for(curnode=outputs.begin();curnode!=outputs.end();++curnode) {
    //cout<<seenlist<<endl;
    //cout<<curnode<<endl;
    //cout<<curnode->node_id<<endl;
    
    location=find(seenlist.begin(),seenlist.end(),(*curnode));
    if (location==seenlist.end()) {
      seenlist.push_back(*curnode);
      destroy_helper((*curnode),seenlist);
    }
  }

  //Now destroy the seenlist, which is all the NNodes in the network
  for(curnode=seenlist.begin();curnode!=seenlist.end();++curnode) {
    delete (*curnode);
  }

  */

}

void Network::destroy_helper(NNode *curnode,list<NNode*> &seenlist) {
  list<Link*> innodes=curnode->incoming;
  list<Link*>::iterator curlink;
  list<NNode*>::iterator location;

  if (!((curnode->type)==SENSOR)) {
    for(curlink=innodes.begin();curlink!=innodes.end();++curlink) {
      location=find(seenlist.begin(),seenlist.end(),((*curlink)->in_node));
      if (location==seenlist.end()) {
	seenlist.push_back((*curlink)->in_node);
	destroy_helper((*curlink)->in_node,seenlist);
      }
    }

  }

}

/* This checks a POTENTIAL link between a potential in_node
   and potential out_node to see if it must be recurrent */
bool Network::is_recur(NNode *potin_node,NNode *potout_node,int &count,int thresh) {
  list<Link*>::iterator curlink;


  ++count;  //Count the node as visited

  if (count>thresh) {
    cout<<"returning false"<<endl;
    return false;  //Short out the whole thing- loop detected
  }

  if (potin_node==potout_node) return true;
  else {
    //Check back on all links...
    for(curlink=(potin_node->incoming).begin();curlink!=(potin_node->incoming).end();curlink++) {
      //But skip links that are already recurrent
      //(We want to check back through the forward flow of signals only
      if (!((*curlink)->is_recurrent)) {
	if (is_recur((*curlink)->in_node,potout_node,count,thresh)) return true;
      }
    }
    return false;
  }
}

//NETWORK GRAPHICS METHODS ------------------------------------
//USING GTKMM

//Network graphing- preprocessing before GTK+ draws a Network
/* Width and height give the size of the drawing window */
void Network::graph(int width,int height) {

  list<NNode*> drawlist;  //Draw-order list of nodes
  list<NNode*>::iterator curnode;  //Moves through outputs
  list<double>::iterator currow;  //For row averaging
  double rowsum;
  int maxrow=0;
  int xpixels;
  int ypixels;
  int ystep;
  int xstep;
  int ypos=50;  //Always start drawing 20 pixels in
  int xpos=3;
  int rownum;
  int nodes_in_row;
  int noise;  //Noise is added to x values so they won't perfectly align
  list<NNode*> *startpath; //For detecting loops

  ScribbleWindow *window;

  // Before setting up graphics, we need to compute the coordinates
  // of the nodes
  for(curnode=outputs.begin();curnode!=outputs.end();++curnode) {

    //Create a new path for this output
    startpath=new list<NNode*>();
    startpath->push_back((*curnode));

    ((*curnode)->rowlevels).push_back(0);
    drawlist.push_back((*curnode));
    findrows((*curnode),0,drawlist,startpath);

    //path used
    delete startpath;
  }

  //Now assign each NNode an average row for drawing purposes
  for(curnode=drawlist.begin();curnode!=drawlist.end();++curnode) {
    rowsum=0;
    for(currow=((*curnode)->rowlevels).begin();currow!=((*curnode)->rowlevels).end();++currow) {
      rowsum+=*currow;
    }
    //Now average it
    (*curnode)->row=floor(rowsum/(((*curnode)->rowlevels).size())+0.5);
    if ((*curnode)->row>maxrow) 
      maxrow=(*curnode)->row;  //Keep track of lowest row
  }

  //Pull the sensors down to below the lowest row
  ++maxrow;

  //Now push the sensors onto the drawlist at the maxrow level
  for(curnode=inputs.begin();curnode!=inputs.end();++curnode) {
    ((*curnode)->rowlevels).push_back(0);
    (*curnode)->row=maxrow;
    drawlist.push_back((*curnode));
  }

  //Pull the outputs to the top row
  for(curnode=outputs.begin();curnode!=outputs.end();++curnode) {
    ((*curnode)->rowlevels).push_back(0);
    (*curnode)->row=0;
  }

  //Compute the size of the drawing area
  ypixels=height-100;
  xpixels=width-6;
  ystep=floor(ypixels/maxrow);

  //Assign coordinates to every node
  for(rownum=0;rownum<=maxrow;rownum++) {
    //See how many NNodes are in this row
    nodes_in_row=0;
    for(curnode=drawlist.begin();curnode!=drawlist.end();++curnode) {
      if (((*curnode)->row)==rownum) {
	nodes_in_row++;
      }
    }

    //Compute the xstep for this row
    xstep=floor(((double) xpixels)/
		((double) (nodes_in_row+1)));
    
    xpos+=xstep;

    for(curnode=drawlist.begin();curnode!=drawlist.end();++curnode) {
      if (((*curnode)->row)==rownum) {
	(*curnode)->ypos=ypos;

	noise=0;
	if (((*curnode)->type)!=SENSOR) {
	  noise=randint(0,xstep/2);
	  if (randbit()) noise=noise*-1;
	}

	(*curnode)->xpos=xpos+noise;

	xpos+=xstep;
      }
    }
    ypos+=ystep;
    xpos=3;
  }

  //Now that each node has a coordinate,we can graph the network
  //Note that the actual drawing takes place during he configuration

  window = new ScribbleWindow(this,&drawlist,width,height);
  window->show();

  myapp->run();

  //delete window;
  
}


//NETWORK RELATED WRAPPERS FOR GUILE


NNode *new_NNode3arg(nodetype ntype,int nodeid, nodeplace placement) {
  return new NNode(ntype,nodeid,placement);
}

int Network::input_start() {
  input_iter=inputs.begin();
  return 1;
}
  
int Network::load_in(double d) {
  (*input_iter)->sensor_load(d);
  input_iter++;
  if (input_iter==inputs.end()) return 0;
  else return 1;
}

//Find the maximum number of neurons between an ouput and an input
int Network::max_depth() {
  list<NNode*>::iterator curoutput; //The current output we are looking at
  int cur_depth; //The depth of the current node
  int max=0; //The max depth
  
  for(curoutput=outputs.begin();curoutput!=outputs.end();curoutput++) {
    cur_depth=(*curoutput)->depth(0,this);
    if (cur_depth>max) max=cur_depth;
  }

  return max;

}

//Find the greatest depth starting from this neuron at depth d
int NNode::depth(int d, Network *mynet) {
  list<Link*> innodes=this->incoming;
  list<Link*>::iterator curlink;
  int cur_depth; //The depth of the current node
  int max=d; //The max depth
  int pause;

  if (d>100) {
    //cout<<mynet->genotype<<endl;
    cout<<"** DEPTH NOT DETERMINED FOR NETWORK WITH LOOP"<<endl;
    return 10;
  }

  //Base Case
  if ((this->type)==SENSOR)
    return d;
  //Recursion
  else {

    for(curlink=innodes.begin();curlink!=innodes.end();++curlink) {
      cur_depth=((*curlink)->in_node)->depth(d+1,mynet);
      if (cur_depth>d) max=cur_depth;
    }
  
    return max;

  } //end else

}

// **************** FOR GRAPHING ******************
 
//Find out which rows each NNode in the Network might belong to
//Path contains a pointer to a list of all nodes visited
//on the way to this node, so we can avoid inifnite loops
void Network::findrows(NNode *curnode,double row,list<NNode*> &drawlist,
		       list<NNode*> *path) {
  list<Link*> innodes=curnode->incoming;
  list<Link*>::iterator curlink;
  list<NNode*>::iterator location;
  list<NNode*> *newpath;
  list<NNode*>::iterator pathnode; //To copy the path

  //Go down one row
  row=row+1.0;

  if (!((curnode->type)==SENSOR)) {
    for(curlink=innodes.begin();curlink!=innodes.end();++curlink) {
      //Skip recurrent links- they don't affect row positioning of nodes
      if ((!((*curlink)->is_recurrent))) {

	//Make sure there is no loop
	location=find(path->begin(),path->end(),((*curlink)->in_node));
	if (location==path->end()) {

	  //Create a new path containing this node
	  newpath=new list<NNode*>();
	  
	  //Copy the current path into the new path
	  for(pathnode=path->begin();pathnode!=path->end();++pathnode) {
	    newpath->push_back((*pathnode));
	  }
	  
	  //Add the next node in the path to the path
	  newpath->push_back(((*curlink)->in_node));
	  
	  //cout<<"found Node "<<((*curlink)->in_node)<<" at row "<<row<<endl;

	  (((*curlink)->in_node)->rowlevels).push_back(row);
	  location=find(drawlist.begin(),drawlist.end(),((*curlink)->in_node));
	  
	  if (location==drawlist.end()) {
	    //Do not add SENSORS here because the traversal order may visit them
	    //out of order
	    if (!((((*curlink)->in_node)->type)==SENSOR))
	      drawlist.push_back((*curlink)->in_node);
	  }
	  //cout<<"calling findrows on node "<<((*curlink)->in_node)->node_id<<endl;
	  findrows((*curlink)->in_node,row,drawlist,newpath);
	  
	  delete newpath; //Path has been used

	} //end if (location==drawlist.end()) 
	
      } //end if ((!((*curlink)->is_recurrent)))
    }
  }
  
}







