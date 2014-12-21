function [CheckMessage , CheckFlag] = checkiomode(IOMode)
% CHECKIOMODE   Check input/output model for errors
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
        switch lower(IOMode.Input.InputFunName)
            case 'none'
                
            otherwise
                if exist(IOMode.Input.InputFunName) ~= 2
                    CheckMessage    = 'Input function is not available';
                    CheckFlag       = 2;
                end
        end
        
end

return