##Evolving an Inverted Pendulum Controller with a Genetic Algorithm

See [here](http://yukunlin.github.io/nagini/) for more details.

###Running the demo
A working set of weights and biases are provided in `weights_working.txt` and
`biases_working.txt`. This is used by the provided Mathematica demo 
`inverted pendulum neural network.nb` (evaluate all cells after opening it).

###Running the genetic algorithm
Python 2.7 and [numpy](http://www.numpy.org/) is required. Run
```
python trainer.py
```
to start the training. Computation is automatically parallelized across all
available cores. The training parameters are set at the top of `trainer.py`.
