function G2 = dnetu(s, dJdx, net, tnet, status),
% DNETU  Calculate the Jacobian in respect to control
%
%  H = DNETU2(s, dJdx, net, tnet, status)
%
%  Calculates the Jacobian of the observation mapping in respect to
%  control signal based on the chain rule and dynamic programming.

% Copyright (C) 2004-2005 Matti Tornio
%
% This package comes with ABSOLUTELY NO WARRANTY; for details
% see License.txt in the program package.  This is free software,
% and you are welcome to redistribute it under certain conditions;

% Extract paramaters
[Ns T] = size(s);
Nx = size(net.w2, 1);
Nu = status.controlchannels;
Iu = Ns-Nu+1:Ns;

% Evaluate Jacobians for f and h
df = dnet(s, net);
dh = dtnet(s, tnet);

% Future values of control do not depend on past values of the sources.
% Approriate part of the Jacobians should be set to zero.
dh(Iu,:,:) = 0;

G{T} = dJdx(Nx*(T-1)+(1:Nx)) * df(:,:,T);
G2(:,T) = G{T}(:,Iu);
for i = T-1:-1:1,
  G{i} = dJdx(Nx*(i-1)+(1:Nx)) * df(:,:,i) + G{i+1} * dh(:,:,i);
  G2(:,i) = G{i}(:,Iu);
end


% *** Old quadratic algorithm to compute the Jacobian
% $$$ 
% $$$ % Calculate derivates on the diagonal
% $$$ for i=1:T,
% $$$   H((1 + (i - 1) * Nx):(i * Nx),i) = df(:,end,i);
% $$$ end
% $$$ 
% $$$ % Calculate the rest of the derivates using the chain rule and
% $$$ % dynamic programming
% $$$ for y=2:T,
% $$$   ds = df(:,:,y);
% $$$   for x=(y - 1):-1:1,
% $$$ 
% $$$     ds = ds * dh(:,:,x);
% $$$     H((1 + (y - 1) * Nx):(y * Nx),x) = ds(:,end);
% $$$   end
% $$$ end