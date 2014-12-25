function net = rtrlinit(ni,no,nu,activationfunc,initwrange)
%  net = rtrlinit(ni,no,nu,activationfunc,initwrange)

% by Chuck Anderson (2001) www.cs.colostate.edu/~anderson

%add one to ni to account for constant input
net.ni = ni;
net.no = no;
net.nu = nu;
net.iwrange = initwrange;
nic = ni + 1;

if strcmp(activationfunc,'sigmoid')
  net.sig1tanh2 = 1;
elseif strcmp(activationfunc,'tanh')
  net.sig1tanh2 = 2;
else
  error('Activation function must be ''sigmoid'' or ''tanh''');
end


net.p = zeros(nu,nu,nu+nic);
net.y = zeros(nu,1);
net.w = (rand(nu,nu+nic)-0.5)*2*initwrange;

net.eyenu = eye(nu);
net.dw = zeros(size(net.w));
net.y = zeros(nu,1);
