function reset_cart  % reset the cart pole to initial state
global pre_state cur_state pre_action cur_action x v_x theta v_theta
pre_state=1;
cur_state=1;
pre_action=-1;  % -1 means no action been taken
cur_action=-1;
x=rand/100;     % the location of cart
v_x=rand/100;   % the velocity of cart
theta=rand/100;   %the angle of pole
v_theta=rand/100;    %the velocity of pole angle
