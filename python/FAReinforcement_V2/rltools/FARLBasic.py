from visual.graph import *

class FARLBase:

    def __init__(self,Q,Environment,Action_Selector,gamma=1.0):

        self.gamma               = gamma    #discount factor
        self.Environment         = Environment
        self.nactions            = Environment.nactions  # number of actions
        self.Q                   = Q #the function approximator
        self.SelectAction        = Action_Selector # the action_selection function
        self.SelectAction.parent = self




        #Default graphic
        x_width  = 400
        y_height = 300
        self.LearningCurveGraph = gdisplay(x=0, y=y_height, width=2*x_width, height=y_height,
                        title="Learning Curve", xtitle="Episode", ytitle="Steps",
                        xmin=0.0, ymin=0.0, foreground=color.black, background=color.white)
        #Learning Curve
        self.Lcurve = gcurve(gdisplay = self.LearningCurveGraph,color=color.blue)
        self.gtitle = label (display = self.LearningCurveGraph.display, pos=self.LearningCurveGraph.display.center,opacity=0, text="Learning Curve")
        self.LearningCurveGraph.display.visible= False


    def SARSAEpisode(self, maxsteps=100):
        # do one episode with sarsa learning
        # maxstepts: the maximum number of steps per episode
        # Q: the current QTable
        # alpha: the current learning rate
        # gamma: the current discount factor
        # epsilon: probablity of a random action
        # statelist: the list of states
        # actionlist: the list of actions

        s                = self.Environment.GetInitialState()
        steps            = 0
        total_reward     = 0
        r                = 0
        # selects an action using the epsilon greedy selection strategy
        a,v   = self.SelectAction(s)

        for i in range(1,maxsteps+1):


            # do the selected action and get the next car state
            sp     = self.Environment.DoAction(a,s)


            # observe the reward at state xp and the final state flag
            r,isfinal    = self.Environment.GetReward(sp)
            total_reward = total_reward + r

            # select action prime
            ap,vp     = self.SelectAction(sp)


            # Update the Qtable, that is,  learn from the experience
            #target_value = r + self.gamma*self.Q(sp,ap) * (not isfinal)
            target_value = r + self.gamma* vp * (not isfinal)
            #self.Q.Addtrace(s,a,r)
            self.Q.Update(s,a,target_value)



            #update the current variables
            s = sp
            a = ap

            #increment the step counter.
            steps = steps + 1

            # if reachs the goal breaks the episode
            if isfinal==True:
                break

        return total_reward,steps

    def kNNCQEpisode(self, maxsteps=100):
        # do one episode with sarsa learning
        # maxstepts: the maximum number of steps per episode
        # Q: the current QTable
        # alpha: the current learning rate
        # gamma: the current discount factor
        # epsilon: probablity of a random action
        # statelist: the list of states
        # actionlist: the list of actions

        s                = self.Environment.GetInitialState()
        steps            = 0
        total_reward     = 0
        r                = 0
        # selects an action using the epsilon greedy selection strategy
        action,a   = self.Q.GetActionList(s)

        for i in range(1,maxsteps+1):


            # do the selected action and get the next car state
            sp     = self.Environment.DoAction(action,s)


            # observe the reward at state xp and the final state flag
            r,isfinal    = self.Environment.GetReward(sp,action)
            total_reward = total_reward + r

            # select action prime
            actionp,ap   = self.Q.GetActionList(sp)

            # Update the Qtable, that is,  learn from the experience
            target_value = r + self.gamma*self.Q(sp) * (not isfinal)
            self.Q.Update(s,a,target_value)


            #update the current variables
            s      = sp
            a      = ap
            action = actionp

            #increment the step counter.
            steps = steps + 1

            # if reachs the goal breaks the episode
            if isfinal==True:
                break

        return total_reward,steps



    def NeuroQEpisode(self, maxsteps=100):
        # do one episode with sarsa learning
        # maxstepts: the maximum number of steps per episode
        # Q: the current QTable
        # alpha: the current learning rate
        # gamma: the current discount factor
        # epsilon: probablity of a random action
        # statelist: the list of states
        # actionlist: the list of actions

        s                = self.Environment.GetInitialState()
        steps            = 0
        total_reward     = 0
        r                = 0
        # selects an action using the epsilon greedy selection strategy
        a   = self.Q.GetAction(s)

        for i in range(1,maxsteps+1):


            # do the selected action and get the next car state
            sp     = self.Environment.DoAction(a,s)


            # observe the reward at state xp and the final state flag
            r,isfinal    = self.Environment.GetReward(sp)
            total_reward = total_reward + r




            # Update the Qtable, that is,  learn from the experience
            self.Q.Update(s,a,r)
            # select action prime
            a     = self.Q.GetAction(sp)

            #update the current variables
            s = sp


            #increment the step counter.
            steps = steps + 1

            # if reachs the goal breaks the episode
            if isfinal==True:
                break

        return total_reward,steps

    def QLearningEpisode(self, maxsteps=100 ):
        """ do one episode of QLearning """
        # maxstepts: the maximum number of steps per episode
        # alpha: the current learning rate
        # gamma: the current discount factor
        # epsilon: probablity of a random action
        # actionlist: the list of actions


        s                = self.Environment.GetInitialState()
        steps            = 0
        total_reward     = 0

        for i in range(1,maxsteps+1):

            # selects an action using the epsilon greedy selection strategy
            a = self.SelectAction(s)


            # do the selected action and get the next state
            sp     = self.Environment.DoAction( a,s )

            # observe the reward at state xp and the final state flag
            r,isfinal    = self.Environment.GetReward(sp)
            total_reward = total_reward + r

            # Update the Qtable, that is,  learn from the experience
            vp = self.Q(sp)
            target_value = r + self.gamma * max(vp) * (not isfinal)
            sp = self.Q.Update(s,a,target_value)

            #update the current variables
            s = sp

            #increment the step counter.
            steps = steps + 1

            # if reachs the goal breaks the episode
            if isfinal==True:
                break

        return total_reward,steps

    def PlotLearningCurve(self,episode,steps,epsilon):

        self.LearningCurveGraph.display.visible= True
        self.gtitle.pos = self.LearningCurveGraph.display.center
        self.Lcurve.plot(pos=(episode,steps))
        self.gtitle.text =" Steps: "+str(steps)+" epsilon: "+str(round(epsilon,5))
