function [CheckMessage , CheckFlag] = checksynapse(SynapseModel)
% CHECKSYNAPSE  Check synapse model for errors
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
        %---PSP
        switch lower(SynapseModel.External.ExtPSPFun)
            case 'none'
                
            otherwise
                if (exist(SynapseModel.External.ExtPSPFun) ~= 2)
                    CheckMessage    = 'External PSP function is not available';
                    CheckFlag       = 2;
                end
        end
        
        switch lower(SynapseModel.Internal.IntPSPFun)
            case 'none'
                
            otherwise
                if (exist(SynapseModel.Internal.IntPSPFun) ~= 2)
                    CheckMessage    = 'Internal PSP function is not available';
                    CheckFlag       = 3;
                end
        end
        
end

return