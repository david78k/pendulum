/**********************************************************************

To compile to optionally run on the CNAPS Server:
 On a Sun OS machine (like schubert) do the following.  Our CNAPS Server
 is named "hammer":
  setenv ADAPTIVE /s/bach/e/sys/hammer.2.0.new/sun4 
  gcc -O3 -o train train.c -DHAMMER -L$ADAPTIVE/lib -lbn -I$ADAPTIVE/include -lm

To compile without linking to the HAMMER Server libraries:
  gcc -O3 -o train train.c -lm 

       Copyright (c) 1996 by Charles W. Anderson
***********************************************************************/

#ifdef HAMMER
 #include <bnlib.h>		/* Just for running on hammer */
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
/*#include <strings.h>*/
#include <sys/types.h>
#include <sys/time.h>		/* for setting random number seed */
#define RN (acgrnd())
#define max(a,b) (((a)>(b))?(a):(b))

int debug= 0;

struct pattern_struct {
  float *data;
  struct pattern_struct *next;
};
typedef struct pattern_struct Pattern;
Pattern *firstPattern;
Pattern *lastPattern;

struct set_struct {
  int num;
  Pattern **patterns;
/*  float *targets;*/
};
typedef struct set_struct Set;

int linearOut=0;		/* 1 if output is linear. */
int InputsThrough=0;		/* 1 to pass inputs through to last layer */

int numInputs, numOutputs, numHidden1, numHidden2=0, numOutputInputs;
int epochs, epochPrintInterval=0;
float correctCrit;

float *wh1;		/* firts hidden layer units' weights */
float *wh2;		/* optional second layer hidden units' weights */
float *wo;		/* output unit's weights */
float *dwh1;
float *dwh2;
float *dwo;
float *wp, *swp;
float *x;			/* input vector */
float *yh1;			/* hidden units' outputs */
float *yh2;			/* second layer hidden units' outputs */
float *yo;			/* output unit's output  */
float *deltao, *deltah1, *deltah2, *deltax; /* derivatives */
float *whchange = NULL;
float *wochange = NULL;
float *error;
float epoch_error;
float oRate, hRate, mom;
float *swh1, *swh2, *swo;	/* best weights so far */
float lowestValidateError;	/* keep track of best so far */
int bestEpoch;

int hammer=0;			/* 1 if using hammer */
int highPrecision= 0;		/* 1 to use 32 bit backprop */

/* Remember all files and associated binary versions so later experiments */
/* can use them without converting again. */
struct file_struct {
  char name[100];
  char binName[100];		/* for binary data for hammer */
  int num;
  char binaryName[100];
  Pattern *first;
};
typedef struct file_struct DataFile;
#define MAXFILES 100
DataFile dataFiles[MAXFILES];
int numDataFiles;
FILE *specFile;

char *trainFiles[100], *validateFiles[100], *testFiles[100];
int nTrain, nValidate=0, nTest;
/*char resultsFile[100] = " ";*/
int summarize=0;		/* if 1, print one line per run */

Set training, testing, validating;

char previousAlgorithm[100] = "none";
char newAlgorithm[100];
char *commandName;

float rmsErrorTraining, rmsErrorValidating, fracCorrValidating,
rmsErrorTesting, fracCorrTesting;
float RsquaredValidating, RsquaredTesting;

/*** function prototypes ***/

int fexists(char *filename);
int mysystem(char *s);		/* delay after doing command */
void initParse(char *file);
int parseNext(void);
int findFile(char *filename);
void addIfNew(char *filename);
void usage(void);
void loadSet(Set *set, char *files[], int nFiles, char *which);
void readPatterns(DataFile *dataFile);
void calcError(Set *set, float *wh1, float *wh2, float *wo,
		float *rmsError, float *fracCorr, float *Rsquared, int print);
void calcOutput(float *x, float *wh1, float *wh2, float *wo);
void printNet(int ni, int nh1, int nh2, int no, float *x, float *wh1,
	      float *wh2, float *wo, float *yh1, float *yh2, float *yo);
void updateWeights(float *x);
float logistic(float x);
float Dlogistic(float y);
void nLinesWords (char *filename, int *rows, int *words);
void init_acgrnd_by_time();
float acgrnd();
void trainHere(void);
void trainHammer(void);
char *date(void);

/**********************************************************************/
main(int argc, char *argv[]) {
  int u, i, j, p, pattern;
  int ep;
  int junk;
  float s;
  float *wp, *swp;
  float sum, junkFloat;
  FILE *filep;
  int oldni, oldnh1, oldnh2, oldno;
  char string[200];

  /* Local variables specific to hammer */
#ifdef HAMMER
  int connect_err = BNERR_CONNECTFAIL;
#endif
  int ret;

  commandName = argv[0];

  if (argc > 3) {
    usage();
    exit(1);
  }

  if (argc == 3) {
    if (strncmp(argv[2],"hammerhi",8) == 0)  {
      hammer = 1;
      highPrecision = 1;
    } else if (strncmp(argv[2],"hammer",6) == 0) {
      hammer = 1;
      highPrecision = 0; 
    } else {
      printf("Second argument, %s, not understood.\n",argv[2]);
      usage();
      exit(1);
    }
  }

  /* Log file */
  if ((filep=fopen("train.log","a")) != NULL) {
    for (i=0; i<argc; i++) 
      fprintf(filep,"%s ", argv[i]);
    fprintf(filep,"\n");
    fclose(filep);
  }

  init_acgrnd_by_time();

#ifdef HAMMER
  if (hammer) {
    if (debug) {
      if (ret = bnSetIOMode(BNIO_ERROR | BNIO_MESSAGE )) bnExit(ret);
    } else {
      if (ret = bnSetIOMode(BNIO_ERROR )) bnExit(ret);
    }
    
    while (connect_err = bnConnect("hammer", HOSTSERVER)) {
      sleep(60);
    }
    if (debug) {printf("Just connected\n"); fflush(stdout);}
    /*connect_err = bnConnect("cpsim", HOSTSIM);*/
  }
#endif

  initParse(argv[1]);

  while(parseNext()) {

    epochPrintInterval = epochs/20;
    numOutputInputs = max(numHidden1,numHidden2);

    if (numOutputInputs == 0 || InputsThrough)
      numOutputInputs += numInputs;

#ifdef HAMMER
    if (hammer) {
      if (highPrecision) {
	if (mom > 0.) {
	  if (numHidden1 > 0 && numHidden2 > 0)
	    strcpy(newAlgorithm,"BP32mom2hid");
	  else
	    strcpy(newAlgorithm,"BP32momentum");
	} else {
	  if (numHidden1 > 0 && numHidden2 > 0)
	    strcpy(newAlgorithm,"BP322hidden");
	  else
	    strcpy(newAlgorithm,"BP32standard");
	}
      } else { /*low precision*/
	if (mom > 0.) {
	  if (numHidden1 > 0 && numHidden2 > 0)
	    strcpy(newAlgorithm,"BPmom2hid");
	  else
	    strcpy(newAlgorithm,"BPmomentum");
	} else {
	  if (numHidden1 > 0 && numHidden2 > 0)
	    strcpy(newAlgorithm,"BP2hidden");
	  else
	    strcpy(newAlgorithm,"BPstandard");
	}
      }

      if (strcmp(previousAlgorithm,newAlgorithm) != 0) {
	if (debug) {
	  printf("Setting algo to %s. Previous was %s.\n",
		 newAlgorithm,previousAlgorithm);
	  fflush(stdout);
	}
	if (ret = bnSetAlgorithm(newAlgorithm)) bnExit(ret);
	strcpy(previousAlgorithm,newAlgorithm);
      }

      if (ret = bnSetConstantInteger("NumInputs",numInputs)) bnExit(ret);
      if (ret = bnSetConstantInteger("NumOutputs",numOutputs)) bnExit(ret);
      if (nValidate > 0) {
	if (ret = bnSetConstantInteger("TestInterval",1)) bnExit(ret);
	if (ret = bnSetControlMode("LogTestHist"))  bnExit(ret);
      }
      if (ret = bnSetControlMode("LogTrainHist"))  bnExit(ret);
    }
#endif      
    /* Load patterns */

    loadSet(&training,trainFiles,nTrain,"train");
    if (nValidate>0) 
      loadSet(&validating,validateFiles,nValidate,"validate");
    loadSet(&testing,testFiles,nTest,"test");

    /* Set up other data structures */

    if (oldni != numInputs || oldnh1 != numHidden1 || 
	oldnh2 != numHidden2 || oldno != numOutputs) {
      free(yo); free(yh1); free(error); free(wh1); free(wo);
      free(dwh1); free(dwo); free(swh1); free(swo);
      if (yh2 != NULL) {
	free(yh2); free(wh2); free(dwh2); free(swh2);
      }
      yo = (float *) malloc (numOutputs * sizeof(float));
      yh1 = (float *) malloc (numHidden1 * sizeof(float));
      error = (float *) malloc (numOutputs * sizeof(float));
      wh1 = (float *) malloc((numInputs+1)*numHidden1 * sizeof(float));
      wo = (float *) malloc(numOutputs*(numOutputInputs+1) * sizeof(float));
      dwh1 = (float *) malloc((numInputs+1)*numHidden1 * sizeof(float));
      dwo = (float *) malloc(numOutputs*(numOutputInputs+1) * sizeof(float));
      swh1 = (float *) malloc((numInputs+1)*numHidden1 * sizeof(float));
      swo = (float *) malloc(numOutputs*(numOutputInputs+1) * sizeof(float));
      if (numHidden2 > 0) {
	yh2 = (float *) malloc (numHidden2 * sizeof(float));
	wh2 = (float *) malloc((numHidden1+1)*numHidden2 * sizeof(float));
	dwh2 = (float *) malloc((numHidden1+1)*numHidden2 * sizeof(float));
	swh2 = (float *) malloc((numHidden1+1)*numHidden2 * sizeof(float));
      } else {
	yh2 = wh2 = dwh2 = swh2 = NULL;
      }
    }
    oldni = numInputs;
    oldnh1 = numHidden1;
    oldnh2 = numHidden2;
    oldno = numOutputs;

    /* Initialize weights to small random values. */

    /* Assume max input value is 1 and we want weighted sum of initial
       weight vector with input to be no greater than 1, which gives a
       logistic value of about 0.7.  Then, max weighted sum would be
       ni * factor = 1
       so factor must be 1/ni. */

    s = 3.0/sqrt((double)numInputs); 
/* older   s = 3.0/numInputs;*/

    for (i=0,wp=wh1; i<(numInputs+1)*numHidden1; i++)
      *wp++ = (RN * 2.0 - 1.0) * s;

    if (numHidden2 > 0) {
/* old      s = 3.0/numHidden1;*/
      s = 3.0 / sqrt((double)numHidden1);
      for (i=0,wp=wh2; i<(numHidden1+1)*numHidden2; i++)
      *wp++ = (RN * 2.0 - 1.0) * s;
    }

    /*init_weight_range * (RN * 2 - 1);*/

    for (i=0,wp=wo; i<numOutputs*(numOutputInputs+1); i++)
      *wp++ = 0.;		/*init_weight_range * (RN * 2 - 1);*/

    /* Initialize previous weight changes to zero. */

    for (wp=dwh1, i=0; i<numHidden1*(numInputs+1); wp++, i++)
      *wp = 0.;
    if (numHidden2 > 0) 
      for (wp=dwh2, i=0; i<numHidden2*(numHidden1+1); wp++, i++)
	*wp = 0.;
    for (wp=dwo, i=0; i< numOutputs*(numOutputInputs+1); i++, wp++) 
      *wp = 0.;

    /* Write weights to file and convert to binary for hammer. */
#ifdef HAMMER
    if (hammer) {
      if ((filep = fopen("nn.initialw","w")) == NULL) {
	fprintf(stderr,"Couldn't open nn.initialw.\n");
	exit(1);
      }
      for (i=0,wp=wh1; i<(numInputs+1)*numHidden1; i++)
	fprintf(filep,"%f\n",*wp++);
      if (numHidden2 > 0)
	for (i=0,wp=wh2; i<(numHidden1+1)*numHidden2; i++)
	  fprintf(filep,"%f\n",*wp++);
      for (i=0,wp=wo; i<numOutputs*(numOutputInputs+1); i++)
	fprintf(filep,"%f\n",*wp++);
      fclose(filep);
      while (! fexists("nn.initialw")) sleep(1);
      sprintf(string,"cv -i%%f -o%%I12 < nn.initialw > initialbp.wts");
      if ((ret=mysystem(string)) != 0) 
	fprintf(stderr,"Error: Return code of %d when cv'ing initial weights.\n",
		ret);
      while (! fexists("initialbp.wts")) sleep(1);
    }
#endif

    /* Log file */
    if ((filep=fopen("train.log","a")) != NULL) {
      fprintf(filep,
       "h1 %3d h2 %3d hr %7.3f or %7.3f mr %5.3f ep %d linOut %d hamm %d %s\n",
	      numHidden1, numHidden2, hRate, oRate, mom, epochs, linearOut, 
	      hammer, date());
      fclose(filep);
    }

    if (!summarize)
      printf("i %d h1 %d h2 %d o %d hr %7.3f or %7.3f mr %5.3f ep %d linOut %d hamm %d %s\n",
        numInputs, numHidden1, numHidden2, numOutputs, hRate, oRate, mom, 
	     epochs, linearOut, hammer, date());

    /***** TRAIN *****/

    if (hammer)
      trainHammer();
    else
      trainHere();

/*    calcError(&testing,swh1,swh2,swo,&rmsErrorTesting,&RsquaredTesting,0);*/

    if (summarize)
      i = 0;
    else
      i = 1;
    calcError(&testing,swh1,swh2,swo,&rmsErrorTesting,&fracCorrTesting,
	      &RsquaredTesting,i);

    if (!summarize) {
      printf("Best epoch %5d validate %10f test %10f fracCorr %10f Rsq %10f\n", 
	     bestEpoch,lowestValidateError,rmsErrorTesting,fracCorrTesting,
	     RsquaredTesting);

      printf("\nWeights\n");
      for (u=0; u<numHidden1; u++) {
	for (i=0; i<numInputs+1; i++)
	  printf("%f\n",*(swh1+u*(numInputs+1)+i));
	printf("\n");
      }

      if (numHidden2 > 0) {
	printf("\n");
	for (u=0; u<numHidden2; u++) {
	  for (i=0; i<numHidden1+1; i++)
	    printf("%f\n",*(swh2+u*(numHidden1+1)+i));
	  printf("\n");
	}
      }

      printf("\n");
      for (u=0; u<numOutputs; u++) {
	for (i=0; i<numOutputInputs+1; i++)
	  printf("%f\n",*(swo+u*(numOutputInputs+1)+i));
	printf("\n");
      }
    }

    if (summarize) {
      printf("%3d %3d %7.3f %7.3f %5.3f %10.4f %5d %10.4f %10.4f %5.3f\n", 
	      numHidden1, numHidden2, hRate, oRate,
	     mom, rmsErrorTraining, bestEpoch, lowestValidateError,
	     rmsErrorTesting,fracCorrTesting);
    }
    fflush(stdout);
  }				/* go on to next experiment */

  exit(0);			/* so tcl will see a zero return code */
}

/**********************************************************************/
void trainHere(void) {
  int ep, pattern, i;
  float epoch_error;
  float *xp, *tp, *wp, *swp;

    /* Main loop. Run for this many repetitions of training set. */

    for (ep=0; ep<epochs; ep++) {

      epoch_error = 0;
      for (pattern=0; pattern<training.num; pattern++) {

	xp = training.patterns[pattern]->data;
	tp = training.patterns[pattern]->data + numInputs;
	calcOutput(xp,wh1,wh2,wo);

	/*  printNet(numInputs,numHidden1,numHidden2,numOutputs,
	    xp,wh1,wh2,wo,yh,yo);*/

	for (i=0; i<numOutputs; i++) {
	  error[i] = tp[i] - yo[i];
	  epoch_error += error[i]*error[i] / numOutputs;
	}
	updateWeights(xp);
      }				/* end of pattern loop */

      rmsErrorTraining = sqrt(epoch_error/training.num);
/*      printf("ep %d e %f\n",ep,rmsErrorTraining);*/

      if (nValidate) {
	/* Do cross-validation */
	calcError(&validating,wh1,wh2,wo,&rmsErrorValidating,
		  &fracCorrValidating,&RsquaredValidating,0);
/** Changes so best epoch is given by fracCorr **/
	if (ep == 0 || rmsErrorValidating < lowestValidateError) { 
/**	if (ep == 0 || fracCorrValidating > lowestValidateError) { **/
	  /* New best weights */
	  bestEpoch = ep+1;
	  lowestValidateError = rmsErrorValidating; 
/**	  lowestValidateError = fracCorrValidating; **/
	  for (i=0,wp=wh1,swp=swh1; i<(numInputs+1)*numHidden1; i++)
	    *swp++ = *wp++;
	  if (numHidden2 > 0)
	    for (i=0,wp=wh2,swp=swh2; i<(numHidden1+1)*numHidden2; i++)
	      *swp++ = *wp++;
	  for (i=0,wp=wo,swp=swo; i<numOutputs*(numOutputInputs+1); i++)
	    *swp++ = *wp++;
	}
      }

      /* At every print interval, calculate the error in the validation set */
      /* and the test set. */

      if (!summarize && 
	  epochPrintInterval > 0 && ((ep+1) % epochPrintInterval) == 0) {
	calcError(&testing,wh1,wh2,wo,&rmsErrorTesting,&fracCorrTesting,
		  &RsquaredTesting,0);
	printf("epoch %5d train %10f validate %10f test %10f fracCorr %5.3f Rsq %10f\n",
	       ep+1,rmsErrorTraining,rmsErrorValidating,rmsErrorTesting,
	       fracCorrTesting,RsquaredTesting);

	fflush(stdout);
      }

    }				/* end of epoch loop for training */

    if (!summarize && epochPrintInterval > 0) {
      calcError(&testing,wh1,wh2,wo,&rmsErrorTesting,&fracCorrTesting,
		&RsquaredTesting,0);
      printf("epoch %5d train %10f validate %10f test %10f fracCorr %5.3f Rsq %10f\n",
	     ep+1,rmsErrorTraining,rmsErrorValidating,rmsErrorTesting,
	     fracCorrTesting,RsquaredTesting);
      /*printNet(numInputs,numHidden1,numHidden2,numOutputs,xp,wh1,wh2,wo,yh,yo);*/
    }

  if (nValidate ==0) {
    /*Best weights are assumed to be final weights */
    bestEpoch = ep;
    /* lowestValidateError = rmsErrorValidating;*/
    for (i=0,wp=wh1,swp=swh1; i<(numInputs+1)*numHidden1; i++)
      *swp++ = *wp++;
    if (numHidden2 > 0)
      for (i=0,wp=wh2,swp=swh2; i<(numHidden1+1)*numHidden2; i++)
	*swp++ = *wp++;
    for (i=0,wp=wo,swp=swo; i<numOutputs*(numOutputInputs+1); i++)
      *swp++ = *wp++;
  }
}

/**********************************************************************/
void trainHammer(void) {
  int ret;
  float h_scale, lsb, msb, testerr, trainerr, best_valid_err;
  FILE *fp1, *fp2, *filep;
  int i;
  float junk;
#ifdef HAMMER
  BPResultData *results;
#endif

#ifdef HAMMER
  if (numHidden2 > 0) {
    if (ret = bnSetConstantInteger("NumLowerHiddens",numHidden1)) 
      bnExit(ret);
    if (ret = bnSetConstantInteger("NumUpperHiddens",numHidden2))
      bnExit(ret);
  } else {
    if (ret = bnSetConstantInteger("NumHiddens",numHidden1)) 
      bnExit(ret);
  }
  if (ret = bnSetConstantInteger("NumEpochs",epochs)) bnExit(ret);
  if (ret = bnSetConstantFloat("LearnRate",oRate)) bnExit(ret);
  h_scale = hRate/oRate;
  if (ret = bnSetConstantFloat("HidLrnScale",h_scale)) bnExit(ret);
  if (mom > 0.0)
    if (ret = bnSetConstantFloat("Momentum",mom)) bnExit(ret);
  /**if (ret = bnSetConstantFloat("DerivOffset",0.01)) bnExit(ret);**/
  if (ret = bnSetFile("InitialWts", "initialbp.wts")) bnExit(ret);

  if (debug) {printf("First training pass...\n");  fflush(stdout);}
/*  sleep(2);*/
  if (ret = bnExecute(Initialize | LoadWts | Train | Result)) bnExit(ret);
/*  sleep(2);*/
  if (debug) { printf("Done first training pass\n");  fflush(stdout);}

  if (!summarize && epochPrintInterval > 0) {
    if (nValidate>0) {
      while (! fexists("testbperr.out")) sleep(1);
      if ((ret=mysystem("cv -i%J -o%f < testbperr.out > testbperr.ascii")) != 0) 
	fprintf(stderr,"Error: Return code of %d when cv'ing testbperr.\n",
		ret);
      while (! fexists("testbperr.ascii")) sleep(1);
    }
    while (! fexists("trnbperr.out")) sleep(1);
    if ((ret=mysystem("cv -i%J -o%f < trnbperr.out > trnbperr.ascii")) != 0) 
      fprintf(stderr,"Error: Return code of %d when cv'ing trnbperr.\n",
	      ret);
    while (! fexists("trnbperr.ascii")) sleep(1);
    if (nValidate>0)
      if ((fp1 = fopen("testbperr.ascii","r")) == NULL)
	fprintf(stderr,"Couldn't open testbperr.ascii\n");
    if ((fp2 = fopen("trnbperr.ascii","r")) == NULL)
      fprintf(stderr,"Couldn't open trnbperr.ascii\n");
    for (i=0;i<epochs;i++) {
      if (nValidate>0) fscanf(fp1,"%f %f",&junk,&junk);
      fscanf(fp2,"%f %f",&junk,&junk);

      if ( ((i+1) % epochPrintInterval) == 0) {
	if (nValidate>0) {
	  fscanf(fp1,"%f %f",&lsb,&msb);
	  testerr = sqrt((msb + lsb/65536.0)/numOutputs/validating.num);
	} else {
	  testerr=0.;
	}

	fscanf(fp2,"%f %f",&lsb,&msb);
    	trainerr = sqrt((msb + lsb/65536.0)/numOutputs/training.num);

	printf("epoch %5d train %10f validate %10f test %10f fracCorr %5.3f Rsq %10f\n",
	     i+1,trainerr,testerr,0.,0.,0.);

      } else {
	if (nValidate>0) fscanf(fp1,"%f %f",&junk,&junk);
	fscanf(fp2,"%f %f",&junk,&junk);
      }

      if (nValidate>0) fscanf(fp1,"%f %f %f %f",&junk,&junk,&junk,&junk);
      fscanf(fp2,"%f %f %f %f",&junk,&junk,&junk,&junk);
    }
    if (nValidate>0) fclose(fp1);
    fclose(fp2);
  }

  /*************** Get the results.   ******************/
  results = bnGetResults();
  if (results == NULL) {
    fprintf(stderr,"No results from bnGetResults().\n");
    exit(1);
  }
  rmsErrorTraining = sqrt(results->bestSqErrTrain
			  /65536.0/numOutputs/training.num);

  if (nValidate > 0) {
    if (ret = bnSetConstantInteger("NumEpochs",results->bestEpochTest)) 
      bnExit(ret);
    if (debug) {printf("Second training pass...\n"); fflush(stdout);}
    if (ret = bnExecute( Initialize | LoadWts | Train | SaveWts )) bnExit(ret);
    if (debug) {printf("Done second training pass\n");  fflush(stdout);}
  } else {
    /* Not cross-validating.  Save final training weights. */
    if (ret = bnExecute( SaveWts )) bnExit(ret);
  }
  while (! fexists("trainedbp.wts")) sleep(1);
  if (debug) {printf("Converting weights..\n"); fflush(stdout); }
  if ((ret=mysystem("cv -i%I12 -o%f < trainedbp.wts > trainedbp.wts.ascii")) 
      != 0) {
    fprintf(stderr,"Error: Return code of %d when cv'ing trainedbp.wts\n",ret);
    exit(1);
  }
  while (! fexists("trainedbp.wts.ascii")) sleep(1);
  mysystem("rm -f trainedbp.wts");
  if ((filep = fopen("trainedbp.wts.ascii","r")) == NULL) {
    fprintf(stderr,"Couldn't open trainedbp.wts.ascii.\n");
    exit(1);
  }
  for (i=0,wp=swh1; i<(numInputs+1)*numHidden1; i++)
    fscanf(filep,"%f\n",wp++);
  if (numHidden2 > 0)
    for (i=0,wp=swh2; i<(numHidden1+1)*numHidden2; i++)
      fscanf(filep,"%f\n",wp++);
  for (i=0,wp=swo; i<numOutputs*(numOutputInputs+1); i++)
    fscanf(filep,"%f\n",wp++);
  fclose(filep);

  if (nValidate > 0) {
    best_valid_err = results->bestSqErrTest/65536.0/numOutputs/validating.num;
    lowestValidateError = (float)sqrt(best_valid_err);
    bestEpoch = results->bestEpochTest;
  } else {
    bestEpoch = epochs;
  }
  if (ret = bnCleanupAlgorithm()) bnExit(ret);
#endif
}

/**********************************************************************/

#define IF(str) if (strncmp(token,str,strlen(str))==0)
#define EIF(str) else if (strncmp(token,str,strlen(str))==0)
#define NEXTTOKEN(what) if (fscanf(specFile,"%s",token) == EOF) { printf("Specification file ended while %s\n",what); exit(1);}
#define NEXTF(next) atof(token)
#define NEXTI(next) atoi(token)


void initParse(char *file) {
  if ((specFile=fopen(file,"r")) == NULL) {
    usage();
    exit(1);
  }
  numDataFiles = 0;
}

/**********************************************************************/

int parseNext(void) {
  static char token[1000];
  int notEOF = 1, advance;

  notEOF = fscanf(specFile,"%s",token) != EOF;

  while(notEOF) {

/*    printf("Token is %s\n",token);*/

    advance = 1;
    IF("-tr") { 
      nTrain = 0;
      NEXTTOKEN("reading training file names");
      while (token[0] != '-') {
	trainFiles[nTrain] = (char *) malloc(sizeof(char)*(1+strlen(token)));
	strcpy(trainFiles[nTrain],token);
	addIfNew(trainFiles[nTrain]);
	nTrain++;
	NEXTTOKEN("reading training file names");	
      }
      advance = 0;
    } 
    EIF("-val") { 
      nValidate = 0;
      NEXTTOKEN("reading validation file names");
      while (token[0] != '-') {
	validateFiles[nValidate] = (char *) malloc(sizeof(char)*
						(1+strlen(token)));
	strcpy(validateFiles[nValidate],token);
	addIfNew(validateFiles[nValidate]);
	nValidate++;
	NEXTTOKEN("reading validation file names");	
      }
      advance = 0;
    }
    EIF("-tes") { 
      nTest = 0;
      NEXTTOKEN("reading testing file names");
      while (token[0] != '-') {
	testFiles[nTest] = (char *) malloc(sizeof(char)*(1+strlen(token)));
	strcpy(testFiles[nTest],token);
	addIfNew(testFiles[nTest]);
	nTest++;
	NEXTTOKEN("reading testing file names");	
      }
      advance = 0;
    }
/*    EIF("-hammer") {hammer = 1;}*/
    EIF("-or") {NEXTTOKEN("output learning rate"); oRate = atof(token);}
    EIF("-hr") {NEXTTOKEN("hidden learning rate"); hRate = atof(token);}
    EIF("-m") {NEXTTOKEN("momentum"); mom = atof(token);}
    EIF("-ni") {NEXTTOKEN("number of inputs"); numInputs = atoi(token);}
    EIF("-nhiddens1") {NEXTTOKEN("number of hidden units 1"); 
		       numHidden1 = atoi(token);}
    EIF("-nhiddens2") {NEXTTOKEN("number of hidden units 2"); 
		       numHidden2 = atoi(token);}
    EIF("-no") {NEXTTOKEN("number of outputs"); numOutputs = atoi(token);}
    EIF("-ep") {NEXTTOKEN("number of epochs"); epochs = atoi(token);}
    EIF("-out") {NEXTTOKEN("output unit type"); 
		 if (strncmp(token,"l",1) == 0) {
		   linearOut = 1;
		 } else if (strncmp(token,"s",1) == 0) {
		   linearOut = 0;
		 } else {
		   printf("-output must be \"linear\" or \"sigmoid\"\n");
		   usage();
		 }
	       }
/**    EIF("-re") {NEXTTOKEN("result file name"); strcpy(resultsFile,token);}**/
    EIF("-co") {NEXTTOKEN("correctness criterion"); correctCrit = atof(token);}
    EIF("-sum") {summarize = 1;}
    EIF("-end") {return 1;}	/* signal that experiment may start */
    else if (token[0] == '-') { /* argument has - but didn't match */
      printf("Unrecognized argument %s\n",token);
      usage();
      exit(1);
    }
    if (advance) 
      notEOF = fscanf(specFile,"%s",token) != EOF;
  }
  return 0;			/* signal that no more tokens */
}
/**********************************************************************/
int findFile(char *filename) {
  int i, found=0;
  
  for (i=0; i<numDataFiles; i++)
    if (strcmp(dataFiles[i].name,filename) == 0) {
      return i;
    }

  return -1;
}

/**********************************************************************
 * Match filename against already read files.  If not found, add it.
 **********************************************************************/
void addIfNew(char *filename) {
  int i, lines, words;
  char string[200];

  if (debug) {printf("Reading %s\n",filename); fflush(stdout);}

  if((i = findFile(filename)) == -1) {
    if (numDataFiles >= MAXFILES) {
      printf("Already have the maximum of %d data files.\n",MAXFILES);
      exit(1);
    }
    strcpy(dataFiles[numDataFiles].name,filename);
    nLinesWords(filename,&lines,&words);
    dataFiles[numDataFiles].num = words / (float)(numInputs+numOutputs);
    if (dataFiles[numDataFiles].num*(numInputs+(float)numOutputs) != (float)words) {
      printf("For file %s, number of words not divisible by num inputs plus \
num outputs.\n",dataFiles[numDataFiles].name);
      printf(" %d Words  %d Inputs %d Outputs %d rows\n",
	     words,numInputs,numOutputs,dataFiles[numDataFiles].num);
      exit(1);
    }
    if (debug) {
      printf("%s l %d w %d num %d\n",dataFiles[numDataFiles].name,lines,words,
       dataFiles[numDataFiles].num);
      fflush(stdout);
    }

    readPatterns(&dataFiles[numDataFiles]);

#ifdef HAmMER
    if (hammer) {
      if (highPrecision)
	sprintf(string,"cv -i%%f -o%%I16 < %s > %s.cv",filename,filename);
      else
	sprintf(string,"cv -i%%f -o%%j8 < %s > %s.cv",filename,filename);
      mysystem(string);
/*      strcpy(dataFiles[numDataFiles].binName,strcat(filename,".cv"));*/
      strcpy(dataFiles[numDataFiles].binName,filename);
      strcat(dataFiles[numDataFiles].binName,".cv");

      if (debug) {
	printf("just converted is %s\n",dataFiles[numDataFiles].binName);
	fflush(stdout);
      }

    }
#endif
    numDataFiles++;
  }
}

/**********************************************************************/

void usage(void) {
  printf("Usage: %s spec-file <hammer>\n\
             Example of a spec-file:\n\
-ninputs 1 -noutputs 1 (these must appear before file names)\n\
-nhiddens1 5 -nhiddens2 5 (-nhiddens2 n is optional)\n\
-train one.data two.data (list of one or more files to compose train set)\n\
-validate three.data (optional. list of one or more for validate set)\n\
-test four.data (list of one or more files for test set)\n\
-orate 0.001 -hrate 0.1 (for hammer, hrate is at most 15*orate)\n\
-mom .9 -epochs 1000\n\
-summarize   (optional. to specify short output, one line per run)\n\
-end   (says end of run specification.  Now do it.)\n\
-orate 0.01 (additional runs, all unspecified values same as previous run)\n\
-end\n\
-orate 0.1\n\
-end\n",commandName);
}

/**********************************************************************
 * Place pointers to all patterns into the training, validation, or
 * testing set.
 **********************************************************************/
void loadSet(Set *set, char *files[], int nFiles, char *which) {
  int i, j, np, ret, k, jj, *ind, nleft, training;
  int *fileIndices = (int *) malloc(nFiles*sizeof(int));
  Pattern *p;
  char dest[200], string[200];

  if (strncmp(which,"train",5) == 0)
    training = 1;
  else
    training = 0;

  set->num = 0;
  for (i=0; i<nFiles; i++) {
    fileIndices[i] = findFile(files[i]);
    /*printf("findfile %s index %d\n",files[i],fileIndices[i]);*/
    set->num += dataFiles[fileIndices[i]].num;
  }

  /* Store pointers to all patterns in first file, then second file...*/
  /* Do this for all, even though when running hammer we only need this
     for the test data. */
    
  set->patterns = (Pattern **) malloc(set->num * sizeof(Pattern *));

  /* Set up indices to be selected at random while storing pattern
     pointers.  Then, when drawing patterns in order during training,
     the patterns will be in random order. */

  if (training) {
    ind = (int *) malloc(set->num * sizeof(int));
    for (i=0; i<set->num; i++)
      ind[i] = i;
/*    for (jj=0; jj<set->num; jj++) printf(" %d",ind[jj]);
    printf("\n");*/
    nleft = set->num;
  }

  np = 0;
  for (i=0; i<nFiles; i++) {
    p = dataFiles[fileIndices[i]].first;
    for (j=0; j<dataFiles[fileIndices[i]].num; j++){
      if (training) {
	k = RN * nleft;
	set->patterns[ind[k]] = p;
	ind[k] = ind[nleft-1];
	nleft--;
/*	for (jj=0; jj<nleft; jj++) printf(" %d",ind[jj]);
	printf("\n");*/
      } else {
	set->patterns[np++] = p;
      }
      p = p->next;
    }
  }

/**
  printf("Printing set\n");
  for (i=0; i<set->num; i++) {
    printf("Pattern %d ",i);
    printData(" ",set->patterns[i]);
    printf("\n");
  }
**/

#ifdef HAMMER
  if (hammer) {
    strcpy(dest," ");

    if (training) {

      /* Training set */
      strcpy(dest,"trainbptr.in");
      sprintf(string,"rm -f %s",dest);
      if (ret = bnSetConstantInteger("NumTrainVecs",set->num)) bnExit(ret);


    } else if (strncmp(which,"val",3) == 0) {

      /* Validation set.  For hammer, this is called test set */
      strcpy(dest,"testbptr.in");
      sprintf(string,"rm -f %s",dest);
      if (ret = bnSetConstantInteger("NumTestVecs",set->num)) bnExit(ret);
    }      

    if (strncmp(dest," ",1) != 0) {
      mysystem(string);
      for (i=0; i<nFiles; i++) {
/*	printf("index %d name is %s\n",fileIndices[i],dataFiles[fileIndices[i]].name);*/
	sprintf(string,"cat %s >> %s",dataFiles[fileIndices[i]].binName,dest);
/*	printf("%s\n",string);*/
	mysystem(string);
      }
      if (training) {
	/* Randomly reorder the training set vectors. */
	while (! fexists(dest)) sleep(1);
	sprintf(string,"scramble %s r%s %d %d %d",dest, dest, numInputs, 
		numOutputs, (highPrecision)?16:8);
	mysystem(string);
	sprintf(string,"r%s",dest);
	while (! fexists(string)) sleep(1);
	sprintf(string,"mv r%s %s",dest,dest); /* replace with randomized*/
	mysystem(string);
	while (! fexists(dest)) sleep(1);
	if (ret = bnSetFile("TrainInput",dest)) bnExit(ret);
      }
      else {
	if (ret = bnSetFile("TestInput",dest)) bnExit(ret);
      }
    }

  }
#endif
}

int  printData(char *s, Pattern *p) { /*"added pattern",first+j);*/
  int i;
  printf("%s ",s);
  for (i=0; i<numInputs+numOutputs; i++) {
    printf("%g ",p->data[i]);
  }
  printf("\n");
}


/**********************************************************************
 * Read all patterns from filename, place at end of Data linked list,
 * and reset data front and rear pointers.
 **********************************************************************/
void readPatterns (DataFile *dataFile) {
  FILE *file;
  int i,j;
  Pattern *p;
  float *xp, junkFloat;
  int greater=0, less=0;

  if((file = fopen(dataFile->name,"r")) == NULL) {
    printf("Couldn't open data file %s.\n",dataFile->name);
    exit(1);
  }

  for (i=0; i<dataFile->num; i++) {
    /* For each new pattern, add it to linked list of all patterns,
     * and save if first one in dataFile pointer */
    p = (Pattern *) malloc(sizeof(Pattern));
    p->data = malloc((numInputs+numOutputs)*sizeof(float));
    if (i == 0) dataFile->first = p;
    if (firstPattern == NULL) 
      firstPattern = p;
    else 
      lastPattern->next = p;
    lastPattern = p;
    xp = p->data;
    for (j=0; j<numInputs+numOutputs; j++) {
      if (fscanf(file, "%f", xp) == EOF) {
	printf("Premature end of file when reading %s.\n",dataFile->name);
	exit(1);
      }
#ifdef HAMMER
      if (hammer) {
	if (highPrecision) {
	  if (*xp > 0.5) greater++;
	  if (*xp < -0.5) less++;
	}
	else {
	  if (*xp > 1.0) greater++;
	  if (*xp < 0.0) less++;
	}
      }
#endif
      xp++;
    }
  }

  fclose(file);

#ifdef HAMMER
  if (hammer) {
    if (greater > 0)
      fprintf(stderr,"%d values larger than %f found in %s\n",greater,
	      (highPrecision)? 0.5 : 1, dataFile->name);
    if (less > 0)
      fprintf(stderr,"%d values less than %f found in %s\n",less,
	      (highPrecision)? -0.5 : 0, dataFile->name);
  }
#endif

/**
  printf("File %s\n",dataFile->name);
  p = dataFile->first;
  for (i=0; i<dataFile->num; i++) {
    for (j = 0; j<numInputs+numOutputs; j++)
      printf("%g ",p->data[j]);
    printf("\n");
    p = p->next;
  }
**/

  if (fscanf(file,"%f",&junkFloat) != EOF) {
    printf("Warning: Unread data left in %s\n",dataFile->name);
  }

/*  fprintf(stderr,"Read %d lines from %s\n",dataFile->num,dataFile->name);*/
}

/**********************************************************************/

float *sumOutVars;
float *sumTargets;
float *sumSqTargets;

void calcError(Set *set, float *wh1, float *wh2, float *wo,
	       float *rmsError, float *fracCorr, float *Rsquared, int print) {
  float set_error = 0.;
  int pattern, i, numCorr, numPatCorr;
  float *xp, *tp;
  float sum, f;
  int maxtargi, maxouti;
  float maxtarg, maxout;

  /* If not already done, allocate storage for arrays used here. */

  if (sumOutVars == NULL) {
/*    error = (float *) malloc (numOutputs * sizeof(float));*/
/*  error done by  main  */
    sumOutVars = (float *) malloc (numOutputs * sizeof(float));
    sumTargets = (float *) malloc (numOutputs * sizeof(float));
    sumSqTargets = (float *) malloc (numOutputs * sizeof(float));
  }
  for (i=0; i<numOutputs; i++) {
    sumOutVars[i] = 0.;
    sumTargets[i] = 0.;
    sumSqTargets[i] = 0.;
  }

  for (pattern=0; pattern < set->num; pattern++) {
    /*printf("pattern %d ",pattern);*/
    for (i=0; i<numOutputs; i++) {
      tp = set->patterns[pattern]->data+numInputs;
      /* printf("targ %g\n",tp[0]); fflush(stdout);*/
      sumTargets[i] += tp[i];
      sumSqTargets[i] += tp[i]*tp[i];
    }
  }

  numCorr = 0;
  numPatCorr = 0;
  for (pattern=0; pattern < set->num; pattern++) {

    xp = set->patterns[pattern]->data;
    tp = set->patterns[pattern]->data+numInputs;
    calcOutput(xp,wh1,wh2,wo);

    maxout = -1.;
    maxtarg = -1.;
    for (i=0; i<numOutputs; i++) {
      if (yo[i] > maxout) {
	maxout = yo[i];
	maxouti = i;
      }
      if (tp[i] > maxtarg) {
	maxtarg = tp[i];
	maxtargi = i;
      }
      error[i] = tp[i] - yo[i];
      set_error += error[i]*error[i] / numOutputs;
      f = yo[i] - sumTargets[i] / set->num;
      sumOutVars[i] += f*f;
      if ((tp[i] >= 0.5 && yo[i] >= 0.5) ||
	  (tp[i] < 0.5 && yo[i] < 0.5))
	numCorr++;
    }
    if (numOutputs > 1) {
      if (maxouti == maxtargi) 
	numPatCorr++;
    } else {
      if ((yo[0] > 0.5 && tp[0] > 0.5) ||
	  (yo[0] <= 0.5 && tp[0] <= 0.5))
	numPatCorr++;
    }
    
    if(print) {
      printf("pat %5d targets",pattern+1);
      for (i=0; i<numOutputs; i++)  printf(" %10f",tp[i]);
      printf(" outputs");
      for (i=0; i<numOutputs; i++)  printf(" %10f",yo[i]);
      printf(" errors");
      for (i=0; i<numOutputs; i++)  printf(" %10f",error[i]);
      printf("\n");
    }

  } /* end of pattern loop */

  sum = 0.;
  for (i=0; i<numOutputs; i++) {
    f = sumTargets[i] / set->num;
    sum += sumOutVars[i] / (sumSqTargets[i] - set->num * f * f);
  }
  *Rsquared = sum / numOutputs;

  *rmsError = sqrt(set_error/set->num);

  /* This statement counts number of output units on correct side of .5*/
  /**  *fracCorr = ((float)numCorr) / set->num / numOutputs;**/
  /*  This next statement counts number of input patterns correctly
      classified. */
  *fracCorr = ((float)numPatCorr) / set->num ;
}
/**********************************************************************/

void calcOutput(float *x, float *wh1, float *wh2, float *wo) {
  int u, i, nio;
  float sum, *wp;

  for (u=0, wp=wh1; u<numHidden1; u++) {
    sum = 0.;
    for (i=0; i<numInputs; i++)
      sum += *wp++ * x[i];
    sum += *wp++;		/* bias weight */
    yh1[u] = logistic(sum);
  }

  if (numHidden2 > 0) {
    for (u=0, wp=wh2; u<numHidden2; u++) {
      sum = 0.;
      for (i=0; i<numHidden1; i++)
	sum += *wp++ * yh1[i];
      sum += *wp++;		/* bias weight */
      yh2[u] = logistic(sum);
    }
  }

  for (u=0, wp=wo; u<numOutputs; u++) {
    sum = 0.;
    if (numHidden2 > 0) {
      for (i=0; i<numHidden2; i++)
	sum += *wp++ * yh2[i];
    } else {
      if (numHidden1 > 0) {
	for (i=0; i<numHidden1; i++)
	  sum += *wp++ * yh1[i];
      }
    }
    if (numHidden1 == 0 || InputsThrough) {
      for (i=0; i<numInputs; i++)
	sum += *wp++ * x[i];
    }
    sum += *wp++;		/* bias weight */
    if (linearOut)
      yo[u] = sum;
    else
      yo[u] = logistic(sum);  /*sum;*/
  }
}

/**********************************************************************/

void
printNet(int ni, int nh1, int nh2, int no, float *x, float *wh1, float *wh2,
	 float *wo, float *yh1, float *yh2, float *yo)
{
  int u, i, nio, nh;

  if (nh2 > 0) 
    printf("Warning: Only printing first hidden layer and output layer.\n");

  if (nh1 > 0) {
 
    /* Printing first hidden layer */

    printf("\n    ");
    for (i=0; i<ni; i++) {
      printf("\n%4.2f",x[i]);
      for (u=0; u<nh1; u++)
	printf(" %6.2f",wh1[u*(ni+1)+i]);
    }
    printf("\n    ");
    for (u=0; u<nh1; u++)
      printf(" %6.2f",wh1[u*(ni+1)+ni]);

    printf("\n    ");
    for (u=0; u<nh1; u++)
      printf(" %6.2f",yh1[u]);

    /* Print second hidden layer */

    if (nh2 > 0) {

      printf("\n\n    ");
      nio = nh1+1;
      for (u=0; u<nh2; u++) {
	for (i=0; i<nh1; i++)
	  printf(" %6.2f",wh2[u*nh1+i]);
	printf(" %6.2f  %6.2f\n",wh2[u*nh1+nh1-1],yh2[u]);
      }
    }

    /* Print output layer */

    printf("\n\n    ");
    if (nh2 > 0) {
      nio = nh2 + 1;
      nh = nh2;
    } else {
      nio = nh1+1;
      nh = nh1;
    }
    if (InputsThrough)
      nio += ni;
    for (u=0; u<no; u++) {
      for (i=0; i< nh; i++)
	printf(" %6.2f",wo[u*nio+i]);
      if (InputsThrough) 
	for (i=0; i<ni; i++)
	  printf(" %6.2f",wo[u*nio+i+nh]);
      printf(" %6.2f  %6.2f\n",wo[u*nio+nio-1],yo[u]);
    }

  } else {

    /* just single output layer */

    printf("\n    ");
    for (i=0; i<ni; i++) {
      printf("\n%4.2f",x[i]);
      for (u=0; u<no; u++)
	printf(" %6.2f",wo[u*(ni+1)+i]);
    }
    printf("\n    ");
    for (u=0; u<no; u++)
      printf(" %6.2f",wo[u*(ni+1)+ni]);

    printf("\n    ");
    for (u=0; u<no; u++)
      printf(" %6.2f",yo[u]);
    printf("\n");
  }
}

/**********************************************************************/

void updateWeights(float *x) {
  static float *deltao = NULL;
  static float *deltah1 = NULL;
  static float *deltah2 = NULL;

  int u, i, wi, nio;
  float r;

  if (deltao == NULL) deltao = (float *) malloc (numOutputs * sizeof(float));
  if (deltah1 == NULL) deltah1 = (float *) malloc (numHidden1 * sizeof(float));
  if (numHidden2 > 0)
    if (deltah2 == NULL) deltah2 = (float *) malloc (numHidden2 * sizeof(float));

  if (numHidden2 > 0)
    nio = numHidden2 + 1;
  else
    nio = numHidden1 + 1;
  if (InputsThrough)
    nio += numInputs;

  /* Deltas */

  for (i=0; i<numOutputs; i++) 
    if (linearOut)
      deltao[i] = error[i];
    else
      deltao[i] = error[i] * Dlogistic(yo[i]);

  if (numHidden2 > 0) {
    /* Two Hidden Layers */
    for (u=0; u<numHidden2; u++) {
      deltah2[u] = 0.;
      for (i=0; i<numOutputs; i++)
	deltah2[u] += deltao[i] * wo[i*nio+u] * Dlogistic(yh2[u]);
    }
    for (u=0; u<numHidden1; u++) {
      deltah1[u] = 0.;
      for (i=0; i<numHidden2; i++)
	deltah1[u] += deltah2[i] * wh2[i*numHidden1+u] * Dlogistic(yh1[u]);
    }
  } else {
    /* One Hidden Layer */
    for (u=0; u<numHidden1; u++) {
      deltah1[u] = 0.;
      for (i=0; i<numOutputs; i++)
	deltah1[u] += deltao[i] * wo[i*nio+u] * Dlogistic(yh1[u]);
    }
  }

  /* update weights */

  /* First Hidden Layer */
  for (u=0; u<numHidden1; u++) {
    for (i=0; i<numInputs; i++) {
      wi = u*(numInputs+1)+i;
/*      printf("u %d i %d inc %d\n",u,i,wi);*/
      wh1[wi] += dwh1[wi] = hRate * deltah1[u] * x[i]
	+ mom * dwh1[wi];
/*      whchange[wi] += fabs(dwh[wi]);*/
    }
    wi = u*(numInputs+1)+numInputs;
/*printf("u %d ni %d inc %d wh %f",u,ni,wi,wh[wi]);*/
/*printf(" deltah %f hRate %f dwh %f",deltah[u],hRate,dwh[wi]);*/
    wh1[wi] += dwh1[wi] = hRate * deltah1[u] + mom * dwh1[wi];
/*printf(" deltah %f hRate %f dwh %f",deltah[u],hRate,dwh[wi]);*/
/*printf(" dwh %f whnew %f\n",dwh[wi],wh[wi]);*/
/*    whchange[wi] += fabs(dwh[wi]);*/
  }

  /* Second Hidden Layer */
  for (u=0; u<numHidden2; u++) {
    for (i=0; i<numHidden1; i++) {
      wi = u*(numHidden1+1)+i;
      wh2[wi] += dwh2[wi] = hRate * deltah2[u] * yh1[i]	+ mom * dwh2[wi];
    }
    wi = u*(numHidden1+1)+numHidden1;
    wh2[wi] += dwh2[wi] = hRate * deltah2[u] + mom * dwh2[wi];
  }

  /* Output Layer */
  for (u=0; u<numOutputs; u++) {
    if (numHidden2 > 0) {
      /* Two Hidden Layers */
      for (i=0; i<numHidden2; i++) {
	wi = u*nio+i;
	wo[wi] += dwo[wi] = oRate * deltao[u] * yh2[i] + mom * dwo[wi];
/*	wochange[wi] += fabs(dwo[wi]);*/
      }
    } else if (numHidden1 > 0) {
      /* One Hidden Layers */
      for (i=0; i<numHidden1; i++) {
	wi = u*nio+i;
	wo[wi] += dwo[wi] = oRate * deltao[u] * yh1[i] + mom * dwo[wi];
/*	wochange[wi] += fabs(dwo[wi]);*/
      }
    }
    if (numHidden1 == 0 || InputsThrough) {
      for (i=0; i<numInputs; i++) {
	wi = u*nio + i + ((numHidden2>0)? numHidden2 : numHidden1);
	wo[wi] += dwo[wi] = oRate * deltao[u] * x[i] + mom * dwo[wi];
/*	wochange[wi] += fabs(dwo[wi]);*/
      }
    }
    wi = u*nio+nio-1;
    wo[wi] += dwo[wi] = oRate * deltao[u] + mom * dwo[wi];
/*    wochange[wi] += fabs(dwo[wi]);*/
  }
}
/**********************************************************************/

float logistic(float x) {
  return (1. / (1. + exp(-x)));
}

/**********************************************************************/

float Dlogistic(float y) {
    return (y * (1 - y));
}

/**********************************************************************/
/* Return the number of rows and words, as counted by wc. */

void nLinesWords (char *filename, int *rows, int *words) {
  char string[100];
  FILE *f;

  sprintf(string,"wc %s > wcout",filename);
  mysystem(string);
  sleep(2);
  f = fopen("wcout","r");
  fscanf(f,"%d %d",rows,words);
  fclose(f);
  mysystem("rm -f wcout");
}

#include <sys/types.h>
#include <sys/time.h>

/*
		Addititive congruential  random number generator
		adapted from  Hill's Computer Graphics. Tests for 
		independence of sequence (Big-R) and uniformity of 
		distribution (Chi-square) are output. The histogram 
		dist. of random numbers is also output (3/22/91). 
		A floating point number between 0 - 1 is returned.
*/
 
/* global quantities for random number generator */
#define M 714025
#define IA 1366
#define IC 150889

long   the_seed;

/*--------------------------------------------------------------*/

void
init_acgrnd_by_time()
{
  time_t time();

  the_seed = time(NULL);
}

/*-------------------------------------------------------------*/

float acgrnd() {
      static long iy,ir[98];
      static int iff=0;
      int j;
      void nrerror();

      if (the_seed < 0 || iff == 0) {
              iff=1;
              if ((the_seed=(IC-(the_seed)) % M) < 0) the_seed = -(the_seed);
              for (j=1;j<=97;j++) {
                      the_seed=(IA*(the_seed)+IC) % M;
                      ir[j]=(the_seed);
              }
              the_seed=(IA*(the_seed)+IC) % M;
              iy=(the_seed);
    }
      j=1 + 97.0*iy/M;
      if (j > 97 || j < 1) exit(0);
      iy=ir[j];
      the_seed=(IA*(the_seed)+IC) % M;
      ir[j]=(the_seed);
      return (float) iy/M;
}

/**********************************************************************
  date() returns string like    Wed Jun 26 10:55:15 1991
**********************************************************************/

char *date()
{
  time_t time(),a;
  char *ctime(), *sfront, *send;

  a = time(NULL);
  sfront = ctime(&a);
  *(sfront+24) = '\0';
/*  send = sfront;
  for (send = sfront; *send != ' ' || *(send+1) != ' '; send++) ;
  *send = '\0';
*/
  return(sfront);
}
/**********************************************************************
  mysystem("command") does unix command and then sleeps for n seconds.
  returns exit status
**********************************************************************/
int mysystem(char *s) {
  int i;
  if (debug) {printf("Doing command %s\n",s); fflush(stdout);}
  i = system(s);
/*  sleep(5);*/
  return i;
}
/**********************************************************************
  fexists("file.txt")   returns 1 if "file.txt" can be opened 'r', 0 otherwise
**********************************************************************/

int fexists(char *filename) {
  FILE *fid;

  if (debug) {printf("Does %s exist?",filename); fflush(stdout);}

  if ((fid=fopen(filename,"r")) != NULL) {
    fclose(fid);
    if (debug) { printf("  yes.\n"); fflush(stdout);}
    return 1;
  } else {
    if (debug) { printf("  no.\n"); fflush(stdout); }
    return 0;
  }
}
