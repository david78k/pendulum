function net = rtrl(net,cycles,sample,alpha)
% net = rtrl(net,cycles,sample,alpha)
%
%   net:  produced by rtrlinit
%   cycles: number of cycles per example.  The partial derivative is
%    accumulated for this many cycles between each weight update.
%   sample:  vector of inputs and target
%   alpha: learning rate

% by Chuck Anderson (2001) www.cs.colostate.edu/~anderson

net.alpha = alpha;
ni = net.ni;
no = net.no;
nu = net.nu;
nic = ni+1;

for cycle = 1:cycles
  net.u = [net.y; sample(1:ni)'; 1];
  net.y = activation(net, net.w * net.u );
  dyr = repmat(actderiv(net,net.y),1,nu);
  for i = 1:nic+nu
    net.p(:,:,i) = dyr .* (net.w(:,1:nu) * net.p(:,:,i) + ...
	diag(ones(1,nu)*net.u(i)));
  end
  net.output(cycle,:) = net.y';
end					% cycle loop
  
net.dw(:,:) = 0;
for j = 1:no
  net.error = sample(j+ni) - net.y(j);
  net.dw = net.dw + net.error * shiftdim(net.p(j,:,:),1);
end
net.w = net.w + alpha * net.dw;

%----------------------------------------------------------------------
function y = activation(net,s)
if net.sig1tanh2 == 1
  y = 1 ./ (1 + exp(-s));
else
  y = tanh(s);
end



%----------------------------------------------------------------------
function dy = actderiv(net,y)
if net.sig1tanh2 == 1
  dy = y .* (1 - y);
else
  dy = 1 - y.^2;
end

