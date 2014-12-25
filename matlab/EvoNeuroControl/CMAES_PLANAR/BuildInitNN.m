function [ net ] = BuildInitNN()

%range_xg         = [0.0 25];
%range_yg         = [0.0 25];

%net = newff([ range_xg ; range_yg],[5 3],{'tansig' 'tansig'});
%net = init(net);

ni = 2+1; % +1 for bias node
nh = 5;
no = 3;

N = ni*nh + nh*no;
net = 4.0*rand(N,1)-2.0;
