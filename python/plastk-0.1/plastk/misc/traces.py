"""
Object classes for recording and plotting time-series data.

This module defines a set of DataRecorder object types for recording
time-series data, a set of Trace object types for
specifying ways of generating 1D-vs-time traces from recorded data,
and a TraceGroup object that will plot a set of traces on stacked,
aligned axes.

$Id: traces.py 229 2008-03-03 04:38:00Z jprovost $
"""
__version__ = '$Revision: 229 $'


from plastk.base.params import ParameterizedObject
import plastk.base.params as params

import numpy

class DataRecorder(ParameterizedObject):
    """
    Record time-series data.

    A DataRecorder instance stores a set of named time-series
    variables, consisting of a sequence of recorded data items of any
    type, along with the times at which they were recorded.

    DataRecorder is an abstract class for which different
    implementations may exist for different means of storing recorded
    data.  For example, the subclass InMemoryRecorder stores all the
    data in memory.

    SAGE:

    A DataRecorder instance is used as follows:
    
      - Method .add_variable adds a named time series variable.
      - Method .record_data records a new data item and timestamp.
      - Method .get_data gets a time-delimited sequence of data from a variable
    
    """

    __abstract = True

    def add_variable(self,name):
        """
        Create a new time-series variable with the given name.
        """
        raise NotImplementedError

    def __get_item__(self,name):
        raise NotImplementedError

                    

class InMemoryRecorder(DataRecorder):
    """
    A data recorder that stores all recorded data in memory.
    """

    def __init__(self,**params):
        super(InMemoryRecorder,self).__init__(**params)
        self._vars = {}

    def add_variable(self,name,**kw):
        self._vars[name] = InMemoryTraceVariable(**kw)

    def __getitem__(self,name):
        return self._vars[name]


class InMemoryTraceVariable(ParameterizedObject):

    dim = params.List(default=[])
    dtype = params.ClassSelector(object,default=int)
    
    def __init__(self,**kw):
        super(InMemoryTraceVariable,self).__init__(**kw)
        self._data = numpy.zeros([8]+self.dim,dtype=self.dtype)
        self._len = 0

    def __len__(self):
        return self._len

    def __iter__(self):
        for i in xrange(len(self)):
            yield self[i]
            
    def __getitem__(self,idx):
        if isinstance(idx,slice):
            start,stop,step = idx.indices(len(self))
            return self._data[start:stop:step]
        elif isinstance(idx,int):
            N = len(self)
            if -N <= idx < N:
                start,stop,step = slice(idx,None).indices(N)
                return self._data[start]
            else:
                raise IndexError, "Trace variable index out of range."


    def __setitem__(self,idx,value):
        if isinstance(idx,slice):
            minsize = idx.stop
        elif isinstance(idx,int):
            minsize = idx+1
        else:
            raise TypeError, "InMemoryTraceVariable indices must be integers."

        while minsize > len(self._data):
            self._grow()
        self._len = minsize

        self._data[idx] = value

    def _grow(self):
        self._data = numpy.concatenate((self._data,numpy.zeros(self._data.shape)))
