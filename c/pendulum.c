#include <stdio.h>
#include <math.h>

void main () {
	printf("pendulum PI = %f\n", M_PI);

	// alpha > 1, beta > 0
	// beta^2 > 4(alpha - 1)
	double theta, alpha, beta, phi, t, ut;
	// first derivative, second derivative
	double phi_1, phi_2;
		
	// initialize variables
	t = 0;
	alpha = 1.1;
	beta = 1;
	
	// phi_2 + sin = u

	while (t < 10) {
		//phi_2 + beta*phi_1 + (alpha - 1)*phi = 0;
		printf("%1.0f %f\n", t, theta);
		t += 1;
	}
}


