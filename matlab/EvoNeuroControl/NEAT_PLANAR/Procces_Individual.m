function [ fitness mse ] = Procces_Individual( individual )
%PROCCES_INDIVIDUAL Summary of this function goes here
%   Detailed explanation goes here



global T1 T2 T3 xt yt TxtSteps grafica  goal
global T1Array T2Array T3Array xgArray ygArray


mse = 0.0;
max_setps = 70;

fitness = 0.0;
for i=1:max_setps
   
    %[xg yg] = randgoal();
    %T1 = randint(1,1,[-10 90]);
    %T2 = randint(1,1,[-90 90]);
    %T3 = randint(1,1,[-90 90]);
    
    xg = xgArray(i);
    yg = ygArray(i);
    %T1 = T1Array(i)*0;
    %T2 = T2Array(i)*0;
    %T3 = T3Array(i)*0;    
    %forkin;
    
    
    input  = [xg/20.0 yg/20.0];
    output = NN_Neat( input , individual );
    T1 = rad2deg(output(1));
    T2 = rad2deg(output(2));
    T3 = rad2deg(output(3));
    forkin;
    
    % penalty function for bad configurations
    penalty = 0.0;
    
    if ( xg>0 && sign(T2)>0)
       penalty =  10*abs(T2);
    end
    if ( xg>0 && sign(T3)>0)
       penalty =  10*abs(T3);
    end
    
    if grafica
        set(goal,'xdata',xg,'ydata',yg);
        set(TxtSteps,'string',strcat('Steps: ',int2str(i)));
        setplot;    
        drawnow;
    end
    error =  (xg-xt)^2 + (yg-yt)^2;
    fitness = fitness + error + penalty;
    mse     = mse     + error + penalty;
end

fitness = 1.0/(1 + fitness); 

