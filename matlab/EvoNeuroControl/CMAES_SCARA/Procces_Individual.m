function [ fitness ] = Procces_Individual( individual )
%PROCCES_INDIVIDUAL Summary of this function goes here
%   Detailed explanation goes here



global  grafica  
global xgArray ygArray zgArray


mse       = 0.0;
max_setps = 70;
fitness   = 0.0;


penalty = 0.0;

for i=1:max_setps
   
    %[xg yg] = randgoal();
    %T1 = randint(1,1,[-10 90]);
    %T2 = randint(1,1,[-90 90]);
    %T3 = randint(1,1,[-90 90]);
    
    xg = xgArray(i);
    yg = ygArray(i);
    zg = zgArray(i);
    %T1 = T1Array(i)*0;
    %T2 = T2Array(i)*0;
    %T3 = T3Array(i)*0;    
    %forkin;
    
   
    input  = [ xg/30.0  yg/30.0 ];
    output = NNUpdate( input , individual );
    
    t1 = pi*output(1);
    t2 = pi*output(2);
    t3 = 0;
    %d4 = 2* output(3)+2 ; % from 2 to 4
    d4  = 1; 
    [X robot_state] = DoAction([t1 t2 t3 d4]);
    xt = X(1);
    yt = X(2);
    zt = X(3);
    
    % penalty function for bad configurations
   
    if (t2<0)
        penalty  =  10 + abs(rad2deg(t2));       
    end
        
    if grafica
        scaraplot(robot_state(1),robot_state(2),robot_state(3),robot_state(4),[xg yg zg]);              
        drawnow;
    end
     
    fitness = fitness + (xg-xt)^2 + (yg-yt)^2  + penalty;
    %fitness = fitness + (xg-xt)^2 + (yg-yt)^2 + (zg-zt)^2 + penalty;
    %mse     = mse     + (xg-xt)^2 + (yg-yt)^2 + (zg-zt)^2;
end

%fitness = 1 + ( max_setps*10^5/ (1 + fitness)); 

