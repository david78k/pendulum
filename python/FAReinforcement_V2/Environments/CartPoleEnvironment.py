from numpy import *
from math import radians

class CartPoleEnvironment:
    """
    This class implements CartPole Environment
    @author    Jose Antonio Martin H.
    @version   1.0
    """

    title ="CartPole Balancing"


    input_ranges  = [[-4.5,4.5],[-3,3],[-radians(45),radians(45)],[-radians(15),radians(15)]]
    output_ranges = [[-1.0,1.0]]
    reward_ranges = [[-20000.0,10.0]]
    deep_in = [5,5,10,5] #2^n partitions
    deep_out= [10] #2^n different partitions


    
    # The current real values of the state
   
    
    #The number of actions.
    action_list = linspace(-1.0,1.0,11)
    nactions    = len(action_list)
    #Flag which is set to true when goal was reached.
    reset = False

    # number of steps of the current trial
    steps = 0
    
    # number of the current episode
    episode = 0

    # do you want to show nice graphs?
    graphs = True
    

    def UpdateState(self):
        pass
        
        

    def GetInitialState(self):
        x         = 0.0
        x_dot     = 0.0
        theta     = 0.0
        theta_dot = 0.01
        s = array([x, x_dot, theta, theta_dot])
        self.StartEpisode()

        return  s
    
    def StartEpisode(self):
        self.steps   = 0
        self.episode = self.episode + 1
        
       
        
        

    def getState(self):
         """
          Returns the current situation.
          A situation can be the current perceptual inputs, a random problem instance ...
         """
         pass
         
     
         
    def GetReward(self, s ):
        # r: the returned reward.
        # f: true if the car reached the goal, otherwise f is false

        #[x,x_dot,theta,theta_dot] = s
        x           = s[0]
        x_dot       = s[1]
        theta       = s[2]
        theta_dot   = s[3]

        r = 10 - 10*abs(10*theta)**2 - 5*abs(x) - 10*theta_dot
        f = False


        twelve_degrees     = radians(12) # 12 degrees
        fourtyfive_degrees = radians(45) # 45 degrees

        #if (x < -4.5 or x > 4.5  or theta < -twelve_degrees or theta > twelve_degrees):
        if x < -4.5 or x > 4.5  or theta < -fourtyfive_degrees or theta > fourtyfive_degrees:
            r = -10000 - 50*abs(x) - 100*abs(theta)
            f = True;

        return r,f

    def GetRewardX(self, s ):
        # r: the returned reward.
        # f: true if the car reached the goal, otherwise f is false

        #[x,x_dot,theta,theta_dot] = s
        x           = s[0]
        x_dot       = s[1]
        theta       = s[2]
        theta_dot   = s[3]

        r = 2.0
        f = False


        twelve_degrees     = radians(12) # 12 degrees
        fourtyfive_degrees = radians(45) # 45 degrees

        #if (x < -4.5 or x > 4.5  or theta < -twelve_degrees or theta > twelve_degrees):
        if x < -4.5 or x > 4.5  or theta < -fourtyfive_degrees or theta > fourtyfive_degrees:
            r = -1000.0
            f = True;

        return r,f


    def DoAction(self, a, x ):
        #  Cart_Pole:  Takes an action (0 or 1) and the current values of the
        #  four state variables and updates their values by estimating the state
        #  TAU seconds later.

        action      = self.action_list[a]

        # Parameters for simulation
        x,x_dot,theta,theta_dot = x
        
        self.steps = self.steps+1


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


    

    
    
    

    

    

        


    

        
        
