PLASTK - The Python Learning Agent Software Toolkit

PLASTK is a Python class library for building and experimenting with
learning agents, i.e. software programs that interact in a closed loop
with an environment (agents) and that learn. Especially, but not
limited to, reinforcement learning agents.

The PLASTK software code and documentation are copyright 2004-2008
Jefferson Provost, Matt MacMahon, and Joe Reisinger.  PLASTK's
ParameterizedObject system code and documentation are copyright James
A. Bednar, Christopher Ball, Julien Ciroux, and Jefferson Provost

PLASTK has an extensible component architecture for defining the
components that make up a software agent system, including agents, and
environments, and various components needed to build agents, such as
function approximators and vector quantizers.

INSTALLATION

PLASTK is currently all pure python.  Just place the plastk/ directory
somewhere in your PYTHONPATH to get started.

PLASTK requries the numpy package for numerical computations, and the
PLASTK gui currently requires Pmw and BLT.  These packages are
available from external package repositories such as Fink or
DarwinPorts/MacPorts on MacOS X, and Debian and Ubuntu on Linux.  (We
are currently working on removing dependency on


DOCUMENTATION and GETTING STARTED

PLASTK is currently under development, as is its documentation.  It
will get better over time.  The best way to get started is with the
GridWorld and Pendulum examples.  These examples can be run as
scripts, e.g.:

[jp@tele-2 src]$ python plastk/examples/gridworld.py

Alternatively, then can be loaded as modules and run from within python, e.g.:

[jp@tele-2 src]$ python
Python 2.5.1 (r251:54863, Feb 16 2008, 13:25:03) 
[GCC 4.0.1 (Apple Computer, Inc. build 5367)] on darwin
Type "help", "copyright", "credits" or "license" for more information.
>>> from plastk.examples import gridworld
>>> gridworld.main()

Both of these methods will open the gridworld example in the GUI in
its default configuration.

To understand PLASTK, begin with the example scripts and work from
there.  Use the source, Luke!

$Id: README.txt 230 2008-03-03 04:53:52Z jprovost $
