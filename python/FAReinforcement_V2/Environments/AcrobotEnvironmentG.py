from numpy import *

from visual import *
from visual.graph import *


class AcrobotEnvironment:
    """
    This class implements Acrobot Environment
    @author    Jose Antonio Martin H.
    @version   1.0
    """

    title ="Acrobot Balancing"


    input_ranges  = [[-pi,pi],[-pi,pi],[-4*pi,4*pi],[-9*pi,9*pi]]
    reward_ranges = [[-1.0,1000.0]]
    deep_in = [10,10,5,5] #2^n partitions
    deep_out= [10] #2^n different partitions



    # The current real values of the state
    maxSpeed1 = 4*pi
    maxSpeed2 = 9*pi
    m1        = 1.0
    m2        = 1.0
    l1        = 1.0
    l2        = 1.0
    l1Square  = l1*l1
    l2Square  = l2*l2
    lc1       = 0.5
    lc2       = 0.5
    lc1Square = lc1*lc1
    lc2Square = lc2*lc2
    I1        = 1.0
    I2        = 1.0
    g         = 9.8
    delta_t   = 0.05



    #The number of actions.
    action_list = (-1.0 , 0.0 , 1.0)
    nactions    = len(action_list)

    #Flag which is set to true when goal was reached.
    reset = False

    # number of steps of the current trial
    steps = 0

    # number of the current episode
    episode = 0

    # do you want to show nice graphs?
    graphs = True



    def __init__(self):

        self.InitGraphs()



    def UpdateState(self):
        pass



    def GetInitialState(self):
        s = array([0, 0, 0, 0])
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


    def GetReward(self, x ):
        # r: the returned reward.
        # f: true if the car reached the goal, otherwise f is false
        y_acrobot = [0,0,0]

        theta1 = x[0]
        theta2 = x[1]
        y_acrobot[1] = y_acrobot[0] - cos(theta1)
        y_acrobot[2] = y_acrobot[1] - cos(theta2)




        #goal
        goal = y_acrobot[0] + 1.5

        #r = y_acrobot[2]
        r = -1
        f = False

        if  y_acrobot[2] >= goal:
        	#r = 10*y_acrobot[2]
            r = 0
            f = True


        return r,f


    def DoAction(self, a, x ):

        self.steps = self.steps+1
        torque      = self.action_list[a]

        # Parameters for simulation
        theta1,theta2,theta1_dot,theta2_dot = x


        for i in range(4):
            d1     = self.m1*self.lc1Square + self.m2*(self.l1Square + self.lc2Square + 2*self.l1*self.lc2 * cos(theta2)) + self.I1 + self.I2
            d2     = self.m2*(self.lc2Square+self.l1*self.lc2*cos(theta2)) + self.I2

            phi2   = self.m2*self.lc2*self.g*cos(theta1+theta2-pi/2.0)
            phi1   = -self.m2*self.l1*self.lc2*theta2_dot*sin(theta2)*(theta2_dot-2*theta1_dot)+(self.m1*self.lc1+self.m2*self.l1)*self.g*cos(theta1-(pi/2.0))+phi2

            accel2 = (torque+phi1*(d2/d1)-self.m2*self.l1*self.lc2*theta1_dot*theta1_dot*sin(theta2)-phi2)
            accel2 = accel2/(self.m2*self.lc2Square+self.I2-(d2*d2/d1))
            accel1 = -(d2*accel2+phi1)/d1



        
    	    theta1_dot = theta1_dot + accel1*self.delta_t

    	    if theta1_dot<-self.maxSpeed1:
                theta1_dot=-self.maxSpeed1

    	    if theta1_dot>self.maxSpeed1:
                theta1_dot=self.maxSpeed1


    	    theta1     =  theta1 + theta1_dot*self.delta_t
    	    theta2_dot =  theta2_dot + accel2*self.delta_t


    	    if theta2_dot<-self.maxSpeed2:
                theta2_dot=-self.maxSpeed2

    	    if theta2_dot>self.maxSpeed2:
                theta2_dot=self.maxSpeed2

    	    theta2 = theta2 + theta2_dot*self.delta_t



##	if theta1<-pi:
##	    theta1 = -pi
##
##        if theta1>pi:
##            theta1 = pi

        if theta2<-pi:
            theta2 = -pi

        if theta2>pi:
            theta2 = pi



        xp = [theta1,theta2,theta1_dot,theta2_dot]

        if self.graphs:
            if not self.scene.visible:
                self.scene.visible=True
            self.PlotFunc(xp,torque,self.steps)

        return xp







    #------------------------------------ PLOT FUNCTIONS -----------------------------


    def InitGraphs(self):

        x_width  = 400
        y_height = 300

        self.scene = display(x=0, width=x_width, height=y_height,
                             title="Acrobot",visible=False)

        self.scene.autoscale=0

        theta1 = 0
        theta2 = 0
        x_acrobot = [0,0,0]
        y_acrobot = [0,0,0]

        x_acrobot[0]=0
        y_acrobot[0]=0

        x_acrobot[1] = x_acrobot[0] + sin(theta1)*2
        y_acrobot[1] = y_acrobot[0] - cos(theta1)*2

        x_acrobot[2] = x_acrobot[1] + sin(theta2)*2
        y_acrobot[2] = y_acrobot[1] - cos(theta2)*2


        self.r1    = sphere(diplsay = self.scene,pos=(0,0,0), radius=0.3)
        self.r2    = sphere(diplsay = self.scene,pos=(x_acrobot[1],y_acrobot[1],0), radius=0.3)
        self.r3    = sphere(diplsay = self.scene,pos=(x_acrobot[2],y_acrobot[2],0), radius=0.3)

        self.link1 = curve(diplsay = self.scene,pos=[self.r1.pos,self.r2.pos], radius=0.1)
        self.link2 = curve(diplsay = self.scene,pos=[self.r2.pos,self.r3.pos], radius=0.1)

        self.top   = arrow(diplsay = self.scene,pos=(0,6,0), axis=(0,-2,0), shaftwidth=1)





    def PlotFunc(self,s,a,steps):

        theta1 = s[0]
        theta2 = s[1]

        x_acrobot = [0,0,0]
        y_acrobot = [0,0,0]

        x_acrobot[0]=0
        y_acrobot[0]=0

        x_acrobot[1] = x_acrobot[0] + sin(theta1)*2
        y_acrobot[1] = y_acrobot[0] - cos(theta1)*2

        x_acrobot[2] = x_acrobot[1] + sin(theta2)*2
        y_acrobot[2] = y_acrobot[1] - cos(theta2)*2


        self.r2.pos = (x_acrobot[1],y_acrobot[1],0)
        self.r3.pos = (x_acrobot[2],y_acrobot[2],0)

        self.link1.pos = [self.r1.pos,self.r2.pos]
        self.link2.pos = [self.r2.pos,self.r3.pos]

         #update display
        #self.scene.title = "Steps: "+str(steps)+ " theta1: "+str(round(theta1,2)) + " theta2: "+str(round(theta2,2))+ "-- Altitude: "+str(round(y_acrobot[2],2))


        rate(1000)




