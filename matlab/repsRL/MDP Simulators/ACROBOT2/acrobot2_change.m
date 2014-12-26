function terminal = acrobot2_change(torque, dt)

global theta1;
global theta2;
global theta1_p;
global theta2_p;

% initialization of variables
m1 = 1;
m2 = 1;

l1 = 1;
l2 = 1;

lc1 = 0.5;
lc2 = 0.5;

I1 = 1;
I2 = 1;
g = 9.8;

x0 = 0;
y0 = 0;

% % add a small amount of Gaussian noise to torque
% torque = torque + 1e-3 * randn();

phi2 = m2*lc2*g*cos(theta1 + theta2-(pi/2));
phi1 = (-m2*l1*lc2*(theta2_p^2)*sin(theta2))-(2*m2*l1*lc2*theta2_p*theta1_p*sin(theta2))+(((m1*lc1)+(m2*l1))*g* cos(theta1-(pi/2)))+phi2;

d2 = m2*((lc2^2)+(l1*lc2*cos(theta2)))+I2;
d1 = m1*(lc1^2) + m2*((l1^2) + (lc2^2) + (2*l1*lc2*cos(theta2))) + I1 + I2;

theta2_dp = (1/ ( (m2*(lc2^2))+I2-((d2^2)/d1))) * (torque + ((d2/d1)*phi1) - m2*l1*lc2*(theta1_p^2)*sin(theta2) -phi2);
theta1_dp = (-1/d1)*((d2*theta2_dp)+phi1);

% Update and Bound the Theta Primes
theta1_p = theta1_p + theta1_dp * dt;
if (theta1_p > (4*pi))
   theta1_p = (4*pi);
elseif (theta1_p < (-4*pi))
   theta1_p = (-4*pi);
end
theta2_p = theta2_p + theta2_dp * dt;
if (theta2_p > (9*pi))
   theta2_p = (9*pi);
elseif (theta2_p < (-9*pi))
   theta2_p = (-9*pi);
end

% Update and Bound the Thetas
theta1 = theta1 + theta1_p * dt;
if (theta1 > (2*pi))
   while (theta1 > (2*pi))
      theta1 = theta1 - (2*pi);
   end
elseif (theta1 < 0)
   while (theta1 < 0)
      theta1 = theta1 + (2*pi);
   end
end
theta2 = theta2 + theta2_p * dt;
if (theta2 > (2*pi))
   while (theta2 > (2*pi))
      theta2 = theta2 - (2*pi);
   end
elseif (theta2_p < 0)
   while (theta2 < 0)
      theta2 = theta2 + (2*pi);
   end
end

% Find Position of the Tip
x1=x0+l1*sin(theta1);
y1=y0-l1*cos(theta1);

x2=x1+l2*sin(theta1+theta2);
y2=y1-l2*cos(theta1+theta2);

% y2
if (y2>l2)
   terminal = 1; 
else
   terminal = 0;
end
