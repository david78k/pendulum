function [W,RMSE,COL,PIJK,Z] = rfir(Wmask,W,DI,DT,EO,alpha,CO,PIJK,Z)

% a MATLAB m-file for real time recurrent learning with FIR 
% synapses (based on Williams and Zipsers: "A Learning Algorithm 
% for Continually Running Fully Recurrent Neural Networks", 
% ICS Report 8805).
%
% This routine can be used for both pre-learning and on-line 
% reccurent learning. It assumes sigmoidal nonlinearity. 
%
% The routine can start from any point in time by approprietly
% defined input parameters. Unless defined otherwise, it assumes fully 
% interconnected N - neurons network with I inputs, O outputs and
% FIR synapses of P-th order.  
%
% Note that biasing of neurons can be included in the list of input
% signals I.
%
% Input parameters:
%
% W:  N x P*(N+I) recurrent weight matrix of the network. This matrix 
%     represent initial condition for recursive learning.  
%
% Wmask: Masking matrix: Entry marked with 1 denotes a presence of connection,
%     0 otherwise.
%
% DI:  I x L matrix of input data (L denotes the number of samples)
%
% DT:  O x L matrix of target data (L denotes the number of samples)
%
% PIJK: P x (N * N * (Px(N+I))) matrix of partial derivatives at P different times
%
% EO:  N x O mapping matrix: maps neurons to output lines. "1" at position 
%            ij denotes that "i"-th neuron output is compared to "j"-th 
%            target data. "0" denotes absence of mapping.   
%
% alpha: positive constant; denotes learning rate
%
% Z:   P*(N+I) x 1 Vector of P previous outputs + inputs   
%
% Output parameters:
%
% WL: N x P*(N+I) matrix of updated weights at point L.
%
% PIJKL: 1 x (N * N * (Px(N+I))) vector of partial derivatives at point L.
%
% COL: N x 1 matrix of neurons outputs at point L.
%
% Z = [u1(t)...uN(t) i1(t)...iI(t) u1(t-1)...uN(t-1) i1(t-1)...iI(t-1)
%           ... u1(t-P+1)...uN(t-P+1) i1(t-P+1)...iI(t-P+1)] '  (transpose)
%
%       where u1 is output of unit 1, i1 is input 1
%
% W = [w1u1(t)...w1uN(t) w1i1(t)...w1iI(t) 
%           ... w1u1(t-P+1)...w1uN(t-P+1) w1i1(t-P+1)...w1iI(t-P+1)
%      w2u1(t)...w2uN(t) w2i1(t)...w2iI(t) 
%           ... w2u1(t-P+1)...w2uN(t-P+1) w2i1(t-P+1)...w2iI(t-P+1)
%        .
%        .
%        .
%      wNu1(t)...wNuN(t) wNi1(t)...wNiI(t) 
%           ... wNu1(t-P+1)...wNuN(t-P+1) wNi1(t-P+1)...wNiI(t-P+1)]
%	  
%
%	where iI(.) is assumed to be bias input and only w.iI(t) is used,
%	all other w.iI(t-p) are masked out
%
%       Copyright (c) 1996 by Charles W. Anderson and Zlatko Sijercic

if nargin < 7, error('Not enough arguments.'); end

[I, L]=size(DI); 
[O, L]=size(DT); 
[N, PNI]=size(W); 
P=PNI/(N+I); 
OM=EO*ones(O,1);
PIJKL=zeros(1,N*N*P*(N+I));
COL=zeros(N,1);  % CO=Z(1:P:P*N,1);
dW = zeros(size(W));
SE=0;
for l=1:L, 

    for t=P*(N+I):-1:2,
        Z(t,1)=Z(t-1,1);
    end

    Z(1:P:P*(N+I),1)=[CO' DI(:,l)']';

    % Pass inputs through sigmoid (logistic) function     

    COL=1./(1 + exp(- (Wmask.*W)*Z)); % COL is output at t+1

    % Update for partial derivatives at k-th output ij-th position

    for k=1:N,
        for i=1:N,
            for j=1:N+I,
                for t=1:P,   % update of all derivatives
                    if Wmask(i,(j-1)*P+t) == 0
                    else
                     S=0;
                        for jj=1:N,   % formula for calculation
                            i1=(jj-1)*(N*P*(N+I))+(i-1)*P*(N+I)+(j-1)*P+t;
                            for tt=1:P,
                                if Wmask(k,(jj-1)*P+tt) == 0
                                else
                                   S=S+W(k,(jj-1)*P+tt)*PIJK(tt,i1);
                                end;
                            end;
                        end;
                     i1=(k-1)*(N*P*(N+I))+(i-1)*P*(N+I)+(j-1)*P+t;
                     PIJKL(1,i1)=COL(k,1)*(1-COL(k,1))*(S+(i==k)*Z((t-1)*(N+I)+j,1));
                    end
                end
            end
        end
    end

    % Update for delta W: dW for point t+1

    ET=EO*DT(:,l);  % this is mapping for target data    
    for k=1:N
      if OM(k) > 0
	E = ET(k) - CO(k,1);
	SE = SE + E*E;
      end
    end

    for i=1:N,
        for j=1:N+I,
            for t=1:P,
                S=0;
                if Wmask(i,(j-1)*P+t) == 0
                   dW(i,(j-1)*P+t)=0;
                else
                     for k=1:N,
                         if OM(k)==0 
                         else 
                            i1=(k-1)*(N*P*(N+I))+(i-1)*P*(N+I)+(j-1)*P+t;
                            S=S+(ET(k)-CO(k,1))*PIJK(1,i1); 
                         end 
                     end
                end
                dW(i,(j-1)*P+t)=alpha*S; 
            end  
        end
    end

    % Updating weights and outputs before going to next time sample    
    W=W+dW;
    for t=1:P-1,
        PIJK(P-t+1,:)=PIJK(P-t,:);
    end
    CO=COL;
    PIJK(1,:)=PIJKL(1,:);

end
RMSE = sqrt(SE/(O*L));

