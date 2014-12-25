function CmaesOpt()
global f1 f2
tic;
%list of parameters
  

% initial training pairs
global xgArray ygArray net
%[xgArray ygArray ]         = MakeGoalArray(100);
%save('goalsave.txt','-ascii','xgArray','ygArray')
goalsave = load('goalsave.txt'); % load the information of the goals in order to train always with the same goal set
xgArray  = goalsave(1,:);
ygArray  = goalsave(2,:);


options = cmaesNN('defaults');
options.MaxFunEvals = Inf;
options.MaxIter     = 1000;
options.PopSize     = 50;
options.Display     = 'off';
options.Plotting    = 'on';
options.Saving      = 'off';

%   StopFitness: '-Inf  % stop if < stopfitness (minimization)'
%              MaxFunEvals: '3e3*((N+5)^2+popsize)  % maximal number of fevals'
%                  MaxIter: 'Inf % maximal number of iterations'
%             StopFunEvals: 'Inf % stop after resp. evaluation to resume later'
%                 StopIter: 'Inf % stop after resp. iteration to resume later'
%                     TolX: '1e-12 % stop if x-changes smaller TolX'
%                   TolFun: '1e-12 % stop if fun-changes smaller TolFun'
%           StopOnWarnings: 'yes'   % 'no'=='off'==0, 'on'=='yes'==1 '
%            DiffMaxChange: 'Inf % maximal variable change(s), can be Nx1-vector'
%            DiffMinChange: '0  % minimal variable change(s), can be Nx1-vector'
%                  LBounds: '-Inf % lower bounds, scalar or Nx1-vector'
%                  UBounds: 'Inf  % upper bounds, scalar or Nx1-vector'
%                  PopSize: '4 + floor(3*log(N))  % population size, lambda'
%             ParentNumber: 'floor(popsize/2)  % popsize equals lambda'
%     RecombinationWeights: 'superlinear % (decrease), or linear, or equal'
%                  Display: 'on   % display initial and final message'
%                 Plotting: 'on   % plot while running'
%                   Resume: 'no     % resume former run from SaveFile'
%                  Science: 'off   % off==do some additional problem capturing'
%                   Saving: 'on     % save data to file while running'
%               SaveModulo: '1  % if >1 record data less frequently after gen=100'
%             SaveFileName: 'variablescmaes.mat'


%%Fitness function
fitnessFunction = @Procces_Individual;
%fitnessFunction = @test_fit;
%Start with default options
%%Run Cmaes
net =  BuildInitNN();
X = net;

[X, FVAL, COUNT, STOP, OUT, BESTEVER] = cmaesNN(fitnessFunction,X,0.01,options);
X = BESTEVER.x;

save Best.mat X




