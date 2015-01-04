"""
A simple inverted pendulum demo.

This script sets up an agent learning to invert an under-actuated
pendulum.  The torques available for moving the pendulum are not
sufficient to turn it to an upright position, to the agent must learn
to "swing up" the pendulum in order to get enough speed to invert it.

$Id: pendulum.py 230 2008-03-03 04:53:52Z jprovost $
"""

import pdb  # We may need the debugger
from math import pi

########################################################################
# Import what we need from PLASTK:

# The base Pendulum environment, and the easy (episodic) Pendulum
# environment. 
from plastk.rl.pendulum import Pendulum,PendulumEZ,PendulumGUI

# The base TDAgent class, and the linear and uniform-tiled subclasses.
from plastk.rl.td import TDAgent,LinearTDAgent,UniformTiledAgent

# The reinforcement learning interface, and some GUI components.
from plastk.rl.loggingrli import LoggingRLI, VarPlotter


########################################################################
# Set the TD agent parameters
#

# Use the Sarsa RL method.
TDAgent.step_method = 'sarsa'

# set the basic TD parameters.
TDAgent.alpha = 0.1
TDAgent.lambda_ = 0.7
TDAgent.gamma = 1.0

# We'll use optimistic intialization for exploration, so set epsilon
# to 0 for fully greedy action selection.
TDAgent.action_selection = 'epsilon_greedy'
TDAgent.initial_epsilon = 0.0
TDAgent.min_epsilon = 0.0

# Optimistic initialization. Steps have negative cost.
LinearTDAgent.initial_w = 0

# initialize the physics of the environment
Pendulum.initial_angle = pi
Pendulum.initial_velocity = 0.0
Pendulum.friction = 0.001

# Each timestep is 0.1 simulated seconds.
Pendulum.delta_t = 0.1

# The available actions.
Pendulum.actions = [-7,7]   # Torque of +/- 7 (newton-meters?)

# If you have BLT installed, use it for plotting variable traces
# it's much faster than matplotlib
VarPlotter.use_blt = True

# if using matplotlib, this value indicates how many action/sensation
# steps elapse between plot updates.
VarPlotter.matplotlib_update_period = 10


def agent_last_value(rli):
    """
    A step-variable function to get the agent's value estimate
    for the last state.
    """
    a = rli.agent
    return a.Q(a.last_sensation,a.last_action)


def main():
    # Make a grid environment with the given grid.
    env = PendulumEZ()


    # Make a linear-tabular agent, i.e. an agent that takes a single
    # integer as the state and does linear updating
    agent = UniformTiledAgent(actions=env.actions,
                              num_tilings=8,
                              num_features=1024,
                              tile_width=2*pi/16)

    # set up the reinforcement-learning interface with agent and env
    rli = LoggingRLI(name = "Pendulum Demo",
                     rename_old_data = False)

    rli.add_step_variable('last value',agent_last_value)
    rli.init(agent,env)


    # Run the rli GUI with a GridWorldDisplay widget and a widget that
    # plots the length of each episode.
    rli.gui(PendulumGUI,
            lambda root,rli: VarPlotter(root,rli=rli,
                                        var='length',
                                        source='episode_data',
                                        title='Steps to Reach Goal'),
            lambda root,rli: VarPlotter(root,rli=rli,
                                        var='last value',
                                        source='step_data',
                                        title='Q Value'))
    #rli.episodes(2,100000)

    return rli

if __name__=='__main__':
    main()

