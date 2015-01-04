"""
The Python Learning Agent Software Toolkit

PLASTK is a class library of tools for building and studying learning
agents, including classes implementing reinforcement learning (RL)
algorithms and environments, hierarchical RL (Options), function
approximators, perceptual feature construction and learning (e.g. tile
coding and self-organzing feature maps.  An overview of the included
modules is below.  See individual module documentation (via pydoc or
help()) for details.

PLASTK depends on several other python modules and packages for
support: including numpy, Tkinter, Python MegaWidgets (Pmw), and BLT
(for some graphing).

* SUBPACKAGES *

plastk.base.* -- Contains the base class definitions for important types
used throughout plastk, as well as some other basic functionality.

plastk.base.rl -- Generic RL agent, and environment classes, and a
                  simple RL Interface (RLI) class.
plastk.base.vq -- Generic vector quantizer class.
plastk.base.fnapprox -- Generic function approximator class.
plastk.base.rand -- random number routines.


The other subpackages contain class libraries of implementations of
different agents, environments, function approximators, vector
quantizers, etc.

plastk.rl.* -- Reinforcemnt learning agents and environments. 
plastk.vq.* -- Vector quantizers.
plastk.fnapprox.* -- Function approximators.
plastk.misc.* -- Miscellaneousl utilities, etc.

$Id: __init__.py 229 2008-03-03 04:38:00Z jprovost $

"""



def test(verbosity=1):
    import tests
    return tests.all(verbosity=verbosity)
    
