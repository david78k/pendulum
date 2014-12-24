from numpy import *

from visual import *
from visual.graph import *


class CartPoleEnvironment:
    """
    This class implements CartPole Environment
    @author    Jose Antonio Martin H.
    @version   1.0
    """

    title ="CartPole Balancing"


    input_ranges  = [[-4.5,4.5],[-3,3],[-radians(45),radians(45)],[-radians(25),radians(25)]]
    reward_ranges = [[-20000.0,10.0]]
    output_ranges = [[-1.0,1.0]]
    deep_in = [5,5,10,5] #2^n partitions
    deep_out= [10] #2^n different partitions



    # The current real values of the state


    #The number of actions.
    action_list = linspace(-1,1,21)
    nactions    = len(action_list)
    #Flag which is set to true when goal was reached.
    reset = False

    # number of steps of the current trial
    steps = 0

    # number of the current episode
    episode = 0

    # do you want to show nice graphs?
    graphs = False



    def __init__(self):

        #if self.graphs:
        self.InitGraphs()



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
        if self.graphs:
            self.ResetGraphs()





    def getState(self):
         """
          Returns the current situation.
          A situation can be the current perceptual inputs, a random problem instance ...
         """
         pass



    def GetReward(self, s,a=0):
        # r: the returned reward.
        # f: true if the car reached the goal, otherwise f is false

        #[x,x_dot,theta,theta_dot] = s
        x           = s[0]
        x_dot       = s[1]
        theta       = s[2]
        theta_dot   = s[3]

        r = 10 - 10*abs(10*theta)**2 - 10*abs(x) - 10*theta_dot - 0*(abs(a/10.0))
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


        r = 0
        f = False


        twelve_degrees     = radians(12) # 12 degrees
        fourtyfive_degrees = radians(45) # 45 degrees

        #if (x < -4.5 or x > 4.5  or theta < -twelve_degrees or theta > twelve_degrees):
        if x < -4.5 or x > 4.5  or theta < -fourtyfive_degrees or theta > fourtyfive_degrees:
            r = -1
            f = True;

        return r,f


    def DoAction(self, a, x ):
        #  Cart_Pole:  Takes an action (0 or 1) and the current values of the
        #  four state variables and updates their values by estimating the state
        #  TAU seconds later.

        action      = a
        #print "action",a

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

        if self.graphs:
            if not self.scene.visible:
                self.scene.visible=True
            self.PlotFunc(xp,action,self.steps)

        return xp







    #------------------------------------ PLOT FUNCTIONS -----------------------------


    def InitGraphs(self):

        x_width  = 400
        y_height = 300

        self.scene = display(x=0, width=x_width, height=y_height,
                             title="Cart Pole Demo",visible=False)

        self.scene.autoscale=0


        self.trwindow  = gdisplay(x=x_width, y=0, width=x_width, height=y_height,
                        title="State Space Trajectory", xtitle='Cart position', ytitle='Pole angle radians',
                        xmin=-4.5/10,xmax=4.5/10, ymin=-radians(45)/10,ymax=radians(45)/10,
                        foreground=color.black, background=color.white)



        self.awindow  = gdisplay(x=x_width*2, y=0, width=150, height=y_height,
                        title="Actions", ytitle='Force',
                        xmin=0.0,xmax=2.0, ymax=1.0, ymin=-1.0,
                        foreground=color.black, background=color.white)


        #self.trwindow.display.visible =False
        #self.awindow.display.visible =False


        #state space
        #trajectory
        self.Gtrajectory = gcurve(gdisplay=self.trwindow,color=color.blue)




        # classifiers
        self.clgraph = gdots(gdisplay=self.awindow)


        #The cart is a Frame
        self.Frame_CartPole = frame(display=self.scene)
        self.Car = box(frame=self.Frame_CartPole,pos=(0,0,0), length=2, height=1, width=1,color=color.orange, material=materials.wood)
        self.CarWellfl = ring(frame=self.Frame_CartPole,pos=(-0.5,-0.5,0.5), axis=(0,0,1), radius=0.3, thickness=0.05,color=(.5,.5,.5),material=materials.rough)
        self.CarWellbl = ring(frame=self.Frame_CartPole,pos=(0.5,-0.5,0.5), axis=(0,0,1), radius=0.3, thickness=0.05,color=(.5,.5,.5),material=materials.rough)
        self.CarWellfr = ring(frame=self.Frame_CartPole,pos=(-0.5,-0.5,-0.5), axis=(0,0,1), radius=0.3, thickness=0.05,color=(.5,.5,.5),material=materials.rough)
        self.CarWellbr = ring(frame=self.Frame_CartPole,pos=(0.5,-0.5,-0.5), axis=(0,0,1), radius=0.3, thickness=0.05,color=(.5,.5,.5),material=materials.rough)
        l = 3.0
        theta = radians(0)
        px = l*sin(theta)
        py = l*cos(theta)
        self.Pendulum    = curve(frame=self.Frame_CartPole,pos=[(0,0,0), (px,py,0)], radius=0.1,material= materials.plastic)
        self.PendulumTop = sphere(frame=self.Frame_CartPole,pos=(px,py,0), radius=0.3,material= materials.plastic)
        self.Gforce      = arrow(frame=self.Frame_CartPole,pos=(-1,0,0), axis=(-1,0,0), shaftwidth=1)
        self.top1        = box(display=self.scene,pos=(-6.5,1,0),height=4, width=1,material = materials.rough)
        self.top2        = box(display=self.scene,pos=(6.5,1,0),height=4, width=1,material = materials.rough)





    def PlotFunc(self,s,a,steps):

        self.awindow.display.visible =True
        self.trwindow.display.visible =True

        x     = s[0]
        theta = s[2]
        l=3
        px = l*sin(theta)
        py = l*cos(theta)
        self.Frame_CartPole.pos.x = x
        self.Pendulum.x[1] = px
        self.Pendulum.y[1] = py
        self.PendulumTop.x = px
        self.PendulumTop.y = py
        #self.scene.title = "Steps: "+str(steps)+ " x:"+str(round(x,3)) + " theta: "+str(round(theta,2))+ " force: "+str(round(a,4))
        self.scene.visible = True

        self.Gforce.x=sign(a)
        self.Gforce.axis[0] = a*2

        #trajectory
        self.Gtrajectory.plot(pos=(x,theta),color=color.blue)

        #force
        #self.clgraph.plot(pos=(steps,abs(a)),color=color.blue)
        self.clgraph.plot(pos=(1,a),color=color.blue)

        #self.awindow.display.title="Force applied: "+str(a)
        # let the visual run a step
        rate(1000)

    def ResetGraphs(self):
        self.clgraph.dots.visible = False
        del self.clgraph
        self.clgraph = gdots(gdisplay=self.awindow)

        self.Gtrajectory.gcurve.visible = False
        del self.Gtrajectory
        self.Gtrajectory = gcurve(gdisplay=self.trwindow,color=color.yellow)






