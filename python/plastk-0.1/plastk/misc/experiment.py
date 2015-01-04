"""
A test harness for running experiments over combinations of multiple
varying parameters.

The clsases in this module allow the construction of experiments that
automatically test all combinations of values for several experimental
parameters (called factors).

USAGE

[Terminology note: To avoid confusion with PLASTK's Parameter system,
the term 'factor' is used herein to denote an experimental parameter
that can take on different values in different trials of an
experiment. A single unique selection of values for all factors in an
experiment is called an experimental 'condition'.]

To construct an experiment that tests some computation over
combinations of levels of each experimental factor, one defines a
subclass of one of Experiment (for simple experiments) or
FactorExperiment (for more complicated experiments).  The subclass
defines the experimental the factors, the values over which they
should vary, and the test procedure to run for each combination of
parameter values.

To run an experiment, one creates an instance of the newly defined
Experiment subclass, and calls its .run() method, indicating which
experimental conditions (i.e. specific combinations of parameter
values) to run.

In addition, the Experiment and FactorExperiment classes provide a
.main() method that reads a standard set of command line arguments to
direct the run of the experiment.  See Experiment.USAGE for details.

* Simple Experiments *

The class Experiment is designed for simple experiments with a simple
factor structure.  An subclass of Experiment need only define a single
class attribute 'factors', consisting of a list of lists of parameter
values, and a method .proc() to define the computation to perform on
each condition.  See the Experiment documentation for details

* Complex Experiments *

Some experiments require more complex manipulations of the factors
that is possible by simple specifying a list of lists of values to be
combined.  In this case the experiment should be defined as a subclass
of FactorExperiment.  In a FacatorExperiment, experimental conditions
are explicitly defined as instances of the class Factor or one of its
various subclasses.(e.g. Nest, Cross, and Filter).  These classas
allow arbitrary combinations of experimental factors including
automatically filtering out uninteresting cases, and nesting on factor
within one or more levels of another (for example when a specific
parameter is only applicable when another parameter has a given
value.)  See the class documentation for FactorExperiment, Factor, etc
for details.


$Id: experiment.py 229 2008-03-03 04:38:00Z jprovost $
"""
import getopt,sys,os
import plastk.base.params as params
from plastk.base import SILENT



class Experiment(params.ParameterizedObject):
    """
    A base class for simple factorial experiments.

    This abstract class allows the definition of a simple set of
    experimental factors to vary over and a procedure to run in each
    experimental condition, with facilities for running all or a
    subset of combinations of factor values by number.

    USAGE
    
    To construct an experiment, create a subclass of Experiment and
    define two attributes: (1) the class attribute 'factors' to be a
    sequence of sequences of values over which the experiment should
    run, and (2) the method .proc(self,...) that should take exactly
    one parameter (in addition to self) for each factor in factors.

    EXAMPLE

    Suppose you wish to test a new algorithm called ABG that takes
    parameters alpha, beta, and gamma, and alpha can take integer
    values from 1 to 10, beta can take real values from 0 to 0.2, and
    gamma can be either True or False.  An experiment to test the new
    algorithm could be set up like this:

    class ABGExp(Experiment):

       alpha = range(1,11)
       beta =  0.0, 0.5, 0.1, 0.15, 0.2
       gamma = False, True

       factors = alpha,beta,gamma
       
       def proc(self,N,alpha,beta,gamma):

           data = open('ABG_test_data.dat','r')
           results = ABG(data,alpha=alpha,beta=beta,gamma=gamma)
           output = open('ABG_a=%d_b=%.2f_g=%d_%d.dat'%(alpha,beta,gamma,N),'w')
           output.write(results)           


    The parameter N in the definition 'def proc(self,N,...)' is the
    condition number of the current condition.
    
    To run the experiment, create an instance of the new class:

    e = ABGExp()

    The instance will have an attribute, .conditions, containing a
    list of lists of Factor values with the full Cartesian product of
    all values of all the factors.  This experiment will have
    10 x 5 x2 or 100 total conditions.  To run conditions, call the
    instance's .run() method giving a list of conditions to run by
    number.  To run all the conditions once:

    e.run(range(100))

    or, more generally,
    
    e.run(range(len(e.conditions)))

    The condition numbers given to .run() are wrapped into the current
    set of conditions, allowing the experiment to run multiple trials
    in each condition, giving each a unique number.  For example, to
    run 5 runs in each condition:

    e.run(range(500))

    Because the .proc() method above is defined to include the
    condition number in the output file name, each trial will get its
    own unique data file.

    Alternatively, after setting up an experiment, an experiment
    script meant to be run from the command line can call e.main()
    with no arguments.  The .main() method will read the list of
    conditions to run from the command line.  .main() also supports a
    variety of command line options, see Experiment.USAGE for details.
    """
    

    job_id = params.String(default='NO_ID',doc="""
        A unique string identifier for a particular instance/run of an
        experiment. Can be set by the --id command line flag in .main()""")

    gui = params.Boolean(default=False, doc="""
        A flag indicating whether the experiment should use a GUI.
        This flag exists purely to provide information to the .proc()
        method, if necessary, and has no other effect on the
        experiment.  Can be set by the -g command line flag in .main().""")

    verbosity = params.String(default="NORMAL", doc="""
        The value of base.parameterizedobject.min_print_level to use
        when running the experiment.  Used ONLY by .main().""")
    
    quiet = params.Boolean(default=False, doc="""
        Equivalent to verbosity="SILENT".  Used only by .main().""")
        
    pwd = params.String(default='', doc="""
        The working directory to change to when running in batch
        mode.  Useful for running experiments on clusters or other
        batch-queue systems.  Used only by .main().""")
    
    USAGE = """
    usage: experiment.py [-ghq]
                         [--verbosity=<print_level>]
                         [--pwd=dir]
                         [--size]
                         [--info]
                         [--id=<job-id>]
                         <condition> [<condition> ...]

     -q  Quiet mode
     -g  Allow graphical output, if any.  (for non-batch runs)
     -n  Dry run, initialize, but don't run the condition.
     
     --size outputs the number of conditions in the experiment
     --info outputs a description of the conditions specified,
            then quits.
     --pwd=dir  sets the working directory to dir
     
     
     <print_level> = one of DEBUG, VERBOSE, NORMAL, WARNING,
                     or SILENT (same as -q)
     <job-id> = A unique ID for the job.  Defaults to PIDXXXXX, where
                XXXXX is the process-id of the run.
     <condition> = The number of the experimental condition to run.
                   If more than one condition is specified, they will be
                   run in series. Specifying 'all' runs each condition
                   exactly once.
    """


    def __init__(self,**params):
        super(Experiment,self).__init__(**params)
        self.conditions = cross(self.factors)
        
    def proc(self,*args):
        """
        Override this method to define the test procedure for the
        experiment.  The argument values will be the values of the
        current condition, passed in in the order in which the factors
        are specified in self.factors.
        """
        raise NotImplementedError

    def run(self,conds):
        """
        Run a set of conditions.

        conds should be a sequence of integer indices into the
        condition list.  Indices >= len(self.conditions) will be
        wrapped into the list, allowing each condition to be run
        multiple times with a unique condition number.
        """
        for i in conds:
            level = i%len(self.conditions)
            self.message("Running Condition:", level)
            self.print_condition(level)
            self.proc(i,*self.conditions[level])

    def print_condition(self,i):
        from pprint import pprint
        pprint(self.conditions[i%len(self.conditions)])

    def job_start(self):
        """
        Print some general job information. Used when running in batch
        from .main().
        """
        # JPALERT: not sure if this method is useful anymore.
        print "=============================================="
        print 'Job',self.job_id,'started.'
        print 'Host:',os.getenv('HOSTNAME')
        print 'pwd:', os.getcwd()
        sys.stdout.flush()
        

    def main(self):
        """
        Read the condition list and options from sys.argv and run
        those conditions.  See Experiment.USAGE for command line details.
        """
        try:
            opts,args = getopt.getopt(sys.argv[1:],'hgq',
                                            ['id=','size','info','pwd=','verbosity='])
        except getopt.GetoptError,err:
            print err
            sys.exit(self.USAGE)

        opts = dict(opts)

        if 'all' in args:
            conditions = range(len(self.conditions))
        else:
            try:
                conditions = [int(arg) for arg in args]
            except ValueError:
                print "All conditions must be specified as integers."
                print self.USAGE
                return

        if '-h' in opts:
            sys.exit(self.USAGE)

        if '--size' in opts:
            print "%d conditions." % len(self.conditions)
            return

        self.job_id = opts.get('--id','PID'+`os.getpid()`)
        self.verbosity = opts.get('--verbosity')
        self.gui = '-g' in opts
        self.quiet = '-q' in opts
        self.pwd = opts.get('--pwd')
        if self.pwd:
            print 'Changing directory to',self.pwd
            os.chdir(self.pwd)

        if self.quiet: ParameterizedObject.print_level = SILENT
        if self.verbosity:
            base.min_print_level = getattr(base,self.verbosity)

        if not conditions:
            print "ERROR: no experimental conditions specified."
        elif '--info' in opts:
            for c in conditions:
                print
                print "Condition",c,':'
                self.print_condition(int(c))
        else:
            self.job_start()
            self.run([int(c) for c in conditions])

        print
        print "Job",self.job_id,"finished."

        

class FactorExperiment(Experiment):
    """
    An abstract experiment class, similar to Experiment, except that
    the condition list of the experiment should be specified as a
    instance of the class Factor, or one of its subcasses, rather than
    being specified as a list of values.  Factors allow more
    sophisticated combinations of factors than are possible with
    simple lists.

    EXAMPLE

    Suppose you want to compare the performance of two algorithms ABG,
    which takes parameters alpha, beta, and gamma, and ABGD, which
    takes an additional parameter, delta.  (See the Experiment class
    documentation for background.)  Doing this would be cumbersome
    using a regular Experiment object, because many identical
    conditions would be run to loop over each meaningless value of
    delta in the ABG condition.  With a FactorExperiment, it is
    possible to nest the delta factor so that it exists only in the
    conditions that test teh ABGD algorithm, and not in the ABG
    conditions.  Like this:

    class CompareExp(FactorExperiment):

       algorithm = Factor(fn=[ABG,ABDG])
       alpha = Factor(a=range(1,11))
       beta  = Factor(b=[0.0, 0.05, 0.10, 0.15, 0.20])
       gamma = Factor(g=[False,True])
       delta = Factor(d=[100,200])

       conditions = Nest(Cross(algorithm,alpha,beta,gamma),
                         delta,fn=ABDG)

       def proc(self,N,a,b,g,d=None):

           # load the data
           data = ...

           # run the test
           if fn==ABG:
               results = ABG(data,alpha=a,beta=b,gamma=g)
           else:
               results = ABGD(data,alpha=a,beta=b,gamma=g,delta=d)

           # save the data
           ...

    Note that in the factor experiment, the conditions are specified
    explicitly.  The 'conditions = ' assignment above assigns the
    conditions to a table that is constructed by crossing factors
    alpha, beta, and gamma (i.e. taking the Cartesian product) and
    then nesting the factor delta in only those cells of the table
    where fn == ABG.

    A Factor, (or Cross, Nest, etc) behaves like a sequence of
    dictionaries, each containing the parameter assignments for
    self.proc(). When the experiment is run, one condition c is selected
    from self.conditions and passed as self.proc(i,**c) where i is the
    index into the condition list used to select c.
    
    A FactorExperiment's .run() and .main() methods behave the same
    way they do in a simple Experiment().
    """
    def __init__(self,**params):
        super(Experiment,self).__init__(**params)
        try:
            factors = self.factors
        except AttributeError:
            pass
        else:
            if factors:
                self.conditions = Cross(*self.factors)

    def run(self,conds):
        for i in conds:
            print
            print "Running condition %d:"%i
            level = i%len(self.conditions)
            self.print_condition(i)
            self.proc(i,**self.conditions.levels[level])

    def print_condition(self,i):
        """
        Print the name of condition i.
        """
        cond = self.conditions[i % len(self.conditions)]
        for k,v in cond.items():
            print k,' = ',v

    def condition_name(self,num):
        """
        Generate a name for the current condition by combining
        the various values of each factor.
        """
        def name(item):
            if isinstance(item,type):
                return item.__name__
            else:
                return item
        return '-'.join(['%s=%s'%(k,name(v)) for k,v in self.conditions[num].iteritems()])


class Factor(object):
    """
    A sequence of variable assignments for an experiment.
    
    A Factor is a sequence object in which each element of the
    sequence contains a dictionary of values that defines a set of
    variable values that are of interest in one condition of an
    experiment.  This class defines a simple 'flat' factor in which
    all the variable assignments are specified explicitly.

    USAGE:

    Factor(var = seq[,var2 = seq2[...]])

    Each keyword argument given to the Factor constructor defines the
    name of a variable and the sequence of values it should take in
    the experiment.  When multiple variables are specified they must
    each have the same number of values, and the values will vary
    together by ordinal position in the sequence.  Such a combined set of
    variable values is called a 'level' of the factor.

    EXAMPLE:

    >>> color = Factor(name = ('red','green','blue'),
    ...                rgb = ('#FF0000','#00FF00','#0000FF'))
    >>> color[0]
    {'rgb': '#FF0000', 'name': 'red'}
    >>> color[2]
    {'rgb': '#0000FF', 'name': 'blue'}

    Subclasses of Factor allow the definition of more complicated
    combinations of variables.
    """
    def __init__(self,**attribs):
        super(Factor,self).__init__
        if not attribs:
            raise "No attributes specified."
        self.levels = [dict() for v in attribs.values()[0]]
        for attrib,values in attribs.items():
            if len(values) != len(self.levels):
                raise "Not all attributes have the same number of values."
            for i,v in enumerate(values):
                self.levels[i][attrib] = v

    __len__ = lambda self: len(self.levels)
    __getitem__ = lambda self,key: self.levels[key%len(self)]
    __iter__ = lambda self: iter(self.levels)

class Cross(Factor):
    """
    A factor representing the crossing (cartesian product) of two or
    more other factors.

    The length of the crossing will be the product of the lengths of
    the constituent factors, and every combination of levels of the
    constituent factors is represented in the crossing.

    EXAMPLE

    >>> conds = Cross(Factor(a = (1,2,3)), Factor(b = (10,20,30)))
    >>> for c in conds:
    ...   print c['a'] + c['b']
    ... 
    11
    21
    31
    12
    22
    32
    13
    23
    33
    """
    def __init__(self,*factors):
        if len(factors) == 1:
            self.levels = factors[0].levels
        else:
            first = factors[0]
            rest = factors[1:]
            self.levels = []
            for l1 in first.levels:
                crossing = Cross(*rest)
                for l2 in crossing.levels:
                    newlevel = dict(l1)
                    newlevel.update(l2)
                    self.levels.append(newlevel)

class Nest(Factor):
    """
    A factor that creates a nesting of one factor in some subset of
    the levels of another factor.

    USAGE

    Nest(nest_factor,nested_factor[,var=val[,var2=val2],...])

    where: 
    nest_factor = The factor in which to nest.
    nested_factor = The factor being nested.
    var = val, ... = a set of variable assignments indicating which
    levels to nest the factor in.

    EXAMPLE

    Create three factors, low, med, and hi.  Cross low and med, and
    then nest hi in the levels where low = 1.
    
    >>> low = Factor(lo = (1,2,3))
    >>> med = Factor(med = (10,20,30)
    ... )
    >>> hi = Factor(hi = (100,200,300))
    >>> conds = Nest(Cross(low,med),hi, lo=1)
    >>> for c in conds:
    ...   print c
    ... 
    {'lo': 1, 'hi': 100, 'med': 10}
    {'lo': 1, 'hi': 200, 'med': 10}
    {'lo': 1, 'hi': 300, 'med': 10}
    {'lo': 1, 'hi': 100, 'med': 20}
    {'lo': 1, 'hi': 200, 'med': 20}
    {'lo': 1, 'hi': 300, 'med': 20}
    {'lo': 1, 'hi': 100, 'med': 30}
    {'lo': 1, 'hi': 200, 'med': 30}
    {'lo': 1, 'hi': 300, 'med': 30}
    {'lo': 2, 'med': 10}
    {'lo': 2, 'med': 20}
    {'lo': 2, 'med': 30}
    {'lo': 3, 'med': 10}
    {'lo': 3, 'med': 20}
    {'lo': 3, 'med': 30}

    """
    def __init__(self,nest_factor,nested_factor,**conditions):        
        from operator import __and__

        self.levels = []
        for l1 in nest_factor.levels:
            if reduce(__and__,[l1[k] == v for k,v in conditions.iteritems()]):
                for l2 in nested_factor.levels:
                    new = dict(l1)
                    new.update(l2)
                    self.levels.append(new)
            else:
                self.levels.append(dict(l1))

class Filter(Factor):
    """
    A Factor formed by filtering some levels from another Factor.

    USAGE

    Filter(factor,pred)

    where: 
    factor = The Factor object to be filtered.
    pred = a predicate (boolean function) that takes the level of a
           factor as input and returns True iff the level should be
           included in the filtered factor.
           
    EXAMPLE

    Cross two factors, and remove the cells where their variable
    values are the same.

    >>> a = Factor(a = range(3))
    >>> b = Factor(b = range(3))
    >>> conds = Filter(Cross(a,b),lambda x: x['a'] != x['b'])
    >>> for c in conds: print c
    ... 
    {'a': 0, 'b': 1}
    {'a': 0, 'b': 2}
    {'a': 1, 'b': 0}
    {'a': 1, 'b': 2}
    {'a': 2, 'b': 0}
    {'a': 2, 'b': 1}
   
    
    """
    def __init__(self,factor,cond):
        self.levels = filter(cond,factor)

   
def cross(factors):
    """
    Take a list of sequences, and return the Cartesian product
    of those sequences:

    EXAMPLE
    
    >>> cross((range(3),range(3)))
    [[0, 0], [1, 0], [2, 0], [0, 1], [1, 1], [2, 1], [0, 2], [1, 2], [2, 2]]

    """
    from operator import __add__
    if not factors:
        return [[]]
    else:
        return [[x] + y for y in cross(factors[1:]) for x in factors[0] ]

    


