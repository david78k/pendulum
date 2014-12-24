#-------------------------------------------------------------------------------
# Name:        ProleDemo.py
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
from Environments.ProleEnvironment2 import ProleEnvironment
from rltools.kNNSCIPY import kNNQ
#from rltools.RNeuroQ import RNeuroQ
from rltools.ActionSelection import *
import cPickle
import time
#from pylab import *





def Experiment(Episodes=100,nk=1):
    print
    print '==================================================================='
    print '           INIT EXPERIMENT','k='+str(nk+1)

    strfilename = "PDATA.npy"

    # results of the experiment
    x = range(1,Episodes+1)
    y =[]

    #Build the Environment
    Env = ProleEnvironment()

    # Build a function approximator
    #Q = kNNQ(nactions=Env.nactions,input_ranges=Env.input_ranges,nelemns=[8,8,8,8,8,8],npoints=False,k=1,alpha=0.3,lm=0.95)
    Q = kNNQ(nactions=Env.nactions,input_ranges=Env.input_ranges,nelemns=[8,8,8,8,8,8],npoints=False,k=2**6,alpha=10.5,lm=0.95)
    Q.Load(strfilename)

    #Experimental
    #Q = lwprQ(nactions=Env.nactions,input_ranges=Env.input_ranges)


    # Get the Action Selector
    As = e_greedy_selection(epsilon=0.1)
    #As = e_softmax_selection(epsilon=0.1)

    #Build the Agent
    RL = FARLBase(Q,Env,As,gamma=1.0)
    RL.Environment.graphs=True



    for i in range(Episodes):
        t1= time.clock()
        result = RL.SARSAEpisode(1000)
        #result = RL.QLearningEpisode(1000)
        t2 = time.clock()-t1
        RL.SelectAction.epsilon *= 0.9
        #RL.Q.alpha *= 0.995

        #RL.PlotLearningCurve(i,result[1],RL.SelectAction.epsilon)
        print 'Episode',i,' Steps:',result[1],'Reward:',result[0],'time',t2,'alpha',RL.Q.alpha
        #Q.Save(strfilename)
        y.append(result[1])


    return [x,y,nk]



def Experiments():
    results=[]
    for i in range(0,10):
        x = Experiment(Episodes=200,nk=i)
        results.append( x )

    cPickle.dump(results,open('mcresultsknnql.dat','w'))




if __name__ == '__main__':
    Experiment(Episodes=10000,nk=3)
    #Experiments()
