#include "mex.h"
#include "math.h"
/*
	Computes typical EPSP kernel K(s) = 1/(1-ts/tm)*(exp(-s/tm)-exp(-s/ts)) .* (s>0)
	Where ts is synapse time constant, tm is membrane time constant
	Should be much faster than doing it in matlab function.

	Syntax:
	kernel = pspKernel(sValues,ts,tm)

	Timothee Masquelier timothee.masquelier@alum.mit.edu, Feb 2007

 */

void
mexFunction(	int nlhs, mxArray *plhs[],
				int nrhs, const mxArray *prhs[] )
{
    double *s = mxGetPr(prhs[0]);
    double ts = *mxGetPr(prhs[1]);
    double tm = *mxGetPr(prhs[2]);

    double *K, coef;

	int* dim = mxGetDimensions( prhs[0] );
	int n = dim[1], i;

	plhs[0] = mxCreateDoubleMatrix(1, n, mxREAL);
	K = mxGetPr(plhs[0]);

	coef = 1/(1-ts/tm);

	for(i=0; i<n; i++) {
		if( *s <= 0 )
			*K = 0;
		else
			*K = coef * (exp(-*s/tm)-exp(-*s/ts));
        s++;
        K++;
	}
}
