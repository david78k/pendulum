function SimParam = newsim(stop_time , start_time , initial_cond , solver_type , solver_option , stop_fun , user_fun)
% NEWSIM    Create new simulation model for Biological Neural Network (BNN)
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

%---checking input arguments and creating with defaults
switch nargin
    case 7              % all arguments
                
    case 6
        user_fun        = 'none';
        
    case 5
        user_fun        = 'none';
        stop_fun        = 'none';
        
    case 4
        user_fun        = 'none';
        stop_fun        = 'none';
        solver_option   = [];
        
    case 3
        user_fun        = 'none';
        stop_fun        = 'none';
        solver_type     = 'ode45';
        solver_option   = [];
        
    case 2
        user_fun        = 'none';
        stop_fun        = 'none';
        initial_cond    = 0;
        solver_type     = 'ode45';
        solver_option   = [];
        
    case 1
        user_fun        = 'none';
        stop_fun        = 'none';
        initial_cond    = 0;
        solver_type     = 'ode45';
        solver_option   = [];
        start_time      = 0;
        
    case 0
        user_fun        = 'none';
        stop_fun        = 'none';
        initial_cond    = 0;
        solver_type     = 'ode45';
        solver_option   = [];
        start_time      = 0;
        stop_time       = 0;
        
end

SimParam = struct('StopTime' , stop_time , 'StartTime' , start_time , 'InitialCond' , initial_cond , ...
    'Solver' , solver_type , 'SolverOption' , solver_option , 'StopFun' , stop_fun , 'UserFun' , user_fun);

%---checking model
[CheckMessage CheckFlag]    = checksim(SimParam);
dispmessage(CheckMessage , 'text' , 'Simulation Build Status');

SimParam.BuildStatus.Message = CheckMessage;
SimParam.BuildStatus.Flag    = CheckFlag;

return