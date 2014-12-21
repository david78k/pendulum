function [CheckMessage , CheckFlag] = checkadapt(AdaptModel)
% CHECKADAPT  Check adaptation model for errors
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
        switch lower(AdaptModel.Weight.AdaptFun)
            case 'none'
                
            otherwise
                if exist(AdaptModel.Weight.AdaptFun) ~= 2
                    CheckMessage    = 'Weight adaptation function is not available';
                    CheckFlag       = 2;
                end
        end
        
        switch lower(AdaptModel.Delay.AdaptFun)
            case 'none'
                
            otherwise
                if exist(AdaptModel.Delay.AdaptFun) ~= 2
                    CheckMessage    = 'Delay adaptation function is not available';
                    CheckFlag       = 3;
                end
        end
        
        switch lower(AdaptModel.Threshold.AdaptFun)
            case 'none'
                
            otherwise
                if exist(AdaptModel.Threshold.AdaptFun) ~= 2
                    CheckMessage    = 'Threshold adaptation function is not available';
                    CheckFlag       = 4;
                end
        end
        
        switch lower(AdaptModel.Model.AdaptFun)
            case 'none'
                
            otherwise
                if exist(AdaptModel.Model.AdaptFun) ~= 2
                    CheckMessage    = 'Model adaptation function is not available';
                    CheckFlag       = 5;
                end
        end
end

return