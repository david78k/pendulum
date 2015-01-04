"""
Linear Function Approximator

$Id: linear.py 230 2008-03-03 04:53:52Z jprovost $
"""
from plastk.base import params
from plastk.base import rand
from plastk.base.fnapprox import FnApprox
from plastk.misc.utils import norm

from numpy import zeros,dot,transpose,identity,rank,reshape,array,ones
from numpy import concatenate as join
from numpy import linalg

class LinearFnApprox(FnApprox):

    num_inputs = params.Integer(default=1,bounds=(1,None))
    num_outputs = params.Integer(default=1,bounds=(1,None))
    alpha = params.Magnitude(default=0.1)

    def __init__(self,**params):
        super(LinearFnApprox,self).__init__(**params)
#        self.w = zeros((self.num_outputs,self.num_inputs)) * 1.0
        self.w = rand.uniform(-1,1,(self.num_outputs,self.num_inputs)) * 1.0
        
    def __call__(self,input,error=False):
        X = self.scale_input(input)
        if not error:
            Y = dot(self.w,X)
        else:
            Y = dot(self.w,X), zeros(self.num_outputs)
        return self.scale_output(Y)
        

    def learn_step(self,input,output):

        if rank(input) == 1:
            input = reshape(input,(self.num_inputs,1))
        if rank(output) == 1:
            output = reshape(output,(self.num_outputs,1))
            
        result = self(input)    
        err = output - result

        self.MSE = norm(err.ravel()**2)/self.num_outputs
        self.debug("MSE =",self.MSE)

        alpha = self.alpha/sum(input**2)
        self.w += alpha*transpose(dot(input,transpose(err)))
        self.debug( "update ratio =", norm(self(input)-result)/norm(err))



# JPALERT: This should be called "AffineFnApprox"

class LinearFnApproxWithBias(FnApprox):

    num_inputs = params.Integer(default=1,bounds=(1,None))
    num_outputs = params.Integer(default=1,bounds=(1,None))
    alpha = params.Magnitude(default=0.1)

    def __init__(self,**params):

        super(LinearFnApproxWithBias,self).__init__(**params)

        self._fn = LinearFnApprox(num_inputs = self.num_inputs+1,
                                  num_outputs = self.num_outputs,
                                  range = self.range,
                                  alpha = self.alpha)

    def __call__(self,input,error=False):

        if rank(input) == 1:
            X = join((input,[1]))
        else:
            X = join((input,[[1]]))
            
            
        return self._fn(X,error=error)

    def learn_step(self,input,output):
        
        if rank(input) == 1:
            input = reshape(input,(len(input),1))
        X = join((input,[[1]]))

        self._fn.learn_step(X,output)


class BatchLinearFnApprox(FnApprox):

    use_bias = params.Boolean(default = False)
    
    def learn_batch(self,data):

        X = array([x for x,y in data])
        Y = array([y for x,y in data])

        if self.use_bias:
            X = join((X,ones((len(X),1))),axis=1)
        W,residuals,rank,s = linalg.lstsq(X,Y)

        self.w = W

    def __call__(self,X):

        if self.use_bias:
            X = join((X,[1]))
        return dot(X,self.w)

