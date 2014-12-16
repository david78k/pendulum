[x,t] = simplefit_dataset;
x(1:5)
t(1:5)
net = feedforwardnet(10);
net = train(net,x,t)
% y = sim(net,x)
gensim(net)
% gensim(net,-1)

% p = [1 2 3 4 5];
% t = [1 3 5 7 9];
% 
% net = newlind(p,t)
% y = sim(net,p)
% gensim(net,-1)
