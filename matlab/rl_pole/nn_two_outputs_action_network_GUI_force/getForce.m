function [f] = getForce (push, t, tau)

fmax = 600;
% fmax = 1;

f = push*fmax*t*exp(-t/tau);
% if push == 1
%     f = fmax*t*exp(-t/tau);
% %     f = fmax*t*exp(-t);
% elseif push == -1
%     f = fmax*-t*exp(-t/tau);
% %     f = fmax*-t*exp(-t);
% else
%     f = 0;
% end