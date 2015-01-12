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

calcNet()
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

void calcNet();
void WeightChangesHO();
void WeightChangesIH();
void initWeights();
void initData();
double sigmoid(double x);
double tanh(double x);
void displayResults();
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
static double trainOutputs[] = {0, 1, 1, 1, 0, 0};
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
static double hiddenVal[NHIDDEN]; 

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

  static void train()
 {

  //initiate the weights
  initWeights();

  //load in the data
  initData();

  //train the network
  int i, j;
    for(j = 0;j <= numEpochs;j++)
    {

        for(i = 0;i<numPatterns;i++)
        {

            //select a pattern at random
            patNum = (int)((randomdef*numPatterns)-0.001);

            //calculate the current network output
            //and error for this pattern
            calcNet();

            //change network weights
            WeightChangesHO();
            WeightChangesIH();
        }

        //display the overall network error
        //after each epoch
        calcOverallError();
        if(j % 100 == 0) {
        	printf("epoch = %d RMSE = %.4f\n", j, RMSerror);
		displayResults();
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
void calcNet()
 {
    //calculate the outputs of the hidden neurons
    //the hidden neurons are tanh
    int i, j;
    for(i = 0;i<numHidden;i++)
    {
	hiddenVal[i] = 0.0;

        for(j = 0;j<numInputs;j++)
        hiddenVal[i] = hiddenVal[i] + (trainInputs[patNum][j] * weightsIH[j][i]);

        hiddenVal[i] = tanh(hiddenVal[i]);
        //hiddenVal[i] = sigmoid(hiddenVal[i]);
    }

   //calculate the output of the network
   //the output neuron is linear
   outPred = 0.0;

   for(i = 0;i<numHidden;i++)
    outPred = outPred + hiddenVal[i] * weightsHO[i];
   //outPred = sigmoid (outPred);
    //calculate the error
    errThisPat = outPred - trainOutputs[patNum];
 }


//************************************
void WeightChangesHO()
 //adjust the weights hidden-output
 {
   int k; 
   for(k = 0;k<numHidden;k++)
   {
    double weightChange = LR_HO * errThisPat * hiddenVal[k];
    weightsHO[k] = weightsHO[k] - weightChange;

    //regularisation on the output weights
    if (weightsHO[k] < -5)
        weightsHO[k] = -5;
    else if (weightsHO[k] > 5)
        weightsHO[k] = 5;
   }
 }


//************************************
void WeightChangesIH()
 //adjust the weights input-hidden
 {
   int i, k; 
  for( i = 0;i<numHidden;i++)
  {
   double x = (1 - (hiddenVal[i]) * hiddenVal[i]) * weightsHO[i] * errThisPat * LR_IH;
   for(k = 0;k<numInputs;k++)
   {
    //double x = (1 - (hiddenVal[i] * hiddenVal[i])) * weightsHO[i] * errThisPat * LR_IH * trainInputs[patNum][k];
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
	calcNet();
	return outPred;
}

/**
 * test and display results
 */
void displayResults()
    {
    int i;
     for(i = 0;i<numPatterns;i++)
        {
//        patNum = i;
//        calcNet();
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
        calcNet();
        RMSerror = RMSerror + (errThisPat * errThisPat);
        }
     RMSerror = RMSerror/numPatterns;
     RMSerror = sqrt(RMSerror);
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
