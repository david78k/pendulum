//package neuralnet;
/**
 * <p>Title: Lego Mindstorms Neural Networks</p>
 *
 * @author Julio César Sandria Reynoso
 * @version 1.0
 *
 * Created on 1 de abril de 2005, 06:09 PM
 */
import java.lang.Math;
/**
 * LMbpn: Lego Mindstorms Back Propagation Network
 */
class LMbpn {
    public static int data1[][] = {{0,0,0}, {1,1}};
    public static int data2[][] = {{1,0,0}, {1,0}};
    public static int data3[][] = {{0,0,1}, {0,1}};
    public static int data4[][] = {{0,1,0}, {0,0}};
    public static double input[] = {0,0,0,1};
    public static double w1[][] = {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}};
    public static double hidden[] = {0,0,1};
    public static double w2[][] = {{0,0}, {0,0}, {0,0}};
    public static double output[] = {0,0};
    public static double delta2[] = {0,0};
    public static double delta1[] = {0,0,0};
    public static int trainedEpochs = 0;

    public static void main(String[] args) {
	
    }

    LMbpn() {
        byte i, j;
        // Initialize weights randomly between 0.1 and 0.9
        for(i=0; i<w1.length; i++)
            for(j=0; j<w1[i].length; j++)
                w1[i][j] = Math.random()*0.8+0.1;
        for(i=0; i<w2.length; i++)
            for(j=0; j<w2[i].length; j++)
                w2[i][j] = Math.random()*0.8+0.1;
    }

    public static void train(int e) {
        for(int i=0; i<e; i++) {
            // Call method learn with training data
            learn( data1[0], data1[1] );
            learn( data2[0], data2[1] );
            learn( data3[0], data3[1] );
            learn( data4[0], data4[1] );
            trainedEpochs++;
        }
    }

    public static void learn( int inp[], int out[] ) {
        int i, j;
        double sum, out_j;
        // Initialize input units
        for(i=0; i<inp.length; i++)
            input[i] = inp[i];
        // Calculate hidden units
        for(j=0; j<hidden.length-1; j++) {
            sum = 0;
            for(i=0; i<input.length; i++)
                sum = sum + w1[i][j]*input[i];
            hidden[j] = 1 / ( 1 + Math.exp(-sum));
        }
        // Calculate output units
        for(j=0; j<output.length; j++) {
            sum = 0;
            for(i=0; i<hidden.length; i++)
                sum = sum + w2[i][j]*hidden[i];
            output[j] = 1 / (1 + Math.exp(-sum));
        }
        // Calculate delta2 errors
        for(j=0; j<output.length; j++) {
            if( out[j] == 0 )
                out_j = 0.1;
            else if( out[j] == 1 )
                out_j = 0.9;
            else
                out_j = out[j];
            delta2[j] = output[j]*(1-output[j])*(out_j-output[j]);
        }
        // Calculate delta1 errors
        for(j=0; j<hidden.length; j++) {
            sum = 0;
            for(i=0; i<output.length; i++)
                sum = sum + delta2[i]*w2[j][i];
            delta1[j] = hidden[j]*(1-hidden[j])*sum;
        }
        // Adjust weights w2
        for(i=0; i<hidden.length; i++)
            for(j=0; j<output.length; j++)
                w2[i][j] = w2[i][j] + 0.35*delta2[j]*hidden[i];
        // Adjust weights w1
        for(i=0; i<input.length; i++)
            for(j=0; j<hidden.length; j++)
                w1[i][j] = w1[i][j] + 0.35*delta1[j]*input[i];
    }

    public static void test(int inp[], int out[]) {
        int i, j;
        double sum;
        // Initialize input units
        for(i=0; i<inp.length; i++)
            input[i] = inp[i];
        // Calculate hidden units
        for(j=0; j<hidden.length-1; j++) {
            sum = 0;
            for(i=0; i<input.length; i++)
                sum = sum + w1[i][j]*input[i];
            hidden[j] = 1 / ( 1 + Math.exp(-sum));
        }
        // Calculate output units
        for(j=0; j<output.length; j++) {
            sum = 0;
            for(i=0; i<hidden.length; i++)
                sum = sum + w2[i][j]*hidden[i];
            output[j] = 1 / (1 + Math.exp(-sum));
        }
        // Assign output to param out[]
        for(i=0; i<output.length; i++)
            if( output[i] >= 0.5 )
                out[i] = 1;
            else
                out[i] = 0;
    }
}

