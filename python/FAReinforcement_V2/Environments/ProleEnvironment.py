from numpy import *
from numpy.random import *
from visual import *
from visual.graph import *
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
        self.x = self.body.x
        self.y = self.body.y

    def update(self):

        self.state.update()

        #pinta el movil
        self.body.pos     = self.state.x
        self.body.axis    = self.state.direction*self.state.L/3.0
        self.body.lentgth = self.state.L

        self.pointer.pos  = self.state.x
        self.pointer.axis = self.state.direction*self.state.L/3.0

        self.trajectory.append(pos=self.state.x)

        self.x = self.body.x
        self.y = self.body.y

    def setac(self,theta,aceleration):
        self.state.setac(theta,aceleration)

    def setvel(self,vel):
        self.state.v = vel







class Agent(Body):
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






class ProleEnvironment:
    """
    This class implements The Prole Protection Environment
    @author    Jose Antonio Martin H.
    @version   1.0
    """

    title ="Prole Protection Problem"


    # ranges of sensory inputs variables
    pos_range =[-12,12]


    input_ranges = [pos_range,pos_range,pos_range,pos_range,pos_range,pos_range]

    #The number of actions.
    action_list = range(2)
    nactions    = len(action_list)
    #Flag which is set to true when goal was reached.
    reset = False

    # number of steps of the current trial
    steps = 0

    # number of the current episode
    episode = 0

    # do you want to show nice graphs?
    graphs = True

    # Goal Position
    goalPos = (8,8,0)

    def __init__(self):
        self.initGraphs()

    def GetInitialState(self):
        self.StartEpisode()
        return self.cur_state


    def UpdateState(self):
        temp1  = array([self.goal.x,self.goal.y,self.Zorro.x,self.Zorro.y,self.Patito.x,self.Patito.y])
        temp2  = array([self.Pata.x,self.Pata.y,self.Pata.x,self.Pata.y,self.Pata.x,self.Pata.y])
        self.cur_state = temp1-temp2
        return self.cur_state



    def StartEpisode(self):
        self.steps   = 0
        self.ResetGraphs()
        self.episode = self.episode + 1
        self.UpdateState()

    def EndEpisode(self):
        pass




    def getState(self):
         """
          Returns the current situation.
          A situation can be the current perceptual inputs, a random problem instance ...
         """
         self.UpdateState()
         return self.cur_state


    def GetReward(self,s):
        # r: the returned reward.
        # f: true if the car reached the goal, otherwise f is false

        #[self.goal.x,self.goal.y,self.Zorro.x,self.Zorro.y,self.Patito.x,self.Patito.y]
        # bound for position; the goal is to reach position = 0.45

        #print 'Patito pos',self.Patito.state.x
        #print 'Zorro pos',self.Zorro.state.x
        #print 'dist',sqrt(sum( (self.Patito.state.x-self.Zorro.state.x)**2 ))


        r = 1.0 / (1.0+ sum((self.Patito.state.x-self.goal.state.x)**2))

        f = False



        if sqrt(sum( (self.Pata.state.x)**2 )) >=10.0:
            r = -100.0
            f = True


        if sqrt(sum( (self.Patito.state.x-self.Zorro.state.x)**2 )) <=1.0:
            r = -100.0
            f = True


        if sqrt(sum((self.Patito.state.x-self.goal.state.x)**2 )) <=1.0:
            r = 1000.0
            f = True






        return r,f

    def DoAction(self,a,s):


        if a==0:
            self.Pata.react(self.Patito,1)
        elif a==1:
            self.Pata.react(self.goal,1)



        if sqrt(sum((self.Zorro.state.x-self.Pata.state.x)**2 )) > 2.0:
            self.Zorro.react(self.Patito,1)
        else:
            self.Zorro.react(self.Pata,-1)


        self.Patito.react(self.Pata,1)
        self.Patito2.react(self.Patito,1)


        self.Pata.update()
        self.Zorro.update()
        self.Patito.update()
        self.Patito2.update()

        self.PlotFunc()

        return self.UpdateState()





    #------------------------------------ PLOT FUNCTIONS -----------------------------


    def initGraphs(self):


        self.scene = display(title='Pursuit and Evasion  -Differential Games Simulator-',
                width=1025, height=770,
                center=(0,0,0), background=(1,1,1),
                autoscale=0,fullscreen=0)

        #Goal
        self.goal=Agent(self.goalPos,color.blue,(-1,0,0),L=3.0)
        self.goal.setac(radians(0),0.0)
        self.goal.update()


        #Pata
        self.Pata=Agent(radians(45),10,(randint(-5,5),randint(-5,5),0),color.orange,(-1,0,0),L=1.5)
        self.Pata.setac(radians(0),0.1)
        self.Pata.update()



        #Patito
        self.Patito=Agent(radians(75),10,(randint(-5,5),randint(-5,5),0),color.yellow,(0,1,0),L=1.5)
        self.Patito.setac(radians(0),0.1)
        self.Patito.update()

        #Zorro
        self.Zorro=Agent(radians(85),10,(randint(-5,5),randint(-5,5),0),(0.5,0.5,0.5),L=2.0)
        self.Zorro.setac(radians(0),0.075)
        self.Zorro.update()

    def PlotFunc(self):


        #update display
        self.scene.title = "Episode: "+str(int(self.episode))+" Steps: "+str(self.steps)
        rate(100)


    def ResetGraphs(self):

        del self.Pata
        del self.Patito
        del self.Zorro
        for o in self.scene.objects:
            o.visible=0


        #Goal
        self.goal=Agent(self.goalPos,color.blue,(-1,0,0),L=3.0)
        self.goal.setac(radians(0),0.0)
        self.goal.update()

        #Pata
        self.Pata=Agent(radians(45),10,(randint(-5,5),randint(-5,5),0),color.orange,(-1,0,0),L=1.5)
        self.Pata.setac(radians(0),0.1)
        self.Pata.update()



        #Patito
        self.Patito=Agent(radians(75),10,(randint(-5,5),randint(-5,5),0),color.yellow,(0,1,0),L=1.5)
        self.Patito.setac(radians(0),0.1)
        self.Patito.update()

        #Zorro
        self.Zorro=Agent(radians(75),10,(randint(-5,5),randint(-5,5),0),(0.5,0.5,0.5),L=2.0)
        self.Zorro.setac(radians(0),0.1)
        self.Zorro.update()

        #Patito
        self.Patito2=Agent(radians(75),10,(randint(-5,5),randint(-5,5),0),color.yellow,(0,1,0),L=1.5)
        self.Patito2.setac(radians(0),0.1)
        self.Patito2.update()




