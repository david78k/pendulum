function [CheckMessage , CheckFlag] = checksim(SimParam)
% CHECKSIM      Check simulation model for errors
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

CheckMessage    = 'OK';
CheckFlag       = 1;

switch nargin
    case 0
        CheckMessage    = 'Nothing to check';
        CheckFlag       = 0;
        
    case 1
        %---stop time
        if (SimParam.StopTime < 0)
            CheckMessage    = 'Stop time must be a positive number';
            CheckFlag       = 2;
        end
        
        if (SimParam.StopTime < SimParam.StartTime)
            CheckMessage    = 'Stop time must be greater than start time';
            CheckFlag       = 3;
        end
        
        %---start time
        if (SimParam.StartTime < 0)
            CheckMessage    = 'Start time must be a positive number';
            CheckFlag       = 4;
        end
        
        %---solver type
        switch lower(SimParam.Solver)
            case {'ode45' , 'ode23' , 'ode113' , 'ode15s' , 'ode23s' , 'ode23t' , 'ode23tb'}
                
            otherwise
                CheckMessage    = 'Unknown solver';
                CheckFlag       = 5;
        end
        
        %---stop function
        switch lower(SimParam.StopFun)
            case 'none'
                
            otherwise
                if ~exist(SimParam.StopFun)
                    CheckMessage    = 'Stop function does not exist';
                    CheckFlag       = 5;
                end
        end
        
        %---user function
        switch lower(SimParam.UserFun)
            case 'none'
                
            otherwise
                if ~exist(SimParam.UserFun)
                    CheckMessage    = 'User defined function does not exist';
                    CheckFlag       = 6;
                end
        end
end

return