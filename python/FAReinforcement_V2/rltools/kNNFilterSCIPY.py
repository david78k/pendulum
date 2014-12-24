from numpy import *
from numpy.random import *
from numpy.linalg import *
from scipy.spatial import cKDTree

class kNNFilter():

    def __init__(self,nactions,input_ranges,nelemns=[],npoints=0,k=1,alpha=0.3):
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
        
        self.ps         = zeros((self.shape[0],nactions,self.shape[1]))
        self.ps_exp     = zeros((self.shape[0],nactions))
        self.ac         = []
        self.knn        = []
        self.alpha      = alpha
         
        self.last_state = zeros((1,self.shape[1]))+0.0
        self.next_state = array(self.last_state)
       

        
        
        for r in input_ranges:
            self.lbounds.append(r[0])
            self.ubounds.append(r[1])


        
        self.lbounds = array(self.lbounds)
        self.ubounds = array(self.ubounds)
        self.cl = array (self.RescaleInputs(self.cl))
        self.knntree = cKDTree(self.cl,100)
        

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

        self.d, self.knn = self.knntree.query(state, self.k, eps=0.0, p=2)
        self.d *= self.d

        self.ac  = 1.0/(1.0+self.d) # calculate the degree of activation
        self.ac /= sum(self.ac)
        return self.knn


   
    def FilterState(self,s,a,sp):
        knn  = self.GetkNNSet(s)
        Predicted_sp = dot(transpose(self.ps[knn,a]),self.ac)
        delta_sp = sp-Predicted_sp
        self.ps[knn,a] += self.alpha*reshape(self.ac,(self.k,1)) * delta_sp
        self.ps_exp[knn,a]+=1
        if any(self.ps_exp[knn,a]<1000):
            return sp
        #print "filtering"
        return dot(transpose(self.ps[knn,a]),self.ac)
        
    def FilterStateD(self,s,a,sp):        
        knn  = self.GetkNNSet(s)
        #Predicted_ds = dot(transpose(self.ps[knn,a]),self.ac)
        delta_ds = (sp-s)-self.ps[knn,a] #978
        #delta_ds = (sp-s)-dot(transpose(self.ps[knn,a]),self.ac)
        self.ps[knn,a] += self.alpha*reshape(self.ac,(self.k,1)) * delta_ds
        self.ps_exp[knn,a]+=1
        if any(self.ps_exp[knn,a]<250):
            return sp
        #print "filtering"
        return s + dot(transpose(self.ps[knn,a]),self.ac)

    def FilterStateAvg(self,s,a,sp):
        knn  = self.GetkNNSet(s)
        #Predicted_sp = dot(transpose(self.ps[knn,a]),self.ac)

        N = reshape(self.ps_exp[knn,a],(self.k,1))
        w = reshape(self.ac,(self.k,1))
              
        self.ps[knn,a] = ((N-1.0)*self.ps[knn,a] + w*(sp-s))/(N+w)
        
        self.ps_exp[knn,a]+=1
        if any(self.ps_exp[knn,a]<500):
            return sp
        
        return s+dot(transpose(self.ps[knn,a]),self.ac)

   
       
   

    
        

