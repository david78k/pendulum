from MoutainCarDemo import *
from visual import * # imports the visual system to see the graphics
from visual.graph import *
import psyco
psyco.full()




class GMountainCar(MountainCar):

    
    def InitGraphs(self):
        
        x_width  = 400
        y_height = 300
        
        
        self.clwindow = gdisplay(x=0, y=0, width=x_width, height=y_height,
                        title="State Space", xtitle="Position", ytitle="Speed",
                        xmax=0.6, xmin=-1.6, ymax=0.08, ymin=-0.08,
                        foreground=None, background=None)
        
        self.scene = display(x=self.clwindow.width, width=x_width, height=y_height,
                             title="Mountain Car Demo")
        
        self.LearningCurveGraph = gdisplay(x=0, y=y_height, width=2*x_width, height=y_height,
                        title="Learning Curve", xtitle="Episode", ytitle="Steps",
                        ymin=0.0, foreground=color.black, background=color.white)
        
        
        #self.clwindow.display.visible = False
        #self.scene.visible = True
        
        
        #Init Mountain        
        self.MountainGraph   = curve(scene=self.scene,x=arange(-1.6,0.6,0.05), color=color.green)
        self.MountainGraph.y = sin(3*self.MountainGraph.x)        
        
        # Init Car
        self.Car = sphere(scene=self.scene,pos=(-0.5,sin(3*-0.5)+0.11),radius=0.1)        
        
        #Goal
        self.Goal = ring(pos=(0.45,sin(3*0.5)+0.1),radius=0.2)
    
        # classifiers
        self.clgraph = gdots(gdisplay=self.clwindow)        
        
        #trajectory
        self.Gtrajectory = gcurve(gdisplay=self.clwindow,color=color.yellow)       
       
        
        #Learning Curve
        self.Lcurve = gcurve(gdisplay=self.LearningCurveGraph,color=color.blue)
        
    def PlotFunc(self,x,a,steps):
        
        # update car position
        self.Car.pos.x= x[0]
        self.Car.pos.y= sin(3*x[0])+0.11
        
        self.Gtrajectory.plot(pos=(x[0],x[1]),color=color.yellow)
        
        #update display
        self.scene.title = "Steps: "+str(steps)
        
        # let the visual run a step
        rate(1000)
    
    def PlotPopulation(self):        
               
        
        #update plot population
        for dot in self.clgraph.dots:
            dot.visible=False        
        self.clgraph.dots=[]
        del self.clgraph.dots
        del self.clgraph       
        self.clgraph = gdots(gdisplay=self.clwindow)
        for cl in self.Population:
            col = color.white
            if cl.Relevance>0.0:
                col = color.red
            self.clgraph.plot(pos=cl.state,color=col)
        
        
        
        self.clwindow.display.visible = True    

        
    def PlotLearningCurve(self,episode,steps):
        self.LearningCurveGraph.display.visible= True
        self.Lcurve.plot(pos=(episode,steps))
        self.Gtrajectory.gcurve.visible = False
        del self.Gtrajectory
        self.Gtrajectory = gcurve(gdisplay=self.clwindow,color=color.yellow)
        self.LearningCurveGraph.display.title = "Learning Curve"+ " "+" Episode: "+str(episode)+ " steps: "+str(steps)+ "\t epsilon: "+str(self.epsilon)
       
    
    


def GMountainCarDemo(maxepisodes):
    MC       = GMountainCar(alpha=0.3,gamma=1.0,epsilon=0.01)
    maxsteps = 1000
    grafica  = True
    MC.InitGraphs()
    
    
    for i in range(maxepisodes):    
    
        total_reward,steps  = MC.SARSAEpisode( maxsteps, grafica )    
        print 'Espisode: ',i,'  Steps:',steps,'  Reward:',str(total_reward),' epsilon: ',str(MC.epsilon)
        MC.PlotLearningCurve(i,steps)
        MC.epsilon = MC.epsilon * 0.99
        
        
                
        if i>200:
            if not MC.scene.visible:
                MC.scene.visible=True
            grafica=True


if __name__ == '__main__':
    GMountainCarDemo(500)              
