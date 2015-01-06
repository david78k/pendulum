/****************************************************************************
*****************************************************************************
Example results.

First argument is maximum number of trials (failures).
Second argument is number of trails to average the balancing time (steps) over.
Next four are  beta, beta_h, rho, rho_h.

on Sun IPC:
>cc -O -o mpole multilayer-pole.c -lm
>mpole 10000 1000 .2 .05 1 .2
 B=0.2 Bh=0.05 R=1 Rh=0.2 nt=10000 bin=1000
  1000     13
  2000     17
  3000     15
  4000     28
Final trial 4638 balanced for 180000 steps (1.000000 hours).

*****************************************************************************/

