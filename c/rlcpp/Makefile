CFLAGS=-Wall -O4

UTILFILES = StateActionUtils.cpp cNeuralNetwork.cpp Algorithm.cpp StateActionAlgorithm.cpp World.h Experiment.cpp
ALGORITHMS = Sarsa.cpp Qlearning.cpp QVlearning.cpp Acla.cpp Cacla.cpp
CPWORLD = CartPole.cpp
CPINTERFACE = CartPoleInterface.cpp
SMALLWORLD = SmallMaze.cpp
SMALLINTERFACE = SmallInterface.cpp

all: cp small

#				cp
cp:
		c++ $(CFLAGS) $(UTILFILES) $(ALGORITHMS) $(CPWORLD) $(CPINTERFACE) -o CartPole
		@echo
		@echo Done compiling CartPole.
		@echo Run with:    ./CartPole \'cfg\'
		@echo where \'cfg\' is a configuration file.
		@echo

#				small
small:
		c++ $(CFLAGS) $(UTILFILES) $(ALGORITHMS) $(SMALLWORLD) $(SMALLINTERFACE) -o SmallMaze
		@echo
		@echo Done compiling SmallMaze.
		@echo Run with:    ./SmallMaze \'cfg\'
		@echo where \'cfg\' is a configuration file.
		@echo

clean:
	rm -f *.out
	@echo done removing

