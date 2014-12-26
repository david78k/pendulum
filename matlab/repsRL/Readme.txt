Installation
============
1. Unzip the file.
2. Add the directory "MDP Simulators" and all of its subdirectories to MATLAB's search path.
3. Run repsRL.m in MATLAB. The script solves the montain car problem 10 times.

Usage
=====
To change the solved problem, uncomment one of the following lines in repsRL.m:

   domain = 'acrobot2';
   domain = 'mcar';
   domain = 'pendulum';
   domain = 'cifar';

and comment out the rest. The following parameters can be adjusted if needed:

   discount         The discount factor of the MDP
   absorbSteps      The maximum length of training episodes
   maxNumSteps      The maximum length of testing episodes
   maxNumEpisodes   The number of training episodes
   numExps          The number of testing episodes
   maxNumStates     The maximum number of representative states
   numTrials        The number of experimental trials
