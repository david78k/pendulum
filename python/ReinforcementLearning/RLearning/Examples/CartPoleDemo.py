# -*- coding: cp1252 -*-
from math import radians 
from Numeric import *
from RandomArray import *
from RLearning.RLBasic import *
import psyco
psyco.full()




class CartPole(RLBase):
   

    def BuildActionList(self):
        return arange(-1.0,1.1,0.1)
        
    
    def BuildSateList(self):
        
        # state discretization for the mountain car problem
        #x1div = (2.0-(-2.0)) / 3.0
        #x2div = (0.1-(-0.1)) / 2.0
        x3div = (radians(15)-(radians(-15)))/8.0
        #x4div = (radians(10)-(radians(-10)))/2.0

        
        x1  = [-1.0,  1.0]       # x position
        x2  = [-1.0, 0.0, 1.0]     # x_dot
        x3  = arange(radians(-15),radians(15)+x3div,x3div) #theta 
        x4  = [-1.0,  1.0]        # theta dot


        I=size(x1)
        J=size(x2)
        K=size(x3)
        L=size(x4)        
        states =[]
        index=0
        for i in range(I):    
            for j in range(J):
                for k in range(K):
                    for l in range(L):
                        states.append([x1[i],x2[j],x3[k],x4[l]])                        
        
        return array(states)

    def GetReward(self, s ):
        # r: the returned reward.
        # f: true if the car reached the goal, otherwise f is false

        #[x,x_dot,theta,theta_dot] = s
        x           = s[0]
        x_dot       = s[1]
        theta       = s[2]
        theta_dot   = s[3]
        
        r = 20 - 10*abs(10*theta)**2 - 5*abs(x) - 10*theta_dot
        f = False
        

        twelve_degrees     = radians(12) # 12 degrees
        fourtyfive_degrees = radians(45) # 45 degrees
        
        #if (x < -4.5 or x > 4.5  or theta < -twelve_degrees or theta > twelve_degrees):
        if x < -4.5 or x > 4.5  or theta < -fourtyfive_degrees or theta > fourtyfive_degrees:
            r = -100000 - 50*abs(x) - 100*abs(theta)   
            f = True;
                        
        return r,f

    

    def DoAction(self, action, x ):
        #  Cart_Pole:  Takes an action (0 or 1) and the current values of the
        #  four state variables and updates their values by estimating the state
        #  TAU seconds later.

        # Parameters for simulation
        x,x_dot,theta,theta_dot = x


        g               = 9.8      #Gravity
        Mass_Cart       = 1.0      #Mass of the cart is assumed to be 1Kg
        Mass_Pole       = 0.1      #Mass of the pole is assumed to be 0.1Kg
        Total_Mass      = Mass_Cart + Mass_Pole
        Length          = 0.5      #Half of the length of the pole 
        PoleMass_Length = Mass_Pole * Length
        Force_Mag       = 10.0
        Tau             = 0.02     #Time interval for updating the values
        Fourthirds      = 4.0/3.0

        force = action * Force_Mag

        temp     = (force + PoleMass_Length * theta_dot * theta_dot * sin(theta)) / Total_Mass
        thetaacc = (g * sin(theta) - cos(theta) * temp) / (Length * (Fourthirds - Mass_Pole * cos(theta) * cos(theta) / Total_Mass))
        xacc     = temp - PoleMass_Length * thetaacc * cos(theta) / Total_Mass
         
        # Update the four state variables, using Euler's method.
        x         = x + Tau * x_dot
        x_dot     = x_dot + Tau * xacc
        theta     = theta + Tau * theta_dot
        theta_dot = theta_dot+Tau*thetaacc

        xp = [x, x_dot, theta, theta_dot]
        
        return xp


    def GetInitialState(self):
        x         = 0.0
        x_dot     = 0.0
        theta     = 0.0
        theta_dot = 0.01

        s = array([x, x_dot, theta, theta_dot])
          
        return  s        
  


def CartPoleDemo(maxepisodes):
    CP  = CartPole(0.3,1.0,0.001)
    maxsteps = 1000
    grafica  = False
    
    
    
    for i in range(maxepisodes):    
    
        total_reward,steps  = CP.SARSAEpisode( maxsteps, grafica )
        #total_reward,steps  = CP.QLearningEpisode( maxsteps, grafica )
        
        print 'Espisode: ',i,'  Steps:',steps,'  Reward:',str(total_reward),' epsilon: ',str(CP.epsilon)
        
        CP.epsilon = CP.epsilon * 0.99
              
               
        
if __name__ == '__main__':
    CartPoleDemo(1000)              
