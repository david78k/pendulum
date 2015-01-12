#include "backprop.h"

double state[NPATTERNS][4] = {
        {0, 0, 0, 0},
        {1, 0, 0, 1},
        {0, 1, 1, 0},
        {0.5, 0.5, 1, 1},
        {0, 0.5, 0.5, 0},
        {1, 1, 1, 1}
};

double target_push[] = {0, 1, 1, 1, 0, 0};

main() {
	//train();
        printf("%f\n", randomdef);
        init();
        printWeights();

        //train();
	int i, epoch;
	double push;
	for(epoch = 0; epoch <= 1500; epoch ++) {
		for(i = 0; i < NPATTERNS; i ++) {
			push = forward(state[i]);
			//backprop();
			backprop(push, target_push[i]);
			//double push = forward(state);
			//backprop(push, target_push);
			//backprop(error);
		}

		 calcOverallError();
        	if(epoch % 100 == 0) {
                	printf("epoch = %d RMSE = %.4f\n", epoch, RMSerror);
                	testAll();
        	}
	}

        printf("train complete\n");
        printWeights();

        //printf("test\n");
	//testAll();

	//train();

        //printf("test\n");
	//testAll();
}
