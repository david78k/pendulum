from numpy import *
from numpy.random import normal
import random as rnd

rnd.seed(0)

class e_greedy_selection:
    def __init__(self,epsilon):
        self.epsilon = epsilon
        self.parent=None

    def __call__(self,s):
        return self.select_action(s)

    def select_action(self,s):
        #selects an action using Epsilon-greedy strategy
        # s: the current state
        v = self.parent.Q(s)

        if (rnd.random()>self.epsilon):
            a = argmax(v)
        else:
            # selects a random action based on a uniform distribution
            a  = rnd.randint(0,self.parent.nactions-1)

        return a,v[a]


class e_softmax_selection:
    def __init__(self,epsilon):
        self.epsilon = epsilon
        self.parent=None

    def __call__(self,s):
        return self.select_action(s)

    def select_action(self,s):
        #selects an action using a kind of soft-max strategy
        # s: the current state
        # this thend to the greedy action when epsilon tends to zero.
         
         v        = array(self.parent.Q(s))
         mag      = sqrt(sum(v**2))
         # if the summatory of Q(s)^2 is zero then all Q-values are zero
         # thus select one action uniformly random
         if mag == 0:
             a = rnd.randint(0,self.parent.nactions-1)
             return a,v[a]

         
         vn       = v / mag                         # normalize values to unitary vector
         normals  = normal(0,self.epsilon,vn.shape)  # random perturbation following epsilon
         vn       = vn + normals
         a        = argmax(v)                       # action whose value is larger

         return a,v[a]


