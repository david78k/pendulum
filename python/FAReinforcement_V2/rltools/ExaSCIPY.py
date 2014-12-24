from FAInterface import FARL
from numpy import *
from numpy.random import *
from numpy.linalg import *
import time

from scipy.spatial import cKDTree

class Exa(FARL):

    def __init__(self,input_ranges,nelemns_in,action_ranges,nelems_a,k=1,alpha=0.3,lm=0.95,epsilon=0):


        self.cl = self.ndlinspace(input_ranges,nelemns_in)
        self.actions = linspace(action_ranges[0][0],action_ranges[0][1],nelems_a[0])
        self.lbounds = []
        self.ubounds = []

        self.k          = k
        self.shape      = self.cl.shape
        self.nactions   = len(self.actions)
        self.Q          = zeros((self.shape[0],self.nactions))
        #self.Q          = uniform(-10,10,(self.shape[0],self.nactions))
        self.e          = zeros((self.shape[0],self.nactions))+0.0
        self.ac         = zeros((k))
        self.knn        = []
        self.alpha      = alpha
        self.lm         = lm #good 0.95
        self.epsilon    = epsilon
        self.last_state = zeros((1,self.shape[1]))+0.0


        for r in input_ranges:
            self.lbounds.append(r[0])
            self.ubounds.append(r[1])


        self.lbounds = array(self.lbounds)
        self.ubounds = array(self.ubounds)
        self.cl = array (self.RescaleInputs(self.cl))

        self.knntree = cKDTree(self.cl)




    def ndtuples(self,*dims):
       """Fast implementation of array(list(ndindex(*dims)))."""

       # Need a list because we will go through it in reverse popping
       # off the size of the last dimension.
       dims = list(dims)

       # N will keep track of the current length of the indices.
       N = dims.pop()

       # At the beginning the current list of indices just ranges over the
       # last dimension.
       cur = arange(N)
       cur = cur[:,newaxis]

       while dims != []:
           d = dims.pop()
           # This repeats the current set of indices d times.
           # e.g. [0,1,2] -> [0,1,2,0,1,2,...,0,1,2]
           cur = kron(ones((d,1)),cur)
           # This ranges over the new dimension and 'stretches' it by N.
           # e.g. [0,1,2] -> [0,0,...,0,1,1,...,1,2,2,...,2]
           front = arange(d).repeat(N)[:,newaxis]
           # This puts these two together.
           cur = column_stack((front,cur))
           N *= d

       return cur

    def ndlinspace(self,input_ranges,nelems):
        x = self.ndtuples(*nelems)+1.0
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


    def ResetTraces(self):
        self.e = zeros((self.shape[0],self.nactions))+0.0


    def RescaleInputs(self,s):
        return self.ScaleValue(array(s),self.lbounds,self.ubounds,-1.0,1.0)

    def ScaleValue(self,x,from_a,from_b,to_a,to_b):
        return (to_a) + (((x-from_a)/(from_b-from_a))*((to_b)-(to_a)))


    def GetkNNSet(self,s):
        #if allclose(s,self.last_state) and self.knn!=[]:
        #    return self.knn

        self.last_state = s
        state   = self.RescaleInputs(s)


        self.d, self.knn = self.knntree.query(state, self.k, eps=0.0, p=2)
        self.d *= self.d



        self.ac  = 1.0/(1.0+self.d) # calculate the degree of activation
        self.ac /= sum(self.ac)
        return self.knn

    def CalcmaxQValue(self,knn):
        maxQa   = self.Q[knn].max(1)
        Qvalue  = dot(maxQa,self.ac)
        return Qvalue

    def GetActionList(self,s):
        knn           = self.GetkNNSet(s)
        max_actions   = self.Q[knn].argmax(1)
        rnd_dist      = random(max_actions.shape)>self.epsilon
        rnd_actions   = randint(0,5,max_actions.shape)
        actionlist    = where(rnd_dist,max_actions,rnd_actions)

        actionvalues  = self.actions[actionlist]
        action        = dot(actionvalues,self.ac)
        return action,actionlist



    def GetValue(self,s):
        """ Return the Q value of state (s) for action (a)
        """
        knn = self.GetkNNSet(s)
        return self.CalcmaxQValue(knn)


    def Update(self,s,a,v,gamma=1.0):
        """ update action value for action(a)
        """
        knn = self.GetkNNSet(s)

        self.e[knn]   =  0.0

        #cumulating traces
        #self.e[knn,a] += self.ac

        #replacing traces
        self.e[knn,a] =  self.ac


        TD_error    =  v - self.GetValue(s)
        self.Q +=  self.alpha * (TD_error) * self.e
        self.e *= self.lm


    def HasPopulation(self):
        return True

    def Population(self):
        pop = self.ScaleValue(self.cl,-1.0,1.0,self.lbounds,self.ubounds)
        for i in range(self.shape[0]):
            yield pop[i]





