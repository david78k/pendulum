#-------------------------------------------------------------------------------
# Name:        AcrobotDemo.py
# Description: A module of Reinforment Learning tools
#
# Author:      Jose Antonio Martin H. (JAMH)
# Contact:     <jamartin@dia.fi.upm.es>
#
# Created:     22/05/2010
# Copyright:   (c) Jose Antonio Martin H. 2010
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#-------------------------------------------------------------------------------
#!/usr/bin/env python
from rltools.FARLBasic import *
from Environments.AcrobotEnvironmentG import AcrobotEnvironment
from rltools.kNNSCIPY import kNNQ
from rltools.ActionSelection import *
import cPickle


import time

def AcrobotExperiment(Episodes=100,nk=1):
    print
    print '==================================================================='
    print '           INIT EXPERIMENT','k='+str(nk)

    # results of the experiment
    x = range(1,Episodes+1)
    y =[]

    #Build the Environment
    ACEnv = AcrobotEnvironment()

    # Build a function approximator
    Q = kNNQ(nactions=ACEnv.nactions,input_ranges=ACEnv.input_ranges,nelemns=[11,11,11,11],npoints=False,k=2**4,alpha=5.0,lm=0.90)

    #Q.Q+=10000
    #Q = kNNQ(nactions=ACEnv.nactions,input_ranges=ACEnv.input_ranges,nelemns=False,npoints=300,k=5,alpha=0.3)
    #Q = NeuroQ(ACEnv.nactions, ACEnv.input_ranges, 30+nk, ACEnv.reward_ranges,ACEnv.deep_in,ACEnv.deep_out,alpha=0.3)
    #Q = RNeuroQ(MCEnv.nactions, MCEnv.input_ranges, 10, MCEnv.reward_ranges,alpha=0.3)
    #Q = SOMQ(nactions=MCEnv.nactions,size_x=20,size_y=20,input_ranges=MCEnv.input_ranges,alpha=0.3)
    #Q = lwprQ(nactions=ACEnv.nactions,input_ranges=ACEnv.input_ranges)
    # Get the Action Selector
    As = e_greedy_selection(epsilon=0.000)
    #As = e_softmax_selection(epsilon=0.1)

    #Build the Agent
    AC = FARLBase(Q,ACEnv,As,gamma=1.0)

    AC.Environment.graphs=True#False
    #AC.Environment.PlotPopulation(MC.Q)




    for i in range(Episodes):
        t1= time.clock()
        result = AC.SARSAEpisode(1000)
        #result = AC.QLearningEpisode(1000)
        t2 = time.clock()-t1
        #AC.SelectAction.epsilon = AC.SelectAction.epsilon * 0.9
        AC.PlotLearningCurve(i,result[1],AC.SelectAction.epsilon)
        #AC.Environment.PlotPopulation(MC.Q)
        print 'Episode',str(i),' Steps:',str(result[1]),'time',t2
        y.append(result[1])

    return [x,y,nk]



def Experiments():
    results=[]
    for i in range(0,10):
        x = AcrobotExperiment(Episodes=1000,nk=i)
        results.append( x )

    cPickle.dump(results,open('acrobotresult1.dat','w'))




if __name__ == '__main__':
    AcrobotExperiment(Episodes=1001,nk=0)
    #Experiments()
