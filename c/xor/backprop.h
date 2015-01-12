/*
MLP neural network in Java
by Phil Brierley
www.philbrierley.com

This code may be freely used and modified at will

Tanh hidden neurons
Linear output neuron

To include an input bias create an
extra input in the training data
and set to 1

Routines included:

forward()
WeightChangesHO()
WeightChangesIH()
initWeights()
initData()
tanh(double x)
displayResults()
calcOverallError()

compiled and tested on
Symantec Cafe Lite

*/

#include <stdio.h>
#include <math.h>

#define NPATTERNS 6
#define NHIDDEN 16

#define randomdef                  ((float) random() / (float)((1 << 31) - 1))

double forward(double state[]);
void backprop(double push, double target);
void WeightChangesHO(double error);
void WeightChangesIH(double error);
void initWeights();
void initData();
double sigmoid(double x);
double tanh(double x);
//void displayResults();
void testAll();
void calcOverallError();

//training data
// dist, state | L, R, state, delay, sense?
// 0.3m, F     | 
/*
double trainInputs[4][2] = {
	{0, 0},
	{1, 0}, 
	{0, 1}, 
	{1, 1} 
};
*/
double trainInputs[NPATTERNS][4] = {
	{0, 0, 0, 0},
	{1, 0, 0, 1}, 
	{0, 1, 1, 0}, 
	{0.5, 0.5, 1, 1}, 
	{0, 0.5, 0.5, 0}, 
	{1, 1, 1, 1} 
};
//static double trainOutputs[] = {0, 1, 1, 0};
double trainOutputs[] = {0, 1, 1, 1, 0, 0};
	/*	static double[][] trainOutputs = new double[][]{
		new double[]{1, 0},
		new double[]{0, 1}
	};
	 */

	//user defineable variables
static int numEpochs = 1500; //number of training cycles
static int numInputs = 4; //number of inputs - this includes the input bias
static int numHidden = NHIDDEN; //number of hidden units
static int numPatterns = NPATTERNS; //number of training patterns
static double LR_IH = 0.7; //learning rate, default 0.7
static double LR_HO = 0.07; //learning rate, default 0.07

	//the outputs of the hidden neurons
static double h[NHIDDEN]; 

	//the weights
static double weightsIH [4][NHIDDEN] ; 
static double weightsHO[NHIDDEN] ;

	//process variables
static int patNum;
static double errThisPat;
static double outPred;
static double RMSerror;


//============================================================
//********** END OF THE MAIN PROGRAM **************************
//=============================================================

void init() {
  //initiate the weights
  initWeights();

  //load in the data
//  initData();
}

void train()
 {

  //train the network
  int i, j;
  double push;
    for(j = 0;j <= numEpochs;j++)
    {

        for(i = 0;i<numPatterns;i++)
        {

            //select a pattern at random
            //patNum = (int)((randomdef*numPatterns)-0.001);
            patNum = i;

            //calculate the current network output
            //and error for this pattern
            push = forward(trainInputs[patNum]);

            //change network weights
	    backprop(push, trainOutputs[patNum]);
        }

        //display the overall network error
        //after each epoch
        calcOverallError();
        if(j % 100 == 0) {
        	printf("epoch = %d RMSE = %.4f\n", j, RMSerror);
		//displayResults();
		testAll();
	}
    }

    //training has finished
    //display the results
//    displayResults();

 }


//************************************
/**
 * forward propagation
 */
double forward(double state[])
 {
    //calculate the outputs of the hidden neurons
    //the hidden neurons are tanh
    int i, j;
    for(i = 0;i<numHidden;i++)
    {
	h[i] = 0.0;

        for(j = 0;j<numInputs;j++)
        h[i] = h[i] + (state[j] * weightsIH[j][i]);

        h[i] = tanh(h[i]);
        //h[i] = sigmoid(h[i]);
    }

   //calculate the output of the network
   //the output neuron is linear
   outPred = 0.0;

   for(i = 0;i<numHidden;i++)
    outPred = outPred + h[i] * weightsHO[i];
   //outPred = sigmoid (outPred);
    //calculate the error
    errThisPat = outPred - trainOutputs[patNum];
   return outPred;
 }

void backprop(double push, double target) {
	double error = push - target;
	WeightChangesHO(error);
        WeightChangesIH(error);
}

//************************************
void WeightChangesHO(double errThisPat)
 //adjust the weights hidden-output
 {
   int k; 
   for(k = 0;k<numHidden;k++)
   {
    double weightChange = LR_HO * errThisPat * h[k];
    weightsHO[k] = weightsHO[k] - weightChange;

    //regularisation on the output weights
    if (weightsHO[k] < -5)
        weightsHO[k] = -5;
    else if (weightsHO[k] > 5)
        weightsHO[k] = 5;
   }
 }


//************************************
void WeightChangesIH(double errThisPat)
 //adjust the weights input-hidden
 {
   int i, k; 
  for( i = 0;i<numHidden;i++)
  {
   double x = (1 - (h[i]) * h[i]) * weightsHO[i] * errThisPat * LR_IH;
   for(k = 0;k<numInputs;k++)
   {
    //double x = (1 - (h[i] * h[i])) * weightsHO[i] * errThisPat * LR_IH * trainInputs[patNum][k];
    double weightChange = x * trainInputs[patNum][k];
    weightsIH[k][i] = weightsIH[k][i] - weightChange;
   }
  }
 }


//************************************
void initWeights()
 {
   int i, j; 

  for(j = 0;j<numHidden;j++)
  {
    weightsHO[j] = (randomdef - 0.5)/2;
    for(i = 0;i<numInputs;i++)
    weightsIH[i][j] = (randomdef - 0.5)/5;
  }

 }


//************************************
void initData()
{

  printf("initialising data\n");

  // the data here is the XOR data
  // it has been rescaled to the range
  // [-1][1]
  // an extra input valued 1 is also added
  // to act as the bias
/*
  trainInputs[0][0]  = 1;
  trainInputs[0][1]  = -1;
  trainInputs[0][2]  = 1;//bias
  trainOutputs[0] = 1;

  trainInputs[1][0]  = -1;
  trainInputs[1][1]  = 1;
  trainInputs[1][2]  = 1;//bias
  trainOutputs[1] = 1;

  trainInputs[2][0]  = 1;
  trainInputs[2][1]  = 1;
  trainInputs[2][2]  = 1;//bias
  trainOutputs[2] = -1;

  trainInputs[3][0]  = -1;
  trainInputs[3][1]  = -1;
  trainInputs[3][2]  = 1;//bias
  trainOutputs[3] = -1;
*/
}

//************************************
double sigmoid(double x) {
	return 1.0 / (1.0 + exp(-x));
}

double tanh(double x)
 {
    if (x > 20)
        return 1;
    else if (x < -20)
        return -1;
    else
        {
        double a = exp(x);
        double b = exp(-x);
        return (a-b)/(a+b);
        }
 }


//************************************
double test(int patternNumber) {
	patNum = patternNumber;
	forward(trainInputs[patNum]);
	return outPred;
}

/**
 * test and display results
 */
void testAll()
    {
    int i;
     for(i = 0;i<numPatterns;i++)
        {
//        patNum = i;
//        forward();
    	 test(i);
        printf("pat%d expected = %.4f neural model = %.4f\n", patNum + 1, /*trainInputs[i],*/ trainOutputs[patNum], outPred);
        }
    }


//************************************
void calcOverallError()
    {
    int i;
     RMSerror = 0.0;
     for(i = 0;i<numPatterns;i++)
        {
        patNum = i;
        forward(trainInputs[patNum]);
        RMSerror = RMSerror + (errThisPat * errThisPat);
        }
     RMSerror = RMSerror/numPatterns;
     RMSerror = sqrt(RMSerror);
    }

printWeights() {
        int i, j;
        for(i = 0; i < 4; i ++) {
                for(j = 0; j < 4; j ++)
                        printf("%.4f ", weightsIH[i][j]);
                printf("\n");
        }
        for(i = 0; i < 4; i ++)
                printf("%.4f ", weightsHO[i]);
        printf("\n");
}

//==============================================================
//********** THIS IS THE MAIN PROGRAM **************************
//==============================================================

/*
main()
 {

  train();

  //training has finished
  //display the results
//  displayResults();
  
 }
*/
