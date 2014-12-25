#-------------------------------------------------------------------------------
# Name:        CartPoleDemo.py
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

#from Environments.CartPoleEnvironment import CartPoleEnvironment
#from Environments.CartPoleEnvironmentG import CartPoleEnvironment
from Environments.CartPoleEnvironmentGN import CartPoleEnvironment

#from kNNQ import kNNQ
#from rltools.kNNQC import kNNQC
from rltools.ExaSCIPY import Exa as kNNQC
#from NeuroQ import NeuroQ
#from RNeuroQ import RNeuroQ
#from SNeuroQ import SNeuroQ
#from SOMQ import SOMQ


from rltools.ActionSelection import *
import cPickle
#from pylab import *
import time
#import psyco
#psyco.full()

def CartPoleExperiment(Episodes=100,nk=0):

    print
    print '==================================================================='
    print '           INIT EXPERIMENT','k='+str(nk+1)

    # results of the experiment
    x = range(1,Episodes+1)
    y =[]
    yr =[]

    #Build the Environment
    Env = CartPoleEnvironment()

    # Build a function approximator
    #Q = kNNQ(nactions=Env.nactions,input_ranges=Env.input_ranges,nelemns=[2,3,10,2],npoints=False,k=1,alpha=0.25)
    #Q = kNNQ(nactions=Env.nactions,input_ranges=Env.input_ranges,nelemns=[2+7,3+7,10+3,2+7],npoints=False,k=nk+1,alpha=0.3,lm=0.95)
    #Q =  NeuroQ(Env.nactions, Env.input_ranges, 20, Env.reward_ranges,Env.deep_in,Env.deep_out,alpha=0.3)
    #Q = SNeuroQ(Env.nactions, Env.input_ranges, 6, Env.output_ranges,alpha=0.2)

    #Experiments
    #Q = kNNQC(Env.input_ranges,[2+2,3+2,10+1,2+1],Env.output_ranges,[11],nk+1,0.3,0.90) #excelent

   #BEST
    Q = kNNQC(Env.input_ranges,[2+2,3+2,10+1,2+1],Env.output_ranges,[11],4,0.6,0.90,0.0) #excelent
    #Q = kNNQC(Env.input_ranges,[2+4,3+4,10+3,2+4],Env.output_ranges,[11],9,0.3,0.90) #notbad
    #Q = kNNQC(Env.input_ranges,[10,10,10,10],Env.output_ranges,[11],32,2.0,0.90) #good

    # Get the Action Selector
    As = e_greedy_selection(epsilon=0.0)
    #As = e_softmax_selection(epsilon=0.1)
    #As = None
    #Build the Agent
    CP = FARLBase(Q,Env,As,gamma=1.0)
    CP.Environment.graphs=True


    for i in range(Episodes):
        #result = CP.SARSAEpisode(1000)
        #result  = CP.NeuroQEpisode(1000)
        t1=time.clock()
        result  = CP.kNNCQEpisode(1000)
        t2=time.clock()
        #result = CP.QLearningEpisode(1000)
        CP.SelectAction.epsilon = CP.SelectAction.epsilon * 0.9
        CP.PlotLearningCurve(i,result[1],CP.SelectAction.epsilon)
        print "Episode:",str(i),'Total Reward:',str(result[0]),'Steps:',str(result[1]),"time",t2-t1

        y.append(result[1])
        yr.append(result[0])
##        if i==50:
##            miny =min(y)
##            figure(i)
##            plot(range(1,len(y)+1),y,'k')
##            title(r'$ k = 4, \quad  \lambda=0.9, \quad  \alpha=0.3 $')
##            grid('on')
##            axis([1, i, 0, 1100])
##            xlabel('Episodes')
##            ylabel('Steps')
##            savefig('cpresultcontinuous.pdf')
##            print "salvado"
##            close(i)



    CP.LearningCurveGraph.display.visible = False

    return [[x,y,nk],[x,yr,nk]]

def Experiments():
    results1=[]
    results2=[]
    for i in range(0,10):
        x = CartPoleExperiment(Episodes=200,nk=i)
        results1.append( x[0] )
        results2.append( x[1] )

    cPickle.dump(results1,open('cartpolestepscq.dat','w'))
    cPickle.dump(results2,open('cartpolerewardcq.dat','w'))



if __name__ == '__main__':
    #Experiments()
    x = CartPoleExperiment(50,3)
    cPickle.dump(x[0],open('contiuouscartpolesteps.dat','w'))
