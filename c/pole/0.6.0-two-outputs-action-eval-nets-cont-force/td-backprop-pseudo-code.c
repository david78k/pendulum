/************************************************************************

Nonlinear TD/Backprop pseudo C-code

Written by Allen Bonde Jr. and Rich Sutton in April 1992. 
Updated in June and August 1993. 
Copyright 1993 GTE Laboratories Incorporated. All rights reserved. 
Permission is granted to make copies and changes, with attribution,
for research and educational purposes.

This pseudo-code implements a fully-connected two-adaptive-layer network
learning to predict discounted cumulative outcomes through Temporal
Difference learning, as described in Sutton (1988), Barto et al. (1983),
Tesauro (1992), Anderson (1986), Lin (1992), Dayan (1992), et alia. This
is a straightforward combination of discounted TD(lambda) with
backpropagation (Rumelhart, Hinton, and Williams, 1986). This is vanilla
backprop; not even momentum is used. See Sutton & Whitehead (1993) for
an argument that backprop is not the best structural credit assignment
method to use in conjunction with TD. Discounting can be eliminated for
absorbing problems by setting GAMMA=1. Eligibility traces can be
eliminated by setting LAMBDA=0. Setting both of these parameters to 0
should give standard backprop except where the input at time t has its
target presented at time t+1. 

This is pseudo code: before it can be run it needs I/O, a random
number generator, library links, and some declarations.  We welcome
extensions by others converting this to immediately usable C code.
 
The network is structured using simple array data structures as follows:


                    OUTPUT
   
                  ()  ()  ()  y[k]
                 /  \/  \/  \      k=0...m-1
     ew[j][k]   /   w[j][k]  \
               /              \
              ()  ()  ()  ()  ()  h[j]
               \              /        j=0...num_hidden
   ev[i][j][k]  \   v[i][j]  /
                 \  /\  /\  /
                  ()  ()  ()  x[i]
                                   i=0...n
                     INPUT


where x, h, and y are (arrays holding) the activity levels of the input,
hidden, and output units respectively, v and w are the first and second
layer weights, and ev and ew are the eligibility traces for the first
and second layers (see Sutton, 1989). Not explicitly shown in the figure
are the biases or threshold weights. The first layer bias is provided by
a dummy nth input unit, and the second layer bias is provided by a dummy
(num-hidden)th hidden unit. The activities of both of these dummy units
are held at a constant value (BIAS).

In addition to the main program, this file contains 4 routines:

    InitNetwork, which initializes the network data structures.

    Response, which does the forward propagation, the computation of all 
        unit activities based on the current input and weights.

    TDlearn, which does the backpropagation of the TD errors, and updates
        the weights.

    UpdateElig, which updates the eligibility traces.

These routines do all their communication through the global variables
shown in the diagram above, plus old_y, an array holding a copy of the
last time step's output-layer activities.

For simplicity, all the array dimensions are specified as MAX_UNITS, the 
maximum allowed number of units in any layer.  This could of course be
tightened up if memory becomes a problem.

REFERENCES

Anderson, C.W. (1986) Learning and Problem Solving with Multilayer
Connectionist Systems, UMass. PhD dissertation, dept. of Computer and
Information Science, Amherts, MA 01003.

Barto, A.G., Sutton, R.S., & Anderson, C.W. (1983) "Neuron-like adaptive
elements that can solve difficult learning control problems," IEEE
Transactions on Systems, Man, and Cybernetics SMC-13: 834-846.

Dayan, P. "The convergence of TD(lambda) for general lambda,"
Machine Learning 8: 341-362.

Lin, L.-J. (1992) "Self-improving reactive agents based on reinforcement
learning, planning and teaching," Machine Learning 8: 293-322.

Rumelhart, D.E., Hinton, G.E., & Williams, R.J. (1986) "Learning
internal representations by error propagation," in D.E. Rumehart & J.L.
McClelland (Eds.), Parallel Distributed Processing: Explorations in the
Microstructure of Cognition, Volume 1: Foundations, 318-362. Cambridge,
MA: MIT Press.

Sutton, R.S. (1988) "Learning to predict by the methods of temporal
differences," Machine Learning 3: 9-44.

Sutton, R.S. (1989) "Implementation details of the TD(lambda) procedure
for the case of vector predictions and backpropagation," GTE
Laboratories Technical Note TN87-509.1, May 1987, corrected August 1989.
Available via ftp from ftp.gte.com as 
/pub/reinforcement-learning/sutton-TD-backprop.ps

Sutton, R.S., Whitehead, S.W. (1993) "Online learning with random
representations," Proceedings of the Tenth National Conference on
Machine Learning, 314-321. Soon to be available via ftp from ftp.gte.com
as /pub/reinforcement-learning/sutton-whitehead-93.ps.Z

Tesauro, G. (1992) "Practical issues in temporal difference learning,"
Machine Learning 8: 257-278.
************************************************************************/

/* Experimental Parameters: */

int    n, num_hidden, m; /* number of inputs, hidden, and output units */
int    MAX_UNITS;  /* maximum total number of units (to set array sizes) */
int    time_steps;  /* number of time steps to simulate */
float  BIAS;   /* strength of the bias (constant input) contribution */
float  ALPHA;  /* 1st layer learning rate (typically 1/n) */
float  BETA;   /* 2nd layer learning rate (typically 1/num_hidden) */
float  GAMMA;  /* discount-rate parameter (typically 0.9) */
float  LAMBDA; /* trace decay parameter (should be <= gamma) */

/* Network Data Structure: */

float  x[time_steps][MAX_UNITS]; /* input data (units) */
float  h[MAX_UNITS]; /* hidden layer */
float  y[MAX_UNITS]; /* output layer */
float  w[MAX_UNITS][MAX_UNITS]; /* weights */

/* Learning Data Structure: */

float  old_y[MAX_UNITS];
float  ev[MAX_UNITS][MAX_UNITS][MAX_UNITS]; /* hidden trace */
float  ew[MAX_UNITS][MAX_UNITS]; /* output trace */
float  r[time_steps][MAX_UNITS]; /* reward */
float  error[MAX_UNITS];  /* TD error */
int    t;  /* current time step */

main()

{
int k;
InitNetwork();

t=0;                   /* No learning on time step 0 */
Response();            /* Just compute old response (old_y)...*/
for (k=0;k<m;k++)
   old_y[k] = y[k];
UpdateElig();          /* ...and prepare the eligibilities */

for (t=1;t<=time_steps;t++)  /* a single pass through time series data */
  {
   Response();         /* forward pass - compute activities */
   for (k=0;k<m;k++)
     error[k] = r[t][k] + GAMMA*y[k] - old_y[k]; /* form errors */
   TDlearn();          /* backward pass - learning */
   Response();         /* forward pass must be done twice to form TD errors */
   for (k=0;k<m;k++)
     old_y[k] = y[k];  /* for use in next cycle's TD errors */
   UpdateElig();       /* update eligibility traces */
  } /* end t */
} /* end main */

/*****
 * InitNetwork()
 *
 * Initialize weights and biases
 *
 *****/

InitNetwork(void)

{
int s,j,k,i;

for (s=0;s<time_steps;s++)
  x[s][n]=BIAS;
h[num_hidden]=BIAS;
for (j=0;j<=num_hidden;j++)
  {
  for (k=0;k<m;k++)
    {
    w[j][k]= some small random value
    ew[i][k]=0.0;
    old_y[k]=0.0;
    }
  for (i=0;i<=n;i++)
    {
    v[i][j]= some small random value
    for (k=0;k<m;k++)
      {
      ev[i][j][k]=0.0;
      }
    }
  }
}/* end InitNetwork */

/*****
 * Response()
 *
 * Compute hidden layer and output predictions
 *
 *****/

Response(void)

{
int i,j,k;

h[num_hidden]=BIAS;
x[t][n]=BIAS;

for (j=0;j<num_hidden;j++)
  {
  h[j]=0.0;
  for (i=0;i<=n;i++)
    {
    h[j]+=x[t][i]*v[i][j];
    }
  h[j]=1.0/(1.0+exp(-h[j])); /* asymmetric sigmoid */
  }
for (k=0;k<m;k++)
  {
  y[k]=0.0;
  for (j=0;j<=num_hidden;j++)
    {
    y[k]+=h[j]*w[j][k];
    }
  y[k]=1.0/(1.0+exp(-y[k])); /* asymmetric sigmoid (OPTIONAL) */
  }
}/* end Response */

/*****
 * TDlearn()
 *
 * Update weight vectors
 *
 *****/

TDlearn(void)

{
int i,j,k;

for (k=0;k<m;k++)
  {
  for (j=0;j<=num_hidden;j++)
    {
    w[j][k]+=BETA*error[k]*ew[j][k];
    for (i=0;i<=n;i++)
      v[i][j]+=ALPHA*error[k]*ev[i][j][k];
    }
  }
}/* end TDlearn */

/*****
 * UpdateElig()
 *
 * Calculate new weight eligibilities
 *
 *****/

UpdateElig(void)

{
int i,j,k;
float temp[MAX_UNITS];

for (k=0;k<m;k++)
  temp[k]=y[k]*(1-y[k]);

for (j=0;j<=num_hidden;j++)
  {
  for (k=0;k<m;k++)
    {
    ew[j][k]=LAMBDA*ew[j][k]+temp[k]*h[j];
    for (i=0;i<=n;i++)
      {
      ev[i][j][k]=LAMBDA*ev[i][j][k]+temp[k]*w[j][k]*h[j]*(1-h[j])*x[t][i];
      }
    }
  }
}/* end UpdateElig */

