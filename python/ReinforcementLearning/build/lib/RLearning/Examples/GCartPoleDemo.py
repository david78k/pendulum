# -*- coding: cp1252 -*-

from CartPoleDemo import *
from visual import * # imports the visual system to see the graphics
from visual.graph import * # more graphics

import time
import psyco
psyco.full()



class GCartPole(CartPole):
    
    def InitGraphs(self):
        
        x_width  = 400
        y_height = 300
        
        self.scene = display(x=0, width=x_width, height=y_height,
                             title="Cart Pole Demo",visible=False)
        self.scene.autoscale=0
        
        self.LearningCurveGraph = gdisplay(x=0, y=y_height, width=2*x_width, height=y_height,
                        title="Learning Curve", xtitle="Episode", ytitle="Steps", foreground=color.black, background=color.white)
        
        #The cart is a Frame
        self.Frame_CartPole = frame(display=self.scene)
        self.Car = box(frame=self.Frame_CartPole,pos=(0,0,0), length=2, height=1, width=1,color=color.orange) 
        self.CarWellfl = ring(frame=self.Frame_CartPole,pos=(-0.5,-0.5,0.5), axis=(0,0,1), radius=0.3, thickness=0.05)
        self.CarWellbl = ring(frame=self.Frame_CartPole,pos=(0.5,-0.5,0.5), axis=(0,0,1), radius=0.3, thickness=0.05)
        self.CarWellfr = ring(frame=self.Frame_CartPole,pos=(-0.5,-0.5,-0.5), axis=(0,0,1), radius=0.3, thickness=0.05)
        self.CarWellbr = ring(frame=self.Frame_CartPole,pos=(0.5,-0.5,-0.5), axis=(0,0,1), radius=0.3, thickness=0.05)
        
        l = 3.0
        theta = radians(0)
        px = l*sin(theta)
        py = l*cos(theta)
        self.Pendulum    = curve(frame=self.Frame_CartPole,pos=[(0,0,0), (px,py,0)], radius=0.1)
        self.PendulumTop = sphere(frame=self.Frame_CartPole,pos=(px,py,0), radius=0.3)
  
        #Learning Curve
        self.Lcurve = gcurve(gdisplay=self.LearningCurveGraph,color= color.blue)
        
    def PlotFunc(self,s,a,steps):
        
        x     = s[0]
        theta = s[2]
        l=3        
        px = l*sin(theta)
        py = l*cos(theta)
        self.Frame_CartPole.pos.x = x
        self.Pendulum.x[1] = px
        self.Pendulum.y[1] = py
        self.PendulumTop.x = px
        self.PendulumTop.y = py
        self.scene.title = "Steps: "+str(steps)+ " x:"+str(round(x,1)) + " theta: "+str(round(theta,2))
        self.scene.visible = True 
        # let the visual run a step
        rate(100)
    
    def PlotLearningCurve(self,episode,steps):
        self.LearningCurveGraph.display.visible= True
        self.Lcurve.plot(pos=(episode,steps))
        self.LearningCurveGraph.display.title = "Learning Curve"+ " "+" Episode: "+str(episode)+ " steps:"+str(steps)
        
    



def GCartPoleDemo(maxepisodes):
    CP   = GCartPole(alpha=0.3,gamma=1.0,epsilon=0.001)
    maxsteps = 1000
    
    grafica  = False    
    #grafica = True
    CP.InitGraphs()
    CP.scene.visible=True
    steps =0
    for i in range(maxepisodes):    
        tt = time.clock()
        last_steps = steps
        total_reward,steps  = CP.SARSAEpisode( maxsteps, grafica )
        
        #total_reward,steps  = CP.QLearningEpisode( maxsteps, grafica )
        tt = time.clock()-tt
        print 'Espisode: ',i,'  Steps:',steps,'  Reward:',str(total_reward),' epsilon: ',str(CP.epsilon),"time",tt 
        
        CP.PlotLearningCurve(i,steps)
        CP.epsilon = CP.epsilon * 0.99
        
        if i>600 or (steps==1000 and last_steps==1000):
            if not CP.scene.visible:
                CP.scene.visible=True
            grafica=True  
        


if __name__ == '__main__':
    GCartPoleDemo(10000)
