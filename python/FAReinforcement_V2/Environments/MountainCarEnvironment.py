

from visual import *
from visual.graph import *


class MCEnvironment:
    """
    This class implements The Mountain Car Environment
    @author    Jose Antonio Martin H.
    @version   1.0
    """

    title ="MountainCar Demo"
    # The current real values of the state
    cur_pos= -0.5
    cur_vel=  0.0
    cur_state =[cur_pos,cur_vel]

    # ranges of sensory inputs variables
    pos_range =[-1.4  ,0.5]
    vel_range =[-0.07 ,0.07]

    input_ranges = [pos_range,vel_range]
    reward_ranges = [[-100.0,100.0]]

    deep_in = [5,5] #2^n exact numbers for each dimention
    deep_out= [10]   #2^n exact numbers for each dimention

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
    graphs = False#True

    # Goal Position
    goalPos = 0.45

    def __init__(self,friction=False):
        self.friction = friction
        self.nactions = len(self.action_list)
        self.initGraphs()

    def GetInitialState(self):
        self.StartEpisode()
        return self.cur_state


    def UpdateState(self):
        self.cur_state = [self.cur_pos,self.cur_vel]


    def StartEpisode(self):

        self.cur_pos = -0.5
        self.cur_vel =  0.0
        self.UpdateState()
        self.steps   = 0
        self.ResetGraphs()
        self.episode = self.episode + 1

    def EndEpisode(self):
        pass




    def getState(self):
         """
          Returns the current situation.
          A situation can be the current perceptual inputs, a random problem instance ...
         """
         return self.cur_state


    def GetReward(self,s):
        # MountainCarGetReward returns the reward at the current state
        # x: a vector of position and velocity of the car
        # r: the returned reward.
        # f: true if the car reached the goal, otherwise f is false

        position = s[0]
        vel      = s[1]
        # bound for position; the goal is to reach position = 0.45
        bpright  = self.goalPos

        r = -1.0
        f = False


        if  position >= bpright:
            r = 100.0
            f = True



        return r,f

    def DoAction(self,a,s):
        #MountainCarDoAction: executes the action (a) into the mountain car
        # acti: is the force to be applied to the car
        # x: is the vector containning the position and speed of the car
        # xp: is the vector containing the new position and velocity of the car
        #print 'action',a
        #print 'state',s
        force      = self.action_list[a]

        self.steps = self.steps +1

        position = s[0]
        speed    = s[1]

        # bounds for position
        bpleft  = -1.4

        # bounds for speed
        bsleft  = -0.07
        bsright =  0.07

        speedt1 = speed + (0.001*force) + (-0.0025 * cos( 3.0*position) )
        #if self.friction:
            #speedt1 = speedt1 * 0.999 # thermodynamic law, for a more real system with friction.

        if speedt1<bsleft:
            speedt1=bsleft
        elif speedt1>bsright:
            speedt1=bsright

        post1 = position + speedt1

        if post1<=bpleft:
            post1=bpleft
            speedt1=0.0

        self.cur_pos =  post1
        self.cur_vel =  speedt1
        self.UpdateState()

        if self.graphs:
            if not self.scene.visible:
                self.scene.visible=True
                self.clwindow.display.visible = True
            self.PlotFunc(force)

        return [post1,speedt1]





    #------------------------------------ PLOT FUNCTIONS -----------------------------


    def initGraphs(self):

        x_width  = 400
        y_height = 300

        self.clwindow = gdisplay(x=0, y=0, width=x_width, height=y_height,
                        title="State Space", xtitle='Pos', ytitle='Vel',
                        xmax=0.6, xmin=-1.6, ymax=0.08, ymin=-0.08,
                        foreground=None, background=None)


        self.scene = display(x=self.clwindow.width, width=x_width, height=y_height,
                             title="Mountain Car Demo",visible=False)


        #self.clwindow.display.visible = False



        #Init Mountain
        self.MountainGraph   = curve(scene=self.scene,x=arange(-1.6,0.6,0.07), color=color.green,radius=0.01)
        self.MountainGraph.y = sin(3*self.MountainGraph.x)

        # Init Car
        self.Car = sphere(scene=self.scene,pos=(-0.5,sin(3*-0.5)+0.11),radius=0.1)

        #Goal
        self.Goal = ring(pos=(self.goalPos,sin(3*self.goalPos)+0.1),radius=0.2,thickness=0.02)

        # classifiers
        self.clgraph = gdots(gdisplay=self.clwindow)

        #trajectory
        self.Gtrajectory = gcurve(gdisplay=self.clwindow,color=color.yellow)



    def PlotFunc(self,a):

        # update car position
        x = self.cur_pos
        y = self.cur_vel

        self.Car.pos.x= x
        self.Car.pos.y= sin(3.0*x)+0.15

        #update display
        #self.scene.title = "Episode: "+str(int(self.episode))+" Steps: "+str(self.steps)
        self.Gtrajectory.plot(pos=(x,y),color=color.yellow)

        #let the visual run a step
        #rate(10000)


    def PlotPopulation(self,Q):
        if not Q.HasPopulation():
            return

        self.clgraph.dots.visible = False
        del self.clgraph
        self.clgraph = gdots(gdisplay=self.clwindow)

        for cl in Q.Population():
            self.clgraph.plot(pos=cl,color=color.white)

    def ResetGraphs(self):
        self.Gtrajectory.gcurve.visible = False
        del self.Gtrajectory
        self.Gtrajectory = gcurve(gdisplay=self.clwindow,color=color.yellow)



