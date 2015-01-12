#include "backprop.h"

main() {
	//train();
        printf("%f\n", randomdef);
        init();
        printWeights();

        //train();
        //printf("train complete\n");
        //printWeights();
	double push = forward(state);
	backprop(push, target_push);
	//backprop(error);

        printf("test\n");
	testAll();
}
