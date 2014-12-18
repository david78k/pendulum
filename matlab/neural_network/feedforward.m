% [x,t] = simplefit_dataset;
x = Fout.Data
t = thetain.Data

% 0    0.0498    0.0996    0.1550    0.2103
% 5.0472    5.3578    5.6632    5.9955    6.3195
x(1:5)
t(1:5)
net = feedforwardnet(10);
% net = newlind(x,t);
net = train(net,x,t)
% y = sim(net,x)
gensim(net)
% gensim(net,0.5)

% p = [1 2 3 4 5];
% t = [1 3 5 7 9];
% 
% net = newlind(p,t)
% y = sim(net,p)
% gensim(net,-1)
