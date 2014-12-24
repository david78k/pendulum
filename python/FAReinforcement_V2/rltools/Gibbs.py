import numpy as np
from pylab import *

action =np.linspace(-1,1,11)


q = np.arange(11)**3+0.0
q[3]=1500
t=100.2

def mexp(x):
    return 1.01**x

def Gibbs(q,t):
    a = mexp(q/t)
    return a / np.sum(a)

x = Gibbs(q,t)

#plot(action,x)
print "temperature",t
print "actions",action
print "qvalues",q
print "probabilities"
print x
print "sum p(x)",np.sum(x)


print "final action",np.dot(action,x)

def dsigmoid(x,center=0.0,s=20):    
    return sign(x-center)*(1.0001-mexp(-((x-center)/s)**4))


x = linspace(-200,200,200)
y = dsigmoid(x)
plot(x,y)

