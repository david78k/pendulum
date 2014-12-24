from numpy import *
from numpy.random import *
from math import *
from Inertial import *

class Agente(Body):
    def __init__(self,maxangle=radians(45),maxaceleration=1,
                 pinit=(0,0,0),pcolor=(0,0,1),direction=vector(0,1,0),L=1.0):
        # Inicializador del Body
        Body.__init__(self,maxangle,maxaceleration,pinit,pcolor,direction,L)

        self.J1      = 0.0
        self.dJ1     = 0.0
        self.theta   = 0.0

    def react(self,target,action=1):

        ant=target.state

        Jk       =  action * 0.5 *  sum(((self.state.x + self.state.direction * 10.0) - ant.x)**2)

        #print Jk
        self.dJ1 = Jk  - self.J1
        self.J1  = Jk

        if (self.state.dtheta)!=0:
            self.theta    =  - 0.9 * ( self.dJ1 / (self.state.dtheta))
        else:
            self.theta    = 0.000000000000000001



        #omega    =   (acos(dot(self.state.direction,ant.direction)))
        #phi      =   omega


        phi      =  self.theta

        self.setac(phi,0.0)










