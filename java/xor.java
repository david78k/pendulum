// Back-Propagation Neural Network
// 
// Written in Python.  See http://www.python.org/
// Placed in the public domain.
// Neil Schemenauer <nas@arctrix.com>


// Translated to Java by: 
// Jose Antonio Martin H. <jamartin@dia.fi.upm.es>


import java.util.Random;
import java.io.*;
//import string
import java.util.Arrays;

//random.seed(0)
public class xor implements Serializable
{ 
  static Random generator = new Random();
  
  // Vars

  int ni;
  int nh;
  int no;
  double N,M;
  double[] ai;
  double[] ah;
  double[] ao;
  public double[][] wi;
  public double[][] wo;
  double[][] ci;
  double[][] co;
  
  
  
  
  
  public static class pattern
  {
   double[] input;
   double[] target;
   
   pattern(double[] in,double[] out)
   {
     input=in;
     target=out;
   }
   
   public String toString() {
     return Arrays.toString(input) + " " + Arrays.toString(target);
   }
  }
  
 
// calculate a random number where:  a <= rand < b
public static double rand(double a, double b)
{   double retval;
    
    retval=(b-a)*generator.nextDouble() + a;
    return retval;
    
}

// Make a matrix 
public static double[][] makeMatrix(int I, int J)
{   double [][] m = new double[I][J];  
    return m;
}

public static double tanh(double x)
{ 

  return (Math.exp(x)- Math.exp(-x)) / (Math.exp(x)+Math.exp(-x));
  

}
// our sigmoid function, tanh is a little nicer than the standard 1/(1+e^-x)
public static double sigmoid(double x)
 {
 	return tanh(x);
  }

// derivative of our sigmoid function
public static double dsigmoid(double y)
 {
 	return 1.0-y*y;
  }
  

xor(int ini,int inh, int ino)
    {   int i,j;
    
        //# number of input, hidden, and output nodes
        ni = ini + 1; // +1 for bias node
        nh = inh;
        no = ino;
        ai= new double[ni];
        ah= new double[nh];
        ao= new double[no];
        
        
        N=0.5;// N: learning rate 0.5 good;
        M=0.1;// M: momentum factor 0.1 good
        
        
        //# init activations for nodes
        for(i=0;i<ni;i++){ai[i] = 1.0;}
        for(i=0;i<nh;i++){ah[i] = 1.0;}
        for(i=0;i<no;i++){ao[i] = 1.0;}
       
       
        
        // create weights
        wi = new double[ni][nh];  //makeMatrix(self.ni, self.nh)
        wo = new double[nh][no];  //makeMatrix(self.nh, self.no)
        
        // set them to random vaules
        for (i=0;i<ni;i++)
          {  for (j=0;j<nh;j++)wi[i][j] = rand(-1.5, 1.5);
          }      
        for (i=0;i<nh;i++)
          {  for (j=0;j<no;j++)wo[i][j] = rand(-1.5, 1.5);
		  }
		   
        // last change in weights for momentum   
        ci = new double[ni][nh];   //makeMatrix(self.ni, self.nh)
        co = new double[nh][no];   //makeMatrix(self.nh, self.no)
    }
    
    public  double[] update(double[] inputs)
    { int i,j;
      double sum;
        //if len(inputs) != self.ni-1:
        //    raise ValueError, 'wrong number of inputs'

        // input activations
        for (i=0;i<ni-1;i++){ai[i] = inputs[i];}
        
          
        //hidden activations
        for (j=0;j<nh;j++)
        {
            sum = 0.0;
            for (i=0;i<ni;i++){ sum = sum + (ai[i] * wi[i][j]) ;}
            ah[j] = sigmoid(sum);
		}
		
        //output activations        
        for (j=0;j<no;j++)
        {        
            sum = 0.0;
            for(i=0;i<nh;i++){ sum = sum + (ah[i] * wo[i][j]) ;}
            ao[j] = sigmoid(sum);
        }
        
        return ao;
    }

    public  double backPropagate(double[] targets)
    {   int i,j,k;
    	double error,change;
        //if len(targets) != self.no:
        //    raise ValueError, 'wrong number of target values'
 
        // calculate error terms for output
        double[] output_deltas = new double[no];//[0.0] * self.no
        
        for(k=0;k<no;k++)
        {
        
            error = targets[k]-ao[k];
            output_deltas[k] = dsigmoid(ao[k]) * error;
		}
		
        // calculate error terms for hidden
        double [] hidden_deltas = new double[nh];  // [0.0] * self.nh
        for(j=0;j<nh;j++)
        {
        
            error = 0.0;
            for(k=0;k<no;k++){ error = error + output_deltas[k]*wo[j][k]; }
            hidden_deltas[j] = dsigmoid(ah[j]) * error;
         }
        //update output weights
        for(j=0;j<nh;j++)
        {       
            for(k=0;k<no;k++)
            {            
                change = output_deltas[k]*ah[j];
                wo[j][k] = wo[j][k] + N * change + M * co[j][k];
                co[j][k] = change;
                // System.out.println(print N*change, M*self.co[j][k]
              
             }
         }
        // update input weights
        for(i=0;i<ni;i++)
        {        
            for(j=0;j<nh;j++)
            {            
                change = hidden_deltas[j]*ai[i];
                wi[i][j] = wi[i][j] + N*change + M*ci[i][j];
                ci[i][j] = change;
            }
        } 
         
        // calculate error
        error = 0.0;
        for(k=0;k<no;k++) { error = error + 0.5 * (targets[k]-ao[k])*(targets[k]-ao[k]);}// please correct this (targets[k]-ao[k])*(targets[k]-ao[k]) must be x^2
        
        
        return error;
     }

    public  void test(pattern[] patterns)
    { double[] res;
        for (int p=0;p<patterns.length;p++)
        { res=update(patterns[p].input);
          for(int j=0;j<res.length;j++)  System.out.println("[Input "+p+"] ->  " + patterns[p].toString() + " " + res[j]);
        }
	}
    
    public  void weights()
    {   int i,j;
        System.out.println("Input weights:");
        for(i=0;i<ni;i++)
        {
            System.out.println(wi[i]);
         }
        System.out.println(" ");
        System.out.println("Output weights:");
        for(i=0;i<nh;i++)
        {
            System.out.println(wo[i]);
         }
        
      }
	
	public double[] online(pattern data)
	{ double[] retval= update(data.input);
	  backPropagate(data.target); 
	  return retval;
	}
	
	public double[] GetResponse(double[] input)
	{ return update(input);}
	
    public  void train(pattern[] patterns,int iterations, double Lr,double  Momenta)
    {   
        N=Lr;
        M=Momenta;
        // N: learning rate 0.5 good
        // M: momentum factor 0.1 good
        int i,p;
        double error=0.0;
		        
        for(i=0;i<iterations;i++)
        {
            error = 0.0;
            for(p=0;p<patterns.length;p++)
            {
                                
                update(patterns[p].input);                
                error += backPropagate(patterns[p].target);
            }
            if (i%10== 0) System.out.println("error = "+ error);
            
		 }

    }
    
    
    public  void singletrain(pattern pat,int iterations)
   {   
        double error;		        
        for(int i=0;i<iterations;i++)
        {
            error = 0.0;        
            update(pat.input);                
            error = error + backPropagate(pat.target);           
            //if (i%10 == 0) System.out.println("error = "+ error);            
		 }

    }
    

    public static void main(String args[])
	{   
		//# Teach network XOR function
		xor.pattern[] pat= new xor.pattern[4];
		xor net = new xor(2, 3, 1);
		 
	    pat[0] = new xor.pattern(new double[]{0,0},new double[]{0});
	    pat[1] = new xor.pattern(new double[]{1,0},new double[]{1});
	    pat[2] = new xor.pattern(new double[]{0,1},new double[]{1});
	    pat[3] = new xor.pattern(new double[]{1,1},new double[]{0});
	    
	    pat[0] = new xor.pattern(new double[]{0,0,0,0},new double[]{0});
	    pat[1] = new xor.pattern(new double[]{1,0,0,1},new double[]{1});
	    pat[2] = new xor.pattern(new double[]{0,1,1,0},new double[]{1});
	    pat[3] = new xor.pattern(new double[]{1,1,1,1},new double[]{0});
	    
	    
	    
	    //# train it with some patterns
	    net.train(pat,1000,0.5,0.1);
	    //# test it
	    net.test(pat);
	
	}



}
