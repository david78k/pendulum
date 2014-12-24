from Numeric import *
from RandomArray import *
class RLBase:
    
    def __init__(self,alpha=0.3,gamma=1.0,epsilon=0.01):
        
        self.alpha       = alpha    #learning rate
        self.gamma       = gamma    #discount factor
        self.epsilon     = epsilon  #probability of a random action selection

        self.statelist   = self.BuildSateList()     # the list of states
        self.actionlist  = self.BuildActionList()   # the list of actions        
        self.nactions    = self.actionlist.shape[0] # number of actions
        self.Q           = self.BuildQTable()       # the Qtable

    def __BuildActionList(self):
        """ overwrite this function for your specific problem"""
        # return a single array of real actions to be perfomed by the DoAction function
        return actionlist

    
        
    def __BuildSateList(self):
        """ overwrite this function for your specific problem"""
        #return an array of vector states
        # this is typically a combinatorial discretization of the input space
        return statelist
    

    def BuildQTable(self):
        nstates     = self.statelist.shape[0]
        nactions    = self.actionlist.shape[0]
        Q = [[0.0 for i in range(nactions)] for i in range(nstates)]
        return Q


    def DiscretizeState(self,x):    
        """DiscretizeState check which entry in the state list is more close to x and return the index of that entry."""
        
        edist2   = sqrt(sum((self.statelist-x)**2,1))
        return     argmin(edist2)    
       

    def e_greedy_selection(self, s):
        #selects an action using Epsilon-greedy strategy
        # Q: the Qtable
        # s: the current state        
                
        if (random()>self.epsilon):
            a = self.GetBestAction(s)    
        else:
            # selects a random action based on a uniform distribution
            a = randint(0,self.nactions)
        
        return a

                        
    def GetBestAction(self, s ):
        #GetBestAction return the best action for state (s)
        #Q: the Qtable
        #the current state
        #has structure  Q(states,actions)
        
        #a = argmax(self.Q[s,:].flat)
        a  = argmax(self.Q[s])    
        return a

    def __GetReward(self, x ):
        """ overwrite this function for your specific problem"""
        # r the current reward
        # f = True if the Goal is reached False otherwise
        return r,f    

    def __DoAction(self, force, x ):
        """ overwrite this function for your specific problem"""
        # retuer xp, the new system state as an ND-vector
        return xp
    
    def __GetInitialState(self):
        """ overwrite this function for your specific problem"""
        # retuer xp, the initial system state as an ND-vector
        # you could fix it or returing a random initial state
        return  x 
        


    def UpdateSARSA(self, s, a, r, sp, ap):
        """ Update Qtable using SARSA"""
            
        self.Q[s][a]  = self.Q[s][a] + self.alpha * ( r + self.gamma*self.Q[sp][ap] - self.Q[s][a] )
        

    def UpdateQ(self, s, a, r, sp):
        """ Update Qvalues by QLearning """
        maxQ = max(self.Q[sp])
        self.Q[s][a] = self.Q[s][a] + self.alpha *  ( r + self.gamma*maxQ - self.Q[s][a] )       

    
    def SARSAEpisode(self, maxsteps=100, grafic = False ):
        # do one episode with sarsa learning
        # maxstepts: the maximum number of steps per episode
        # Q: the current QTable
        # alpha: the current learning rate
        # gamma: the current discount factor
        # epsilon: probablity of a random action
        # statelist: the list of states
        # actionlist: the list of actions
        
        x                = self.GetInitialState()
        steps            = 0
        total_reward     = 0


        # convert the continous state variables to an index of the statelist
        s   = self.DiscretizeState(x)
        
        # selects an action using the epsilon greedy selection strategy
        a   = self.e_greedy_selection(s)


        for i in range(1,maxsteps+1):
                        
            # convert the index of the action into an action value
            action = self.actionlist[a]    
            
            # do the selected action and get the next car state    
            xp     = self.DoAction( action , x )    
            
            # observe the reward at state xp and the final state flag
            r,isfinal    = self.GetReward(xp)
            total_reward = total_reward + r
            
            # convert the continous state variables in [xp] to an index of the statelist
            sp     = self.DiscretizeState(xp)
            
            # select action prime
            ap     = self.e_greedy_selection(sp)
            
            
            # Update the Qtable, that is,  learn from the experience
            self.UpdateSARSA( s, a, r, sp, ap)
            
            
            #update the current variables
            s = sp
            a = ap
            x = xp
            
            #if graphic function then show graphics
            if grafic:
                self.PlotFunc(x,a,steps)
                
            
            #increment the step counter.
            steps = steps+1
            
            # if reachs the goal breaks the episode
            if isfinal==True:
                break
        
        return total_reward,steps
    
    def QLearningEpisode(self, maxsteps=100, grafic = False ):
        """ do one episode of QLearning """
        # maxstepts: the maximum number of steps per episode       
        # alpha: the current learning rate
        # gamma: the current discount factor
        # epsilon: probablity of a random action       
        # actionlist: the list of actions
        
        x                = self.GetInitialState()
        steps            = 0
        total_reward     = 0

        # convert the continous state variables to an index of the statelist
        s   = self.DiscretizeState(x)

        for i in range(1,maxsteps+1):
            
            # selects an action using the epsilon greedy selection strategy
            a  = self.e_greedy_selection(s)
            
            # convert the index of the action into an action value
            action = self.actionlist[a]    
            
            # do the selected action and get the next car state    
            xp     = self.DoAction( action , x )    
            sp     = self.DiscretizeState(xp)
            
            # observe the reward at state xp and the final state flag
            r,isfinal    = self.GetReward(xp)
            total_reward = total_reward + r
        
            # Update the Qtable, that is,  learn from the experience
            self.UpdateQ( s, a, r, sp)

            #update the current variables
            s = sp            
            x = xp
            
            #if graphic function then show graphics            
            if grafic==True:                               
                self.PlotFunc(x,a,steps)            
            
            #increment the step counter.
            steps = steps + 1
            
            # if reachs the goal breaks the episode
            if isfinal==True:
                break

        return total_reward,steps
