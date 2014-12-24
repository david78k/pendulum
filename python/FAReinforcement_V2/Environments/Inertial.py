from numpy import *
from numpy.random import *
from visual import *
from math import *

LIM_X0=0.0000000000000000000000000000000000000000001
class InertMachine:
    def __init__(self,maxangle=radians(45),maxaceleration=1,
                 pinit=(0,0,0),direction=vector(0,1,0),L=1.0):

        self.L  = L # differential longitudinal axes
        self.x  = array(pinit)
        self.dx = self.x * 0.0
        self.v  = 0
        self.a  = 0
        self.direction = array(direction)
        self.maxangle  = maxangle
        self.maxaceleration = maxaceleration
        self.theta  = 0.0
        self.dtheta = 0.0

    def update(self):
        self.v   = self.v + self.a
        self.x   = self.x + self.v * self.direction
        self.a   = 0

    def setac(self,theta,aceleration):

        if abs(theta) > self.maxangle:
            theta = sign(theta) * self.maxangle

        #theta = (self.maxangle/2.0) * tanh(10000*theta)*1.3

        if abs(aceleration) > self.maxaceleration:
            aceleration = sign(aceleration) * self.maxaceleration


        self.dtheta = theta-self.theta
        self.theta  = theta
        phi         = self.v * tan(theta) / self.L

        #self.dtheta = phi-self.theta
        #self.theta  = phi

        self.direction = rotate(self.direction, angle=phi, axis=(0,0,1))
        #x=0
        #y=1
        #alpha = 1.75
        #xp = x*cos(alpha)+y*sin(alpha)
        #yp = y*cos(alpha)-x*sin(alpha)


        #if self.v + aceleration < 0:
        #    aceleration = 0

        self.a = aceleration


class Body:
    def __init__(self,maxangle=radians(45),maxaceleration=1,
                 pinit=(0,0,0),pcolor=(0,0,1),direction=(0,1,0),L=1.0):

        self.state = InertMachine(maxangle,maxaceleration,pinit,direction,L)

        # parte visual
        self.body       = box(pos=pinit, axis=(0.1,0.1,0),length=L, height=L/5.0, width=0.2,color=pcolor)
        self.pointer    = arrow(pos=pinit, axis=(0,0.1,0), shaftwidth=1,color=pcolor)
        self.trajectory = curve(color=pcolor)


    def update(self):

        self.state.update()

        #pinta el movil
        self.body.pos     = self.state.x
        self.body.axis    = self.state.direction*self.state.L/3.0
        self.body.lentgth = self.state.L

        self.pointer.pos  = self.state.x
        self.pointer.axis = self.state.direction*self.state.L/3.0

        self.trajectory.append(pos=self.state.x)

    def setac(self,theta,aceleration):
        self.state.setac(theta,aceleration)



