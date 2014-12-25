clf



n = -5:0.1:5; a = 2*deg2rad(90)./(1.+exp(-1 .*n))-deg2rad(90); plot(n,a,'g')
hold on
n = -5:0.1:5; a = deg2rad(90).*tansig(n); plot(n,a,'b')
hold off
