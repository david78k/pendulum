from FAInterface import FARL
from numpy import *
from numpy.random import *
from numpy.linalg import *
import time
#from scikits.ann import kdtree
#from scikits.ann2 import kdtree
#from pyflann import *

class kNNQ(FARL):

    def __init__(self,nactions,input_ranges,nelemns=[],npoints=0,k=1,alpha=0.3,lm=0.95):
        if  not (nelemns==False)^(npoints==False):
            raise ValueError,'Plese indicate either: [nelemns] Xor [npoints]'

        if nelemns:
            self.cl = self.ndlinspace(input_ranges,nelemns)
        else:
            self.cl = self.CreateRandomSpace(input_ranges,npoints)

        self.lbounds = []
        self.ubounds = []

        self.k          = k
        self.shape      = self.cl.shape
        self.nactions   = nactions
        self.Q          = zeros((self.shape[0],nactions))

        #self.Q         = uniform(-100,0,(self.shape[0],nactions))+0.0
        self.e          = zeros((self.shape[0],nactions))+0.0
        #self.ac         = zeros((self.shape[0]))+0.0 #classifiers activation
        self.ac         = []
        self.knn        = []
        self.alpha      = alpha
        self.lm         = lm #good 0.95
        self.last_state = zeros((1,self.shape[1]))+0.0




        for r in input_ranges:
            self.lbounds.append(r[0])
            self.ubounds.append(r[1])



        self.lbounds = array(self.lbounds)
        self.ubounds = array(self.ubounds)
        self.cl = array (self.RescaleInputs(self.cl))


        #self.knntree = kdtree(self.cl)



    def ndlinspace(self,input_ranges,nelems):
        """ ndlinspace: n-dimensional linspace function
            input_ranges = [[-1,1],[-0.07,0.07]]
            nelems = (5,5)
        """
        #x = array(list(ndindex(*nelems)),float)+1.0
        x = indices(nelems).T.reshape(-1,len(nelems))+1.0
        lbounds = []
        ubounds = []
        from_b  = array(nelems,float)
        for r in input_ranges:
            lbounds.append(r[0])
            ubounds.append(r[1])


        lbounds = array(lbounds,float)
        ubounds = array(ubounds,float)

        y = (lbounds) + (((x-1)/(from_b-1))*((ubounds)-(lbounds)))


        return y

    def Load(self,strfilename):
        self.Q = load(strfilename)

    def Save(self,strfilename):
        save(strfilename,self.Q)

    def ResetTraces(self):
        self.e = zeros((self.shape[0],self.nactions))+0.0


    def RescaleInputs(self,s):
        return self.ScaleValue(array(s),self.lbounds,self.ubounds,-1.0,1.0)

    def ScaleValue(self,x,from_a,from_b,to_a,to_b):
        return (to_a) + (((x-from_a)/(from_b-from_a))*((to_b)-(to_a)))


    def CreateRandomSpace(self,input_ranges,npoints):
        d = []
        x = array([])
        for r in input_ranges:
            d.append( uniform(r[0],r[1],(npoints,1)))


        return concatenate(d,1)



    def GetkNNSet(self,s):
        if allclose(s,self.last_state) and self.knn!=[]:
            return self.knn

        self.last_state = s
        state   = self.RescaleInputs(s)


        self.d   = sum((self.cl-state)**2,1)
        self.knn = self.d.argsort()[0:self.k] # find indices of the knn
        self.d   = self.d[self.knn]


        self.ac  = 1.0/(1.0+self.d) # calculate the degree of activation
        self.ac /= sum(self.ac)
        return self.knn


    def CalckNNQValues(self,M):
        Qvalues = dot(transpose(self.Q[M]),self.ac)
        return Qvalues

    def GetValue(self,s,a=None):
        """ Return the Q value of state (s) for action (a)
        """
        M = self.GetkNNSet(s)

        if a==None:
            return self.CalckNNQValues(M)


        return self.CalckNNQValues(M)[a]


    def Update(self,s,a,v,gamma=1.0):
        """ update action value for action(a)
        """

        M = self.GetkNNSet(s)


        #cumulating traces
        #self.e[M,a] = self.e[M,a] +  self.ac[M].flatten()

        #replacing traces
        self.e[M]   =  0.0
        self.e[M,a] =  self.ac

        TD_error    =  v - self.GetValue(s,a)
        self.Q +=  self.alpha * (TD_error) * self.e
        #add(self.Q, self.alpha * (TD_error) * self.e,self.Q)
        self.e *= self.lm


    def HasPopulation(self):
        return True

    def Population(self):
        pop = self.ScaleValue(self.cl,-1.0,1.0,self.lbounds,self.ubounds)
        for i in range(self.shape[0]):
            yield pop[i]





