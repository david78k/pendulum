"""
A demo experiment using gridworld, temporal-difference learners, and
the plastk.exper module to control the experimental conditions.

This demo instantiates a gridworld and an agent that will learn to get
to the goal in the world using reinforcement learning.

$Id: grid_exper.py 231 2008-03-03 16:04:56Z jprovost $
"""
import pdb  # We may need the debugger

########################################################################
# Import what we need from PLASTK: the top-level modules,
# and stuff for gridworlds, temporal difference (td) agents, and
# an interface that supports GUIs and logging.
from plastk.rl.gridworld import GridWorld,GridWorldDisplay
from plastk.rl.td import TDAgent,TabularTDAgent
from plastk.rl.loggingrli import LoggingRLI,VarPlotter

# Import the factored experiment class from
from plastk.misc.experiment import FactorExperiment,Factor,Cross
from plastk.misc import pkl

########################################################################
# Set the TD agent parameters
#

# Use the Sarsa reinforcement learning algorithm
TDAgent.step_method = 'sarsa'

# Standard TD-learning parameters
TDAgent.alpha = 0.2
TDAgent.lambda_ = 0.9

# start in a random position in the world
GridWorld.random_start_pos = True

# 10% chance that the agent's action will not go in the intended
# direction.
GridWorld.correct_action_probability = 0.9

# Have the gridworld remember the path taken and display previous
# paths in the GUI.
GridWorld.crumbs = True
GridWorld.clear_crumbs_on_pose_set = False
GridWorld.recolor_crumbs_on_pose_set = True

#
# The grid for the environment.
#
small_grid = [
    '#############',
    '#...........#',
    '#...........#',
    '#...........#',
    '#...........#',
    '#...........#',
    '######.######',
    '#...........#',
    '#...........#',
    '#...........#',
    '#...........#',
    '#G..........#',
    '#############',
    ]                

large_grid = [
    '#########################',
    '#...........#...........#',
    '#...........#...........#',
    '#.......................#',
    '#...........#...........#',
    '#...........#...........#',
    '######.###########.######',
    '#...........#...........#',
    '#...........#...........#',
    '#.......................#',
    '#...........#...........#',
    '#...........#..........G#',
    '#########################',
    ]                

def optimistic_setup():
    """
    This function sets the agent parameter defaults for "optimistic
    initialization".  The gridworld gives -1 reward for a step and 0 for reaching the
    goal, and the agent's initial Q estimate is 0.  Optimistic
    initialization forces exploration, so the agent uses totally
    greedy action selection, with epsilonset to 0.
    """
    GridWorld.step_reward = -1
    GridWorld.goal_reward = 0
    TabularTDAgent.initial_q = 0
    TDAgent.action_selection = 'epsilon_greedy'
    TDAgent.initial_epsilon = 0.0
    TDAgent.min_epsilon = 0.00
    TDAgent.gamma = 1.0



def pessimistic_setup():
    """
    This function sets the agent parameter defaults for "pessimistic
    initialization".  The gridworld gives 0 reward for a step and 1
    for reaching the goal, and the agent's initial Q estimate is 0.
    The agent uses episilon-greedy exploration w epsilon=0.1.
    """
    GridWorld.step_reward = 0
    GridWorld.goal_reward = 1
    TDAgent.action_selection = 'epsilon_greedy'
    TDAgent.initial_epsilon = 0.1
    TDAgent.min_epsilon = 0.0
    TDAgent.epsilon_half_life = 5000
    TDAgent.gamma = 0.95


class TDExperiment(FactorExperiment):

    init_type = Factor(init_name = ('optmistic','pessimistic'),
                       init_fn = (optimistic_setup,pessimistic_setup))
    grid_size = Factor(grid_name = ('small', 'large'),
                       grid = (small_grid,large_grid),
                       num_episodes = (1000,2000) )

    conditions = Cross(grid_size,init_type)

    def proc(self, N, init_name,init_fn,grid_name,grid,num_episodes):


        # setup the (optimistic/pessimistic) parameters
        init_fn()
    
        # Make a grid environment with the given grid.
        env = GridWorld(grid=grid)

        # Make a tabular agent. 
        agent = TabularTDAgent(actions = env.actions,
                               num_features = env.num_states)

        # set up the reinforcement-learning interface with agent and env
        self.rli = rli = LoggingRLI(name = self.filestem(N),
                                    rename_old_data = False)
        rli.init(agent,env)

        # Run the rli GUI with a GridWorldDisplay widget and a widget that
        # plots the length of each episode.
        if self.gui:
            rli.gui(GridWorldDisplay,
                    EpisodeVarPlotter('length'))
        else:
            rli.episodes(num_episodes,10000)

        # save the data
        pkl.save(rli.episode_data,
                 self.filestem(N)+'.pkl.gz')

    def filestem(self,c):
        """
        Generate a human-readable filename prefix for a condition.

        The condition c can be either a condition  number or a
        dictionary selected from the experiment's conditions
        attribute.  If c is a dict the stem produced is

           grid_experiment-<init_name>-<grid_name>

        If c is an int, the stem recursively defined as:

           self.filestem(self.conditions[c]+'-%d'%c
        """
        if isinstance(c,int):
            return self.filestem(self.conditions[c])+'-%d'%c
        elif isinstance(c,dict):
            init = c['init_name']
            grid = c['grid_name']
            return 'grid_experiment-%s-%s'%(init,grid)
        else:
            raise ValueError, 'c must be either an int or a dict.'

exp = TDExperiment()
if __name__ == '__main__':

    from plastk.base import VERBOSE
    LoggingRLI.print_level = VERBOSE
    # The experiment has 4 conditions.  The .run method takes a list
    # of trial numbers and runs them.  Each trial number x is mapped to
    # a condition by taking x%4.  I.e. Trials 0,4,8,... are in
    # condition 0.  1,5,9... are in condition 1, etc.  To run 5 trials in
    # each condition we do:

    exp.run(range(20))

