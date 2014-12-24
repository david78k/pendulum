clear all;
ALPHA = 0.5;            % learning rate parameter 
BETA = 0.0;             % magnitude of noise added to choice 
GAMMA = 0.85;          % discount factor for future reinf 
m=1.1;  %mass of cart + pole 
mp=0.1; %mass of the pole
g=9.8;  %重力加速度
length=0.5;  %half length of pole
Force=10;   %force =10N
T=0.02;  % Update time interval
% Define global variable
global NUM_BOX q_val pre_state cur_state pre_action cur_action  x v_x theta v_theta
NUM_BOX=162;    % Number of states sets to 162
load q_val   % Q value has been trained
reset_cart;

h=figure;
axis([-3 3 0 1.5]);
set(gca, 'DataAspectRatio',[1 1 1]);
set(h,'DoubleBuffer','on')
set(gca, 'PlotBoxAspectRatio',[1 1 1]);

success=0;   % succesee 0 times
reinf=0;

while success<1000000
    get_action(x,v_x,theta,v_theta,reinf);
    if (cur_action==1)   % push left
        F=-1*Force;
    else  F=Force;    % push right
    end %if
    a_theta=(m*g*sin(theta)-cos(theta)*(F+mp*length*v_theta^2*sin(theta)))/(4/3*m*length-mp*length*cos(theta)^2);
    a_x=(F+mp*length*(v_theta^2*sin(theta)-a_theta*cos(theta)))/m;
    v_theta=v_theta+a_theta*T;
    theta=theta+v_theta*T;
    v_x=v_x+a_x*T;
    x=x+v_x*T;
    X=[x   x+cos(pi/2-theta)];
    Y=[0.2  0.2+sin(pi/2-theta)];
    obj=rectangle('Position',[x-0.4,0.1,0.8,0.1],...
          'facecolor','b');
    obj2=line(X,Y);  
    pause(T/100)
    delete(obj)
    delete(obj2)
end %for

