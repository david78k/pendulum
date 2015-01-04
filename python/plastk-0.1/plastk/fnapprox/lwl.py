"""
Locally Weighted Learning


$Id: lwl.py 229 2008-03-03 04:38:00Z jprovost $
"""

import numpy
import numpy.linalg as linalg
from numpy import array,transpose
from numpy import concatenate as join

import plastk.base.rand as rand
import plastk.misc.vectordb as vectordb

from plastk.base import params
from plastk.base.fnapprox import FnApprox
from plastk.misc.utils import inf,stats,simple_gaussian_kernel


class LocalLearner(FnApprox):
    """
    Abstract base class for locally weighted learning.
    """
    input_weights = params.List(default=[])

    db = params.ClassSelector(vectordb.VectorDB,default=vectordb.VectorTree())

    def __init__(self,**params):
        super(LocalLearner,self).__init__(**params)
        if self.input_weights:
            self.input_weight_vec = numpy.array(self.input_weights)
        else:
            self.input_weight_vec = 1.0

    def learn_step(self,X,Y):
        X *= self.input_weight_vec
        self.db.add(X, Y)



class LocallyWeightedLearner(LocalLearner):

    bandwidth = params.Number(default=1.0)

    ###############
    
    def __call__(self,X,error=False):

        h = self.bandwidth
        X = X * self.input_weight_vec

        XYs,dists = self._query(X)

        if not XYs:
            if error:
                N = self.num_outputs
                XYs = [(X,numpy.zeros(N))]
                dists = [inf]
            else:
                return None
        
        weights = self.kernel(numpy.array(dists)/h)
        self.verbose("Combining",len(weights),"points.")
        result,err =  self._combine(X,
                                    [x for x,y in XYs],
                                    [y for x,y in XYs],
                                    weights)

        if error:
            return result,err
        else:
            return result

    def _query(self,X):
        h = self.bandwidth
        return self.db.find_in_radius(X, h*2)

    def _combine(self,q,Xs,Ys,weights):
        raise NYI

    def kernel(self,x):
        return simple_gaussian_kernel(x)

    
class LocallyWeightedAverage(LocallyWeightedLearner):

    def _combine(self,q,Xs,Ys,weights):
        return weighted_average(Ys,weights)



def weighted_average(Ys,weights):
    sumw = sum(weights)

    if sumw == 0.0:
        N = len(Ys[0])
        avg = numpy.zeros(N)
        err = numpy.array([inf]*N)
    else:
        avg = numpy.sum( [y*w for y,w in zip(Ys, weights)] ) / sumw
        var = sum( [w*(y-avg)**2 for y,w in zip(Ys,weights)] ) / sumw
        err = numpy.sqrt(var)/numpy.sqrt(sumw)

    return avg,err
    

LWA = LocallyWeightedAverage

class LocallyWeightedLinearRegression(LocallyWeightedLearner):
    """
    Locally Weighted Linear Regression algorithm taken from
    Atkeson, Moore, and Schall, "Locally Weighted Learning"
    """

    ridge_range = params.Number(default=0.0)

    def _combine(self,q,Xs,Ys,weights):
        q = array(q)
        X = array(Xs)

        rows,cols = X.shape
        
        if rows < cols:
            self.verbose("Falling back to weighted averaging.")
            return weighted_average(Ys,weights)
        
        Y = array(Ys)
        W = numpy.identity(len(weights))*weights
        Z = numpy.dot(W,X)
        v = numpy.dot(W,Y)

        if self.ridge_range:
            ridge = numpy.identity(cols) * rand.uniform(0,self.ridge_range,(cols,1))
            Z = join((Z,ridge))
            v = join((v,numpy.zeros((cols,1))))
            

        B,residuals,rank,s = linalg.lstsq(Z,v)

        if len(residuals) == 0:
            self.verbose("Falling back to weighted averaging.")
            return weighted_average(Ys,weights)
        
        estimate = numpy.dot(q,B)

        # we estimate the variance as the sum of the
        # residuals over the squared sum of the weights
        variance = residuals/sum(weights**2)

        stderr = numpy.sqrt(variance)/numpy.sqrt(sum(weights))

        return estimate,stderr
        
LWLR = LocallyWeightedLinearRegression        


class KNearestLearner(LocallyWeightedLearner):
    k = params.Integer(default=3,bounds=(1,None))

    def _query(self,X):
        return self.db.k_nearest(X,self.k)

class KNearestLWA(KNearestLearner,LWA): pass
class KNearestLWLR(KNearestLearner,LWLR): pass


class InputFilter(LocallyWeightedLearner):

    change_threshold = params.Magnitude(default=0.1)

    def learn_step(self,X,Y):

        h = self.bandwidth
        X = X * self.input_weight_vec

        XYs, dists = self._query(X)

        if not XYs:
            super(InputFilter,self).learn_step(X,Y)
        else:
            W1 = self.kernel(numpy.array(dists)/h)
            Xs = [x for x,y in XYs]
            Ys = [y for x,y in XYs]

            result1,err1 = self._combine(X,Xs,Ys,W1)

            W2 = numpy.concatenate((W1,[self.kernel(0)]))
            Xs.append(X)
            Ys.append(Y)

            result2,err2 = self._combine(X,Xs,Ys,W2)

            if (abs(err2-err1)/err1 > self.change_threshold
                or abs(result2-result1)/result1 >self.change_threshold):

                self.verbose("Adding point",(X,Y),".")
                super(InputFilter,self).learn_step(X,Y)
            else:
                self.verbose("Not adding point",(X,Y),"insufficient change.")


class FilteredLWA(InputFilter,LocallyWeightedAverage): pass
class FilteredKNLWA(InputFilter,KNearestLWA): pass
class FilteredLWLR(InputFilter,LocallyWeightedLinearRegression): pass
class FilteredKNLWLR(InputFilter,KNearestLWLR): pass            
            
