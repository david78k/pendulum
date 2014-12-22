#include "mex.h"
#include <math.h>
#include "param.c"
/* #include "stdio.h" */


/* 	function [neuron]=STDPContinuous(neuron,spikeList,afferentList,dump,beSmart,PARAM)
 *
 *  This code was orinally developped for:
 *  Masquelier T, Guyonneau R, Thorpe SJ (2009).Competitive STDP-based Spike Pattern Learning. Neural Comp 21(5), 1259-1276 doi:10.1162/neco.2008.06-08-804
 *  And adapted to memristor-based STDP (optional) in:
 *  Zamarreño-Ramos C, Camuñas-Mesa LA, Perez-Carrasco JA, Masquelier T, Serrano-Gotarredona T and Linares-Barranco B (2011). On Spike-Timing-Dependent-Plasticity, Memristive Devices, and building a Self-Learning Visual Cortex. Front Neurosci, 5:26 doi:/10.3389/fnins.2011.00026
 *  And later used in:
 *  Masquelier T (2012). Relative spike time coding and STDP-based orientation selectivity in the early visual system in natural continuous and saccadic vision: a computational model. J Comput Neurosc.
 *
 *  timothee.masquelier@alum.mit.edu  
 *
 *  Feel free to use and modify but please cite us if appropriate.
 *
 *  It is an event-driven implementation, particularly efficient if the input spike rate is low.
 *  Neuron model: Spike Response Model (Gerstner et al., 1993)
 *  STDP model:
 *      * nearest-spike approximation
 *      * 2 modes:
 *          classic:
 *              additive weight updates
 *              classic exponential window
 *          memristor: (see Camunas-Mesa et al.)
 *              quadratic weight dependance
 *              window: linear region when t_pre - t_post is in [-t_op, t_on], otherwise exponential
 *
 *  All the synaptic weight are dumped periodically (every 1/100th of the total simulated time) in ASCII files in ../mat/ (the directory must exist)
 *
 *  Arguments:
 *      neuron: structure array with fields:
 *          weights (double): array of synaptic weights (size=number of afferents)
 *          epspAmplitude (double): array of EPSP magnitudes (only zeros if starting from the scratch)
 *          epspTime (double): array of EPSP times (only zeros if starting from the scratch)
 *          epspAfferent (uint16): array of EPSP afferent indexes (only zeros if starting from the scratch)
 *          nEpsp (double): current number of EPSP (which can be bigger that the size of the 3 above mentioned arrays. We use cycling indexes and erase old (no longer efficient) EPSP). Zero if starting from the scratch.
 *          ipspTime (double): array of IPSP times (only zeros if starting from the scratch)
 *          nIpsp (double): current number of IPSP (which can be bigger that the size of the ipspTime array. We use cycling indexes and erase old (no longer efficient) IPSP). Zero if starting from the scratch.
 *          nextFiring (double): next scheduled firing time, taken the current EPSPs into account. Inf if starting from the scratch.
 *          firingTime (double): array of past firing times (only zeros if starting from the scratch).
 *          nFiring (double): number of stored past firing times in the above mentioned array (which must be bigger). Zero if starting from the scratch.
 *          alreadyDepressed (logical): array of flags saying if a given afferent has already been depressed (nearest spike approximation for LTD)(size=number of afferents). All false if starting from the scratch.
 *          maxPotential (double): upper bound on potential wich could be reached with the current EPSPs. Avoid unnecessary computation if threshold is not reachable. Zero if starting from the scratch.
 *          trPot (double): MUST BE 0 (not used any longer)
 *      spikeList (double): array of input spike times in s.
 *      afferentList (uint16): array of input spike afferent indexes.
 *      dump (logical): if true will dump potential as a function of time in a file called dump.txt
 *      beSmart (logical): save time by not computing the potential when it is estimated that the threshold cannot be reached. Rigorous only when no inhibition, and no PSP Kernel. Otherwise use at your own risks...
 *      PARAM: structure with fields:
 *          stdp_t_pos (double): tau^+ STDP time constant in s (for LTP)
 *          stdp_t_neg (double): tau^- STDP time constant in s (for LTD)
 *          stdp_a_pos (double): a^+ STDP constant (for LTP)
 *          stdp_a_neg (double): a^- STDP constant (for LTD). Should be <0
 *          stdp_cut (double): time delay (in number of time constants) for STDP modifications to be considered negligible (eg 7)
 *          minWeight (double): lower bound for weight (usually 0, however when using memristors, max restistance / min resistance ~ 10, therefore minWeight ~0.1).
 *          memristor (logical): use or not memristor-based STDP with quadratic weight dependance, and linear region when t_pre - t_post is in [-t_op, t_on]
 *          t_op (double): limit of the linear LTP region
 *          t_on (double): limit of the linear LTD region
 *          tmpResolution (double): temporal resolution in s
 *          epspKernel (double): array containing the EPSP kernel (should be scaled so that the max is 1)
 *          epspMaxTime (double): index of the max in above mentioned kernel
 *          usePssKernel (logical): use or not the post synaptic spike kernel (negative spike after potential that follows the pulse)
 *          pssKernel (double): array containing the PSS kernel
 *          ipspKernel (double): array containing IPSP kernel (positive, by convention. should be scaled so that the max is 1)
 *          inhibStrength (double): inhibition strength (in fraction of threshold. positive, by convention)
 *          nAfferent (double): number of afferents
 *          threshold (double): neurons' threshold (arbitrary units)
 *          refractoryPeriod (double): refractory period in s, during which the neuron is not allowed to fire (and does not integrate the EPSPs)
 *          fixedFiringMode (logical): use or not the fixed firing mode, in which periodic firing is imposed
 *          fixedFiringLatency (double): in fixedFiringMode specify the latency of the first firing
 *          fixedFiringPeriod (double): in fixedFiringMode specify the period of the first firing
 */


const int nNeuronField = 13;

const	char *neuronField[13] = {
		"weight",
		"epspAmplitude",
		"epspTime",
		"epspAfferent",
		"nEpsp",        
		"ipspTime",
		"nIpsp",        
   		"nextFiring",
		"firingTime",
		"nFiring",
		"alreadyDepressed",
		/*"nInefficient",*/
		/*"storedPotential",*/
		/*"nStoredPotential",*/
		"maxPotential",
		/*"currentPotential",*/
		"trPot",
};

typedef struct tag_neuron {
        double  *weight;
        double  *epspAmplitude;
        double  *epspTime;
        unsigned short  *epspAfferent;
        double	*nEpsp;
		int	NEPSP;
        
        double  *ipspTime;
        double	*nIpsp;
		int	NIPSP;
        
        double  *nextFiring;
        double  *firingTime;
        double	*nFiring;
		int	NFIRING;
        bool	*alreadyDepressed;
        /*double	*nInefficient;*/
        /*double  *storedPotential;*/
        /*double	*nStoredPotential;*/
        double  *maxPotential;
        double  currentPotential;
        double  *trPot;
} NEURON,*NEURONS;

NEURON matlabToC_neuron_byIdx(mxArray *matlabNeuron, int idx) {
	NEURON neuron;
	int i;
	mxArray *field;

	for(i=0; i<nNeuronField; i++) {
		field = mxGetField(matlabNeuron,idx,neuronField[i]);

		if(field==NULL) { /* missing neuron field*/
			mexPrintf("Missing neuron field:");
			mxErrMsgTxt(neuronField[i]);
		}

		switch(i) {
			case 0:
				neuron.weight = mxGetPr(field);
				break;
			case 1:
				neuron.epspAmplitude = mxGetPr(field);
				neuron.NEPSP = (int) mxGetN(field);
				break;
			case 2:
				neuron.epspTime = mxGetPr(field);
				break;
			case 3:
				neuron.epspAfferent = (unsigned short*) mxGetPr(field);
				break;
			case 4:
				neuron.nEpsp = mxGetPr(field);
				break;
			case 5:
				neuron.ipspTime = mxGetPr(field);
				neuron.NIPSP = (int) mxGetN(field);
				break;
			case 6:
				neuron.nIpsp = mxGetPr(field);
				break;                
			case 7:
				neuron.nextFiring = mxGetPr(field);
				break;
			case 8:
				neuron.firingTime = mxGetPr(field);
				neuron.NFIRING = (int) mxGetN(field);
				break;
			case 9:
				neuron.nFiring =  mxGetPr(field);
				break;
			case 10:
				neuron.alreadyDepressed = (bool*) mxGetPr(field);
				break;
 			/*//case 9:
 			//	neuron.nInefficient = mxGetPr(field);
  			//	break;
			//case 10:
			//	neuron.storedPotential = mxGetPr(field);
			//	break;
			//case 11:
			//	neuron.nStoredPotential = mxGetPr(field);
			//	break;*/
			case 11:
				neuron.maxPotential = mxGetPr(field);
				break;
			/*//case 13:
			//	neuron.currentPotential = mxGetPr(field);
			//	break;*/
			case 12:
				neuron.trPot = mxGetPr(field);
				break;                         
		} 
	}
	return neuron;
}
NEURON matlabToC_neuron(mxArray *matlabNeuron) {
	return matlabToC_neuron_byIdx(matlabNeuron,0);
}

NEURONS matlabToC_neurons(mxArray *matlabNeuron) {
	int nNeuron = (int) mxGetN(matlabNeuron);
	NEURONS neurons = mxMalloc(nNeuron*sizeof(NEURON));
	int i;
	for(i=0; i<nNeuron; i++)
		neurons[i] = matlabToC_neuron_byIdx(matlabNeuron,i);

	return neurons;
}



double local_max(double x1, double x2) {
    if(x1>x2)
        return x1;
    return x2;
	/* return x1>x2 ? x1 : x2; */
}
double local_min(double x1, double x2) {
    if(x1<x2)
        return x1;
    return x2;
	/* return x1<x2 ? x1 : x2; */
}
double maxArray(int n, double *x) {
	int i;
	double result = -mxGetInf();
	for(i=0; i<n; i++)
		if(x[i]>result)
			result = x[i];
	return result;
}
int i_round(double x) {
	return (int) (x+.5);
}

void LTD(NEURON *neuron, double spikeTime, unsigned short afferent, PARAM *param) {

	double lastFiringTime;

	if(*((*neuron).nFiring)==0)	/* nothing to do if the neuron has never fired*/
		return;
	if((*neuron).alreadyDepressed[afferent]) /* consider only pairs of spikes */
		return;

	lastFiringTime = (*neuron).firingTime[((int)(*(*neuron).nFiring)-1)%(*neuron).NFIRING];

	if(spikeTime-lastFiringTime < 0) /* debug */
		mexErrMsgTxt("Inserting and EPSP anterior to last firing. This should not happen");

	if(spikeTime-lastFiringTime > (*param).stdp_cut * (*param).stdp_t_neg) /* outside time window */
		return;

    if(!(*param).memristor)    
        (*neuron).weight[afferent] += (*param).stdp_a_neg * exp(-(spikeTime-lastFiringTime) / (*param).stdp_t_neg);    
    else { /*This is what memristors implement*/
        if(spikeTime-lastFiringTime < (*param).t_on)
            (*neuron).weight[afferent] += (*neuron).weight[afferent]*(*neuron).weight[afferent] * (spikeTime-lastFiringTime) * (*param).stdp_a_neg * exp(-(*param).t_on / (*param).stdp_t_neg) / (*param).t_on;
        else
            (*neuron).weight[afferent] += (*neuron).weight[afferent]*(*neuron).weight[afferent] * (*param).stdp_a_neg * exp(-(spikeTime-lastFiringTime) / (*param).stdp_t_neg);
    }
	(*neuron).weight[afferent] = local_max( (*param).minWeight, (*neuron).weight[afferent] );
	(*neuron).alreadyDepressed[afferent] = true;
}

void LTP(NEURON *neuron, double firingTime, PARAM *param) {
	bool *alreadyPotentiated = mxCalloc((*param).nAfferent,sizeof(bool));
	int e;
	for(e=(int)*(*neuron).nEpsp;e>=1;e--) {
        
        
		double epspTime = (*neuron).epspTime[(e-1)%(*neuron).NEPSP];
		unsigned short epspAfferent = (*neuron).epspAfferent[(e-1)%(*neuron).NEPSP];   

		if(firingTime-epspTime < 0) { /* debug */
			mexPrintf("\nfiringTime=%f epspTime=%f e=%d nEpsp=%d nFiring=%d\n",firingTime,epspTime,e,(int)*(*neuron).nEpsp,(int)*(*neuron).nFiring);
			mxErrMsgTxt("LTP and firingTime < epspTime. This should not happen");
		}

		if(firingTime-epspTime > (*param).stdp_cut * (*param).stdp_t_pos ) /* outside firingTime window */
			break; /* no nead to do earlier epsp */
		
		if(!alreadyPotentiated[epspAfferent]) {/* consider only pairs of spikes*/
            if(!(*param).memristor)
                (*neuron).weight[epspAfferent] += (*param).stdp_a_pos * exp(-(firingTime-epspTime)/(*param).stdp_t_pos);
        
            else { /*This is what memristors implement*/            
                if(firingTime-epspTime < (*param).t_op)
                    (*neuron).weight[epspAfferent] += (*neuron).weight[epspAfferent]*(*neuron).weight[epspAfferent] * (firingTime-epspTime) * (*param).stdp_a_pos * exp(-(*param).t_op/(*param).stdp_t_pos) / (*param).t_op;                
                else
                    (*neuron).weight[epspAfferent] += (*neuron).weight[epspAfferent]*(*neuron).weight[epspAfferent] * (*param).stdp_a_pos * exp(-(firingTime-epspTime)/(*param).stdp_t_pos);
            }
			(*neuron).weight[epspAfferent] = local_min(1,(*neuron).weight[epspAfferent]);
			alreadyPotentiated[epspAfferent] = true;
		}
	}

	mxFree(alreadyPotentiated);
}

void computePot(NEURON *neuron, double time, PARAM *param) {
    /* compute evolution of pot over [time,time+PARAM.epspMaxTime] */
    
	int e,i,shift;

	double *period,*potential,w;



	period = mxMalloc((*param).nPeriod*sizeof(double));
	for(i=0;i<(*param).nPeriod;i++)
		period[i] = time + i * (*param).tmpResolution;



	potential = mxCalloc((*param).nPeriod,sizeof(double));


	/* sum EPSP */
/* 	for(e=*(*neuron).nEpsp-1; e>*(*neuron).nInefficient-1; e--) { */
	for(e=*(*neuron).nEpsp-1; e>=0; e--) {
        
        /* approximate epsp */
        shift = i_round((period[0]-(*neuron).epspTime[e%(*neuron).NEPSP])/(*param).tmpResolution);
        
        if(shift < 0) /* debug */
            mexErrMsgTxt("Integrating an EPSP posterior to current time. This should not happen");

        if(shift >= (*param).nEpspKernel ) /* now reaching unefficient (too old) EPSP */
            break;

        w = (*neuron).epspAmplitude[e%(*neuron).NEPSP];
        if(w==0) /* we can skip it*/
            continue;
        
        for(i=0; i< local_min((*param).nPeriod,(*param).nEpspKernel-shift); i++) 
            potential[i] += w * (*param).epspKernel[shift+i];
	}    
	/* debug */
	if( *(*neuron).nEpsp-1 - e >= .9*(*neuron).NEPSP) {
        mexPrintf("Current EPSP array size = %d\n",(*neuron).NEPSP);
        mexPrintf("*(*neuron).nEpsp-1 - e = %f\n", *(*neuron).nEpsp-1 - e );        
        mexPrintf("nEpsp = %f\n",*(*neuron).nEpsp);        
        mexPrintf("e = %d\n",e);
        mexPrintf("Shift = %d\n",shift);
        mexPrintf("nEpspKernel = %d\n",(*param).nEpspKernel);
		mxErrMsgTxt("Risk of overwriting non-neglible EPSPs. Increase EPSP array size.");
    }

    /* sum IPSP */
    if((*param).inhibStrength>0) {
        for(e=*(*neuron).nIpsp-1; e>=0; e--) {

            /* compute shift */
            shift = i_round((period[0]-(*neuron).ipspTime[e%(*neuron).NIPSP])/(*param).tmpResolution);

            if(shift < 0) /* debug */
                mexErrMsgTxt("Integrating an IPSP posterior to current time. This should not happen");

            if(shift >= (*param).nIpspKernel ) /* now reaching unefficient (too old) IPSP */
                break;

            for(i=0; i< local_min((*param).nPeriod,(*param).nIpspKernel-shift); i++)
                potential[i] -= (*param).inhibStrength * (*param).threshold * (*param).ipspKernel[shift+i];
        }
        /* debug */
        if( *(*neuron).nIpsp-1 - e >= .9*(*neuron).NIPSP) {
            mexPrintf("Current IPSP array size = %d",(*neuron).NIPSP);
            mxErrMsgTxt("Risk of overwriting non-neglible IPSPs. Increase IPSP array size");
        }
    }
    
    /* add PSS */
    if((*param).usePssKernel)
        if(*(*neuron).nFiring>0) {
            shift = i_round((period[0]-(*neuron).firingTime[((int)*(*neuron).nFiring-1)%(*neuron).NFIRING])/(*param).tmpResolution);
            
            if(shift < 0) /* debug */
                mexErrMsgTxt("Adding a postsynaptic spike after potential posterior to current time. This should not happen");
            
            for(i=0; i< local_min((*param).nPeriod,(*param).nPssKernel-shift); i++)
                potential[i] += (*param).threshold * (*param).pssKernel[shift+i];
        }
    
    (*neuron).currentPotential = potential[0];


	*(*neuron).maxPotential = maxArray((*param).nPeriod,potential); /* potential that is going to be reached in a near future */

    if( *(*neuron).maxPotential>=(*param).threshold+*(*neuron).trPot && /* theshold is reached somewhere */
    (*(*neuron).nFiring==0 || time-(*neuron).firingTime[((int)*(*neuron).nFiring-1)%(*neuron).NFIRING] >= (*param).refractoryPeriod) ) {/* not in refractory period: */
        
        for(i=0;i<(*param).nPeriod; i++){ /* for now simple sequential search */
            if(potential[i] >= (*param).threshold+*(*neuron).trPot) {
                    /* truncate */
                    /* *(*neuron).nextFiring = period[i];  */
                
                    /* interpolate */
                if(i==0)
                    *(*neuron).nextFiring = period[i];
                else
                    *(*neuron).nextFiring = period[i]-(period[i]-period[i-1])/(potential[i]-potential[i-1])*(potential[i]-(*param).threshold-*(*neuron).trPot);
                    /* *(*neuron).nextFiring = local_max(time,period[i]-(period[i]-period[i-1])/(potential[i]-potential[i-1])*(potential[i]-(*param).threshold-*(*neuron).trPot)); /* note: max is there to avoid scheduling an event in the past due to rounding pb */                    
                break;
            }
        }
    }
    else
        *(*neuron).nextFiring = mxGetInf();   
    
    
    mxFree(period);
	mxFree(potential);

}
void integrate(NEURON *neuron, double time, unsigned short afferent, PARAM *param, bool beSmart) {

	int cursor;
    
    /* are we in refractory period?*/
    bool refr = ( *(*neuron).nFiring>0 && time-(*neuron).firingTime[((int)*(*neuron).nFiring-1)%(*neuron).NFIRING] < (*param).refractoryPeriod );

    if((*neuron).weight[afferent]>0 || (*param).stdp_a_pos!=0) { /*otherwise no need to keep track of this incoming spike*/
        *((*neuron).nEpsp)+=1;
        cursor = ((int)*(*neuron).nEpsp-1)%(*neuron).NEPSP;
        (*neuron).epspAmplitude[cursor] = (!refr) * (*neuron).weight[afferent];   /* note: clamping during the refractory period*/
        (*neuron).epspTime[cursor] = time;
        (*neuron).epspAfferent[cursor] = afferent;
    }
    if((*param).stdp_a_neg!=0) /*do LTD only if useful*/
    	LTD(neuron,time,afferent,param);

	if((*param).fixedFiringMode) /*  no need to integrate with this special mode */
		return;

	/* check if threshold is reachable */
	if(beSmart)
		if(*(*neuron).maxPotential + (*neuron).weight[afferent] < (*param).threshold+*(*neuron).trPot){ /* threshold not reachable even with most optimistic scenario*/
			*(*neuron).maxPotential += (!refr) * (*neuron).weight[afferent]; /* optimistic max */
			return;
		}
    
    computePot(neuron,time,param);
}



void flush(NEURON *neuron) {
	/* clear epsp */
	*(*neuron).nEpsp = 0;
/* 	*(*neuron).nInefficient = 0; */
    *(*neuron).maxPotential = 0;
/* 	*(*neuron).currentPotential = mxGetNaN(); */
    *(*neuron).nIpsp = 0;
	*(*neuron).nextFiring = mxGetInf();
}

void fire(NEURON *neuron, double time, PARAM *param) {
	int i;
    
    /*w_out*/
    /*for(i=0; i<(*param).nAfferent;i++) {
		(*neuron).weight[i] += -.3*(*param).stdp_a_pos;
        (*neuron).weight[i] = local_max( (*param).minWeight, (*neuron).weight[i] );
    }*/
    
    if((*param).stdp_a_pos!=0) /*do LTP only if useful*/
    	LTP(neuron,time,param);

	/* postsynaptique spike count */
	*(*neuron).nFiring += 1;

	/* postsynaptique spike times */
	(*neuron).firingTime[((int)*(*neuron).nFiring-1)%(*neuron).NFIRING] = time;

	/* clear epsp */
	flush(neuron);	    
    
	/* remove alreadyDepressed flags */
	for(i=0; i<(*param).nAfferent;i++)
		(*neuron).alreadyDepressed[i] = false;

	/* fixed firing mode: schedule next firing */
	if((*param).fixedFiringMode) {
		*(*neuron).nextFiring = i_round(1000*(time+(*param).fixedFiringPeriod))*1.0/1000;
		/* mexPrintf("next firing scheduled for: %f",*(*neuron).nextFiring); */
	}


	/*if(*((*neuron).nFiring)>20)
		if(time - (*neuron).firingTime[(int)*((*neuron).nFiring)-21] < .200)
			mexErrMsgTxt("Bursting (postsynaptic firing rate > 100 Hz for more than 200 ms)");*/
    

}


void
mexFunction(	int nlhs, mxArray *plhs[],
				int nrhs, const mxArray *prhs[] )
{
	if(nrhs==6) {

	/* variables */
	mxArray *output = mxDuplicateArray(prhs[0]); /* neurons */
	double t,*spikeList = mxGetPr(prhs[1]), nextFiring = mxGetInf();
	unsigned short i,j, nNeuron = mxGetN(prhs[0]), nextOneToFire;
    unsigned long s,nSpikeList = mxGetN(prhs[1]);
	unsigned short *afferentList = (unsigned short *) mxGetPr(prhs[2]); /* afferentList */
   	bool dump = *mxGetLogicals(prhs[3]);
   	bool beSmart = *mxGetLogicals(prhs[4]);
	PARAM param = matlabToC_param(prhs[5]); /* param */
	FILE *file,*wFile;
	NEURONS neuron = matlabToC_neurons(output);	/* get neurons from matlab structure */
    int cursor;
    char wFileName[50];

	/* array size to compute evolution of pot over [time,time+PARAM.epspMaxTime] */
	param.nPeriod = ceil( param.epspMaxTime / param.tmpResolution )+1;

	/* affects output */
	plhs[0] = output;

	if(dump){ /* Dumping mode. Open file and write header. */
		mexPrintf("Dumping data in dump.txt\n");
		file = fopen("dump.txt", "at");
		/* fprintf(file, "time        potential\n"); */
	}

	for(s=0; s<nSpikeList; s++)  { /* main loop */

        if(s%i_round(local_max(1,nSpikeList/100.0))==0) {
            mexPrintf(".");
            if(param.stdp_a_pos!=0 || param.stdp_a_neg!=0) {
                sprintf(wFileName,"../mat/weight.t=%011.3fs.txt",t);
                wFile = fopen(wFileName, "w");        
                for(i=0; i<nNeuron; i++) {
                    for(j=0; j<param.nAfferent; j++) {
                        fprintf(wFile," %05.3f, ",neuron[i].weight[j]);
                    }
                    fprintf(wFile,"\n");              
                } 
                fclose(wFile);
            }
        }

		if(nextFiring <= spikeList[s]) { /* next event is a postsynaptic spike */

			/* move on */
            t = nextFiring;

			/* the winner fires */
            fire(&neuron[nextOneToFire],t,&param);

			/* the others are inhibited */
            if(param.inhibStrength>0)
                for(i=0; i<nNeuron; i++)
                    if(i!=nextOneToFire) {                    
                        /* inhib via IPSP*/
                        *(neuron[i].nIpsp)+=1;
                        cursor = ((int)*(neuron[i].nIpsp)-1)%neuron[i].NIPSP;
                        neuron[i].ipspTime[cursor] = t;
                    }

			/* remove scheduled firing */
			nextFiring = mxGetInf();
        }

        /* move to next event, which is a presynaptic spike */
        t = spikeList[s];

        /* integrate this spike */
		for(i=0; i<nNeuron; i++) {
			integrate(&neuron[i],t,afferentList[s],&param,beSmart);
			if(!mxIsInf(*(neuron[i].nextFiring)))
				if(*(neuron[i].nextFiring)<nextFiring) {
					nextOneToFire = i;
					nextFiring = *(neuron[i].nextFiring);
				}
		}
                 
        
		if(dump) {
			fprintf(file,"%011.5f",t);
			for(i=0; i<nNeuron; i++)
				fprintf(file," %08.2f",neuron[i].currentPotential);
			fprintf(file,"\n");
		}


	} /* main loop */

	
    mexPrintf("\n");

	/*plhs[0] = CToMatlab_neuron(neuron); */

	if(dump)
		fclose(file);



	}
	else
		mexErrMsgTxt("syntax : [neuron]=mainC(neuron,spikeList,afferentList,dump,beSmart,PARAM). See C file for info.");


}
