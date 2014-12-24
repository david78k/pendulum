

# interface that express the function approximator for
# Reinforcement Learning Algorithms.

class FARL:

    def __call__(self,s,a=None):
        """ implement here the returned Qvalue of state (s) and action(a)
            e.g. Q.GetValue(s,a) is equivalent to Q(s,a)
        """
        if a==None:
            return self.GetValue(s)

        return self.GetValue(s,a)


    def GetValue(self,s,a):
        """ Return the Q value of state (s) for action (a)

        """
        raise NotImplementedError



    def Update(self,s,a,v):
        """ update action value for action(a)

        """
        raise NotImplementedError

    def UpdateAll(self,s,v):
        """ update action value for action(a)

        """
        raise NotImplementedError

    def HasPopulation(self):
        raise NotImplementedError

    def Population(self):
        raise NotImplementedError

    def Addtrace(self,*args):
        pass




