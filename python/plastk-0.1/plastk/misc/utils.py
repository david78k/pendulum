"""
Various utility functions.

$Id: utils.py 229 2008-03-03 04:38:00Z jprovost $
"""



import numpy
from numpy import ones,exp,sqrt,zeros,argsort,inf
import sys,operator

import plastk.base.rand as rand


def smooth(data,N=10,step=1):
    """
    Smooth sequence data using a moving window of width N.  Returns a
    list of tuples, <i, mu, stderr>, where i is the index of the last
    element of in the moving window, mu is the average of the window,
    and stderr is the standard error of the values in the window.

    If step > 1, the values are subsampled, and every step'th  value
    is returned.
    """
    results = []
    for i in xrange(N,len(data)):
        avg,var,stderr = stats(numpy.array(data[i-N:i]))
        results.append((i-1,avg,stderr))
    return results[0::step]

def smooth2(data,N=10):
    k = numpy.array([1.0]*N)/N
    sm = numpy.convolve(data,k)
    return sm[N:]


def median_filter(data,N=5):
    """
    Median filter sequence data with window of width N.
    """
    results = zeros(len(data-N))
    for i in xrange(N,len(data)):
        x = data[i-N:i]
        s = argsort(x)
        results[i] = x[s[(N/2)+1]]
    return results
        

def mmin(ar):
    """
    Take the min value over an array of arbitrary dimension. Works for
    slices and other arrays where ar.flat is undefined.
    """
    if len(ar.shape) == 1:
        return min(ar)
    else:
        return min([mmin(x) for x in ar])

def mmean(ar):
    """
    Take the average of an array of arbitrary dimension. Works for
    slices and other arrays where ar.flat is undefined.
    """
    from numpy import average
    if len(ar.shape) == 1:
        return average(ar)
    else:
        return average([mmean(x) for x in ar])

def msum(ar):
    """
    Take the sum of an array of arbitrary dimension. Works for
    slices and other arrays where ar.flat is undefined.
    """
    from numpy import sum
    if len(ar.shape) == 1:
        return sum(ar)
    else:
        return sum([msum(x) for x in ar])
    
def mvariance(data):
    """
    Take the variance of an array of arbitrary dimension. Works for
    slices and other arrays where ar.flat is undefined.
    """
    from numpy import multiply
    tmp = data - mmean(data)
    return mmean(multiply(tmp,tmp))
    
def mmax(ar):
    """
    Take the max of an array of arbitrary dimension. Works for
    slices and other arrays where ar.flat is undefined.
    """
    if len(ar.shape) == 1:
        return max(ar)
    else:
        return max([mmax(x) for x in ar])

def stats(data):
    """
    Assumes a matrix of data with variables on the columns
    and observations on the rows.  Returns the mean,
    variance and standard error of the data.
    """
    from numpy import average,sqrt
    mean = average(data)
    var  = average((data-mean)**2)
    stderr = sqrt(var)/sqrt(len(data))
    return mean,var,stderr


def curve_avg_ttests(data1,data2,step=1,xmax=None):
    """
    Compute two-tailed significance data for the differences between
    two averaged curves, such as learning curves.  Step through the
    x-axis of the data, computing means and significances at each step.

    data1 = sequence of sequences comprising first curve.
    data2 = sequence of sequences comprising second curve.
    step = the step size for stepping through the data.
    xmax = only compute up to this point on the x axis.
    """

    tempmax = min(min(map(len,data1)),map(len,data2))

    if xmax:
        xmax = min(tempmax,xmax)
    else:
        xmax = tempmax
        
    a = scipy.array(data1)
    b = scipy.array(data2)

    results = []
    for i in xrange(0,xmax,step):
        t,p = scipy.stats.ttest_ind(a[:,i],b[:,i])
        mu_a = scipy.average(a[:,i])
        mu_b = scipy.average(b[:,i])
        results.append( (i,mu_a,mu_b,t,p) )

    return results



def median(data):
    """
    Return the median of a sequence of data.
    """
    N = len(data)
    inds = argsort(data)
    return data[inds[N/2+1]]

def partition(data,pivot):
    lt,ge = [],[]
    for x in data:
        if x < pivot:
            lt.append(x)
        else:
            ge.append(x)
    return lt,ge
    

def norm(ar,L=2):
    return sum(abs(ar)**L)**(1.0/L)

def L2norm(ar):
    from numpy import dot,sqrt
    return sqrt(sum(ar**2))

def matrixnorm(m):
    """
    L2 norm of each row of a matrix.
    """
    return numpy.sqrt(numpy.sum(m*m,axis=1))


def histogram(data,num_bins=10,bins=[]):
    """
    Histogram the given sequence of data.

    If bins is empty, then the data are histogrammed into num_bins
    bins, equally spaced between min(data) and max(data). If bins is
    non-empty, then the data are binned into len(bins) different
    bins, where bins[i] is the separator between bin i-1
    and partition i.  All data less than bins[0[ are placed in bin 0.

    The function returns (bins,counts) where bins is defined above,
    and counts is the number of data in each bin.
    """
    if not bins:
        xmin = min(data)
        xmax = max(data)
        xrange = xmax-xmin
        step = xrange/float(num_bins)
        bins = [(i*step+xmin) for i in range(num_bins)]
        
    counts = [0 for x in bins]

    for x in data:
        if x <= bins[0]:
            counts[0] += 1
        elif x > bins[-1]:
            counts[-1] += 1
        else:            
            for i,(lo,hi) in enumerate(zip(bins[:-1],bins[1:])):
                if lo < x <= hi:
                    counts[i] += 1
    return bins,counts

def plot_hist(bins,counts,fmt='%10.2f',bar_width=50):
    """
    Print a text plot of a histogram as generated by the histogram()
    function.
    """
    if max(counts) > bar_width:
        unit_size = max(counts)/bar_width
    else:
        unit_size = 1
    print "UNIT_SIZE =", unit_size

    for b,c in zip(bins,counts):
        prefix = fmt % b + ' : '
        print fmt % b,':', '*'*c/unit_size, c


def normalize_sum(ar):
    """
    Given an array, return a normalized version of the array whose
    elements sum to unity.
    """
    d = float(sum(ar))
    if d == 0:
        return ar * 0.0
    return ar/d

def entropy(X):
    """
    Computes the entropy of a histogram contained in sequence X.
    """
    from numpy import log,sum
    def fn(x):
        if x == 0:
            return 0
        else:
            return x*(log(x)/log(2))

    P = X/float(sum(X))
    return -sum(map(fn,P))

def gini_coeff(X,pad_len=0,pad_value=0):
    """
    Computes the Gini coefficient of a set of values contained in the
    1-d array X.

    If pad_len > len(X), X is padded out to length pad_len with
    the value pad_value (default 0).
    """
    
    # from http://mathworld.wolfram.com/GiniCoefficient.html
    # note there is probably a more efficient (O(n log n)) computation using
    # argsort(X), but this one was easiest to implement.
    
    from numpy import argsort,zeros,concatenate as join 
    if pad_len > len(X):
        X = join((X,zeros(pad_len-len(X))+pad_value))
    G = 0.0
    n = len(X)
    for xi in X:
        for xj in X:
            G += abs(xi-xj)
    return G/(2*n*n*mmean(X)) * (n/(n-1))

def gaussian(dist,stddev):
    """
    Compute a gaussian kernel function at the given distance from the
    kernel center with the given standard deviation. The computation
    caps extremely large distances to prevent math errors from
    exponentiating huge negative numbers.
    """
    X = - dist**2/(2*stddev**2)

    # cut off exponent at -500 to prevent overflow errors
    if isinstance(X,numpy.ndarray):
        numpy.putmask(X,X<-500,-500)
    elif X < -500:
        X = -500
        
    return exp(X)

def simple_gaussian_kernel(D):
    """
    A simple gaussian kernel function.  Assumes that input D has
    already been adjusted to account for the kernel width.   The computation
    caps extremely large distances to prevent math errors from
    exponentiating huge negative numbers.
    """
    X = -(D**2)
    # cut off exponent at -500 to prevent overflow errors
    if isinstance(X,numpy.ndarray):
        numpy.putmask(X,X<-500,-500)
    elif X < -500:
        X = -500
        
    return exp(X)

def decay(time,half_life):
    """
    Compute the exponential decay factor for a time given a decay with
    a specific half-life.
    """
    return 0.5**(time/float(half_life))



def choose(a,b,pred):
    """
    Return a if pred(a,b) is true, otherwise return b.

    Deprecated in Python 2.5.  Use 'a if pred(a,b) else b' instead.
    """
    if pred(a,b):
        return a
    else:
        return b

def best_N(seq,N=1,choice=max):
    """
    Return the 'best' N items in a sequence, where the best is
    determined by the given choice function that chooses one item from
    a sequence.  E.g. best(L,5,choice=min) will choose the lowest 5
    items in list L.
    """
    from sets import Set

    if len(seq) <= N:
        return seq
    
    if not isinstance(seq,Set):
        seq = Set(seq)
    r = (reduce(choice,seq),)
    if N==1:
        return r
    else:
        return r + best_N(seq-Set(r),N-1,choice)

def best_N_destructive(seq,N=1,pred=operator.gt):
    """
    Delete all but the best N items in seq, where the best is
    determined by the given comparison predicate.
    """
    if len(seq) > N:
        for i in xrange(N):
            for j in xrange(i+1,len(seq)):
                if pred(seq[j],seq[i]):
                    tmp = seq[j]
                    seq[j] = seq[i]
                    seq[i] = tmp
        del seq[N:]

    return seq


def weighted_sample(distr):
    """
    Given a discrete probability distribution incoded as an array of
    weights randomly sample from the distribution and return the index
    of the selected element.
    """
    return rand.sample_index(distr)


def analyse_transitions(T):
    """
    Print a summary analysis of transition data from a Markov decision
    process (MDP).
    
    Given a rank-3 array T in which each cell T[s,a,r] contains the
    frequency of occurence of the transition into state r from state s
    given action a in an MDP, print a table giving the most frequently
    occuring result for each state and action, along with the
    frequency of that state.
    """
    # JPALERT: I'm not sure where this function should live. It is not
    # quite general enough to be in misc.utils, but I don't really
    # have another home for it.  Maybe misc.mdp?
    from numpy import zeros
    states,actions,results = T.shape

    entropies = zeros((states,actions)) * 0.0
    counts = zeros((states,actions)) * 0.0
    max_prob = zeros((states,actions)) * 0.0
    max_act = zeros((states,actions)) * 0.0
    
    for s in range(states):
        for a in range(actions):
            entropies[s,a] = entropy(normalize_sum(T[s,a]))
            counts[s,a] = sum(T[s,a])
            max_prob[s,a] = max(normalize_sum(T[s,a]))
            max_act[s,a] = argmax(normalize_sum(T[s,a]))

    print '       : ',
    for c in 'FBLR':
        print '%10s' % c,
    print
    print '-------------------------------------------------------------'
    

    for r in range(states):        
        print '%6d' % r,': ',
        for c in range(actions):
            print '%6.2f (%2d)' % (max_prob[r,c],max_act[r,c]),
        print


def get_schema_reliabilities(T,threshold=0.9):
    """
    Print the most reliable transitions (schemas) in a Markov decision
    process (MDP).

    Given a rank-3 array T in which each cell T[s,a,r] contains the
    frequency of occurence of the transition into state r from state s
    given action a in an MDP, print each transition (s,a,r) where
    T[s,a,r] is greater than threshold.
    """
    # JPALERT: I'm not sure where this function should live. It is not
    # quite general enough to be in misc.utils, but I don't really
    # have another home for it.  Maybe misc.mdp?
    num_contexts,num_actions,num_results = T.shape
    
    for c in range(num_contexts):
        for a in range(num_actions):
            for r in range(num_results):
                if T[c,a,r] >= threshold:
                    print (c,a,r), ' => %.2f' % T[c,a,r]


def print_table(table,annote=None,
                width=10,fmt='%.2f',col_labels=None,row_labels=None,
                out=sys.stdout):
    """
    Print a rank-2 array as a table of data using the given
    formatting, with optional row and column labels.  If given, the
    parameter annote should contain a rank-2 array of
    annotations to be printed with each cell.  In the case of
    annotations, the format string fmt must contain a second directive
                to format the annotation.
    """
    rows,cols = table.shape

    if not col_labels:
        col_labels = map(str,range(cols))
    if not row_labels:
        row_labels = map(str,range(rows))
           
    sfmt = '%%%ds' % width

    out.write(sfmt % '')
    for s in col_labels:
        out.write(sfmt % s)
    out.write('\n')
    for i in range(width*(cols+1)):
        out.write('-')
    out.write('\n')
                   
    for r in range(rows):
        out.write(sfmt % ('%s :' % row_labels[r]))
        for c in range(cols):
            if annote:
                out.write(sfmt % (fmt % (table[r,c],annote[r,c])))
            else:
                out.write(sfmt % (fmt % table[r,c]))
        out.write('\n')
            
        

def read_matlab(file):
    """
    Read a file containing an array in MatLab text (.dat) format, and
    return the corresponding array.
    """
    import sre
    f = open(file,'r')
    data = []
    str = f.readline()
    while str:
        l = sre.split(' ',str)
        del(l[-1])
        data.append(map(float,l))
        str = f.readline()

    return numpy.array(data)

def write_matlab(data,filename):
    """
    Assumes data is a 2D array (or list of lists).  Writes
    data to file, one row per line.
    """

    f = open(filename,'w')
    for row in data:
        for x in row:
            f.write(`x`)
            f.write(' ')
        f.write('\n')
    f.close()



def uniq(L):
    """
    'Uniquify' list L, returning a new list in which each sequence of
    repeated elements in L has been replaced by a single element from
    the sequence.  (Similar to the uniq command in UNIX.)
    """
    if not L: return []
    result = [ L[0] ]
    for x in L[1:]:
        if x != result[-1]:
            result.append(x)
    return result

def time_call(N,fn,*args,**kw):
    """
    Run fn(*args,**kw) N times, and return the average time taken per
    call as measured by time.clock().
    """
    import time
    total = 0
    for i in range(N):
        start = time.clock()
        fn(*args,**kw)
        end = time.clock()
        total += (end-start)
    return total/N
    

####################
class Stack(list):
    """
    A simple stack data structure.

    This subclass of list adds the standard stack operations top(),
    empty(), and push().
    """
    __slots__ = []
    def top(self):
        return self[-1]
    def empty(self):
        return len(self) == 0
    def push(self,x):
        return self.append(x)
    
####################

class LogFile(object):
    """
    A picklable/checkpointable logfile object.

    A new Logfile object opens the given file in the given mode and
    provides write() and writeline() methods for logging data to the
    file.when the LogFile object is pickled, the current write
    position [file.tell()] of the underlying file is saved.  If the
    LogFile object is later unpickled, the file is opened in append
    mode, and the write position is set to the saved position using
    file.seek().  This enables a process with periodic checkpointing
    to resume logging from the point of the last checkpoint, even if
    the original process had continued writing to the file for some
    time after the last checkpoint.
    """    
    def __init__(self,name,mode='w'):
        self._file = file(name,mode)
        
        
    def __getstate__(self):
        self._file.flush()
        state = self._file.name,self._file.tell()
        print "Logfile saving state:", state
        return state

    def __setstate__(self,state):
        print "LogFile restoring state:", state
        name,pos = state
        self._file = file(name,'a')
        self._file.seek(pos)
        self._file.truncate()

    def write(self,str):
        self._file.write(str)

    def writelines(self,seq):
        for l in seq:
            self.write(l)
            
####################

class Struct(object):
    """
    A simple dynamic structure object, to save from having to declare a
    new class every time one needs a data capsule that provides
    getattr access to its data.

    Usage:
    >>> s = Struct(foo=1,bar=2)
    >>> s.foo
    1
    >>> s.bar
    2
    """
    def __init__(self,**kw):
        for k,v, in kw.iteritems():
            setattr(self,k,v)

