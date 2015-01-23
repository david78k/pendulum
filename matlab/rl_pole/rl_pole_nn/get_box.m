% get_box: Given the current state, returns a number from 1 to 162
% designating the region of the state space encompassing the current state.
% Returns a value of -1 if a failure state is encountered.
function [box]=get_box(x,x_dot,theta,theta_dot)
one_degree=0.0174532; % 2pi/360 */
six_degrees=0.1047192;
twelve_degrees=0.2094384;
fifty_degrees=0.87266;
box=1;
if (x < -2.4 | x > 2.4 | theta < -twelve_degrees | theta > twelve_degrees)
    box=-1; %/* to signal failure */
else
    if (x < -0.8)
        box = 1;
    else if (x < 0.8)
            box = 2;
        else
            box = 3;
        end
    end
    if (x_dot < -0.5)
    else if (x_dot < 0.5)
            box =box+ 3;
        else
            box =box+ 6;
        end
    end
    if (theta < -six_degrees)
    else if (theta < -one_degree)
            box =box+ 9;
        else if (theta < 0)
                box =box+ 18;
            else if (theta < one_degree)
                    box=box+ 27;
                else if (theta < six_degrees)
                        box =box+ 36;
                    else
                        box =box+ 45;
                    end
                end
            end
        end
    end
    if (theta_dot < -fifty_degrees)
    else if (theta_dot < fifty_degrees)
            box =box+ 54;
        else
            box =box+ 108;
        end
    end
end