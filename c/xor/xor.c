#include <stdio.h>
#include <math.h>

#define randomdef                  ((float) random() / (float)((1 << 31) - 1))

float LR_IH = 0.7;
float LR_HO = 0.07;

double h[4], wih[4][4], who[4];
float state[4];

int count_error = 0;
double sum_error = 0.0;

float input_states[4][4] = {{0.0, 0.0, 0.0, 0.0}, 
			{0.0, 1.0, 1.0, 0.0}, 
			{1.0, 0.0, 0.0, 1.0}, 
			{1.0, 1.0, 1.0, 1.0} 
};
float targets[4] = {0.0, 1.0, 1.0, 0.0};

// forward prop
double forward() {
  int i, j;
  double push = 0.0, sum = 0.0;

  for(i = 0; i < 4; i ++) {
    sum = 0.0;
    for(j = 0; j < 4; j ++)
      sum += wih[j][i]*state[j];
    h[i] = 1.0 / (1.0 + exp(-sum));
  }
  sum = 0.0;
  for(i = 0; i < 4; i ++) {
    sum += who[i]*h[i];
  }
  push = 1.0 / (1.0 + exp(-sum));
  return push;
}

// backward prop
backprop(double push, double target_push) {
  int i, j;
  double sum = 0.0;
  double error = (push - target_push);
  double gradient = 0.0;

  sum_error += error * error;
  count_error ++;

  for(i = 0; i< 4; i ++) {
    gradient = error * h[i];
    who[i] += LR_HO * gradient;
  }
  for(i = 0; i< 4; i ++) {
    double x = 1 - h[i]*h[i];
    gradient = x * who[i] * error;
    for (j = 0; j < 4; j ++)
      wih[i][j] += LR_IH * gradient * state[j];
  }
}

init() {

}

train() {
	int i, j;
	for(i = 0; i < 4; i ++) {
		//printf("%d\n", i);
		for (j = 0; j < 4; j ++) {
			//printf("%.1f ", input_states[i][j]);
			state[j] = input_states[i][j];
			printf("%.1f ", state[j]);
		}
		printf("\n");
	}
}

main() {
	printf("%f\n", randomdef);
	train();	
	printf("train complete\n");
}

