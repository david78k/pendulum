function [f] = getForce (push, t)

if push == 1
    f = t*exp(-t);
elseif push == -1
    f = -t*exp(-t);
else
    f = 0;
end