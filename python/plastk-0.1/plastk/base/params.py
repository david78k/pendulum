"""
Subclasses of Parameter, implementing specific parameter types.

$Id: params.py 229 2008-03-03 04:38:00Z jprovost $
"""
__version__='$Revision: 229 $'

import re
import types

from parameterizedobject import Parameter, descendents, ParameterizedObject

# CEBHACKALERT: needs to be finished
class Enumeration(Parameter):
    """
    Enumeration is a Parameter with a list of available values.

    An Enumeration's value is always one from its list of available values.
    """
    __slots__ = ['available']

    def __init__(self, default=None, available=[], **params):
        """
        Create an Enumeration, checking that 'default' is in 'available'.
        """
        Parameter.__init__(self,default=default,**params)
        # CB: just needs to be list-like. has __iter__ method?
        if not type(available)==list:
            raise ValueError("Enumeration must be created with a list of available values.")
        self.available = available
        self.__check_value(default) 
        
    def __set__(self,obj,val):
        """
        Set to the given value, raising an exception if that value is
        not in the list of available ones.
        """
        self.__check_value(val)
        super(Enumeration,self).__set__(obj,val)

    def __check_value(self,val):
        """
        Raises an error if the given value isn't in the list of available ones.
        """
        if not self.available.count(val) >= 1:
            raise ValueError("%s not in %s's list of available values."%(val,self._attrib_name))


def produce_value(value_obj):
    """
    A helper function that produces an actual parameter from a stored
    object: if the object is callable, call it, otherwise return the
    object.
    """
    if callable(value_obj):
        return value_obj()
    else:
        return value_obj


class Dynamic(Parameter):
    """
    Parameter whose value can be generated dynamically by a callable
    object.
    
    If a Parameter is declared as Dynamic, it can be set a callable
    object (such as a function or callable class), and getting the
    parameter's value will call that callable.

    Note that at present, the callable object must allow attributes
    to be set on itself.
    
    [Python 2.4 limitation: the callable object must be an instance of a
    callable class, rather than a named function or a lambda function,
    otherwise the object will not be picklable or deepcopyable.]


    Setting Dynamic.time_fn allows the production of dynamic values to
    be controlled: a new value will be produced only if the current
    value of time_fn is greater than what it was last time the
    parameter value was requested.

    If time_fn is set to None, a new value is always produced.

    If Dynamic.time_fn is set to something other than None, it must,
    when called, produce a number.
    """
    # CB: making Dynamic support iterators and generators is sf.net
    # feature request 1864370. When working on that task, note that
    # detection of a dynamic generator by 'callable' needs to be
    # replaced by something that matches whatever Dynamic becomes
    # capable of using.
    
    time_fn = None # could add a slot for time_fn to allow instances
                   # to override
    
    # CBENHANCEMENT: Add an 'epsilon' slot.
    # See email 'Re: simulation-time-controlled Dynamic parameters'
    # Dec 22, 2007 CB->JAB
        
    def __init__(self,**params):
        """
        Call the superclass's __init__ and set instantiate=True if the
        default is dynamic.
        """
        super(Dynamic,self).__init__(**params)

        if callable(self.default):
            self._set_instantiate(True)
            self._initialize_generator(self.default)


    def _initialize_generator(self,gen):
        """Add 'last time' and 'last value' attributes to the generator."""
        gen._Dynamic_last = None
        # CEB: I'd use None for this, except can't compare a fixedpoint
        # number with None (e.g. 1>None but FixedPoint(1)>None can't be done)
        gen._Dynamic_time = -1
                
        
    def __get__(self,obj,objtype):
        """
        Call the superclass's __get__; if the result is not dynamic
        return that result, otherwise ask that result to produce a
        value and return it.
        """
        gen = super(Dynamic,self).__get__(obj,objtype)

        if not hasattr(gen,'_Dynamic_last'):
            return gen
        else:
            return self._produce_value(gen)
            

    def __set__(self,obj,val):
        """
        Call the superclass's set and keep this parameter's
        instantiate value up to date (dynamic parameters
        must be instantiated).

        If val is dynamic, initialize it as a generator.
        """
        super(Dynamic,self).__set__(obj,val)

        dynamic = callable(val)        
        if dynamic: self._initialize_generator(val)
        if not obj: self._set_instantiate(dynamic)


    def _produce_value(self,gen,force=False):
        """
        Return a value from gen.

        If there is no time_fn, then a new value will be returned
        (i.e. gen will be asked to produce a new value).

        If force is True, or the value of time_fn() is greater than
        what it was was last time produce_value was called, a
        new value will be produced and returned. Otherwise,
        the last value gen produced will be returned.
        """
        if self.time_fn is None:
            value = produce_value(gen)
            gen._Dynamic_last = value
        else:
            
            time = self.time_fn()

            if force or time>gen._Dynamic_time:
                value = produce_value(gen)
                gen._Dynamic_last = value
                gen._Dynamic_time = time
            else:
                value = gen._Dynamic_last

        return value


    def _value_is_dynamic(self,obj,objtype=None):
        """
        Return True if the parameter is actually dynamic (i.e. the
        value is being generated).
        """
        return hasattr(super(Dynamic,self).__get__(obj,objtype),'_Dynamic_last')


    def _inspect(self,obj,objtype=None):
        """Return the last generated value for this parameter."""
        gen=super(Dynamic,self).__get__(obj,objtype)
        
        if hasattr(gen,'_Dynamic_last'):
            return gen._Dynamic_last 
        else:
            return gen 

    
    def _force(self,obj,objtype=None):
        """Force a new value to be generated, and return it."""
        gen=super(Dynamic,self).__get__(obj,objtype)
        
        if hasattr(gen,'_Dynamic_last'):
            return self._produce_value(gen,force=True)
        else:
            return gen 
        


import operator
is_number = operator.isNumberType



# CEBALERT: Number does not fully work with FixedPoint
# (can't use in parameter declaration but can set existing parameter
# to fixedpoint; see testNumber.txt)
class Number(Dynamic):
    """
    Number is a numeric parameter. Numbers have a default value,
    and bounds.  There are two types of bounds: ``bounds`` and
    ``softbounds``.  ``bounds`` are hard bounds: the parameter must have
    a value within the specified range.  The default bounds are
    (None,None), meaning there are actually no hard bounds.  One
    or both bounds can be set by specifying a value
    (e.g. bounds=(None,10) means there is no lower bound, and an
    upper bound of 10).

    Number is also a type of Dynamic parameter, so its value
    can be set to a callable to get a dynamically generated
    number (see Dynamic).

    When not being dynamically generated, bounds are checked when
    a Number is created or set. Using a default value outside the
    hard bounds, or one that is not numeric, results in an
    exception. When being dynamically generated, bounds are
    checked when a the value of a Number is requested. A generated
    value that is not numeric, or is outside the hard bounds,
    results in an exception.

    A separate function set_in_bounds() is provided that will
    silently crop the given value into the legal range, for use
    in, for instance, a GUI.

    ``softbounds`` are present to indicate the typical range of
    the parameter, but are not enforced. Setting the soft bounds
    allows, for instance, a GUI to know what values to display on
    sliders for the Number.

    Example of creating a Number::
      AB = Number(default=0.5, bounds=(None,10), softbounds=(0,1), doc='Distance from A to B.')
    """
    __slots__ = ['bounds','_softbounds']
 
    def __init__(self,default=0.0,bounds=None,softbounds=None,**params):
        """
        Initialize this parameter object and store the bounds.

        Non-dynamic default values are checked against the bounds.
        """
        super(Number,self).__init__(default=default,**params)
        
        self.bounds = bounds
        self._softbounds = softbounds  
        if not callable(default): self._check_value(default)  
        

    def __get__(self,obj,objtype):
        """
        Same as the superclass's __get__, but if the value was
        dynamically generated, check the bounds.
        """
        result = super(Number,self).__get__(obj,objtype)
        if self._value_is_dynamic(obj,objtype): self._check_value(result)
        return result


    def __set__(self,obj,val):
        """
        Set to the given value, raising an exception if out of bounds.
        """
        if not callable(val): self._check_value(val)
        super(Number,self).__set__(obj,val)
        

    def set_in_bounds(self,obj,val):
        """
        Set to the given value, but cropped to be within the legal bounds.
        All objects are accepted, and no exceptions will be raised.  See
        crop_to_bounds for details on how cropping is done.
        """
        if not callable(val):
            bounded_val = self.crop_to_bounds(val)
        else:
            bounded_val = val
            
        super(Number,self).__set__(obj,bounded_val)


    def crop_to_bounds(self,val):
        """
        Return the given value cropped to be within the hard bounds
        for this parameter.

        If a numeric value is passed in, check it is within the hard
        bounds. If it is larger than the high bound, return the high
        bound. If it's smaller, return the low bound. In either case, the
        returned value could be None.  If a non-numeric value is passed
        in, set to be the default value (which could be None).  In no
        case is an exception raised; all values are accepted.
        """
        # Currently, values outside the bounds are silently cropped to
        # be inside the bounds; it may be appropriate to add a warning
        # in such cases.
        if (is_number(val)):
            if self.bounds==None:
                return val
            vmin, vmax = self.bounds 
            if vmin != None: 
                if val < vmin:
                    return  vmin

            if vmax != None:
                if val > vmax:
                    return vmax

        else:
            # non-numeric value sent in: reverts to default value
            return  self.default

        return val


    def _check_value(self,val):
        """
        Checks that the value is numeric and that it is within the hard
        bounds; if not, an exception is raised.
        """
        if not (is_number(val)):
            raise ValueError("Parameter '%s' only takes numeric values"%(self._attrib_name))
        if self.bounds!=None:
            vmin,vmax = self.bounds
            if vmin != None and vmax != None:
                if not (vmin <= val <= vmax):
                    raise ValueError("Parameter '%s' must be between %s and %s (inclusive)"%(self._attrib_name,vmin,vmax))
            elif vmin != None:
                if not vmin <= val: 
                    raise ValueError("Parameter '%s' must be at least %s"%(self._attrib_name,vmin))
            elif vmax != None:
                if not val <=vmax:
                    raise ValueError("Parameter '%s' must be at most %s"%(self._attrib_name,vmax))


    def get_soft_bounds(self):
        """
        For each soft bound (upper and lower), if there is a defined bound (not equal to None)
        then it is returned, otherwise it defaults to the hard bound. The hard bound could still be None.
        """
        if self.bounds==None:
            hl,hu=(None,None)
        else:
            hl,hu=self.bounds

        if self._softbounds==None:
            sl,su=(None,None)
        else:
            sl,su=self._softbounds

                
        if (sl==None): l = hl
        else:          l = sl

        if (su==None): u = hu
        else:          u = su

        return (l,u)



class Integer(Number):

    def _check_value(self,val):
        if not isinstance(val,int):
            raise ValueError("Parameter '%s' must be an integer."%self._attrib_name)
        super(Integer,self)._check_value(val)


class Magnitude(Number):

    def __init__(self,default=1.0,softbounds=None,**params):
        Number.__init__(self,default=default,bounds=(0.0,1.0),softbounds=softbounds,**params)


class Boolean(Parameter):
    __slots__ = ['bounds']

    # CB: what does bounds=(0,1) mean/do for this Parameter?
    def __init__(self,default=False,bounds=(0,1),**params):
        """Initialize a Boolean parameter, allowing values True or False."""
        Parameter.__init__(self,default=default,**params)
        self.bounds = bounds

    def __set__(self,obj,val):
        if not isinstance(val,bool):
            raise ValueError("Boolean '%s' only takes a Boolean value."%self._attrib_name)

        if val is not True and val is not False:
            raise ValueError("Boolean '%s' must be True or False."%self._attrib_name)

        super(Boolean,self).__set__(obj,val)


class String(Parameter):

    def __init__(self,default="",**params):
        """Initialize a string parameter."""
        Parameter.__init__(self,default=default,**params)
        
    def __set__(self,obj,val):
        if not isinstance(val,str):
            raise ValueError("String '%s' only takes a string value."%self._attrib_name)

        super(String,self).__set__(obj,val)


class NumericTuple(Parameter):
    __slots__ = ['length']

    def __init__(self,default=(0,0),length=None,**params):
        """
        Initialize a numeric tuple parameter with a fixed length.
        The length is determined by the initial default value, and
        is not allowed to change after instantiation.
        """
        if length is None:
            self.length = len(default)
        else:
            self.length = length
            
        self._check(default)
        Parameter.__init__(self,default=default,**params)
        
    def _check(self,val):
        if not isinstance(val,tuple):
            raise ValueError("NumericTuple '%s' only takes a tuple value."%self._attrib_name)
        
        if not len(val)==self.length:
            raise ValueError("%s: tuple is not of the correct length (%d instead of %d)." %
                             (self._attrib_name,len(val),self.length))
        for n in val:
            if not is_number(n):
                raise ValueError("%s: tuple element is not numeric: %s." % (self._attrib_name,str(n)))
            
    def __set__(self,obj,val):
        self._check(val)
        super(NumericTuple,self).__set__(obj,val)


class XYCoordinates(NumericTuple):

    def __init__(self,default=(0.0,0.0),**params):
        super(XYCoordinates,self).__init__(default=default,length=2,**params)

                 
class Callable(Parameter):
    """
    Parameter holding a value that is a callable object, such as a function.
    
    A keyword argument instantiate=True should be provided when a
    function object is used that might have state.  On the other hand,
    regular standalone functions cannot be deepcopied as of Python
    2.4, so instantiate must be False for those values.
    """
    def __init__(self,default=None,**params):
        Parameter.__init__(self,default=wrap_callable(default),**params)

    def __set__(self,obj,val):
        if not callable(val):
            raise ValueError("Callable '%s' only takes a callable object."%self._attrib_name)
        super(Callable,self).__set__(obj,wrap_callable(val))


def wrap_callable(c):
    """
    Wrap a callable object in an InstanceMethodWrapper, if necessary.

    If c is an instancemethod, then wrap it and return the wrapper,
    otherwise return c.
    """
    if isinstance(c,types.MethodType):
        return InstanceMethodWrapper(c)
    else:
        return c
        

# CEBALERT: this should be a method of ClassSelector.
def concrete_descendents(parentclass):
    """
    Return a dictionary containing all subclasses of the specified
    parentclass, including the parentclass.  Only classes that are
    defined in scripts that have been run or modules that have been
    imported are included, so the caller will usually first do ``from
    package import *``.

    If the class has an attribute ``abstract``, and it is True, the
    class will not be included.
    """
    return dict([(c.__name__,c) for c in descendents(parentclass)
                 if not c.abstract])


class Composite(Parameter):
    """
    A parameter that is in fact a composite of a set of other
    parameters or attributes of the class.  The constructor argumentt
    'attribs' takes a list of attribute names.  Getting the parameter
    returns a list of the values of the constituents of the composite,
    in the order specified.  Likewise, setting the parameter takes a
    sequence of values and sets the value of the constituent
    attributes sets all the constituents
    """
    __slots__=['attribs','objtype']

    def __init__(self,attribs=[],**kw):
        super(Composite,self).__init__(default=None,**kw)
        self.attribs = attribs

    def __get__(self,obj,objtype):
        """
        Return the values of all the attribs, as a list.
        """
        if not obj:
            return [getattr(objtype,a) for a in self.attribs]
        else:
            return [getattr(obj,a) for a in self.attribs]

    def __set__(self,obj,val):
        """
        Set the values of all the attribs.
        """
        assert len(val) == len(self.attribs),"Compound parameter '%s' got the wrong number of values (needed %d, but got %d)." % (self._attrib_name,len(self.attribs),len(val))
        
        if not obj:
            for a,v in zip(self.attribs,val):
                setattr(self.objtype,a,v)
        else:
            for a,v in zip(self.attribs,val):
                setattr(obj,a,v)


class Selector(Parameter):
    """
    Parameter whose value is set to some form of one of the
    possibilities in its range.

    Subclasses must implement get_range().
    """
    __abstract = True
    
    def get_range(self):
        raise NotImplementedError("get_range() must be implemented in subclasses.")

    
class ObjectSelector(Selector):
    """
    Parameter whose value is set to an object from its list of possible objects.
    """
    __slots__ = ['objects'] 

    def __init__(self,default=None,objects=[],instantiate=True,**params):
        self.objects = objects
        self._check_value(default)
        Parameter.__init__(self,default=default,instantiate=instantiate,**params)
        
    # CBNOTE: if the list of objects is changed, the current value for
    # this parameter in existing POs could be out of the new range.
    
    def _check_value(self,val,obj=None):
        """
        val must be None or one of the objects in self.objects.
        """
        if val is not None and val not in self.objects:
            raise ValueError("%s not in Parameter %s's list of possible objects" \
                             %(val,self._attrib_name))

# CBNOTE: I think it's not helpful to do a type check for the value of
# an ObjectSelector. If we did such type checking, any user
# of this Parameter would have to be sure to update the list of possible
# objects before setting the Parameter's value. As it is, only users who care about the
# correct list of objects being displayed need to update the list.
##     def __set__(self,obj,val):
##         self._check_value(val,obj)
##         super(ObjectSelector,self).__set__(obj,val)


    def get_range(self):
        """
        Return the possible objects to which this parameter could be set.

        (Returns the dictionary {object.name:object}.)
        """
        return dict([(obj.name,obj) for obj in self.objects])

    
class ClassSelector(Selector):
    """
    Parameter whose value is an instance of the specified class.    
    """
    __slots__ = ['class_','suffix_to_lose']

    def __init__(self,class_,default=None,instantiate=True,
                 suffix_to_lose='',**params):
        self.class_ = class_

        # CBENHANCEMENT: currently offers the possibility to cut off
        # the end of a class name (suffix_to_lose), but this could be
        # extended to any processing of the class name.
        self.suffix_to_lose = suffix_to_lose

        self._check_value(default)
        Parameter.__init__(self,default=default,instantiate=instantiate,**params)


    def _check_value(self,val,obj=None):
        """
        val must be None or an instance of self.class_
        """
        if not (isinstance(val,self.class_) or val is None):
            raise ValueError("Parameter '%s' must be an instance of %s"%(self._attrib_name,\
                                                                         self.class_.__name__))

    def __set__(self,obj,val):
        self._check_value(val,obj)
        super(ClassSelector,self).__set__(obj,val)

        
    def get_range(self):
        """
        Return the possible types for this parameter's value.

        (I.e. return {visible_name: <class>} for all classes that are
        concrete_descendents() of self.class_.)

        Only classes from modules that have been imported are added
        (see concrete_descendents()).
        """
        classes = concrete_descendents(self.class_)
        return dict([(self.__classname_repr(name),class_) for name,class_ in classes.items()])


    def __classname_repr(self, class_name):
        """
        Return class_name stripped of self.suffix_to_lose.
        """
        return re.sub(self.suffix_to_lose+'$','',class_name)




class List(Parameter):
    """
    Parameter whose value is a list of objects, usually of a specified type.

    The bounds allow a minimum and/or maximum length of
    list to be enforced.  If the class is non-None, all
    items in the list are checked to be of that type.
    """
    __slots__ = ['class_','bounds']

    def __init__(self,default=[],class_=None,instantiate=True,
                 bounds=(0,None),**params):
        self.class_ = class_
        self.bounds = bounds
        self._check_bounds(default)
        Parameter.__init__(self,default=default,instantiate=instantiate,
                           **params)

    # Could add range() method from ClassSelector, to allow
    # list to be populated in the GUI
    
    def __set__(self,obj,val):
        """Set to the given value, raising an exception if out of bounds."""
        self._check_bounds(val)
        super(List,self).__set__(obj,val)

    def _check_bounds(self,val):
        """
        Checks that the list is of the right length and has the right contents.
        Otherwise, an exception is raised.
        """
        if not (isinstance(val,list)):
            raise ValueError("List '%s' must be a list."%(self._attrib_name))

        if self.bounds!=None:
            min_length,max_length = self.bounds
            l=len(val)
            if min_length != None and max_length != None:
                if not (min_length <= l <= max_length):
                    raise ValueError("%s: list length must be between %s and %s (inclusive)"%(self._attrib_name,min_length,max_length))
            elif min_length != None:
                if not min_length <= l: 
                    raise ValueError("%s: list length must be at least %s."%(self._attrib_name,min_length))
            elif max_length != None:
                if not l <= max_length:
                    raise ValueError("%s: list length must be at most %s."%(self._attrib_name,max_length))

        if self.class_!=None:
            for v in val:
                assert isinstance(v,self.class_),repr(v)+" is not an instance of " + repr(self.class_) + "."


class Dict(ClassSelector):
    """
    Parameter whose value is a dictionary.
    """
    def __init__(self,**params):
        super(Dict,self).__init__(dict,**params)

class InstanceMethodWrapper(object):
    """
    Wrapper for pickling instance methods.

    Constructor takes an instance method (e.g. topo.sim.time) as
    its only argument.  Wrapper instance is callable, picklable, etc.
    """
    # CEBALERT: Both repr and name disguise that this is an
    # InstanceMethodWrapper (note that we probably won't need this
    # class in python 2.5).

    def __repr__(self):
        return repr(self.im.im_func)

    # Hope __name__ doesn't get set...
    def _fname(self):
        return self.im.im_func.func_name
    __name__ = property(_fname)
    
    def __init__(self,im):
        self.im = im

    def __getstate__(self):
        return (self.im.im_self,
                self.im.im_func.func_name)

    def __setstate__(self,state):
        obj,func_name = state
        self.im = getattr(obj,func_name)

    def __call__(self,*args,**kw):
        return self.im(*args,**kw)


