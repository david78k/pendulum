"""
Base vector quantizer class.

$Id: vq.py 229 2008-03-03 04:38:00Z jprovost $
"""

import params, rand

class VQ(params.ParameterizedObject):
    """
    The abstract vector quantizer class.  A vector quantiser must
    iomplement these methods.
    """
    def present_input(self,X):
        raise NotImplementedError
    def train(self,X):
        raise NotImplementedError
    def winner(self):
        raise NotImplementedError
    def get_model_vector(self,index):
        raise NotImplementedError
    def num_model_vectors(self):
        raise NotImplementedError
    def get_activation(self):
        raise NotImplementedError


class VQTrainingMemory(VQ):

    memory_size = params.Integer(default = 1000, bounds=(1,None))

    def __init__(self,**args):

        super(VQTrainingMemory,self).__init__(**args)
        self._mem = []
        
    def train(self,X,**kw):
        self._mem.append((X,kw))
        
        N = len(self._mem)
        i = rand.randrange(N)
        self.debug("Training on memory element",i)
        V,training_kw = self._mem[i]
        super(VQTrainingMemory,self).train(V,**training_kw)

        if N > self.memory_size:
            self._mem.pop(0)
            
        self.present_input(X)
            
        
