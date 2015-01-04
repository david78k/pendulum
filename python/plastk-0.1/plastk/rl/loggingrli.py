"""
A more advanced version of base.rl.RLI with support for automatic
variable logging, checkpointing, and a simple component-based GUI.

$Id: loggingrli.py 229 2008-03-03 04:38:00Z jprovost $
"""
import time,sys,threading,os

from plastk.base.rl import RLI
from plastk.misc.traces import InMemoryRecorder
import plastk.base.params as params
from plastk.base.params import Parameter,ParameterizedObject
from plastk.base import rand

NewColumn = 'new column'

class LoggingRLI(RLI):
    filestem =      Parameter(default='')
    catch_signals = Parameter(default=[])

    ckpt_extension    = Parameter(default = '.ckpt')
    steps_per_ckpt    = Parameter(sys.maxint)
    episodes_per_ckpt = Parameter(sys.maxint)

    rename_old_data = Parameter(default=True)

    gui_button_orient = Parameter(default='horizontal')

    data_recorder_type = Parameter(default = InMemoryRecorder)
    data_recorder_params = Parameter(default = dict())
    data_file_suffix = Parameter(default='.data.gz')

    ckpt_attribs = ['ep_count',
                    'step_count',
                    'steps_per_ckpt',
                    'episodes_per_ckpt',
                    'last_ckpt_step',
                    'last_ckpt_episode',
                    'last_sensation',
                    'next_action',
                    'env',
                    'agent',
                    'episode_data',
                    'step_data'
                    ]

    
    def __init__(self,**args):
        super(LoggingRLI,self).__init__(**args)
        self.step_vars = {}
        self.ep_vars = {}
        self.caught_signal = None
        if not self.filestem:
            self.filestem = self.name
        self.episode_filename = self.filestem + '-episodes' + self.data_file_suffix
        self.step_filename = self.filestem + '-steps' + self.data_file_suffix

        self.checkpointing = False

        self.gui_root = False
        self.gui_runstate = None

        self.action = ''
        self.last_sensation = ''

            
    def init(self,agent,env,**kw):
        super(LoggingRLI,self).init(agent,env,**kw)

        self.step_count = self.ep_count = 0

        if os.access(self.episode_filename,os.F_OK):
            self.remove_or_rename(self.episode_filename)

        self.episode_data = ed = InMemoryRecorder()
        ed.add_variable('start',dtype=int)
        ed.add_variable('length',dtype=int)
        ed.add_variable('reward',dtype=float)

        for name,(fn,dim,dtype) in self.ep_vars.items():
            ed.add_variable(name,dim=dim,dtype=type)

        if self.step_vars:
            if os.access(self.step_filename,os.F_OK):
                self.remove_or_rename(self.step_filename)

            self.step_data = sd = self.data_recorder_type(**self.data_recorder_params)
            for name,(fn,dim,dtype) in self.step_vars.items():
                sd.add_variable(name,dim=dim,dtype=dtype)

        self.last_ckpt_step = 0
        self.last_ckpt_episode = 0


    def remove_or_rename(self,filename):
        # if the  data file already exists either rename it or delete it
        if not self.rename_old_data:
            self.warning("Removing old data file:",filename)
            os.remove(filename)
        else:
            i = 0
            while True:
                stem,ext = filename.split('.cdf')
                new_filename = '%s-%d.cdf'%(stem,i) 
                if os.access(new_filename,os.F_OK):
                    i += 1
                    continue
                self.warning("Renaming old data file to",new_filename)
                os.rename(filename,new_filename)
                break
        
    def steps(self,num_steps,max_episodes=sys.maxint):
        for i in xrange(num_steps):
            if self.ep_count >= max_episodes:
                break
            super(LoggingRLI,self).steps(1)

        
    def add_step_variable(self,name,fn,dtype=int,dim=[]):
        self.step_vars[name] = fn,dim,dtype

    def add_episode_variable(self,name,fn,dtype=int,dim=[]):
        self.ep_vars[name] = fn,dim,dtype


    def start_episode(self):

        if (self.checkpointing and self.ep_count - self.last_ckpt_episode >= self.episodes_per_ckpt):
            self.ckpt_save()
        
        if self.gui_runstate == 'Episode':
           self.gui_runstate_control.invoke('Stop')
           self.request_gui_redraw()
           while self.gui_runstate == 'Stop':
               time.sleep(0.1)

        self.verbose("Starting episode",self.ep_count)
        super(LoggingRLI,self).start_episode()

        self.episode_data['start'][self.ep_count] = self.step_count
        self.episode_data['length'][self.ep_count] = 0
        self.episode_data['reward'][self.ep_count] = 0.0

        for var,(fn,dim,dtype) in self.ep_vars.iteritems():
            self.episode_data[var][self.ep_count] = fn(self)

        self.ep_count += 1

        

    def collect_data(self,sensation,action,reward,next_sensation):
        from numpy import array

        self.sensation = sensation
        self.action = action
        self.reward = reward
        self.next_sensation = next_sensation

        if self.caught_signal:
            import sys
            self.close()
            raise "Caught signal %d" % self.caught_signal
        
        self.episode_data['reward'][self.ep_count-1] += reward
        self.episode_data['length'][self.ep_count-1] += 1

        if self.step_vars:
            for var,(fn,dim,dtype) in self.step_vars.items():
                self.step_data[var][self.step_count] = fn(self)

        if (self.checkpointing and self.step_count - self.last_ckpt_step >= self.steps_per_ckpt):
            self.ckpt_save()

        self.step_count += 1
        


    ###################################################
    # Checkpointing

    def ckpt_steps(self,num_steps,max_episodes=sys.maxint):
        self.checkpointing = True
        self.setup_signals()
        self.steps(num_steps-self.step_count,max_episodes=max_episodes)    
        self.clear_signals()
        self.checkpointing = False
    def ckpt_episodes(self,num_episodes,timeout):
        self.checkpointing = True
        self.setup_signals()
        self.episodes(num_episodes-self.ep_count,timeout)
        self.clear_signals()
        self.checkpointing = False


    def ckpt_filename(self):
        return self.filestem + self.ckpt_extension
    
    def ckpt_save(self):
        from plastk import pkl
        
        self.verbose("Attempting checkpoint, %d episodes, %d steps."%(self.ep_count,self.step_count))
        if self.ckpt_ok():
            self.last_ckpt_step = self.step_count
            self.last_ckpt_episode = self.ep_count

            self.env.sim = self.agent.sim = None
            ckpt = dict(rand_seed = rand.get_seed())

            self.verbose("Checkpointing...")
            for a in self.ckpt_attribs:
                ckpt[a] = getattr(self,a)
                self.verbose(a, ' = ', ckpt[a])

            pkl.dump(ckpt,self.ckpt_filename())
            self.env.sim = self.agent.sim = self
        else:
            self.verbose("No checkpoint, ckpt_ok failed")
            return        

    def ckpt_restore_state(self,filename):
        from plastk import pkl
        ckpt = pkl.load(filename)

        self.verbose("Restoring checkpoint state")
        for a in self.ckpt_attribs:
            self.verbose(a,' = ', ckpt[a])
            setattr(self,a,ckpt[a])
            
        rand.seed(*ckpt['rand_seed'])

        self.env.sim = self.agent.sim = self        
        return ckpt

    def ckpt_resume(self):
        import os
        ckpt_filename = self.ckpt_filename()
        if os.access(ckpt_filename,os.F_OK):
            self.message("Found checkpoint file",ckpt_filename)
            self.ckpt_restore_state(ckpt_filename)
            return True
        else:
            return False

    def ckpt_ok(self):
        """
        Override this method to provide a check to make
        sure it's okay to checkpoint.
        (default = True)
        """
        return True

    def setup_signals(self):
        import signal
        for sig in self.catch_signals:
            self.verbose("Setting handler for signal",sig)
            signal.signal(sig,self.signal_handler)
    def clear_signals(self):
        import signal
        for sig in self.catch_signals:
            self.verbose("Clearing handler for signal",sig)
            signal.signal(sig,signal.SIG_DFL)
        
    def signal_handler(self,signal,frame):
        self.caught_signal = signal

    #########################################
    # GUI

    def gui(self,*frame_types):
        """
        Each of frame_types must be either
        (1) the string NewColumn, to start a new column or
        (2) a function that takes (tk_root,rli) and returns a Tkinter widget
            where tk_root is a Tkinter frame and rli is the controlling plastk rli.
            The Tkinter widget must have a redraw method, which takes no arguments.
        """
        import Tkinter as Tk
        from threading import Thread,Event
        self.gui_root = Tk.Tk()
        frame = self.gui_init(self.gui_root,frame_types)
        frame.pack(side='top',expand=1,fill='both')
            
        self.gui_root.title( self.name )
        self.gui_root.bind('<<redraw>>',self.gui_redraw)
        self.gui_root.bind('<<destroy>>', self.gui_destroy)
        self.gui_runloop_thread = Thread(target=self.gui_runloop)
        self.gui_runloop_thread.setDaemon(True)

        self.gui_redraw_event = Event()       
        
        def destroy():
            self.gui_runstate = 'Quit'
            
        self.gui_root.protocol('WM_DELETE_WINDOW',destroy)
        self.gui_running = True
        self.gui_runloop_thread.start()
        self.gui_root.mainloop()
        print "GUI Finished."
        self.gui_root = False


    def gui_runloop(self):
        while True:
            time.sleep(0.1)
            while  self.gui_runstate != 'Quit' and self.gui_runstate != 'Stop':
                self.steps(1)
                self.request_gui_redraw()
                if self.gui_runstate == 'Step':
                    self.gui_runstate_control.invoke('Stop')
            if self.gui_runstate == 'Quit':
                break
        print "Ending GUI run loop."

          
    def gui_init(self,root,frame_types):
        import Tkinter as Tk
        import Pmw
        gui_frame = Tk.Frame(root)
        control_frame = gui_frame
        #control_frame = Tk.Frame(gui_frame)
        #control_frame.pack(side='left',fill='both',expand=1)
	self.gui_runstate_control = Pmw.RadioSelect(control_frame,
                                                    labelpos = 'w',
                                                    orient = self.gui_button_orient,
                                                    command = self.gui_runstate_callback,
                                                    label_text = '',
                                                    frame_borderwidth = 1,
                                                    frame_relief = 'ridge')
	self.gui_runstate_control.pack(side='top',fill='none')
	# Add some buttons to the RadioSelect.
	for text in ('Quit','Run', 'Stop', 'Step','Episode'):
	    self.gui_runstate_control.add(text)
	self.gui_runstate_control.invoke('Stop')

    
        self.subframes = []
        g_frame = Tk.Frame(control_frame)
        g_frame.pack(side='left',expand=1,fill='both')
        for ft in frame_types:
            if ft == NewColumn:
                g_frame = Tk.Frame(control_frame)
                g_frame.pack(side='left',expand=1,fill='both')
            else:
                f = ft(g_frame,self)
                self.subframes.append(f)
                f.pack(side='top',expand=1,fill='both')
            
        return gui_frame

    def request_gui_redraw(self):
        if self.gui_root:
            self.gui_root.event_generate("<<redraw>>", when='tail')
        self.gui_redraw_event.wait(1.0)
        self.gui_redraw_event.clear()

    def gui_redraw(self,event):
        for f in self.subframes:
            f.redraw()
        self.gui_runstate_control.invoke(self.gui_runstate)
        self.gui_redraw_event.set()

    def gui_runstate_callback(self,tag):
        self.gui_runstate = tag
        if tag == 'Quit':
            self.gui_root.event_generate('<<destroy>>',when='tail')

    def gui_destroy(self,event):
        self.gui_root.quit()
        self.gui_root.destroy()
            


try:
    import Tkinter as Tk
    from _tkinter import TclError
    import Pmw
    from matplotlib.figure import Figure,SubplotParams
    from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
except ImportError,err:
    print err
    print "LoggingRLI GUI will not be available."
else:
    class VarPlotter(ParameterizedObject,Tk.Frame):
        var = params.String(default='length')
        title = params.String(default='')
        
        tkargs = params.Dict(default={})
        rli = params.ClassSelector(LoggingRLI)
        initial_length = params.Integer(default=500,bounds=(1,None))
        matplotlib_update_period = params.Integer(default=1,bounds=(1,None))
        use_blt = params.Boolean(default=True)
        var = params.String(default='length')
        source = params.String(default='episode_data')

        num_xticks = params.Integer(default=5, bounds=(2,None))
        num_yticks = params.Integer(default=3, bounds=(2,None))
        
        
        def __init__(self,root,**params):
            ParameterizedObject.__init__(self,**params)

            if not self.title:
                self.title = self.source+' : '+self.var
            if 'name' not in params:
                self.name = '%s-%s'%(type(self).__name__,self.var)
            
            Tk.Frame.__init__(self,root,**self.tkargs)

            self.trace_len = Tk.StringVar()
            self.trace_len.set('500')
            group = Pmw.Group(self,tag_text=self.title)
            group.pack(side='top',fill='both',expand=1)
            Pmw.EntryField(group.interior(),
                           label_text='Trace length',
                           labelpos='w',
                           validate='numeric',
                           entry_textvariable=self.trace_len).pack(side='top',fill='x')

            if self.use_blt:
                try:
                    self.plot = Pmw.Blt.Graph(group.interior(),height='2i')
                except TclError,err:
                    self.warning(err)
                    self.warning("Can't load Blt.Graph, using MatPlotLib instead.")
                    self.warning('Set parameter %s.%s.use_blt = False to eliminate this warning.'%(VarPlotter.__module__,VarPlotter.__name__))
                    VarPlotter.use_blt = False
                except:
                    raise                
                else:
                    self.plot.line_create('values',label='',symbol='',smooth='step')
                    self.plot.grid_on()
                    self.plot.pack(side='top',expand=1,fill='both')

            if not self.use_blt:
                self.figure = Figure((4,2),dpi=100)                
                self.plot = self.figure.add_axes((0.15,0.15,0.8,0.75))
                self.line = None
                self.plot.hold(0)
                self.canvas = FigureCanvasTkAgg(self.figure, master=group.interior())
                self.canvas.get_tk_widget().pack(side='top',expand=1,fill='both')
                self.last_yvalues = []
                self.last_xvalues = []

        def get_values(self,N):
            v = getattr(self.rli,self.source)[self.var][:]
            M = len(v[:])
            if M > N:
                return range(M-N,M),v[M-N:M]
            else:
                return range(M),v[:]

        def redraw(self):
            try:
                N = int(self.trace_len.get())
            except ValueError:
                return

            xvalues,yvalues = self.get_values(N)
            if len(yvalues):
                if self.use_blt:
                    ydata = tuple(yvalues[:])
                    xdata = tuple(xvalues)
                    self.plot.element_configure('values',xdata=xdata,ydata=ydata)
                else:
                    if self.rli.step_count % self.matplotlib_update_period == 0:
                        # JPALERT: should replace this with blitting techniques described in
                        # http://www.scipy.org/Cookbook/Matplotlib/Animations
                        self.plot.plot(xvalues,yvalues,linestyle='steps')
                        self.canvas.show()
            self.last_yvalues = yvalues
            self.last_xvalues = xvalues
            
                               
    class TextList(Tk.Frame):
        def __init__(self, root, rli, **args):
            Tk.Frame.__init__(self, root, **args)
            self.rli = rli
            self.textlist = []
            self.ep_count = rli.ep_count
            
            self.list = Pmw.ComboBox(self, dropdown=0, history=0,
                                        labelpos='nw', label_text=self.name)
            self.list.component('scrolledlist').component('listbox').config(exportselection=0)
            self.list.pack(side='top',fill='both',expand=1)

        def redraw(self):
            self.list.component('listbox').insert(self.rli.step_count, self.get_line())
            self.list.selectitem(self.list.component('listbox').size()-1)
            if self.ep_count != self.rli.ep_count:
                self.list.clear()
                self.ep_count = self.rli.ep_count

    class ActionList(TextList):
        name = 'Actions'
        def get_line(self):
            return self.rli.action

    class SensationList(TextList):
        name = 'Sensations'
        def get_line(self):
            return self.rli.last_sensation
