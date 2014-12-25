#include "genetics.h"
#include "visual.h"

//This is used for list sorting of Organisms by fitness..highest fitness first
bool order_orgs(Organism *x, Organism *y) {
  return (x)->fitness > (y)->fitness;
}
  
//This is used for list sorting of Species by fitness of best organism
//highest fitness first
bool order_species(Species *x, Species *y) {

  //cout<<"Comparing "<<((*((x->organisms).begin()))->orig_fitness)<<" and "<<((*((y->organisms).begin()))->orig_fitness)<<": "<<(((*((x->organisms).begin()))->orig_fitness) > ((*((y->organisms).begin()))->orig_fitness))<<endl;

  return (((*((x->organisms).begin()))->orig_fitness) > 
	  ((*((y->organisms).begin()))->orig_fitness));
}

/* ************************************************************** */
/* *                       TRAIT METHODS HERE                   * */
/* ************************************************************** */

//Genetic trait operation: Reserved for future system expansion
void Trait::mutate() {
  for(int count=0;count<NEAT::num_trait_params;count++) {
    if (randfloat()>trait_param_mut_prob) {
      params[count]+=(randposneg()*randfloat())*trait_mutation_power;
      if (params[count]<0) params[count]=0;
    }
  }
}


ostream& operator<< (ostream& os, const Trait *thetrait) {
  if (thetrait==0) cout<<"EMPTY TRAIT"<<endl;
  else {

    cout<<"Trait #"<<thetrait->trait_id<<endl;
    for (int count=0;count<NEAT::num_trait_params;count++) {
      cout<<(thetrait->params)[count]<<" ";
    }
    cout<<endl; 
  }
}


/* *************************************************************** */


//Gene printing method
ostream& operator<< (ostream& os, const Gene *thegene) {
  Link *thelink=thegene->lnk;
  NNode *inode=thelink->in_node;
  NNode *onode=thelink->out_node;

  cout<<"[Link ("<<inode->node_id<<", "<<onode->node_id<<") INNOV ("<<thegene->innovation_num<<", "<<thegene->mutation_num<<")";
  cout<<" Weight "<<(thegene->lnk)->weight;
  if (((thegene->lnk)->linktrait)!=0)
    cout<<" Link's trait_id "<<((thegene->lnk)->linktrait)->trait_id;
  if (thegene->enable==false) cout<<"-DISABLED-";
  if (((thegene->lnk)->is_recurrent)) cout<<"RECUR";
  cout<<"]"<<endl;
  return os;

}

//Genome printing method
ostream& operator<< (ostream& os, const Genome *thegenome) {
  list<NNode*>::iterator curnode;
  list<Gene*>::iterator curgene;
  vector<Trait*>::iterator curtrait;

  list<NNode*> nodes=thegenome->nodes;
  list<Gene*> genes=thegenome->genes;
  vector<Trait*> traits=thegenome->traits;

  cout<<"GENOME START"<<endl;
  
  for(curnode=nodes.begin();curnode!=nodes.end();++curnode) {
    if (((*curnode)->gen_node_label)==INPUT) cout<<"I";
    else if (((*curnode)->gen_node_label)==OUTPUT) cout<<"O";
    else if (((*curnode)->gen_node_label)==HIDDEN) cout<<"H";
    else if (((*curnode)->gen_node_label)==BIAS) cout<<"B";
    cout<<(*curnode)<<" ";
  }
  cout<<endl;

  for(curgene=genes.begin();curgene!=genes.end();++curgene) {
    cout<<(*curgene)<<" ";
  }
  cout<<endl;

  cout<<"Traits: ";
  for(curtrait=traits.begin();curtrait!=traits.end();++curtrait) {
    cout<<(*curtrait)<<" ";
  }
  cout<<endl;

  cout<<"GENOME END"<<endl;

  return os;

}

//Non-operator printing of genome to screen
void Genome::print_genome() {
  list<NNode*>::iterator curnode;
  list<Gene*>::iterator curgene;
  vector<Trait*>::iterator curtrait;

  Genome *thegenome=this;

  list<NNode*> nodes=thegenome->nodes;
  list<Gene*> genes=thegenome->genes;
  vector<Trait*> traits=thegenome->traits;



  cout<<"GENOME START"<<endl;
  
  for(curnode=nodes.begin();curnode!=nodes.end();++curnode) {
    if (((*curnode)->gen_node_label)==INPUT) cout<<"I";
    else if (((*curnode)->gen_node_label)==OUTPUT) cout<<"O";
    else if (((*curnode)->gen_node_label)==HIDDEN) cout<<"H";
    else if (((*curnode)->gen_node_label)==BIAS) cout<<"B";
    cout<<(*curnode)<<" ";
  }
  cout<<endl;

  for(curgene=genes.begin();curgene!=genes.end();++curgene) {
    cout<<(*curgene)<<" ";
  }
  cout<<endl;

  cout<<"Traits: ";
  for(curtrait=traits.begin();curtrait!=traits.end();++curtrait) {
    cout<<(*curtrait)<<" ";
  }
  cout<<endl;

  cout<<"GENOME END"<<endl;

}

//For debugging: A number of tests can be run on a genome to check its
// integrity
//Note: Some of these tests do not indicate a bug, but rather are meant
//  to be used to detect specific system states
bool Genome::verify() {
  list<NNode*>::iterator curnode;
  list<Gene*>::iterator curgene;
  list<Gene*>::iterator curgene2;
  NNode *inode;
  NNode *onode;

  bool disab;

  int last_id;

  int pause;

  cout<<"Verifying Genome id: "<<this->genome_id<<endl;

  if (this==0) {
    cout<<"ERROR GENOME EMPTY"<<endl;
    cin>>pause;
  }

  //Check each gene's nodes
  for(curgene=genes.begin();curgene!=genes.end();++curgene) {
    inode=((*curgene)->lnk)->in_node;
    onode=((*curgene)->lnk)->out_node;
    
    //Look for inode
    curnode=nodes.begin();
    while((curnode!=nodes.end())&&
	  ((*curnode)!=inode))
      ++curnode;

    if (curnode==nodes.end()) {
      cout<<"MISSING iNODE FROM GENE NOT IN NODES OF GENOME!!"<<endl;
      cin>>pause;
      return false;
    }

    //Look for onode
    curnode=nodes.begin();
    while((curnode!=nodes.end())&&
	  ((*curnode)!=onode))
      ++curnode;

    if (curnode==nodes.end()) {
      cout<<"MISSING oNODE FROM GENE NOT IN NODES OF GENOME!!"<<endl;
      cin>>pause;
      return false;
    }

  }

  //Check for NNodes being out of order
  last_id=0;
  for(curnode=nodes.begin();curnode!=nodes.end();++curnode) {
    if ((*curnode)->node_id<last_id) {
      cout<<"ALERT: NODES OUT OF ORDER in "<<this<<endl;
      cin>>pause;
      return false;
    }

    last_id=(*curnode)->node_id;
  }


  //Make sure there are no duplicate genes
  for(curgene=genes.begin();curgene!=genes.end();++curgene) {
    
    for(curgene2=genes.begin();curgene2!=genes.end();++curgene2) {
      if (((*curgene)!=(*curgene2))&&
	  ((((*curgene)->lnk)->is_recurrent)==(((*curgene2)->lnk)->is_recurrent))&&
	  ((((((*curgene2)->lnk)->in_node)->node_id)==((((*curgene)->lnk)->in_node)->node_id))&&
	   (((((*curgene2)->lnk)->out_node)->node_id)==((((*curgene)->lnk)->out_node)->node_id)))) {
	cout<<"ALERT: DUPLICATE GENES: "<<(*curgene)<<(*curgene2)<<endl;
	cout<<"INSIDE GENOME: "<<this<<endl;

	cin>>pause;
      }
								       

    }
  }

  //See if a gene is not disabled properly
  //Note this check does not necessary mean anything is wrong
  /*
  if (nodes.size()>=15) {
    disab=false;
    //Go through genes and see if one is disabled
    for(curgene=genes.begin();curgene!=genes.end();++curgene) {
      if (((*curgene)->enable)==false) disab=true;
    }

    if (disab==false) {
      cout<<"ALERT: NO DISABLED GENE IN GENOME: "<<this<<endl;
      //cin>>pause;
    }

  }
  */

  //Check for 2 disables in a row
  //Note:  Again, this is not necessarily a bad sign
  if (nodes.size()>=500) {
    disab=false;
    for(curgene=genes.begin();curgene!=genes.end();++curgene) {
      if ((((*curgene)->enable)==false)&&(disab==true)) {
	cout<<"ALERT: 2 DISABLES IN A ROW: "<<this<<endl;
      }
      if (((*curgene)->enable)==false) disab=true;
      else disab=false;
    }
  }

  cout<<"GENOME OK!"<<endl;

  return true;
}

//Create a network from a genome
Network *Genome::genesis(int id) {
  list<NNode*>::iterator curnode; 
  list<Gene*>::iterator curgene;
  NNode *newnode;
  Trait *curtrait;
  Link *curlink;
  Link *newlink;
  
  //Inputs and outputs will be collected here for the network
  //All nodes are collected in an all_list- 
  //this will be used for later safe destruction of the net
  list<NNode*> inlist;
  list<NNode*> outlist;
  list<NNode*> all_list;

  //Gene translation variables
  NNode *inode;
  NNode *onode;

  //The new network
  Network *newnet;

  //DEBUG: Remove
  //print_Genome_tofile(this,"genesisfile");

  //Create the nodes
  for(curnode=nodes.begin();curnode!=nodes.end();++curnode) {
    newnode=new NNode((*curnode)->type,(*curnode)->node_id);

    //Derive the node parameters from the trait pointed to
    curtrait=(*curnode)->nodetrait;
    newnode->derive_trait(curtrait);
 
    //Check for input or output designation of node
    if (((*curnode)->gen_node_label)==INPUT) 
      inlist.push_back(newnode);
    if (((*curnode)->gen_node_label)==BIAS) 
      inlist.push_back(newnode);
    if (((*curnode)->gen_node_label)==OUTPUT)
      outlist.push_back(newnode);

    //Keep track of all nodes, not just input and output
    all_list.push_back(newnode);

    //Have the node specifier point to the node it generated
    (*curnode)->analogue=newnode;

  }

  //Create the links by iterating through the genes
  for(curgene=genes.begin();curgene!=genes.end();++curgene) {
    //Only create the link if the gene is enabled
    if (((*curgene)->enable)==true) {
      curlink=(*curgene)->lnk;
      inode=(curlink->in_node)->analogue;
      onode=(curlink->out_node)->analogue;
      //NOTE: This line could be run through a recurrency check if desired
      // (no need to in the current implementation of NEAT)
      newlink=new Link(curlink->weight,inode,onode,curlink->is_recurrent);
      
      (onode->incoming).push_back(newlink);
      (inode->outgoing).push_back(newlink);

      //Derive link's parameters from its Trait pointer
      curtrait=(curlink->linktrait);
      
      curlink->derive_trait(curtrait);
    }
  }

  //Create the new network
  newnet=new Network(inlist,outlist,all_list,id);

  //Attach genotype and phenotype together
  newnet->genotype=this;
  phenotype=newnet;

  return newnet;

}

//Methods for file printing-----------------------------------

void Trait::print_to_file(ofstream &outFile) { 
  outFile<<"trait "<<trait_id<<" ";
  for(int count=0;count<NEAT::num_trait_params;count++)
    outFile<<params[count]<<" ";

  outFile<<endl;

}


void NNode::print_to_file(ofstream &outFile) {
  outFile<<"node "<<node_id<<" ";
  if (nodetrait!=0) outFile<<nodetrait->trait_id<<" ";
  else outFile<<"0 ";
  outFile<<type<<" ";
  outFile<<gen_node_label<<endl;
}

void Gene::print_to_file(ofstream &outFile) {
  outFile<<"gene ";
  //Start off with the trait number for this gene
  if ((lnk->linktrait)==0) outFile<<"0 ";
  else outFile<<((lnk->linktrait)->trait_id)<<" ";
  outFile<<(lnk->in_node)->node_id<<" ";
  outFile<<(lnk->out_node)->node_id<<" ";
  outFile<<(lnk->weight)<<" ";
  outFile<<(lnk->is_recurrent)<<" ";
  outFile<<innovation_num<<" ";
  outFile<<mutation_num<<" ";
  outFile<<enable<<endl;
}

//Print the genome to a file
void Genome::print_to_file(ofstream &outFile) {
  vector<Trait*>::iterator curtrait;
  list<NNode*>::iterator curnode;
  list<Gene*>::iterator curgene;

  outFile<<"genomestart "<<genome_id<<endl;
 
  //Output the traits
  for(curtrait=traits.begin();curtrait!=traits.end();++curtrait) {
    (*curtrait)->trait_id=curtrait-traits.begin()+1;
    (*curtrait)->print_to_file(outFile);
  }

  //Output the nodes
  for(curnode=nodes.begin();curnode!=nodes.end();++curnode) {
    (*curnode)->print_to_file(outFile);
  }

  //Output the genes
  for(curgene=genes.begin();curgene!=genes.end();++curgene) {
    (*curgene)->print_to_file(outFile);
  }

  outFile<<"genomeend "<<genome_id<<endl;

}

void Genome::print_to_filename(char *filename) {
  ofstream oFile(filename,ios::out);
  print_to_file(oFile);
  oFile.close();
}

//Return id of final NNode in Genome
int Genome::get_last_node_id() {
  return ((*(nodes.rbegin()))->node_id)+1;
}  

//Return last innovation number in Genome
double Genome::get_last_gene_innovnum() {
  return ((*(genes.rbegin()))->innovation_num)+1;       
}

//Return a pointer to a new genome that is a copy of this one
Genome *Genome::duplicate(int new_id) {
  //Collections for the new Genome
  vector<Trait*> traits_dup;
  list<NNode*> nodes_dup;
  list<Gene*> genes_dup;

  //Iterators for the old Genome
  vector<Trait*>::iterator curtrait;
  list<NNode*>::iterator curnode;
  list<Gene*>::iterator curgene;

  //New item pointers
  Trait *newtrait;
  NNode *newnode;
  Gene *newgene;
  Trait *assoc_trait;  //Trait associated with current item
  
  NNode *inode; //For forming a gene 
  NNode *onode; //For forming a gene
  Trait *traitptr;

  Genome *newgenome;

  //verify();

  //Duplicate the traits
  for(curtrait=traits.begin();curtrait!=traits.end();++curtrait) {
    newtrait=new Trait(*curtrait);
    traits_dup.push_back(newtrait);
  }

  //Duplicate NNodes
  for(curnode=nodes.begin();curnode!=nodes.end();++curnode) {
    //First, find the trait that this node points to
    if (((*curnode)->nodetrait)==0) assoc_trait=0;
    else {
      curtrait=traits_dup.begin();
      while(((*curtrait)->trait_id)!=(((*curnode)->nodetrait)->trait_id))
	++curtrait;
      assoc_trait=(*curtrait);
    }
    
    newnode=new NNode(*curnode,assoc_trait);

    (*curnode)->dup=newnode;  //Remember this node's old copy
    //(*curnode)->activation_count=55;
    nodes_dup.push_back(newnode);    
  }

  //Duplicate Genes
  for(curgene=genes.begin();curgene!=genes.end();++curgene) {
    //First find the nodes connected by the gene's link

    inode=(((*curgene)->lnk)->in_node)->dup;
    onode=(((*curgene)->lnk)->out_node)->dup;

    //Get a pointer to the trait expressed by this gene
    traitptr=((*curgene)->lnk)->linktrait;
    if (traitptr==0) assoc_trait=0;
    else {
      curtrait=traits_dup.begin();
      while(((*curtrait)->trait_id)!=(traitptr->trait_id))
	++curtrait;
      assoc_trait=(*curtrait);
    }
    
    newgene=new Gene(*curgene,assoc_trait,inode,onode);
    genes_dup.push_back(newgene);

  }

  //Finally, return the genome
  newgenome=new Genome(new_id,traits_dup,nodes_dup,genes_dup);

  return newgenome;
  
}

void Genome::mutate_random_trait() {
  vector<Trait*>::iterator thetrait; //Trait to be mutated
  int traitnum;

  //Choose a random traitnum
  traitnum=randint(0,(traits.size())-1);

  //Retrieve the trait and mutate it
  thetrait=traits.begin();
  (*(thetrait[traitnum])).mutate();
  
  //TRACK INNOVATION? (future possibility)
  
}

//This chooses a random gene, extracts the link from it,
//and repoints the link to a random trait
void Genome::mutate_link_trait(int times) {
  int traitnum;
  int genenum;
  list<Gene*>::iterator thegene;     //Link to be mutated
  vector<Trait*>::iterator thetrait; //Trait to be attached
  int count;
  int loop;

  for(loop=1;loop<=times;loop++) {

    //Choose a random traitnum
    traitnum=randint(0,(traits.size())-1);
    
    //Choose a random linknum
    genenum=randint(0,genes.size()-1);
    
    //set the link to point to the new trait
    thegene=genes.begin();
    for(count=0;count<genenum;count++)
      ++thegene;
    
    thetrait=traits.begin();
    
    ((*thegene)->lnk)->linktrait=thetrait[traitnum];
    
    //TRACK INNOVATION- future use
    //(*thegene)->mutation_num+=randposneg()*randfloat()*linktrait_mut_sig;

  }
}

//This chooses a random node
//and repoints the node to a random trait
void Genome::mutate_node_trait(int times) {
  int traitnum;
  int nodenum;
  list<NNode*>::iterator thenode;     //Link to be mutated
  list<Gene*>::iterator thegene;  //Gene to record innovation
  vector<Trait*>::iterator thetrait; //Trait to be attached
  int count;
  int loop;

  for(loop=1;loop<=times;loop++) {

    //Choose a random traitnum
    traitnum=randint(0,(traits.size())-1);
    
    //Choose a random nodenum
    nodenum=randint(0,nodes.size()-1);
    
    //set the link to point to the new trait
    thenode=nodes.begin();
    for(count=0;count<nodenum;count++)
      ++thenode;
    
    thetrait=traits.begin();
    
    (*thenode)->nodetrait=thetrait[traitnum];
    
    //TRACK INNOVATION! - possible future use
    //for any gene involving the mutated node, perturb that gene's
    //mutation number
    //for(thegene=genes.begin();thegene!=genes.end();++thegene) {
    //  if (((((*thegene)->lnk)->in_node)==(*thenode))
    //  ||
    //  ((((*thegene)->lnk)->out_node)==(*thenode)))
    //(*thegene)->mutation_num+=randposneg()*randfloat()*nodetrait_mut_sig;
    //}
  }
}


/* Add Gaussian noise to linkweights either GAUSSIAN
   or COLDGAUSSIAN (from zero) */
/* COLDGAUSSIAN means ALL connection weights will be given completely 
   new values */
void Genome::mutate_link_weights(double power,double rate,mutator mut_type) {
  list<Gene*>::iterator curgene;
  double num;  //counts gene placement
  double gene_total;
  double powermod; //Modified power by gene number
  //The power of mutation will rise farther into the genome
  //on the theory that the older genes are more fit since
  //they have stood the test of time

  double randnum;
  double randchoice; //Decide what kind of mutation to do on a gene
  double endpart; //Signifies the last part of the genome
  double gausspoint;
  double coldgausspoint;

  bool severe;  //Once in a while really shake things up

  //Wright variables
  //double oldval;
  //double perturb;


  /* --------------- WRIGHT'S MUTATION METHOD -------------- 

  //Use the fact that we know ends are newest
  gene_total=(double) genes.size();
  endpart=gene_total*0.8;
  num=0.0;

  for(curgene=genes.begin();curgene!=genes.end();curgene++) {

    //Mutate rate 0.2 controls how many params mutate in the vector
    if ((randfloat()<rate)||
	((gene_total>=10.0)&&(num>endpart))) {
      
      oldval=((*curgene)->lnk)->weight;
      
      //The amount to perturb the value by
      perturb=randfloat()*power;

      //Once in a while leave the end part alone
      if (num>endpart)
	if (randfloat()<0.2) perturb=0;  

      //Decide positive or negative
      if (randbit()) {
	//Positive case

	//if it goes over the max, find something smaller
	if (oldval+perturb>100.0) {
	  perturb=(100.0-oldval)*randfloat();
	}

	((*curgene)->lnk)->weight+=perturb;

      }
      else {
	//Negative case
	
	//if it goes below the min, find something smaller
	if (oldval-perturb<100.0) {
	  perturb=(oldval+100.0)*randfloat();
	}

	((*curgene)->lnk)->weight-=perturb;

      }
    }

    num+=1.0;
    
  }

  */

  /* ------------------------------------------------------ */

  if (randfloat()>0.5) severe=true;
  else severe=false;

  //Go through all the Genes and perturb their link's weights
  num=0.0;
  gene_total=(double) genes.size();
  endpart=gene_total*0.8;
  //powermod=randposneg()*power*randfloat();  //Make power of mutation random
  //powermod=randfloat();
  powermod=1.0;

  //Possibility: Jiggle the newest gene randomly
  //if (gene_total>10.0) {
  //  lastgene=genes.end();
  //  lastgene--;
  //  if (randfloat()>0.4)
  //    ((*lastgene)->lnk)->weight+=0.5*randposneg()*randfloat();
  //}

  //Loop on all genes
  for(curgene=genes.begin();curgene!=genes.end();curgene++) {
     
    //Possibility: Have newer genes mutate with higher probability
    //Only make mutation power vary along genome if it's big enough
    //if (gene_total>=10.0) {
      //This causes the mutation power to go up towards the end up the genome
      //powermod=((power-0.7)/gene_total)*num+0.7;
    //}
    //else powermod=power;

    //The following if determines the probabilities of doing cold gaussian
    //mutation, meaning the probability of replacing a link weight with
    //another, entirely random weight.  It is meant to bias such mutations
    //to the tail of a genome, because that is where less time-tested genes
    //reside.  The gausspoint and coldgausspoint represent values above
    //which a random float will signify that kind of mutation.  

    if (severe) {
      gausspoint=0.3;
      coldgausspoint=0.1;
    }
    else if ((gene_total>=10.0)&&(num>endpart)) {
      gausspoint=0.5;  //Mutate by modification % of connections
      coldgausspoint=0.3; //Mutate the rest by replacement % of the time
    }
    else {
      //Half the time don't do any cold mutations
      if (randfloat()>0.5) {
	gausspoint=1.0-rate;
	coldgausspoint=1.0-rate-0.1;
      }
      else {
	gausspoint=1.0-rate;
	coldgausspoint=1.0-rate;
     }
  }

  //Possible methods of setting the perturbation:
  //randnum=gaussrand()*powermod;
  //randnum=gaussrand();

    randnum=randposneg()*randfloat()*power*powermod;
    if (mut_type==GAUSSIAN) {
      randchoice=randfloat();
      if (randchoice>gausspoint)
	((*curgene)->lnk)->weight+=randnum;
      else if (randchoice>coldgausspoint)
	((*curgene)->lnk)->weight=randnum;
    }
    else if (mut_type==COLDGAUSSIAN)
      ((*curgene)->lnk)->weight=randnum;
    
    //Record the innovation
    //(*curgene)->mutation_num+=randnum;
    (*curgene)->mutation_num=((*curgene)->lnk)->weight;

    num+=1.0;

  }

}

/* Toggle genes from enable on to enable off or 
   vice versa.  Do it times times.  */
void Genome::mutate_toggle_enable(int times) {
  int genenum;
  int count;
  list<Gene*>::iterator thegene;  //Gene to toggle
  list<Gene*>::iterator checkgene;  //Gene to check
  int genecount;

    for (count=1;count<=times;count++) {
     
      //Choose a random genenum
      genenum=randint(0,genes.size()-1);

      //find the gene
      thegene=genes.begin();
      for(genecount=0;genecount<genenum;genecount++)
	++thegene;
      
      //Toggle the enable on this gene
      if (((*thegene)->enable)==true) {
	//We need to make sure that another gene connects out of the in-node
	//Because if not a section of network will break off and become isolated
	checkgene=genes.begin();
	while((checkgene!=genes.end())&&
	      (((((*checkgene)->lnk)->in_node)!=(((*thegene)->lnk)->in_node))||
	       (((*checkgene)->enable)==false)||
	       ((*checkgene)->innovation_num==(*thegene)->innovation_num)))
	  ++checkgene;
	
	//Disable the gene if it's safe to do so
	if (checkgene!=genes.end())
	  (*thegene)->enable=false;
      }
      else (*thegene)->enable=true;
    }
}

/* Find first disabled gene and enable it */
void Genome::mutate_gene_reenable() {
  list<Gene*>::iterator thegene;  //Gene to enable

  thegene=genes.begin();

  //Search for a disabled gene
  while((thegene!=genes.end())&&((*thegene)->enable==true))
    ++thegene;

  //Reenable it
  if (thegene!=genes.end())
    if (((*thegene)->enable)==false) (*thegene)->enable=true;

}

//This mutator adds a node to a Genome by inserting it in the middle
//of an existing link between 2 nodes.  This broken link will be disabled
//and now represented by 2 links with the new node between them.
//The innovs list passed in is used to compare the innovation
//with other innovations in the list and see whether they match.
//If they do, the same innovation numbers will be assigned to the new
//genes. If a disabled link is chosen, then the method just exits with false
bool Genome::mutate_add_node(list<Innovation*> &innovs,int &curnode_id,double &curinnov) {
  list<Gene*>::iterator thegene;  //random gene containing the original link
  int genenum;  //The random gene number
  NNode *in_node; //Here are the nodes connected by the gene
  NNode *out_node; 
  Link *thelink;  //The link inside the random gene

  double randmult;  //using a gaussian to find the random gene

  list<Innovation*>::iterator theinnov; //For finding a historical match
  bool done=false;
  
  Gene *newgene1;  //The new Genes
  Gene *newgene2;
  NNode *newnode;   //The new NNode
  Trait *traitptr; //The original link's trait

  double splitweight;  //If used, Set to sqrt(oldweight of oldlink)
  double oldweight;  //The weight of the original link

  int trycount;  //Take a few tries to find an open node
  bool found;

  //First, find a random gene already in the genome  
  trycount=0;
  found=false;

  //For a very small genome, we need to bias splitting towards
  //older links to avoid a "chaining" effect which is likely
  //to occur when we keep splitting between the same two
  //nodes over and over again (on newer and newer connections)
  if (genes.size()<15) {
    thegene=genes.begin();
    while (((thegene!=genes.end())
	    &&(!((*thegene)->enable)))||
	   ((thegene!=genes.end())
	    &&(((*thegene)->lnk->in_node)->gen_node_label==BIAS)))
      ++thegene;
    
    //Now randomize which node is chosen at this point
    //We bias the search towards older genes because 
    //this encourages splitting to distribute evenly
    while (((thegene!=genes.end())&&
	    (randfloat()<0.3))||
	   ((thegene!=genes.end())
	    &&(((*thegene)->lnk->in_node)->gen_node_label==BIAS)))
      {
	++thegene;
      }

    if ((!(thegene==genes.end()))&&
	((*thegene)->enable))
      {
	found=true;
      }
  }
  //In this else:
  //Alternative uniform random choice of genes
  //When the genome is not tiny, it is safe to choose randomly
  else {
    while ((trycount<20)&&(!found)) {
      
      //Choose a random genenum
      
      //Possible gaussian method
      //randmult=gaussrand()/4;
      //if (randmult>1.0) randmult=1.0;
      //This tends to select older genes for splitting
      //genenum=(int) floor((randmult*(genes.size()-1.0))+0.5);
      
      //Pure random splitting
      genenum=randint(0,genes.size()-1);
      
      //find the gene
      thegene=genes.begin();
      for(int genecount=0;genecount<genenum;genecount++)
	++thegene;
      
      //If either the gene is disabled, or it has a bias input, try again
      if (!(((*thegene)->enable==false)||
	    (((((*thegene)->lnk)->in_node)->gen_node_label)==BIAS)))
	found=true;
      
      ++trycount;
      
    }
  }

  //If we couldn't find anything say goodbye
  if (!found) 
    return false;

  //Disabled the gene
  (*thegene)->enable=false;

  //Extract the link
  thelink=(*thegene)->lnk;
  oldweight=(*thegene)->lnk->weight;

  //Extract the nodes
  in_node=thelink->in_node;
  out_node=thelink->out_node;
  
  //Check to see if this innovation has already been done   
  //in another genome
  //Innovations are used to make sure the same innovation in
  //two separate genomes in the same generation receives
  //the same innovation number.
  theinnov=innovs.begin();

  while(!done) {

    if (theinnov==innovs.end()) {

      //The innovation is totally novel

      //Get the old link's trait
      traitptr=thelink->linktrait;

      //Create the new NNode
      //By convention, it will point to the first trait
      newnode=new NNode(NEURON,curnode_id++,HIDDEN);
      newnode->nodetrait=(*(traits.begin()));
      
      //Create the new Genes
      if (thelink->is_recurrent) {
	newgene1=new Gene(traitptr,1.0,in_node,newnode,TRUE,curinnov,0);
	newgene2=new Gene(traitptr,oldweight,newnode,out_node,FALSE,curinnov+1,0);
	curinnov+=2.0;
      }
      else {
	newgene1=new Gene(traitptr,1.0,in_node,newnode,FALSE,curinnov,0);
	newgene2=new Gene(traitptr,oldweight,newnode,out_node,FALSE,curinnov+1,0);
	curinnov+=2.0;
      }

      //Add the innovations (remember what was done)
      innovs.push_back(new Innovation(in_node->node_id,out_node->node_id,curinnov-2.0,curinnov-1.0,newnode->node_id,(*thegene)->innovation_num));      

      done=true;
    }

    /* We check to see if an innovation already occured that was:
       -A new node
       -Stuck between the same nodes as were chosen for this mutation
       -Splitting the same gene as chosen for this mutation 
       If so, we know this mutation is not a novel innovation
       in this generation
       so we make it match the original, identical mutation which occured
       elsewhere in the population by coincidence */
    else if (((*theinnov)->innovation_type==NEWNODE)&&
	     ((*theinnov)->node_in_id==(in_node->node_id))&&
	     ((*theinnov)->node_out_id==(out_node->node_id))&&
	     ((*theinnov)->old_innov_num==(*thegene)->innovation_num)) 
      {
      
	//Here, the innovation has been done before
	
	//Get the old link's trait
	traitptr=thelink->linktrait;

	//Create the new NNode
	newnode=new NNode(NEURON,(*theinnov)->newnode_id,HIDDEN);      
	//By convention, it will point to the first trait
	//Note: In future may want to change this
	newnode->nodetrait=(*(traits.begin()));

	//Create the new Genes
	if (thelink->is_recurrent) {
	  newgene1=new Gene(traitptr,1.0,in_node,newnode,TRUE,(*theinnov)->innovation_num1,0);
	  newgene2=new Gene(traitptr,oldweight,newnode,out_node,FALSE,(*theinnov)->innovation_num2,0);
	}
	else {
	  newgene1=new Gene(traitptr,1.0,in_node,newnode,FALSE,(*theinnov)->innovation_num1,0);
	  newgene2=new Gene(traitptr,oldweight,newnode,out_node,FALSE,(*theinnov)->innovation_num2,0);
	}

	done=true;
      }
    else ++theinnov;
  }

  //Now add the new NNode and new Genes to the Genome
  genes.push_back(newgene1);
  genes.push_back(newgene2);
  node_insert(nodes,newnode);

  return true;

} 

/* Mutate the genome by adding a new link between 2 random NNodes */
/* If the 2 NNodes are already connected, keep trying tries times */
bool Genome::mutate_add_link(list<Innovation*> &innovs,double &curinnov,int tries) {
  
  int nodenum1,nodenum2;  //Random node numbers
  list<NNode*>::iterator thenode1,thenode2;  //Random node iterators
  int nodecount;  //Counter for finding nodes
  int trycount; //Iterates over attempts to find an unconnected pair of nodes
  NNode *nodep1; //Pointers to the nodes
  NNode *nodep2; //Pointers to the nodes
  list<Gene*>::iterator thegene; //Searches for existing link
  bool found=false;  //Tells whether an open pair was found
  list<Innovation*>::iterator theinnov; //For finding a historical match
  int recurflag; //Indicates whether proposed link is recurrent
  Gene *newgene;  //The new Gene

  int traitnum;  //Random trait finder
  vector<Trait*>::iterator thetrait;

  double newweight;  //The new weight for the new link
  
  bool done;
  bool do_recur;
  bool loop_recur;
  int first_nonsensor;

  //These are used to avoid getting stuck in an infinite loop checking
  //for recursion
  //Note that we check for recursion to control the frequency of
  //adding recurrent links rather than to prevent any paricular
  //kind of error
  int thresh=(nodes.size())*(nodes.size());
  int count=0;

  //Make attempts to find an unconnected pair
  trycount=0;
  

  //Decide whether to make this recurrent
  if (randfloat()<recur_only_prob) 
    do_recur=true;
  else do_recur=false;

  //Find the first non-sensor so that the to-node won't look at sensors as
  //possible destinations
  first_nonsensor=0;
  thenode1=nodes.begin();
  while(((*thenode1)->get_type())==SENSOR) {
    first_nonsensor++;
    ++thenode1;
  }

  //Here is the recurrent finder loop- it is done separately
  if (do_recur) {

    while(trycount<tries) {
      
      //Some of the time try to make a recur loop
      if (randfloat()>0.5) {
        loop_recur=true;
      }
      else loop_recur=false;

      if (loop_recur) {
	nodenum1=randint(first_nonsensor,nodes.size()-1);
	nodenum2=nodenum1;
      }
      else {
	//Choose random nodenums
	nodenum1=randint(0,nodes.size()-1);
	nodenum2=randint(first_nonsensor,nodes.size()-1);
      }

      //Find the first node
      thenode1=nodes.begin();
      for(nodecount=0;nodecount<nodenum1;nodecount++)
	++thenode1;
      
      //Find the second node
      thenode2=nodes.begin();
      for(nodecount=0;nodecount<nodenum2;nodecount++)
	++thenode2;
      
      nodep1=(*thenode1);
      nodep2=(*thenode2);
      
      //See if a recur link already exists  ALSO STOP AT END OF GENES!!!!
      thegene=genes.begin();
      while ((thegene!=genes.end()) && 
	     ((nodep2->type)!=SENSOR) &&   //Don't allow SENSORS to get input
	     (!((((*thegene)->lnk)->in_node==nodep1)&&
		(((*thegene)->lnk)->out_node==nodep2)&&
		((*thegene)->lnk)->is_recurrent))) {
	++thegene;
      }
      
      if (thegene!=genes.end())
	trycount++;
      else {
	count=0;
	recurflag=phenotype->is_recur(nodep1->analogue,nodep2->analogue,count,thresh);
	//Exit if the network is faulty (contains an infinite loop)
	if (count>thresh) {
	  cout<<"LOOP DETECTED DURING A RECURRENCY CHECK"<<endl;
	  return false;
	}
	
	//Make sure it finds the right kind of link (recur)
	if (!(recurflag))
	  trycount++;
	else {
	  trycount=tries;
	  found=true;
	}
	
      }
      
    }
  }
  else {
    //Loop to find a nonrecurrent link
    while(trycount<tries) {
      
      //cout<<"TRY "<<trycount<<endl;
      
      //Choose random nodenums
      nodenum1=randint(0,nodes.size()-1);
      nodenum2=randint(first_nonsensor,nodes.size()-1);
      
      //Find the first node
      thenode1=nodes.begin();
      for(nodecount=0;nodecount<nodenum1;nodecount++)
	++thenode1;
      
      //cout<<"RETRIEVED NODE# "<<(*thenode1)->node_id<<endl;
      
      //Find the second node
      thenode2=nodes.begin();
      for(nodecount=0;nodecount<nodenum2;nodecount++)
	++thenode2;
      
      nodep1=(*thenode1);
      nodep2=(*thenode2);
      
      //See if a link already exists  ALSO STOP AT END OF GENES!!!!
      thegene=genes.begin();
      while ((thegene!=genes.end()) && 
	     ((nodep2->type)!=SENSOR) &&   //Don't allow SENSORS to get input
	     (!((((*thegene)->lnk)->in_node==nodep1)&&
		(((*thegene)->lnk)->out_node==nodep2)&&
		(!(((*thegene)->lnk)->is_recurrent))))) {
	++thegene;
      }
      
      if (thegene!=genes.end())
	trycount++;
      else {
	
	count=0;
	recurflag=phenotype->is_recur(nodep1->analogue,nodep2->analogue,count,thresh);
	//Exit if the network is faulty (contains an infinite loop)
	if (count>thresh) {
	  cout<<"LOOP DETECTED DURING A RECURRENCY CHECK"<<endl;
	  return false;
	}
	
	//Make sure it finds the right kind of link (recur or not)
	if (recurflag)
	  trycount++;
	else {
	  trycount=tries;
	  found=true;
	}
	
      }
      
    } //End of normal link finding loop
  }

  //Continue only if an open link was found
  if (found) {

    //Check to see if this innovation already occured in the population
    theinnov=innovs.begin();

    //If it was supposed to be recurrent, make sure it gets labeled that way
    if (do_recur) recurflag=1;

    done=false;

    while(!done) {

      //The innovation is totally novel
      if (theinnov==innovs.end()) {

	//If the phenotype does not exist, exit on false,print error
	//Note: This should never happen- if it does there is a bug
	if (phenotype==0) {
	  cout<<"ERROR: Attempt to add link to genome with no phenotype"<<endl;
	  return false;
	}

	//Useful for debugging
	//cout<<"nodep1 id: "<<nodep1->node_id<<endl;
	//cout<<"nodep1: "<<nodep1<<endl;
	//cout<<"nodep1 analogue: "<<nodep1->analogue<<endl;
	//cout<<"nodep2 id: "<<nodep2->node_id<<endl;
	//cout<<"nodep2: "<<nodep2<<endl;
	//cout<<"nodep2 analogue: "<<nodep2->analogue<<endl;
	//cout<<"recurflag: "<<recurflag<<endl;

	//NOTE: Something like this could be used for time delays,
        //      which are not yet supported.  However, this does not
        //      have an application with recurrency.
	//If not recurrent, randomize recurrency
	//if (!recurflag) 
	//  if (randfloat()<recur_prob) recurflag=1;
	
	//Choose a random trait
	traitnum=randint(0,(traits.size())-1);
	thetrait=traits.begin();

	//Choose the new weight
	//newweight=(gaussrand())/1.5;  //Could use a gaussian
	newweight=randposneg()*randfloat()*10.0;

	//Create the new gene
	newgene=new Gene(((thetrait[traitnum])),newweight,nodep1,nodep2,recurflag,curinnov,newweight);

	//Add the innovation
	innovs.push_back(new Innovation(nodep1->node_id,nodep2->node_id,curinnov,newweight,traitnum));

	curinnov=curinnov+1.0;
	
	done=true;
      }
      //OTHERWISE, match the innovation in the innovs list
      else if (((*theinnov)->innovation_type==NEWLINK)&&
	       ((*theinnov)->node_in_id==(nodep1->node_id))&&
	       ((*theinnov)->node_out_id==(nodep2->node_id))&&
	       ((*theinnov)->recur_flag==recurflag)) {
	  
	  thetrait=traits.begin();

	  //Create new gene
	  newgene=new Gene(((thetrait[(*theinnov)->new_traitnum])),(*theinnov)->new_weight,nodep1,nodep2,recurflag,(*theinnov)->innovation_num1,0);

	  done=true;
	  
	}
      else {
	//Keep looking for a matching innovation from this generation
	++theinnov;
      }
    }
  
    //Now add the new Genes to the Genome
    genes.push_back(newgene);
    
    return true;
  }
  else {
    return false;
  }

}


//Inserts a NNode into a given ordered list of NNodes in order
void Genome::node_insert(list<NNode*> &nlist,NNode *n) {
  list<NNode*>::iterator curnode;

  int id=n->node_id;

  curnode=nlist.begin();
  while ((curnode!=nlist.end())&&
	 (((*curnode)->node_id)<id)) 
	 ++curnode;

  nlist.insert(curnode,n);

}
	 

/* This method mates this Genome with another Genome g.
     For every point in each Genome, where each Genome shares
     the innovation number, the Gene is chosen randomly from
     either parent.  If one parent has an innovation absent in
     the other, the baby may inherit the innovation
     if it is from the more fit parent.
     The new Genome is given the id in the genomeid argument.
 */
Genome *Genome::mate_multipoint(Genome *g,int genomeid,double fitness1,double fitness2) {
  //The baby Genome will contain these new Traits, NNodes, and Genes
  vector<Trait*> newtraits; 
  list<NNode*> newnodes;   
  list<Gene*> newgenes;    
  Genome *new_genome;

  list<Gene*>::iterator curgene2;  //Checks for link duplication

  //iterators for moving through the two parents' traits
  vector<Trait*>::iterator p1trait;
  vector<Trait*>::iterator p2trait;
  Trait *newtrait;

  //iterators for moving through the two parents' genes
  list<Gene*>::iterator p1gene;
  list<Gene*>::iterator p2gene;
  double p1innov;  //Innovation numbers for genes inside parents' Genomes
  double p2innov;
  Gene *chosengene;  //Gene chosen for baby to inherit
  int traitnum;  //Number of trait new gene points to
  NNode *inode;  //NNodes connected to the chosen Gene
  NNode *onode;
  NNode *new_inode;
  NNode *new_onode;
  list<NNode*>::iterator curnode;  //For checking if NNodes exist already 
  int nodetraitnum;  //Trait number for a NNode

  bool disable;  //Set to true if we want to disabled a chosen gene

  disable=false;
  Gene *newgene;

  bool p1better; //Tells if the first genome (this one) has better fitness or not

  bool skip;

  //First, average the Traits from the 2 parents to form the baby's Traits
  //It is assumed that trait vectors are the same length
  //In the future, may decide on a different method for trait mating
  p2trait=(g->traits).begin();
  for(p1trait=traits.begin();p1trait!=traits.end();++p1trait) {
    newtrait=new Trait(*p1trait,*p2trait);  //Construct by averaging
    newtraits.push_back(newtrait);
    ++p2trait;
  }

  //Figure out which genome is better
  //The worse genome should not be allowed to add extra structural baggage
  //If they are the same, use the smaller one's disjoint and excess genes only
  if (fitness1>fitness2) 
    p1better=true;
  else if (fitness1==fitness2) {
    if (genes.size()<(g->genes.size()))
      p1better=true;
    else p1better=false;
  }
  else 
    p1better=false;

  //Now move through the Genes of each parent until both genomes end
  p1gene=genes.begin();
  p2gene=(g->genes).begin();
  while(!((p1gene==genes.end())&&
	(p2gene==(g->genes).end()))) {


    skip=false;  //Default to not skipping a chosen gene

    if (p1gene==genes.end()) {
      chosengene=*p2gene;
      ++p2gene;
      if (p1better) skip=true;  //Skip excess from the worse genome
    }
    else if (p2gene==(g->genes).end()) {
      chosengene=*p1gene;
      ++p1gene;
      if (!p1better) skip=true; //Skip excess from the worse genome
    }
    else {
      //Extract current innovation numbers
      p1innov=(*p1gene)->innovation_num;
      p2innov=(*p2gene)->innovation_num;

      if (p1innov==p2innov) {
	if (randfloat()<0.5) {
	  chosengene=*p1gene;
	}
	else {
	  chosengene=*p2gene;
	}

	//If one is disabled, the corresponding gene in the offspring
	//will likely be disabled
	if ((((*p1gene)->enable)==false)||
	    (((*p2gene)->enable)==false)) 
	  if (randfloat()<0.75) disable=true;

	++p1gene;
	++p2gene;
	
      }
      else if (p1innov<p2innov) {
	chosengene=*p1gene;
	++p1gene;

	if (!p1better) skip=true;

      }
      else if (p2innov<p1innov) {
	chosengene=*p2gene;
	++p2gene;
	if (p1better) skip=true;
      }
    }
    
    //Uncomment this line to let growth go faster (from both parents excesses)
    //skip=false;

    //Check to see if the chosengene conflicts with an already chosen gene
    //i.e. do they represent the same link    
    curgene2=newgenes.begin();
    while ((curgene2!=newgenes.end())&&
	   (!((((((*curgene2)->lnk)->in_node)->node_id)==((((chosengene)->lnk)->in_node)->node_id))&&
	      (((((*curgene2)->lnk)->out_node)->node_id)==((((chosengene)->lnk)->out_node)->node_id))&&((((*curgene2)->lnk)->is_recurrent)== (((chosengene)->lnk)->is_recurrent)) ))&&
	   (!((((((*curgene2)->lnk)->in_node)->node_id)==((((chosengene)->lnk)->out_node)->node_id))&&
	      (((((*curgene2)->lnk)->out_node)->node_id)==((((chosengene)->lnk)->in_node)->node_id))&&
	      (!((((*curgene2)->lnk)->is_recurrent)))&&
	      (!((((chosengene)->lnk)->is_recurrent))) )))
      {	
	++curgene2;
      }
    
    if (curgene2!=newgenes.end()) skip=true;  //Links conflicts, abort adding
    
    if (!skip) {
  
    //Now add the chosengene to the baby
    
    //First, get the trait pointer
    if ((((chosengene->lnk)->linktrait))==0) traitnum=(*(traits.begin()))->trait_id;
    else
      traitnum=(((chosengene->lnk)->linktrait)->trait_id)-(*(traits.begin()))->trait_id;  //The subtracted number normalizes depending on whether traits start counting at 1 or 0

    //Next check for the nodes, add them if not in the baby Genome already
    inode=(chosengene->lnk)->in_node;
    onode=(chosengene->lnk)->out_node;

    //Check for inode in the newnodes list
    if (inode->node_id<onode->node_id) {
      //inode before onode

      //Checking for inode's existence
      curnode=newnodes.begin();
      while((curnode!=newnodes.end())&&
	    ((*curnode)->node_id!=inode->node_id)) 
	++curnode;

      if (curnode==newnodes.end()) {
	//Here we know the node doesn't exist so we have to add it
	//(normalized trait number for new NNode)
	
	if (!(inode->nodetrait)) nodetraitnum=0;
	else
	  nodetraitnum=((inode->nodetrait)->trait_id)-((*(traits.begin()))->trait_id);			       
      
	new_inode=new NNode(inode,newtraits[nodetraitnum]);
	node_insert(newnodes,new_inode);
      
      }
      else {
	new_inode=(*curnode);

      }

      //Checking for onode's existence
      curnode=newnodes.begin();
      while((curnode!=newnodes.end())&&
	    ((*curnode)->node_id!=onode->node_id)) 
	++curnode;
      if (curnode==newnodes.end()) {
	//Here we know the node doesn't exist so we have to add it
	//normalized trait number for new NNode

	if (!(onode->nodetrait)) nodetraitnum=0;
        else
	  nodetraitnum=((onode->nodetrait)->trait_id)-(*(traits.begin()))->trait_id;			       

	new_onode=new NNode(onode,newtraits[nodetraitnum]);

	node_insert(newnodes,new_onode);

      }
      else {
	new_onode=(*curnode);
      }

    }
    //If the onode has a higher id than the inode we want to add it first
    else {
      //Checking for onode's existence
      curnode=newnodes.begin();
      while((curnode!=newnodes.end())&&
	    ((*curnode)->node_id!=onode->node_id)) 
	++curnode;
      if (curnode==newnodes.end()) {
	//Here we know the node doesn't exist so we have to add it
	//normalized trait number for new NNode
	if (!(onode->nodetrait)) nodetraitnum=0;
        else
	  nodetraitnum=((onode->nodetrait)->trait_id)-(*(traits.begin()))->trait_id;			       

	new_onode=new NNode(onode,newtraits[nodetraitnum]);
        //newnodes.push_back(new_onode);
	node_insert(newnodes,new_onode);

      }
      else {
	new_onode=(*curnode);

      }

      //Checking for inode's existence
      curnode=newnodes.begin();
      while((curnode!=newnodes.end())&&
	    ((*curnode)->node_id!=inode->node_id)) 
	++curnode;
      if (curnode==newnodes.end()) {
	//Here we know the node doesn't exist so we have to add it
	//normalized trait number for new NNode
	if (!(inode->nodetrait)) nodetraitnum=0;  //fix
        else
	  nodetraitnum=((inode->nodetrait)->trait_id)-(*(traits.begin()))->trait_id;		                 
	new_inode=new NNode(inode,newtraits[nodetraitnum]);

	node_insert(newnodes,new_inode);

      }
      else {
	new_inode=(*curnode);

      }

    } //End NNode checking section- NNodes are now in new Genome
    
    //cout<<"new_inode trait: "<<new_inode->nodetrait<<endl;
    //cout<<"new_onode trait: "<<new_onode->nodetrait<<endl;

    //Add the Gene
    newgene=new Gene(chosengene,newtraits[traitnum],new_inode,new_onode);
    if (disable) {
      newgene->enable=false;
      disable=false;
    }
    newgenes.push_back(newgene);
    }

  }

  new_genome=new Genome(genomeid,newtraits,newnodes,newgenes);

  //Return the baby Genome
  return (new_genome);
  
}



/* This is like mate_multipoint except it averages at each
   matching gene instead of choosing one or the other */
Genome *Genome::mate_multipoint_avg(Genome *g,int genomeid,double fitness1,double fitness2) {
  //The baby Genome will contain these new Traits, NNodes, and Genes
  vector<Trait*> newtraits; 
  list<NNode*> newnodes;   
  list<Gene*> newgenes;    

  //iterators for moving through the two parents' traits
  vector<Trait*>::iterator p1trait;
  vector<Trait*>::iterator p2trait;
  Trait *newtrait;

  list<Gene*>::iterator curgene2; //Checking for link duplication

  //iterators for moving through the two parents' genes
  list<Gene*>::iterator p1gene;
  list<Gene*>::iterator p2gene;
  double p1innov;  //Innovation numbers for genes inside parents' Genomes
  double p2innov;
  Gene *chosengene;  //Gene chosen for baby to inherit
  int traitnum;  //Number of trait new gene points to
  NNode *inode;  //NNodes connected to the chosen Gene
  NNode *onode;
  NNode *new_inode;
  NNode *new_onode;

  list<NNode*>::iterator curnode;  //For checking if NNodes exist already 
  int nodetraitnum;  //Trait number for a NNode

  //This Gene is used to hold the average of the two genes to be averaged
  Gene *avgene;

  Gene *newgene;

  bool skip;

  bool p1better;  //Designate the better genome

  /* BLX-alpha variables - for assigning weights within a good space */
  /* This is for BLX-style mating, which isn't used in this implementation,
     but can easily be made from multipoint_avg */
  //double blx_alpha;
  //double w1,w2;
  //double blx_min, blx_max;
  //double blx_range;   //The space range
  //double blx_explore;  //Exploration space on left or right
  //double blx_pos;  //Decide where to put gnes distancewise
  //blx_pos=randfloat();

  //First, average the Traits from the 2 parents to form the baby's Traits
  //It is assumed that trait vectors are the same length
  //In future, could be done differently
  p2trait=(g->traits).begin();
  for(p1trait=traits.begin();p1trait!=traits.end();++p1trait) {
    newtrait=new Trait(*p1trait,*p2trait);  //Construct by averaging
    newtraits.push_back(newtrait);
    ++p2trait;
  }

  //Set up the avgene
  avgene=new Gene(0,0,0,0,0,0,0);

  //Figure out which genome is better
  //The worse genome should not be allowed to add extra structural baggage
  //If they are the same, use the smaller one's disjoint and excess genes only
  if (fitness1>fitness2) 
    p1better=true;
  else if (fitness1==fitness2) {
    if (genes.size()<(g->genes.size()))
      p1better=true;
    else p1better=false;
  }
  else 
    p1better=false;


  //Now move through the Genes of each parent until both genomes end
  p1gene=genes.begin();
  p2gene=(g->genes).begin();
  while(!((p1gene==genes.end())&&
	(p2gene==(g->genes).end()))) {

    avgene->enable=true;  //Default to enabled

    skip=false;

    if (p1gene==genes.end()) {
      chosengene=*p2gene;
      ++p2gene;

      if (p1better) skip=true;

    }
    else if (p2gene==(g->genes).end()) {
      chosengene=*p1gene;
      ++p1gene;

      if (!p1better) skip=true;
    }
    else {
      //Extract current innovation numbers
      p1innov=(*p1gene)->innovation_num;
      p2innov=(*p2gene)->innovation_num;

      if (p1innov==p2innov) {
	//Average them into the avgene
	if (randfloat()>0.5) (avgene->lnk)->linktrait=((*p1gene)->lnk)->linktrait;
	else (avgene->lnk)->linktrait=((*p2gene)->lnk)->linktrait;
	
	//WEIGHTS AVERAGED HERE
	(avgene->lnk)->weight=(((*p1gene)->lnk)->weight+((*p2gene)->lnk)->weight)/2.0;

	/*

	//BLX-alpha method (Eschelman et al 1993)
	//Not used in this implementation, but the commented code works
	//with alpha=0.5, this will produce babies evenly in exploitation and exploration space
	//and uniformly distributed throughout
	blx_alpha=-0.4;
	w1=(((*p1gene)->lnk)->weight);
	w2=(((*p2gene)->lnk)->weight);
	if (w1>w2) {
	  blx_max=w1; blx_min=w2;
	}
	else {
	  blx_max=w2; blx_min=w1;
	}
	blx_range=blx_max-blx_min;
	blx_explore=blx_alpha*blx_range;
	//Now extend the range into the exploraton space
	blx_min-=blx_explore;
	blx_max+=blx_explore;
	blx_range=blx_max-blx_min;
	//Set the weight in the new range
	(avgene->lnk)->weight=blx_min+blx_pos*blx_range;
	*/

	if (randfloat()>0.5) (avgene->lnk)->in_node=((*p1gene)->lnk)->in_node;
	else (avgene->lnk)->in_node=((*p2gene)->lnk)->in_node;

	if (randfloat()>0.5) (avgene->lnk)->out_node=((*p1gene)->lnk)->out_node;
	else (avgene->lnk)->out_node=((*p2gene)->lnk)->out_node;

	if (randfloat()>0.5) (avgene->lnk)->is_recurrent=((*p1gene)->lnk)->is_recurrent;
	else (avgene->lnk)->is_recurrent=((*p2gene)->lnk)->is_recurrent;

	avgene->innovation_num=(*p1gene)->innovation_num;
	avgene->mutation_num=((*p1gene)->mutation_num+(*p2gene)->mutation_num)/2.0;

	if ((((*p1gene)->enable)==false)||
	    (((*p2gene)->enable)==false)) 
	  if (randfloat()<0.75) avgene->enable=false;

	chosengene=avgene;
	++p1gene;
	++p2gene;
      }
      else if (p1innov<p2innov) {
	chosengene=*p1gene;
	++p1gene;

	if (!p1better) skip=true;
      }
      else if (p2innov<p1innov) {
	chosengene=*p2gene;
	++p2gene;

	if (p1better) skip=true;
      }
    }

    //THIS LINE MUST BE DELETED TO SLOW GROWTH
    //skip=false;

    //Check to see if the chosengene conflicts with an already chosen gene
    //i.e. do they represent the same link    
    curgene2=newgenes.begin();
    while ((curgene2!=newgenes.end()))
 
      {

	if (((((((*curgene2)->lnk)->in_node)->node_id)==((((chosengene)->lnk)->in_node)->node_id))&&
	   (((((*curgene2)->lnk)->out_node)->node_id)==((((chosengene)->lnk)->out_node)->node_id))&&
	    ((((*curgene2)->lnk)->is_recurrent)== (((chosengene)->lnk)->is_recurrent)))||
	   ((((((*curgene2)->lnk)->out_node)->node_id)==((((chosengene)->lnk)->in_node)->node_id))&&
           (((((*curgene2)->lnk)->in_node)->node_id)==((((chosengene)->lnk)->out_node)->node_id))&&
	    (!((((*curgene2)->lnk)->is_recurrent)))&&
	    (!((((chosengene)->lnk)->is_recurrent)))     ))
	  { 
	    skip=true;

	  }
	++curgene2;
      }

    if (!skip) {

    //Now add the chosengene to the baby
    
    //First, get the trait pointer
    if ((((chosengene->lnk)->linktrait))==0) traitnum=(*(traits.begin()))->trait_id;
    else
      traitnum=(((chosengene->lnk)->linktrait)->trait_id)-(*(traits.begin()))->trait_id;  //The subtracted number normalizes depending on whether traits start counting at 1 or 0
    
    //Next check for the nodes, add them if not in the baby Genome already
    inode=(chosengene->lnk)->in_node;
    onode=(chosengene->lnk)->out_node;

    //Check for inode in the newnodes list
    if (inode->node_id<onode->node_id) {

      //Checking for inode's existence
      curnode=newnodes.begin();
      while((curnode!=newnodes.end())&&
	    ((*curnode)->node_id!=inode->node_id)) 
	++curnode;

      if (curnode==newnodes.end()) {
	//Here we know the node doesn't exist so we have to add it
	//normalized trait number for new NNode
	
	if (!(inode->nodetrait)) nodetraitnum=0;
	else
	  nodetraitnum=((inode->nodetrait)->trait_id)-((*(traits.begin()))->trait_id);			       

	new_inode=new NNode(inode,newtraits[nodetraitnum]);

	node_insert(newnodes,new_inode);
      }
      else {
	new_inode=(*curnode);

      }

      //Checking for onode's existence
      curnode=newnodes.begin();
      while((curnode!=newnodes.end())&&
	    ((*curnode)->node_id!=onode->node_id)) 
	++curnode;
      if (curnode==newnodes.end()) {
	//Here we know the node doesn't exist so we have to add it
	//normalized trait number for new NNode

	if (!(onode->nodetrait)) nodetraitnum=0;
        else
	  nodetraitnum=((onode->nodetrait)->trait_id)-(*(traits.begin()))->trait_id;			
	new_onode=new NNode(onode,newtraits[nodetraitnum]);

	node_insert(newnodes,new_onode);
      }
      else {
	new_onode=(*curnode);
      }
    }
    //If the onode has a higher id than the inode we want to add it first
    else {
      //Checking for onode's existence
      curnode=newnodes.begin();
      while((curnode!=newnodes.end())&&
	    ((*curnode)->node_id!=onode->node_id)) 
	++curnode;
      if (curnode==newnodes.end()) {
	//Here we know the node doesn't exist so we have to add it
	//normalized trait number for new NNode
	if (!(onode->nodetrait)) nodetraitnum=0;
        else
	  nodetraitnum=((onode->nodetrait)->trait_id)-(*(traits.begin()))->trait_id;			       
	
        new_onode=new NNode(onode,newtraits[nodetraitnum]);

	node_insert(newnodes,new_onode);
      }
      else {
	new_onode=(*curnode);
      }

      //Checking for inode's existence
      curnode=newnodes.begin();
      while((curnode!=newnodes.end())&&
	    ((*curnode)->node_id!=inode->node_id)) 
	++curnode;
      if (curnode==newnodes.end()) {
	//Here we know the node doesn't exist so we have to add it
	//normalized trait number for new NNode
	if (!(inode->nodetrait)) nodetraitnum=0;
        else
	  nodetraitnum=((inode->nodetrait)->trait_id)-(*(traits.begin()))->trait_id;			       

        new_inode=new NNode(inode,newtraits[nodetraitnum]);

	node_insert(newnodes,new_inode);
      }
      else {
	new_inode=(*curnode);

      }

    } //End NNode checking section- NNodes are now in new Genome
    
    //Add the Gene
    newgene=new Gene(chosengene,newtraits[traitnum],new_inode,new_onode);
   
    newgenes.push_back(newgene);

    }  //End if which checked for link duplicationb

  }

  delete avgene;  //Clean up used object

  //Return the baby Genome
  return (new Genome(genomeid,newtraits,newnodes,newgenes));
  
}

/* This method is similar to a standard single point crossover
     operator.  Traits are averaged as in the previous 2 mating
     methods.  A Gene is chosen in the small Genome for splitting.
     When the Gene is reached, it is averaged with the matching
     Gene from the larger Genome, if one exists.  Then every other
     Gene is taken from the larger Genome  */
Genome *Genome::mate_singlepoint(Genome *g,int genomeid) {
  //The baby Genome will contain these new Traits, NNodes, and Genes
  vector<Trait*> newtraits; 
  list<NNode*> newnodes;   
  list<Gene*> newgenes;    

  //iterators for moving through the two parents' traits
  vector<Trait*>::iterator p1trait;
  vector<Trait*>::iterator p2trait;
  Trait *newtrait;

  list<Gene*>::iterator curgene2;  //Check for link duplication

  //iterators for moving through the two parents' genes
  list<Gene*>::iterator p1gene;
  list<Gene*>::iterator p2gene;
  list<Gene*>::iterator stopper;  //To tell when finished
  list<Gene*>::iterator p2stop;
  list<Gene*>::iterator p1stop;
  double p1innov;  //Innovation numbers for genes inside parents' Genomes
  double p2innov;
  Gene *chosengene;  //Gene chosen for baby to inherit
  int traitnum;  //Number of trait new gene points to
  NNode *inode;  //NNodes connected to the chosen Gene
  NNode *onode;
  NNode *new_inode;
  NNode *new_onode;
  list<NNode*>::iterator curnode;  //For checking if NNodes exist already 
  int nodetraitnum;  //Trait number for a NNode

  //This Gene is used to hold the average of the two genes to be averaged
  Gene *avgene;

  int crosspoint; //The point in the Genome to cross at
  int genecounter; //Counts up to the crosspoint
  bool skip; //Used for skipping unwanted genes

  //First, average the Traits from the 2 parents to form the baby's Traits
  //It is assumed that trait vectors are the same length
  p2trait=(g->traits).begin();
  for(p1trait=traits.begin();p1trait!=traits.end();++p1trait) {
    newtrait=new Trait(*p1trait,*p2trait);  //Construct by averaging
    newtraits.push_back(newtrait);
    ++p2trait;
  }

  //Set up the avgene
  avgene=new Gene(0,0,0,0,0,0,0);

  //Decide where to cross  (p1gene will always be in smaller Genome)
  if (genes.size()<(g->genes).size()) {
    crosspoint=randint(0,(genes.size())-1);
    p1gene=genes.begin();
    p2gene=(g->genes).begin();
    stopper=(g->genes).end();
    p1stop=genes.end();
    p2stop=(g->genes).end();
  }
  else {
    crosspoint=randint(0,((g->genes).size())-1);
    p2gene=genes.begin();
    p1gene=(g->genes).begin();
    stopper=genes.end();
    p1stop=(g->genes.end());
    p2stop=genes.end();
  }

  genecounter=0;  //Ready to count to crosspoint

  skip=false;  //Default to not skip a Gene
  //Note that we skip when we are on the wrong Genome before
  //crossing

  //Now move through the Genes of each parent until both genomes end
  while(p2gene!=stopper) {

    avgene->enable=true;  //Default to true

    if (p1gene==p1stop) {
      chosengene=*p2gene;
      ++p2gene;
    }
    else if (p2gene==p2stop) {
      chosengene=*p1gene;
      ++p1gene;
    }
    else {
      //Extract current innovation numbers

      if (p1gene==g->genes.end()) cout<<"WARNING p1"<<endl;
      if (p2gene==g->genes.end()) cout<<"WARNING p2"<<endl;

      p1innov=(*p1gene)->innovation_num;
      p2innov=(*p2gene)->innovation_num;

      if (p1innov==p2innov) {

	//Pick the chosengene depending on whether we've crossed yet
	if (genecounter<crosspoint) {
	  chosengene=*p1gene;
	}
	else if (genecounter>crosspoint) {
	  chosengene=*p2gene;
	}
	//We are at the crosspoint here
	else {

	  //Average them into the avgene
	  if (randfloat()>0.5) (avgene->lnk)->linktrait=((*p1gene)->lnk)->linktrait;
	  else (avgene->lnk)->linktrait=((*p2gene)->lnk)->linktrait;
	  
	  //WEIGHTS AVERAGED HERE
	  (avgene->lnk)->weight=(((*p1gene)->lnk)->weight+((*p2gene)->lnk)->weight)/2.0;
	
	  
	  if (randfloat()>0.5) (avgene->lnk)->in_node=((*p1gene)->lnk)->in_node;
	  else (avgene->lnk)->in_node=((*p2gene)->lnk)->in_node;
	  
	  if (randfloat()>0.5) (avgene->lnk)->out_node=((*p1gene)->lnk)->out_node;
	  else (avgene->lnk)->out_node=((*p2gene)->lnk)->out_node;
	  
	  if (randfloat()>0.5) (avgene->lnk)->is_recurrent=((*p1gene)->lnk)->is_recurrent;
	  else (avgene->lnk)->is_recurrent=((*p2gene)->lnk)->is_recurrent;
	  
	  avgene->innovation_num=(*p1gene)->innovation_num;
	  avgene->mutation_num=((*p1gene)->mutation_num+(*p2gene)->mutation_num)/2.0;

	if ((((*p1gene)->enable)==false)||
	    (((*p2gene)->enable)==false)) 
	  avgene->enable=false;

	  chosengene=avgene;
	}

	++p1gene;
	++p2gene;
	++genecounter;
      }
      else if (p1innov<p2innov) {
	if (genecounter<crosspoint) {
	  chosengene=*p1gene;
	  ++p1gene;
	  ++genecounter;
	}
	else {
	  chosengene=*p2gene;
	  ++p2gene;
	}
      }
      else if (p2innov<p1innov) {
	++p2gene;
	skip=true;  //Special case: we need to skip to the next iteration
	//becase this Gene is before the crosspoint on the wrong Genome
      }
    }
    
    //Check to see if the chosengene conflicts with an already chosen gene
    //i.e. do they represent the same link    
    curgene2=newgenes.begin();

    while ((curgene2!=newgenes.end())&&
	   (!((((((*curgene2)->lnk)->in_node)->node_id)==((((chosengene)->lnk)->in_node)->node_id))&&
	      (((((*curgene2)->lnk)->out_node)->node_id)==((((chosengene)->lnk)->out_node)->node_id))&&((((*curgene2)->lnk)->is_recurrent)== (((chosengene)->lnk)->is_recurrent)) ))&&
	   (!((((((*curgene2)->lnk)->in_node)->node_id)==((((chosengene)->lnk)->out_node)->node_id))&&
	      (((((*curgene2)->lnk)->out_node)->node_id)==((((chosengene)->lnk)->in_node)->node_id))&&
	      (!((((*curgene2)->lnk)->is_recurrent)))&&
	      (!((((chosengene)->lnk)->is_recurrent))) )))
      {

	++curgene2;
      }

      
    if (curgene2!=newgenes.end()) skip=true;  //Link is a duplicate

    if (!skip) {
      //Now add the chosengene to the baby
      
      //First, get the trait pointer
      if ((((chosengene->lnk)->linktrait))==0) traitnum=(*(traits.begin()))->trait_id;
      else
	traitnum=(((chosengene->lnk)->linktrait)->trait_id)-(*(traits.begin()))->trait_id;  //The subtracted number normalizes depending on whether traits start counting at 1 or 0
      
      //Next check for the nodes, add them if not in the baby Genome already
      inode=(chosengene->lnk)->in_node;
      onode=(chosengene->lnk)->out_node;
      
      //Check for inode in the newnodes list
      if (inode->node_id<onode->node_id) {
	//cout<<"inode before onode"<<endl;
	//Checking for inode's existence
	curnode=newnodes.begin();
	while((curnode!=newnodes.end())&&
	      ((*curnode)->node_id!=inode->node_id)) 
	  ++curnode;

	if (curnode==newnodes.end()) {
	  //Here we know the node doesn't exist so we have to add it
	  //normalized trait number for new NNode
	
	  if (!(inode->nodetrait)) nodetraitnum=0;
	  else
	    nodetraitnum=((inode->nodetrait)->trait_id)-((*(traits.begin()))->trait_id);			       
	  
	  new_inode=new NNode(inode,newtraits[nodetraitnum]);

	  node_insert(newnodes,new_inode);
	}
	else {
	  new_inode=(*curnode);
	}
	
	//Checking for onode's existence
	curnode=newnodes.begin();
	while((curnode!=newnodes.end())&&
	      ((*curnode)->node_id!=onode->node_id)) 
	  ++curnode;
	if (curnode==newnodes.end()) {
	  //Here we know the node doesn't exist so we have to add it
	  //normalized trait number for new NNode
	  
	  if (!(onode->nodetrait)) nodetraitnum=0;
	  else
	    nodetraitnum=((onode->nodetrait)->trait_id)-(*(traits.begin()))->trait_id;			     

	  new_onode=new NNode(onode,newtraits[nodetraitnum]);
	  node_insert(newnodes,new_onode);

	}
	else {
	  new_onode=(*curnode);
	}
      }
      //If the onode has a higher id than the inode we want to add it first
      else {
	//Checking for onode's existence
	curnode=newnodes.begin();
	while((curnode!=newnodes.end())&&
	      ((*curnode)->node_id!=onode->node_id)) 
	  ++curnode;
	if (curnode==newnodes.end()) {
	  //Here we know the node doesn't exist so we have to add it
	  //normalized trait number for new NNode
	  if (!(onode->nodetrait)) nodetraitnum=0;
	  else
	    nodetraitnum=((onode->nodetrait)->trait_id)-(*(traits.begin()))->trait_id;			       
	
	  new_onode=new NNode(onode,newtraits[nodetraitnum]);
	  node_insert(newnodes,new_onode);
	}
	else {
	  new_onode=(*curnode);
	}

	//Checking for inode's existence
	curnode=newnodes.begin();

	while((curnode!=newnodes.end())&&
	      ((*curnode)->node_id!=inode->node_id)) 
	  ++curnode;
	if (curnode==newnodes.end()) {
	  //Here we know the node doesn't exist so we have to add it
	  //normalized trait number for new NNode
	  if (!(inode->nodetrait)) nodetraitnum=0;
	  else
	    nodetraitnum=((inode->nodetrait)->trait_id)-(*(traits.begin()))->trait_id;			       

	  new_inode=new NNode(inode,newtraits[nodetraitnum]);
	  //newnodes.push_back(new_inode);
	  node_insert(newnodes,new_inode);
	}
	else {
	  new_inode=(*curnode);
	}
	
      } //End NNode checking section- NNodes are now in new Genome
      
      //Add the Gene
      newgenes.push_back(new Gene(chosengene,newtraits[traitnum],new_inode,new_onode));

    }  //End of if (!skip)

    skip=false;

  }


  delete avgene;  //Clean up used object

  //Return the baby Genome
  return (new Genome(genomeid,newtraits,newnodes,newgenes));
  
}

/* This function gives a measure of compatibility between
     two Genomes by computing a linear combination of 3
     characterizing variables of their compatibilty.
     The 3 variables represent PERCENT DISJOINT GENES,
     PERCENT EXCESS GENES, MUTATIONAL DIFFERENCE WITHIN
     MATCHING GENES.  So the formula for compatibility
     is:  disjoint_coeff*pdg+excess_coeff*peg+mutdiff_coeff*mdmg.
     The 3 coefficients are global system parameters */
double Genome::compatibility(Genome *g) {

  //iterators for moving through the two potential parents' Genes
  list<Gene*>::iterator p1gene;
  list<Gene*>::iterator p2gene;  

  //Innovation numbers
  double p1innov;
  double p2innov;

  //Intermediate value
  double mut_diff;

  //Set up the counters
  double num_disjoint=0.0;
  double num_excess=0.0;
  double mut_diff_total=0.0;
  double num_matching=0.0;  //Used to normalize mutation_num differences

  double max_genome_size; //Size of larger Genome

  //Get the length of the longest Genome for percentage computations
  if (genes.size()<(g->genes).size()) 
    max_genome_size=(g->genes).size();
  else max_genome_size=genes.size();

  //Now move through the Genes of each potential parent 
  //until both Genomes end
  p1gene=genes.begin();
  p2gene=(g->genes).begin();
  while(!((p1gene==genes.end())&&
	(p2gene==(g->genes).end()))) {

    if (p1gene==genes.end()) {
      ++p2gene;
      num_excess+=1.0;
    }
    else if (p2gene==(g->genes).end()) {
      ++p1gene;
      num_excess+=1.0;
    }
    else {
      //Extract current innovation numbers
      p1innov=(*p1gene)->innovation_num;
      p2innov=(*p2gene)->innovation_num;

      if (p1innov==p2innov) {
	num_matching+=1.0;
	mut_diff=((*p1gene)->mutation_num)-((*p2gene)->mutation_num);
	if (mut_diff<0.0) mut_diff=0.0-mut_diff;
	mut_diff_total+=mut_diff;

	++p1gene;
	++p2gene;
      }
      else if (p1innov<p2innov) {
	++p1gene;
	num_disjoint+=1.0;
      }
      else if (p2innov<p1innov) {
	++p2gene;
	num_disjoint+=1.0;
      }
    }
  } //End while

  //Return the compatibility number using compatibility formula
  //Note that mut_diff_total/num_matching gives the AVERAGE
  //difference between mutation_nums for any two matching Genes
  //in the Genome

  /*
    //Normalizing for genome size
  return (disjoint_coeff*(num_disjoint/max_genome_size)+
	  excess_coeff*(num_excess/max_genome_size)+
	  mutdiff_coeff*(mut_diff_total/num_matching));
  */

  //Look at disjointedness and excess in the absolute (ignoring size)
  return (disjoint_coeff*(num_disjoint/1.0)+
	  excess_coeff*(num_excess/1.0)+
	  mutdiff_coeff*(mut_diff_total/num_matching));
}

/* Return # of non-disabled genes */
int Genome::extrons() {
  list<Gene*>::iterator curgene;
  int total=0;

  for(curgene=genes.begin();curgene!=genes.end();curgene++) {
    if ((*curgene)->enable) ++total;
  }

  return total;
}

//GUILE WRAPPERS
//This is a Guile Wrapper
Genome *new_Genome_fromfile(char *filename, int id) {

  Genome *newgenome;

  ifstream iFile(filename,ios::in);
  
  //Make sure it worked
  if (!iFile) {
    cerr<<"Can't open "<<filename<<" for input"<<endl;
    return 0;
  }

  newgenome=new Genome(id,iFile);

  iFile.close();

  return newgenome;

}

//Does not require knowing the genome id in order to load
Genome *new_Genome_load(char *filename) {
  Genome *newgenome;

  int id;

  char curword[20];  //max word size of 20 characters

  ifstream iFile(filename,ios::in);
  
  //Make sure it worked
  if (!iFile) {
    cerr<<"Can't open "<<filename<<" for input"<<endl;
    return 0;
  }

  iFile>>curword;
  
  iFile>>id;

  newgenome=new Genome(id,iFile);

  iFile.close();

  return newgenome;
}

//Display a graph of a Genome's resultant Network directly from a file
Genome *display_Genome(char *filename, int width, int height) {
  Genome *g;
  Network *n;

  //Get the Genome from the file
  g=new_Genome_load(filename);

  //Generate its network
  n=g->genesis(g->genome_id);
  
  //Graph the network
  n->graph(width,height);

  delete g;
  delete n;

}

int print_Genome_tofile(Genome *g,char *filename) {
  
  ofstream oFile(filename,ios::out);

  //Make sure it worked
  if (!oFile) {
    cerr<<"Can't open "<<filename<<" for output"<<endl;
    return 0;
  }

  g->print_to_file(oFile);

  oFile.close();

  return 1;

}

int print_Genome(Genome *g) {
  cout<<g<<endl;
  return 1;
}

/* Retrns a brand new empty list of innovations (for Guile) */
list<Innovation*> *new_innov_list() {
  list<Innovation*> *il=new list<Innovation*>();

  return il;
}

Population *new_Pop_fromfile(char *filename) {
  Population *newpop;

  newpop=new Population(filename);

  return newpop;;

}

Population *new_Pop_fromGenomefile(char *filename) {
  Population *newpop;
  Genome *spawner;

  ifstream iFile(filename,ios::in);
  
  //Make sure it worked
  if (!iFile) {
    cerr<<"Can't open "<<filename<<" for input"<<endl;
    return 0;
  }

  spawner=new Genome(0,iFile);

  iFile.close();

  newpop=new Population(spawner,NEAT::pop_size);

  delete spawner;

  return newpop;

}

/* --------------------------------------------- */
/* POPULATION RELATED METHODS                    */
/* --------------------------------------------- */

bool Species::add_Organism(Organism *o){
  organisms.push_back(o);
  return true;
}

Organism *Species::get_champ() {
  double champ_fitness=-1.0;
  Organism *thechamp;
  list<Organism*>::iterator curorg;

  for(curorg=organisms.begin();curorg!=organisms.end();++curorg) {
    if (((*curorg)->fitness)>champ_fitness) {
      thechamp=(*curorg);
      champ_fitness=thechamp->fitness;
    }
  }

  return thechamp;

}

//Remove an organism from Species
bool Species::remove_org(Organism *org) {
  list<Organism*>::iterator curorg;

  curorg=organisms.begin();
  while((curorg!=organisms.end())&&
	((*curorg)!=org))
    ++curorg;

  if (curorg==organisms.end()) {
    cout<<"ALERT: Attempt to remove nonexistent Organism from Species"<<endl;
    return false;
  }
  else {
    organisms.erase(curorg);
    return true;
  }
  
}


/* Regenerate the network based on a change
   in the genotype */
void Organism::update_phenotype() {
  
  //First, delete the old phenotype (net)
  delete net;

  //Now, recreate the phenotype off the new genotype
  net=gnome->genesis(gnome->genome_id);

}

Organism *Species::first() {
  return *(organisms.begin());
}

//Debug Population
///Note: This checks each genome by verifying each one
//       Only useful for debugging
bool Population::verify() {
  list<Organism*>::iterator curorg;
  
  bool verification;

  for(curorg=organisms.begin();curorg!=organisms.end();++curorg) {
    verification=((*curorg)->gnome)->verify();
  }

  return verification;
} 

//Create a population of size size off of Genome g
//The new Population will have the same topology as g
//with linkweights slightly perturbed from g's
bool Population::spawn(Genome *g,int size) {
  int count;
  Genome *new_genome;
  Organism *new_organism;

  //Create size copies of the Genome
  //Start with perturbed linkweights
  for(count=1;count<=size;count++) {
    cout<<"CREATING ORGANISM "<<count<<endl;

    new_genome=g->duplicate(count); 
    //new_genome->mutate_link_weights(10.0,1.0,GAUSSIAN);
    new_genome->mutate_link_weights(1.0,1.0,GAUSSIAN);
    new_organism=new Organism(0.0,new_genome,1);
    organisms.push_back(new_organism);
  }

  //Keep a record of the innovation and node number we are on
  cur_node_id=new_genome->get_last_node_id();
  cur_innov_num=new_genome->get_last_gene_innovnum();

  //Separate the new Population into species
  speciate();

  return true;
}

//speciate separates the organisms into species by
//checking compatibilities against a threshold
//Any organism that does is not compatible with the first
//organism in any existing species becomes a new species
bool Population::speciate() {
  list<Organism*>::iterator curorg;  //For stepping through Population
  list<Species*>::iterator curspecies; //Steps through species
  Organism *comporg=0;  //Organism for comparison 
  Species *newspecies; //For adding a new species

  int counter=0; //Species counter

  //Step through all existing organisms
  for(curorg=organisms.begin();curorg!=organisms.end();++curorg) {
    
    //For each organism, search for a species it is compatible to
    curspecies=species.begin();
    if (curspecies==species.end()){
      //Create the first species
      newspecies=new Species(++counter);
      species.push_back(newspecies);
      newspecies->add_Organism(*curorg);  //Add the current organism
      (*curorg)->species=newspecies;  //Point organism to its species
    } 
    else {
      comporg=(*curspecies)->first();
      while((comporg!=0)&&
	    (curspecies!=species.end())) {

	if ((((*curorg)->gnome)->compatibility(comporg->gnome))<compat_threshold) {

	  //Found compatible species, so add this organism to it
	  (*curspecies)->add_Organism(*curorg);
	  (*curorg)->species=(*curspecies);  //Point organism to its species
	  comporg=0;  //Note the search is over
	}
	else {

	  //Keep searching for a matching species
	  ++curspecies;
	  if (curspecies!=species.end()) 
	    comporg=(*curspecies)->first();
	}
      }

      //If we didn't find a match, create a new species
      if (comporg!=0) {
	newspecies=new Species(++counter);
	species.push_back(newspecies);
	newspecies->add_Organism(*curorg);  //Add the current organism
	(*curorg)->species=newspecies;  //Point organism to its species
      }

    } //end else 
    
  } //end for

  last_species=counter;  //Keep track of highest species

  return true;

}

//Print Population to a file specified by a string
bool Population::print_to_file(char *filename){
  list<Organism*>::iterator curorg;

  ofstream outFile(filename,ios::out);

  //Make sure it worked
  if (!outFile) {
    cerr<<"Can't open "<<filename<<" for output"<<endl;
                return false;
  }

  //Print all the Organisms' Genomes to the outFile
  for(curorg=organisms.begin();curorg!=organisms.end();++curorg) {
    ((*curorg)->gnome)->print_to_file(outFile);
    //We can confirm by writing the genome #'s to the screen
    //cout<<((*curorg)->gnome)->genome_id<<endl;
  }

  outFile.close();

  return true;

}
 
//Print Species to a file outFile
bool Species::print_to_file(ofstream &outFile) {
  list<Organism*>::iterator curorg;

  //Print a comment on the Species info
  outFile<<endl<<"/* Species #"<<id<<" : (Size "<<organisms.size()<<") (AF "<<ave_fitness<<") (Age "<<age<<")  */"<<endl<<endl;

  //Show user what's going on
  cout<<endl<<"/* Species #"<<id<<" : (Size "<<organisms.size()<<") (AF "<<ave_fitness<<") (Age "<<age<<")  */"<<endl;

  //Print all the Organisms' Genomes to the outFile
  for(curorg=organisms.begin();curorg!=organisms.end();++curorg) {
    
    //Put the fitness for each organism in a comment
    outFile<<endl<<"/* Organism #"<<((*curorg)->gnome)->genome_id<<" Fitness: "<<(*curorg)->fitness<<" Error: "<<(*curorg)->error<<" */"<<endl;

    //If it is a winner, mark it in a comment
    if ((*curorg)->winner) outFile<<"/* ##------$ WINNER "<<((*curorg)->gnome)->genome_id<<" SPECIES #"<<id<<" $------## */"<<endl;

    ((*curorg)->gnome)->print_to_file(outFile);
    //We can confirm by writing the genome #'s to the screen
    //cout<<((*curorg)->gnome)->genome_id<<endl;
  }

  return true;

}

bool Population::print_to_file_by_species(char *filename) {

  list<Species*>::iterator curspecies;

  ofstream outFile(filename,ios::out);

  //Make sure it worked
  if (!outFile) {
    cerr<<"Can't open "<<filename<<" for output"<<endl;
                return false;
  }


  //Step through the Species and print them to the file
  for(curspecies=species.begin();curspecies!=species.end();++curspecies) {
    (*curspecies)->print_to_file(outFile);
  }

  outFile.close();

  return true;

}

//Can change the fitness of the organisms in the Species to be higher 
//for very new species (to protect them).
//Divides the fitness by the size of the Species, so that fitness is
//"shared" by the species
void Species::adjust_fitness() {
  list<Organism*>::iterator curorg;

  int num_parents;
  int count;

  int age_debt; 

  //cout<<"Species "<<id<<" last improved "<<(age-age_of_last_improvement)<<" steps ago when it moved up to "<<max_fitness_ever<<endl;
       
  age_debt=(age-age_of_last_improvement+1)-dropoff_age;

  if (age_debt==0) age_debt=1;
    
  for(curorg=organisms.begin();curorg!=organisms.end();++curorg) {

    //Remember the original fitness before it gets modified
    (*curorg)->orig_fitness=(*curorg)->fitness;

    //Make fitness decrease after a stagnation point dropoff_age
    //Added an if to keep species pristine until the dropoff point
    if (age_debt>=1) {

      //Possible graded dropoff
      //((*curorg)->fitness)=((*curorg)->fitness)*(-atan(age_debt));
 
      //Extreme penalty for a long period of stagnation (divide fitness by 100)
      ((*curorg)->fitness)=((*curorg)->fitness)*0.01;
      cout<<"dropped fitness to "<<((*curorg)->fitness)<<endl;
    }

    //Give a fitness boost up to some young age (niching)
    //The age_significance parameter is a system parameter
    //  if it is 1, then young species get no fitness boost
    if (age<=10) ((*curorg)->fitness)=((*curorg)->fitness)*age_significance; 

    //Do not allow negative fitness
    if (((*curorg)->fitness)<0.0) (*curorg)->fitness=0.0001; 

    //Share fitness with the species
    (*curorg)->fitness=((*curorg)->fitness)/(organisms.size());

  }

  //Sort the population and mark for death those after survival_thresh*pop_size
  organisms.sort(order_orgs);

  //Update age_of_last_improvement here
  if (((*(organisms.begin()))->orig_fitness)> 
      max_fitness_ever) {
    age_of_last_improvement=age;
    max_fitness_ever=((*(organisms.begin()))->orig_fitness);
  }

  //Decide how many get to reproduce based on survival_thresh*pop_size
  //Adding 1.0 ensures that at least one will survive
  num_parents=(int) floor((survival_thresh*((double) organisms.size()))+1.0);
  
  //Mark for death those who are ranked too low to be parents
  curorg=organisms.begin();
  (*curorg)->champion=true;  //Mark the champ as such
  for(count=1;count<=num_parents;count++) {
    if (curorg!=organisms.end())
      ++curorg;
  }
  while(curorg!=organisms.end()) {
    (*curorg)->eliminate=true;  //Mark for elimination
    ++curorg;
  }             

}

double Species::compute_average_fitness() {
  list<Organism*>::iterator curorg;

  double total=0.0;

  for(curorg=organisms.begin();curorg!=organisms.end();++curorg) {
    total+=(*curorg)->fitness;
  }

  ave_fitness=total/(organisms.size());

  return ave_fitness;

}

double Species::compute_max_fitness() {
  double max=0.0;
  list<Organism*>::iterator curorg;

  for(curorg=organisms.begin();curorg!=organisms.end();++curorg) {
    if (((*curorg)->fitness)>max)
      max=(*curorg)->fitness;
  }

  max_fitness=max;

  return max;
}

//Compute the collective offspring the entire
//species (the sum of all organism's offspring)
//is assigned
//skim is fractional offspring left over from a
//previous species that was counted.
//These fractional parts are kept unil they add
//up to 1
double Species::count_offspring(double skim) {
  list<Organism*>::iterator curorg;
  int e_o_intpart;  //The floor of an organism's expected offspring
  double e_o_fracpart; //Expected offspring fractional part
  double skim_intpart;  //The whole offspring in the skim

  expected_offspring=0;

  for(curorg=organisms.begin();curorg!=organisms.end();++curorg) {
    e_o_intpart=(int) floor((*curorg)->expected_offspring);
    e_o_fracpart=fmod((*curorg)->expected_offspring,1.0);

    expected_offspring+=e_o_intpart;

    //Skim off the fractional offspring
    skim+=e_o_fracpart;

    //NOTE:  Some precision is lost by computer
    //       Must be remedied later
    if (skim>1.0) {
      skim_intpart=floor(skim);
      expected_offspring+=(int) skim_intpart;
      skim-=skim_intpart;
    }
  }

  return skim;
                              
}

//epoch turns over a Population to the next generation based on fitness
bool Population::epoch(int generation) {
  
  list<Species*>::iterator curspecies;
  list<Species*>::iterator deadspecies;  //For removing empty Species

  list<Organism*>::iterator curorg;
  list<Organism*>::iterator deadorg;

  list<Innovation*>::iterator curinnov;  
  list<Innovation*>::iterator deadinnov;  //For removing old Innovs

  double total=0.0; //Used to compute average fitness over all Organisms

  double overall_average;  //The average modified fitness among ALL organisms

  int orgcount;
  
  //The fractional parts of expected offspring that can be 
  //Used only when they accumulate above 1 for the purposes of counting
  //Offspring
  double skim; 
  int total_expected;  //precision checking
  int total_organisms=organisms.size();
  int max_expected;
  Species *best_species;
  int final_expected;

  int pause;

  //Rights to make babies can be stolen from inferior species
  //and given to their superiors, in order to concentrate exploration on
  //the best species
  int NUM_STOLEN=babies_stolen; //Number of babies to steal
  int one_fifth_stolen;
  int one_tenth_stolen;

  list<Species*> sorted_species;  //Species sorted by max fit org in Species
  int stolen_babies; //Babies taken from the bad species and given to the champs

  int half_pop;

  int best_species_num;  //Used in debugging to see why (if) best species dies
  bool best_ok;

  //We can try to keep the number of species constant at this number
  int num_species_target=100;
  int num_species=species.size();
  double compat_mod=0.3;  //Modify compat thresh to control speciation

  //Record what generation we are on
  final_gen=generation;

  /*
  //Keeping species diverse
  //This commented out code forces the system to aim for 
  // num_species species at all times, enforcing diversity
  //This tinkers with the compatibility threshold, which
  // normally would be held constant
  if (generation>1) {
    if (num_species<num_species_target)
      compat_threshold-=compat_mod;
    else if (num_species>num_species_target)
     compat_threshold+=compat_mod;
    
    if (compat_threshold<0.3) compat_threshold=0.3;

  }
  */

  cout<<"Number of Species: "<<num_species<<endl;
  cout<<"compat_thresh: "<<compat_threshold<<endl;

  //Use Species' ages to modify the objective fitness of organisms
  // in other words, make it more fair for younger species
  // so they have a chance to take hold
  //Also penalize stagnant species
  //Then adjust the fitness using the species size to "share" fitness
  //within a species.
  //Then, within each Species, mark for death 
  //those below survival_thresh*average
  for(curspecies=species.begin();curspecies!=species.end();++curspecies) {
    (*curspecies)->adjust_fitness();
  }
  
  //Go through the organisms and add up their fitnesses to compute the
  //overall average
  for(curorg=organisms.begin();curorg!=organisms.end();++curorg) {
    total+=(*curorg)->fitness;
  }
  overall_average=total/total_organisms;
  cout<<"Generation "<<generation<<": "<<"overall_average = "<<overall_average<<endl;

  //Now compute expected number of offspring for each individual organism
  for(curorg=organisms.begin();curorg!=organisms.end();++curorg) {
    (*curorg)->expected_offspring=(((*curorg)->fitness)/overall_average);
  }

  //Now add those offspring up within each Species to get the number of
  //offspring per Species
  skim=0.0;
  total_expected=0;
  for(curspecies=species.begin();curspecies!=species.end();++curspecies) {
    skim=(*curspecies)->count_offspring(skim);
    total_expected+=(*curspecies)->expected_offspring;
  }    

  //Need to make up for lost foating point precision in offspring assignment
  //If we lost precision, give an extra baby to the best Species
  if (total_expected<total_organisms) {
    //Find the Species expecting the most
    max_expected=0;
    final_expected=0;
    for(curspecies=species.begin();curspecies!=species.end();++curspecies) {
      if ((*curspecies)->expected_offspring>=max_expected) {
	max_expected=(*curspecies)->expected_offspring;
	best_species=(*curspecies);
      }
      final_expected+=(*curspecies)->expected_offspring;
    }
    //Give the extra offspring to the best species
    ++(best_species->expected_offspring);
    final_expected++;
  
    //If we still arent at total, there is a problem
    //Note that this can happen if a stagnant Species
    //dominates the population and then gets killed off by its age
    //Then the whole population plummets in fitness
    //If the average fitness is allowed to hit 0, then we no longer have 
    //an average we can use to assign offspring.
    if (final_expected<total_organisms) {
      cout<<"Population died!"<<endl;
      //cin>>pause;
      for(curspecies=species.begin();curspecies!=species.end();++curspecies) {
	(*curspecies)->expected_offspring=0;
      }
      best_species->expected_offspring=total_organisms;
    }
  }
  
  //Stick the Species pointers into a new Species list for sorting
  for(curspecies=species.begin();curspecies!=species.end();++curspecies) {
    sorted_species.push_back(*curspecies);
  }

  //Sort the Species by max fitness (Use an extra list to do this)
  //These need to use ORIGINAL fitness
  sorted_species.sort(order_species);

  best_species_num=(*(sorted_species.begin()))->id;

  for(curspecies=sorted_species.begin();curspecies!=sorted_species.end();++curspecies) {

    //Print out for Debugging/viewing what's going on 
    cout<<"orig fitness of Species"<<(*curspecies)->id<<"(Size "<<(*curspecies)->organisms.size()<<"): "<<(*((*curspecies)->organisms).begin())->orig_fitness<<" last improved "<<((*curspecies)->age-(*curspecies)->age_of_last_improvement)<<endl;
  }

  //Check for Population-level stagnation
  curspecies=sorted_species.begin();
  (*(((*curspecies)->organisms).begin()))->pop_champ=true; //DEBUG marker of the best of pop
  if (((*(((*curspecies)->organisms).begin()))->orig_fitness)>
      highest_fitness) {
    highest_fitness=((*(((*curspecies)->organisms).begin()))->orig_fitness);
    highest_last_changed=0;
    cout<<"NEW POPULATION RECORD FITNESS: "<<highest_fitness<<endl;
  }
  else {
    ++highest_last_changed;
    cout<<highest_last_changed<<" generations since last population fitness record: "<<highest_fitness<<endl;
  }


  //Check for stagnation- if there is stagnation, perform delta-coding
  if (highest_last_changed>=dropoff_age+5) {

    cout<<"PERFORMING DELTA CODING"<<endl;

    highest_last_changed=0;

    half_pop=pop_size/2;
   
    cout<<"half_pop"<<half_pop<<" pop_size-halfpop: "<<pop_size-half_pop<<endl;

    curspecies=sorted_species.begin();

    (*(((*curspecies)->organisms).begin()))->super_champ_offspring=half_pop;
    (*curspecies)->expected_offspring=half_pop;
    (*curspecies)->age_of_last_improvement=(*curspecies)->age;

    ++curspecies;

    if (curspecies!=sorted_species.end()) {

      (*(((*curspecies)->organisms).begin()))->super_champ_offspring=pop_size-half_pop;
      (*curspecies)->expected_offspring=pop_size-half_pop;
      (*curspecies)->age_of_last_improvement=(*curspecies)->age;
      
      ++curspecies;
      
      //Get rid of all species under the first 2
      while(curspecies!=sorted_species.end()) {
	(*curspecies)->expected_offspring=0;
	++curspecies;
      }
    }
    else {
      curspecies=sorted_species.begin();
      (*(((*curspecies)->organisms).begin()))->super_champ_offspring+=pop_size-half_pop;
      (*curspecies)->expected_offspring=pop_size-half_pop;
    }

  }
  //STOLEN BABIES:  The system can take expected offspring away from
  //  worse species and give them to superior species depending on
  //  the system parameter babies_stolen (when babies_stolen > 0)
  else if (babies_stolen>0) {
    //Take away a constant number of expected offspring from the worst few species

    stolen_babies=0;
    curspecies=sorted_species.end();
    curspecies--;
    while ((stolen_babies<NUM_STOLEN)&&
	   (curspecies!=sorted_species.begin())) {
      
      //cout<<"Considering Species "<<(*curspecies)->id<<": age "<<(((*curspecies)->age))<<" expected offspring "<<(((*curspecies)->expected_offspring))<<endl;
      
      if ((((*curspecies)->age)>5)&&
	  (((*curspecies)->expected_offspring)>2)) {
	//cout<<"STEALING!"<<endl;
	
	//This species has enough to finish off the stolen pool
	if (((*curspecies)->expected_offspring-1)>=(NUM_STOLEN-stolen_babies)) {
	  (*curspecies)->expected_offspring-=(NUM_STOLEN-stolen_babies);
	  stolen_babies=NUM_STOLEN;
	}
	//Not enough here to complete the pool of stolen
	else {
	  stolen_babies+=(*curspecies)->expected_offspring-1;
	  (*curspecies)->expected_offspring=1;
	  
	}
      }
      
      curspecies--;

      //if (stolen_babies>0)
	//cout<<"stolen babies so far: "<<stolen_babies<<endl;
    }
    
    //cout<<"STOLEN BABIES: "<<stolen_babies<<endl;
    
    //Mark the best champions of the top species to be the super champs
    //who will take on the extra offspring for cloning or mutant cloning
    curspecies=sorted_species.begin();
 
    //Determine the exact number that will be given to the top three
    //They get , in order, 1/5 1/5 and 1/10 of the stolen babies
    one_fifth_stolen=babies_stolen/5;
    one_tenth_stolen=babies_stolen/10;

    //Don't give to dying species even if they are champs
    while(((*curspecies)->last_improved()>dropoff_age)&&(curspecies!=sorted_species.end()))
      ++curspecies;

    //Concentrate A LOT on the number one species
    if ((stolen_babies>=one_fifth_stolen)&&(curspecies!=sorted_species.end())) {
      (*(((*curspecies)->organisms).begin()))->super_champ_offspring=one_fifth_stolen;
      (*curspecies)->expected_offspring+=one_fifth_stolen;
      stolen_babies-=one_fifth_stolen;
      //cout<<"Gave "<<one_fifth_stolen<<" babies to Species "<<(*curspecies)->id<<endl;
      cout<<"The best superchamp is "<<(*(((*curspecies)->organisms).begin()))->gnome->genome_id<<endl;

      //Print this champ to file "champ" for observation if desired
      //IMPORTANT:  This causes generational file output 
      //print_Genome_tofile((*(((*curspecies)->organisms).begin()))->gnome,"champ");

      curspecies++;

    }
    
    //Don't give to dying species even if they are champs
    while(((*curspecies)->last_improved()>dropoff_age)&&(curspecies!=sorted_species.end()))
      ++curspecies;

    if ((curspecies!=sorted_species.end())) {
      if (stolen_babies>=one_fifth_stolen) {
	(*(((*curspecies)->organisms).begin()))->super_champ_offspring=one_fifth_stolen;
	(*curspecies)->expected_offspring+=one_fifth_stolen;
	stolen_babies-=one_fifth_stolen;
	//cout<<"Gave "<<one_fifth_stolen<<" babies to Species "<<(*curspecies)->id<<endl;
	curspecies++;
	
      }
    }

    //Don't give to dying species even if they are champs
    while(((*curspecies)->last_improved()>dropoff_age)&&(curspecies!=sorted_species.end()))
      ++curspecies;
    
    if (curspecies!=sorted_species.end())
      if (stolen_babies>=one_tenth_stolen) {
	(*(((*curspecies)->organisms).begin()))->super_champ_offspring=one_tenth_stolen;
	(*curspecies)->expected_offspring+=one_tenth_stolen;
	stolen_babies-=one_tenth_stolen;
	
	//cout<<"Gave "<<one_tenth_stolen<<" babies to Species "<<(*curspecies)->id<<endl;
	curspecies++;
	
      }
    
    //Don't give to dying species even if they are champs
    while(((*curspecies)->last_improved()>dropoff_age)&&(curspecies!=sorted_species.end()))
      ++curspecies;

    while((stolen_babies>0)&&
	  (curspecies!=sorted_species.end())) {
      //Randomize a little which species get boosted by a super champ
      
      if (randfloat()>0.1)
	if (stolen_babies>3) {
	  (*(((*curspecies)->organisms).begin()))->super_champ_offspring=3;
	  (*curspecies)->expected_offspring+=3;
	  stolen_babies-=3;
	  //cout<<"Gave 3 babies to Species "<<(*curspecies)->id<<endl;
	}
	else {
	  //cout<<"3 or less babies available"<<endl;
	  (*(((*curspecies)->organisms).begin()))->super_champ_offspring=stolen_babies;
	  (*curspecies)->expected_offspring+=stolen_babies;
	  //cout<<"Gave "<<stolen_babies<<" babies to Species "<<(*curspecies)->id<<endl;
	  stolen_babies=0;
	  
	}
      
      curspecies++;

      //Don't give to dying species even if they are champs
      while((curspecies!=sorted_species.end())&&((*curspecies)->last_improved()>dropoff_age))
	++curspecies;
      
    }
    
    //cout<<"Done giving back babies"<<endl;

    //If any stolen babies aren't taken, give them to species #1's champ
    if (stolen_babies>0) {
      
      //cout<<"Not all given back, giving to best Species"<<endl;
      
      curspecies=sorted_species.begin();
      (*(((*curspecies)->organisms).begin()))->super_champ_offspring+=stolen_babies;
      (*curspecies)->expected_offspring+=stolen_babies;
      stolen_babies=0;
    }
    
  }

  //Kill off all Organisms marked for death.  The remainder
  //will be allowed to reproduce.
  curorg=organisms.begin();
  while(curorg!=organisms.end()) {
    if (((*curorg)->eliminate)) {
      //Remove the organism from its Species
      ((*curorg)->species)->remove_org(*curorg);

      //Delete the organism from memory
      delete (*curorg);

      //Remember where we are
      deadorg=curorg;
      ++curorg;

      //Remove the organism from the master list
      organisms.erase(deadorg);
     
    }
    else {
      ++curorg;
    }

  }

  cout<<"Reproducing"<<endl;

  //Perform reproduction.  Reproduction is done on a per-Species
  //basis.  (So this could be paralellized potentially.)
  for(curspecies=species.begin();curspecies!=species.end();++curspecies) {
    (*curspecies)->reproduce(generation,this,sorted_species);
  }    

  cout<<"Reproduction Complete"<<endl;

  //Destroy and remove the old generation from the organisms and species  
  curorg=organisms.begin();
  while(curorg!=organisms.end()) {

    //Remove the organism from its Species
    ((*curorg)->species)->remove_org(*curorg);

    //Delete the organism from memory
    delete (*curorg);
    
    //Remember where we are
    deadorg=curorg;
    ++curorg;
    
    //Remove the organism from the master list
    organisms.erase(deadorg);
    
  }

  //Remove all empty Species and age ones that survive
  //As this happens, create master organism list for the new generation
  curspecies=species.begin();
  orgcount=0;
  while(curspecies!=species.end()) {
    if (((*curspecies)->organisms.size())==0) {
      delete (*curspecies);

      deadspecies=curspecies;
      ++curspecies;

      species.erase(deadspecies);
    }
    //Age surviving Species and 
    //Rebuild master Organism list: NUMBER THEM as they are added to the list
    else {
      //Age any Species that is not newly created in this generation
      if ((*curspecies)->novel) {
	(*curspecies)->novel=false;
      }
      else ++((*curspecies)->age);

      //Go through the organisms of the curspecies and add them to 
      //the master list
      for(curorg=((*curspecies)->organisms).begin();curorg!=((*curspecies)->organisms).end();++curorg) {
	((*curorg)->gnome)->genome_id=orgcount++;
	organisms.push_back(*curorg);
      }
      ++curspecies;

    }
  }      

  //Remove the innovations of the current generation
  curinnov=innovations.begin();
  while(curinnov!=innovations.end()) {
    delete (*curinnov);

    deadinnov=curinnov;
    ++curinnov;

    innovations.erase(deadinnov);
  }

  //DEBUG: Check to see if the best species died somehow
  // We don't want this to happen
  curspecies=species.begin();
  best_ok=false;
  while(curspecies!=species.end()) {
    if (((*curspecies)->id)==best_species_num) best_ok=true;
    ++curspecies;
  }
  if (!best_ok) {
    cout<<"ERROR: THE BEST SPECIES DIED!"<<endl;
  }
  else {
    cout<<"The best survived: "<<best_species_num<<endl;
  }

  //DEBUG: Checking the top organism's duplicate in the next gen
  //This prints the champ's child to the screen
  for(curorg=organisms.begin();curorg!=organisms.end();++curorg) {
    if ((*curorg)->pop_champ_child) {
      cout<<"At end of reproduction cycle, the child of the pop champ is: "<<(*curorg)->gnome<<endl;
    }
  }

  cout<<"babies_stolen at end: "<<babies_stolen<<endl;
  
  cout<<"Epoch complete"<<endl; 

  return true;
  
}

//Perform mating and mutation to form next generation
bool Species::reproduce(int generation, Population *pop,list<Species*> &sorted_species) {
  int count;
  list<Organism*>::iterator curorg;

  int poolsize;  //The number of Organisms in the old generation

  int orgnum;  //Random variable
  int orgcount;
  Organism *mom; //Parent Organisms
  Organism *dad;
  Organism *baby;  //The new Organism

  Genome *new_genome;  //For holding baby's genes

  list<Species*>::iterator curspecies;  //For adding baby
  Species *newspecies; //For babies in new Species
  Organism *comporg;  //For Species determination through comparison

  Species *randspecies;  //For mating outside the Species
  double randmult;
  int randspeciesnum;
  int spcount;  
  list<Species*>::iterator cursp;

  Network *net_analogue;  //For adding link to test for recurrency
  int pause;

  bool outside;

  bool found;  //When a Species is found

  bool champ_done=false; //Flag the preservation of the champion  

  Organism *thechamp;

  int giveup; //For giving up finding a mate outside the species

  bool mut_struct_baby;
  bool mate_baby;

  //The weight mutation power is species specific depending on its age
  double mut_power=weight_mut_power;

  //Roulette wheel variables
  double total_fitness=0.0;
  double marble;  //The marble will have a number between 0 and total_fitness
  double spin;  //Fitness total while the wheel is spinning

  //Compute total fitness of species for a roulette wheel
  //Note: You don't get much advantage from a roulette here
  // because the size of a species is relatively small.
  // But you can use it by using the roulette code here
  //for(curorg=organisms.begin();curorg!=organisms.end();++curorg) {
  //  total_fitness+=(*curorg)->fitness;
  //}

  //Check for a mistake
  if ((expected_offspring>0)&&
      (organisms.size()==0)) {
    cout<<"ERROR:  ATTEMPT TO REPRODUCE OUT OF EMPTY SPECIES"<<endl;
    return false;
  }

  poolsize=organisms.size()-1;

  thechamp=(*(organisms.begin()));

  //Create the designated number of offspring for the Species
  //one at a time
  for (count=0;count<expected_offspring;count++) {

    mut_struct_baby=false;
    mate_baby=false;

    outside=false;

    //Debug Trap
    if (expected_offspring>pop_size) {
      cout<<"ALERT: EXPECTED OFFSPRING = "<<expected_offspring<<endl;
      cin>>pause;
    }

    //If we have a super_champ (Population champion), finish off some special clones
    if ((thechamp->super_champ_offspring) > 0) {
      mom=thechamp;
      new_genome=(mom->gnome)->duplicate(count);

      if ((thechamp->super_champ_offspring) == 1) {

      }

      //Most superchamp offspring will have their connection weights mutated only
      //The last offspring will be an exact duplicate of this super_champ
      //Note: Superchamp offspring only occur with stolen babies!
      //      Settings used for published experiments did not use this
      if ((thechamp->super_champ_offspring) > 1) {
	if ((randfloat()<0.8)||
	    (mutate_add_link_prob==0.0)) 
	  //ABOVE LINE IS FOR:
	  //Make sure no links get added when the system has link adding disabled
	  new_genome->mutate_link_weights(mut_power,1.0,GAUSSIAN);
	else {
	  //Sometimes we add a link to a superchamp
	  net_analogue=new_genome->genesis(generation);
	  new_genome->mutate_add_link(pop->innovations,pop->cur_innov_num,newlink_tries);
	  delete net_analogue;
	  mut_struct_baby=true;
	}
      }

      baby=new Organism(0.0,new_genome,generation);
      
      if ((thechamp->super_champ_offspring) == 1) {
	if (thechamp->pop_champ) {
	  //cout<<"The new org baby's genome is "<<baby->gnome<<endl;
	  baby->pop_champ_child=true;
	  baby->high_fit=mom->orig_fitness;
	}
      }

      thechamp->super_champ_offspring--;
    }
    //If we have a Species champion, just clone it 
    else if ((!champ_done)&&
	(expected_offspring>5)) {

      mom=thechamp; //Mom is the champ

      new_genome=(mom->gnome)->duplicate(count);

      baby=new Organism(0.0,new_genome,generation);  //Baby is just like mommy

      champ_done=true;

    }
    //First, decide whether to mate or mutate
    //If there is only one organism in the pool, then always mutate
    else if ((randfloat()<mutate_only_prob)||
	poolsize== 0) {

      //Choose the random parent

      //RANDOM PARENT CHOOSER
      orgnum=randint(0,poolsize);
      curorg=organisms.begin();
      for(orgcount=0;orgcount<orgnum;orgcount++)
	++curorg;                       


      /*
      //Roulette Wheel
      marble=randfloat()*total_fitness;
      curorg=organisms.begin();
      spin=(*curorg)->fitness;
      while(spin<marble) {
	++curorg;
	
	//Keep the wheel spinning
	spin+=(*curorg)->fitness;
      }
      //Finished roulette
      */

      mom=(*curorg);

      new_genome=(mom->gnome)->duplicate(count);

      //Do the mutation depending on probabilities of 
      //various mutations

      if (randfloat()<mutate_add_node_prob) {
	//cout<<"mutate add node"<<endl;
	new_genome->mutate_add_node(pop->innovations,pop->cur_node_id,pop->cur_innov_num);
	mut_struct_baby=true;
      }
      else if (randfloat()<mutate_add_link_prob) {
	//cout<<"mutate add link"<<endl;
	net_analogue=new_genome->genesis(generation);
	new_genome->mutate_add_link(pop->innovations,pop->cur_innov_num,newlink_tries);
	delete net_analogue;
	mut_struct_baby=true;
      }
      //NOTE:  A link CANNOT be added directly after a node was added because the phenotype
      //       will not be appropriately altered to reflect the change
      else {
	//If we didn't do a structural mutation, we do the other kinds

	if (randfloat()<mutate_random_trait_prob) {
	  //cout<<"mutate random trait"<<endl;
	  new_genome->mutate_random_trait();
	}
	if (randfloat()<mutate_link_trait_prob) {
	  //cout<<"mutate_link_trait"<<endl;
	  new_genome->mutate_link_trait(1);
	}
	if (randfloat()<mutate_node_trait_prob) {
	  //cout<<"mutate_node_trait"<<endl;
	  new_genome->mutate_node_trait(1);
	}
	if (randfloat()<mutate_link_weights_prob) {
	  //cout<<"mutate_link_weights"<<endl;
	  new_genome->mutate_link_weights(mut_power,1.0,GAUSSIAN);
	}
	if (randfloat()<mutate_toggle_enable_prob) {
	  //cout<<"mutate toggle enable"<<endl;
	  new_genome->mutate_toggle_enable(1);
	  
	}
	if (randfloat()<mutate_gene_reenable_prob) {
	  //cout<<"mutate gene reenable"<<endl;
	  new_genome->mutate_gene_reenable();
	}
      }

      baby=new Organism(0.0,new_genome,generation);
 
    }

    //Otherwise we should mate 
    else {

      //Choose the random mom
      orgnum=randint(0,poolsize);
      curorg=organisms.begin();
      for(orgcount=0;orgcount<orgnum;orgcount++)
        ++curorg;
      
      /*
      //Roulette Wheel
      marble=randfloat()*total_fitness;
      curorg=organisms.begin();
      spin=(*curorg)->fitness;
      while(spin<marble) {
	++curorg;

	//Keep the wheel spinning
	spin+=(*curorg)->fitness;
      }
      //Finished roulette
      */

      mom=(*curorg);         

      //Choose random dad

      if ((randfloat()>interspecies_mate_rate)) {
	//Mate within Species

	orgnum=randint(0,poolsize);
	curorg=organisms.begin();
	for(orgcount=0;orgcount<orgnum;orgcount++)
	  ++curorg;
	
	/*
	//Use a roulette wheel
	marble=randfloat()*total_fitness;
	curorg=organisms.begin();
	spin=(*curorg)->fitness;
	while(spin<marble) {
	  ++curorg;
	  }
	  
	  //Keep the wheel spinning
	  spin+=(*curorg)->fitness;
	}
	//Finished roulette
	*/	

	dad=(*curorg);
      }
      else {
	//Mate outside Species  
	randspecies=this;

	//Select a random species
	giveup=0;  //Give up if you cant find a different Species
	while((randspecies==this)&&(giveup<5)) {
	  
	  //This old way just chose any old species
	  //randspeciesnum=randint(0,(pop->species).size()-1);
	
	  //Choose a random species tending towards better species
	  randmult=gaussrand()/4;
	  if (randmult>1.0) randmult=1.0;
	  //This tends to select better species
	  randspeciesnum=(int) floor((randmult*(sorted_species.size()-1.0))+0.5);
	  cursp=(sorted_species.begin());
	  for(spcount=0;spcount<randspeciesnum;spcount++)
	    ++cursp;
	  randspecies=(*cursp);

	  ++giveup;
	}
	
	//OLD WAY: Choose a random dad from the random species
	//Select a random dad from the random Species
	//NOTE:  It is possible that a mating could take place
	//       here between the mom and a baby from the NEW
	//       generation in some other Species
	//orgnum=randint(0,(randspecies->organisms).size()-1);
        //curorg=(randspecies->organisms).begin();
        //for(orgcount=0;orgcount<orgnum;orgcount++)
        //  ++curorg;
        //dad=(*curorg);            

	//New way: Make dad be a champ from the random species
	dad=(*((randspecies->organisms).begin()));

	outside=true;
	//cout<<"outside species"<<endl;
      }

      //Perform mating based on probabilities of differrent mating types
      if (randfloat()<mate_multipoint_prob) { 
	new_genome=(mom->gnome)->mate_multipoint(dad->gnome,count,mom->orig_fitness,dad->orig_fitness);
	//cout<<"mate multipoint baby: "<<new_genome<<endl;
      }
      else if (randfloat()<(mate_multipoint_avg_prob/(mate_multipoint_avg_prob+mate_singlepoint_prob))) {
	new_genome=(mom->gnome)->mate_multipoint_avg(dad->gnome,count,mom->orig_fitness,dad->orig_fitness);
	//cout<<"mate multipoint_avg baby: "<<new_genome<<endl;
      }
      else {
	new_genome=(mom->gnome)->mate_singlepoint(dad->gnome,count);
      }

      mate_baby=true;

      //Determine whether to mutate the baby's Genome
      //This is done randomly or if the mom and dad are the same organism
      if ((randfloat()>mate_only_prob)||
	  ((dad->gnome)->genome_id==(mom->gnome)->genome_id)||
	  (((dad->gnome)->compatibility(mom->gnome))==0.0))
	{

	//Do the mutation depending on probabilities of 
	//various mutations
	if (randfloat()<mutate_add_node_prob) {
	  new_genome->mutate_add_node(pop->innovations,pop->cur_node_id,pop->cur_innov_num);
	  //cout<<"mutate_add_node: "<<new_genome<<endl;
	  mut_struct_baby=true;
	}
	else if (randfloat()<mutate_add_link_prob) {
	  net_analogue=new_genome->genesis(generation);
	  new_genome->mutate_add_link(pop->innovations,pop->cur_innov_num,newlink_tries);
	  delete net_analogue;
	  //cout<<"mutate_add_link: "<<new_genome<<endl;
	  mut_struct_baby=true;
	}
	else {
	  //Only do other mutations when not doing sturctural mutations

	  if (randfloat()<mutate_random_trait_prob) {
	    new_genome->mutate_random_trait();
	    //cout<<"..mutate random trait: "<<new_genome<<endl;
	  }
	  if (randfloat()<mutate_link_trait_prob) {
	    new_genome->mutate_link_trait(1);
	    //cout<<"..mutate link trait: "<<new_genome<<endl;
	  }
	  if (randfloat()<mutate_node_trait_prob) {
	    new_genome->mutate_node_trait(1);
	    //cout<<"mutate_node_trait: "<<new_genome<<endl;
	  }
	  if (randfloat()<mutate_link_weights_prob) {
	    new_genome->mutate_link_weights(mut_power,1.0,GAUSSIAN);
	    //cout<<"mutate_link_weights: "<<new_genome<<endl;
	  }
	  if (randfloat()<mutate_toggle_enable_prob) {
	    new_genome->mutate_toggle_enable(1);
	    //cout<<"mutate_toggle_enable: "<<new_genome<<endl;
	  }
	  if (randfloat()<mutate_gene_reenable_prob) {
	    new_genome->mutate_gene_reenable(); 
	    //cout<<"mutate_gene_reenable: "<<new_genome<<endl;
	  }
	}

	//Create the baby
	baby=new Organism(0.0,new_genome,generation);

      }
      else {
	//Create the baby without mutating first
	baby=new Organism(0.0,new_genome,generation);
      }
      
    }
    
    //Add the baby to its proper Species
    //If it doesn't fit a Species, create a new one

    baby->mut_struct_baby=mut_struct_baby;
    baby->mate_baby=mate_baby;

    curspecies=(pop->species).begin();
    if (curspecies==(pop->species).end()){
      //Create the first species
      newspecies=new Species(++(pop->last_species),true);
      (pop->species).push_back(newspecies);
      newspecies->add_Organism(baby);  //Add the baby
      baby->species=newspecies;  //Point the baby to its species
    } 
    else {
      comporg=(*curspecies)->first();
      found=false;
      while((curspecies!=(pop->species).end())&&
	    (!found)) {	
	if (comporg==0) {
	  //Keep searching for a matching species
          ++curspecies;
          if (curspecies!=(pop->species).end())
            comporg=(*curspecies)->first();
	}
	else if (((baby->gnome)->compatibility(comporg->gnome))<compat_threshold) {
	  //Found compatible species, so add this organism to it
	  (*curspecies)->add_Organism(baby);
	  baby->species=(*curspecies);  //Point organism to its species
	  found=true;  //Note the search is over
	}
	else {
	  //Keep searching for a matching species
	  ++curspecies;
	  if (curspecies!=(pop->species).end()) 
	    comporg=(*curspecies)->first();
	}
      }
      
      //If we didn't find a match, create a new species
      if (found==false) {
	newspecies=new Species(++(pop->last_species),true);
	(pop->species).push_back(newspecies);
	newspecies->add_Organism(baby);  //Add the baby
	baby->species=newspecies;  //Point baby to its species
      }

    } //end else 
  
  }

  return true;
}

//Take a snapshot of the current generation for visualization purpose
void Population::snapshot() {
  Generation_viz *thisgen=new Generation_viz;
  list<Species*>::iterator curspecies;
  Species_viz *curviz;

  //Add each Species in this generation to the snapshot
  for(curspecies=species.begin();curspecies!=species.end();++curspecies) {
    curviz=new Species_viz((*curspecies));
    (thisgen->species_list).push_back(curviz);
  }

  //Add the snapshot to the Population's entire list of snapshots
  generation_snapshots.push_back(thisgen);
  
}


//Visualize the Population's Speciation using a graphic depiction
//width and height denote the size of the window
void Population::visualize(int width,int height,int start_gen,int stop_gen) {
  list<Generation_viz*>::iterator curgen;
  list<Species_viz*>::iterator curspec;
  int spaces; //Number of blank spaces in the current row
  int xpixels;
  int ypixels;
  double popsize=(double) organisms.size();
  double species_pixels; //Number pf pixels for current species
  int xpos; 
  int ypos;
  int space=2;
  PopulationWindow *window;
  double fitness_total=0.0; //For averaging
  double total_species=0.0;
  double variance_total=0.0;  //Intermediate part of variance calculation  
  bool y_for_gens;  //Marks which axis should be used for time

  //Don't allow illegal values
  if ((start_gen<stop_gen)&&
      (start_gen>0)&&
      (stop_gen<=(generation_snapshots.size()))) {

  //Make a decision about which axis to use for time (generations)
  if (width>height) y_for_gens=false;
  else y_for_gens=true;

  //First compute some statistics to help in visualization
  //Get the mean max_fitness of all species at all generations
  for(curgen=generation_snapshots.begin();curgen!=generation_snapshots.end();++curgen) {

    for(curspec=((*curgen)->species_list).begin();curspec!=((*curgen)->species_list).end();++curspec) {
      fitness_total+=(*curspec)->max_fitness;
      total_species+=1.0;
    }    
  }
  mean_fitness=fitness_total/total_species;

  cout<<"Got mean fitness: "<<mean_fitness<<endl;

  //Get the standard deviation
  for(curgen=generation_snapshots.begin();curgen!=generation_snapshots.end();++curgen) {

    for(curspec=((*curgen)->species_list).begin();curspec!=((*curgen)->species_list).end();++curspec) {
      variance_total+=pow((((*curspec)->max_fitness)-mean_fitness),2);
    }
  }
  variance=variance_total/(total_species-1.0);
  standard_deviation=sqrt(total_species*variance/(total_species-1.0));
  
  cout<<"Got mean standard deviation: "<<standard_deviation<<endl;

  //Go through each generation
  for(curgen=generation_snapshots.begin();curgen!=generation_snapshots.end();++curgen) {
    //Within each generation, compute the x values for the Species

    //First we see how many blank spaces (Size 3 pixels) we will have
    spaces=((*curgen)->species_list).size()-1;
    
    //There are 25 pixels given for printing a gen_num on left, space 3 on right
    xpixels=width-23-spaces*space;  //6 is the borders, pixels for each space
    ypixels=height-23-spaces*space;

    xpos=25;  //Starting xpos
    ypos=height-25;  //Starting ypos if we were orienting along y

    //Now go through the Species snapshots themselves
    for(curspec=((*curgen)->species_list).begin();curspec!=((*curgen)->species_list).end();++curspec) {
      //Compute the # of pixels this Species will comprise
      if (y_for_gens)
	species_pixels=(((double) (*curspec)->size)/popsize)*((double) xpixels);
      else species_pixels=(((double) (*curspec)->size)/popsize)*((double) ypixels);
      species_pixels=floor (species_pixels+0.5);
      
      if (y_for_gens) {
	(*curspec)->startx=xpos;
	xpos+=(int) species_pixels;
	(*curspec)->endx=xpos;
	xpos+=space;  //Put a potential space between each Species
      }
      else { 
	(*curspec)->starty=ypos;
	ypos-=(int) species_pixels;
	(*curspec)->endy=ypos;
	ypos-=space;  //Put a potential space between each Species
      }

      
    }

  }

  //Now pop up the window and draw the snapshot
  window = new PopulationWindow(this,width,height,start_gen,stop_gen);
  window->show();

  myapp->run();

  }
  else {
    cout<<"Sorry, the generations you asked for cannot be printed."<<endl;
    cout<<"Please check the values to make sure they are valid."<<endl;
  }


}







