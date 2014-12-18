p = [1 2 3 4 5];
t = [1 3 5 7 9];

net = newlind(p,t)
y = sim(net,p)
gensim(net,-1)
