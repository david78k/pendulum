% An example of using rtrl.m.  Learn 2-bit exclusive-or.
%       Copyright (c) 1996 by Charles W. Anderson

format compact
P = 1; N = 4; I = 3;
W = (rand(N,P*(N+I))-.5)*1.0;
Wmask = ones(N,P*(N+I));
for k=1:N
  for i=2:P
    Wmask(k,i*(N+I)) = 0; %assuming last input is bias
  end
end
L = 100;

DI = [rand(2,L)>0.5;ones(1,L)];
DT = xor(DI(1,:),DI(2,:));
DT = [0 0 0 DT(1:L-3)];
O = zeros(N,1); %No units are targets.
O(1) = 1;  %First unit is target.
alpha = input('alpha? (try 2)');

DItest = [0 0 1; 1 0 1; 0 1 1; 1 1 1]';
DItest = [DItest DItest DItest];
DTtest = [0 0 0 1 1 0 0 1 1 0 0 1 ];
step = 0;
for i=1:200,
  step = step + 1;
  PIJK = zeros(P,N*N*(P*(N+I)));
  Z = zeros(P*(N+I),1);
  CO = zeros(N,1);

  [W,RMSE,CO,PIJK,Z] = rfir(Wmask,W,DI,DT,O,alpha,CO,PIJK,Z);

  if mod(i,5)==0   % mod(x,N) = rem( rem(x,N)+N, N );
    fprintf('New Weights:\n');
    disp(W)
    fprintf('Epoch %d ',step);
    fprintf('RMSE = %f\n',RMSE);
    ztest = zeros(N+I,1);
    fprintf('In Targ  Unit Outputs\n');
    for l = 1:size(DItest,2);
      ztest(N+1:N+I,1) = DItest(:,l);
      COtest = 1./(1+exp(-W*ztest));
      %    if (l>2)
      fprintf('%.0f %.0f  %.0f  %.2f %.2f %.2f %.2f\n',...
	  DItest(1,l),DItest(2,l),DTtest(l),COtest(1),COtest(2),COtest(3),COtest(4));
      %    end
      ztest(1:N,1) = COtest;
    end
  end

end




