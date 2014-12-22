This readme and the code were contributed by Timothee Masquelier
timothee.masquelier@alum.mit.edu
Sept 2011

This code was used in:
Masquelier T (2012) Relative spike time coding and STDP-based orientation
selectivity in the early visual system in natural continuous and saccadic
vision: a computational model. J Comput Neurosc (in press).

Feel free to use/modify but please cite us if appropriate.

The code only contains the LGN and V1 stages.

The retina simulator, developed by Adrien Wohrer, can be downloaded here:
http://www-sop.inria.fr/neuromathcomp/public/software/virtualretina/
Note that it only compiles on Linux.
Then it should be launched using for eg:
Retina video/*.png -ret cat.retina.xml -r 10 -outD . -nodisp
(the cat.retina.xml file we provide contains the retinal parameters used in our baseline simulation)
This produces a text file spikes.spk that contains the RGC spikes, and that should be placed in ../mat/
This file is then read by my code.
Please read the Virtual Retina documentation for more information.

All my code is in the ./src/ directory
It's been tested on Linux and Windows (I guess it should also work on Mac OS).
It contains Matlab scripts and functions, and 2 mex files (in C), that should be compiled first (from within Matlab):
>> mex STDPContinuous.c
>> mex pspKernel.c

Then the main script can be launched:
>> main

The script reads the ./mat/spikes.spk file produced by Virtual Retina.
It then formats it (cropping), and then launches LGN, V1 and plots the final RFs (Fig 6 in the paper)
See comments at the beginning of each script.

20130515 T Masquelier supplied an update in response to a modeler's
request for help where a problem where STDPContinuous.c was not
compiling on a particular platform was fixed by renaming min and max
to local_min and local_max within that program.
