"""
A simple gridworld demo.

This demo instantiates a simple grid-like world of cells and an agent
that will learn to get to goal in the world by moving from cell to
cell, updating it's policy using reinforcement learning.

$Id: gridworld.py 230 2008-03-03 04:53:52Z jprovost $
"""

import pdb  # We may need the debugger

########################################################################
# Import what we need from PLASTK: the top-level modules,
# and stuff for gridworlds, temporal difference (td) agents, and
# an interface that supports GUIs and logging.
from plastk.rl.gridworld import GridWorld,GridWorldDisplay
from plastk.rl.td import TDAgent,TabularTDAgent
from plastk.rl.loggingrli import LoggingRLI,VarPlotter


########################################################################
# Set the TD agent parameters
#

# Use the Sarsa reinforcement learning algorithm
TDAgent.step_method = 'sarsa'

# Standard TD-learning parameters
TDAgent.alpha = 0.2
TDAgent.lambda_ = 0.9
TDAgent.gamma = 1.0


# Initialize the value of a state to 0.  Because the step reward is
# negative (see below) this 'optimistic initialization' will cause the
# agent to explore the entire space before converging on a solution.
TabularTDAgent.initial_q = 0

# Because we're using optimistic intialization for exploration, we
# want greedy action selection, so set epsilon to 0.
TDAgent.action_selection = 'epsilon_greedy'
TDAgent.initial_epsilon = 0.0
TDAgent.min_epsilon = 0.00

# start in a random position in the world
GridWorld.random_start_pos = True

# 10% chance that the agent's action will not go in the intended
# direction.
GridWorld.correct_action_probability = 0.9

# Give a reward of -1 for each step and 0 for reaching the goal.
GridWorld.step_reward = -1
GridWorld.goal_reward = 0

# Have the gridworld remember the path taken and display previous
# paths in the GUI.
GridWorld.crumbs = True
GridWorld.clear_crumbs_on_pose_set = True
GridWorld.recolor_crumbs_on_pose_set = False

#
# The grid for the environment.
#
grid1 = [
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

grid2 = [
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

# use grid1 by default.
GridWorld.grid = grid1

# If you have BLT installed, use it for plotting variable traces
# it's much faster than matplotlib
VarPlotter.use_blt = True

# if using matplotlib, this value indicates how many action/sensation
# steps elapse between plot updates.
VarPlotter.matplotlib_update_period = 5

def pessimistic():
    """
    This function sets the agent parameter defaults for "pessimistic
    initialization".  The gridworld gives 0 reward for a step and 1
    for reaching the goal, and the agent's initial Q estimate is 0.
    The agent uses episilon-greedy exploration w epsilon=0.1.
    """
    TDAgent.action_selection = 'epsilon_greedy'
    TDAgent.initial_epsilon = 0.1
    TDAgent.min_epsilon = 0.0
    TDAgent.epsilon_half_life = 5000
    TDAgent.gamma = 0.95
    GridWorld.step_reward = 0
    GridWorld.goal_reward = 1

def agent_last_value(rli):
    """
    A step-variable function to get the agent's value estimate
    for the last state.
    """
    a = rli.agent
    return a.Q(a.last_sensation,a.last_action)

# The main function. Call this to run the example.
def main():
    # Make a grid environment with the given grid.
    env = GridWorld()

    # Make a tabular agent. 
    agent = TabularTDAgent(actions = env.actions)



    # set up the reinforcement-learning interface with agent and env
    rli = LoggingRLI(name = "Gridworld Demo",
                     rename_old_data = False)

    # add a step variable to record the value estimate of the last step
    rli.add_step_variable('last_value',dtype=float,fn=agent_last_value)

    # init the RLI with the agent and environment
    rli.init(agent,env)

    # Run the rli GUI with a GridWorldDisplay widget, a widget that
    # plots the length of each episode, and a widget that plots the
    # agent's estimate of the Q-value of the last state.
    rli.gui(GridWorldDisplay,
            lambda root,r:VarPlotter(root,var='length',
                                     title='Steps to Reach Goal',rli=r),
            lambda root,r:VarPlotter(root,var='last_value',
                                     title='Q Value',rli=r,
                                     source='step_data')
            )

    #rli.episodes(100,10000)

    return rli

if __name__=='__main__':
    main()

