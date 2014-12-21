function AdaptModel = newadapt(adapt_fun_weight , adapt_fun_weight_param , ...
                        adapt_fun_delay , adapt_fun_delay_param , ...
                        adapt_fun_threshold , adapt_fun_threshold_param, ...
                        adapt_fun_model , adapt_fun_model_param)
% NEWADAPT    Create a new adaptation model for Biological Neural Network (BNN)
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

%---checking input arguments and creating with defaults
switch nargin
    case 8         % all arguments
        weight_prop     = struct('AdaptFun' , adapt_fun_weight , 'AdaptFunParam' , adapt_fun_weight_param);
        delay_prop      = struct('AdaptFun' , adapt_fun_delay , 'AdaptFunParam' , adapt_fun_delay_param);
        threshold_prop  = struct('AdaptFun' , adapt_fun_threshold , 'AdaptFunParam' , adapt_fun_threshold_param);
        model_prop      = struct('AdaptFun' , adapt_fun_model , 'AdaptFunParam' , adapt_fun_model_param);
        
    case 7
        weight_prop     = struct('AdaptFun' , adapt_fun_weight , 'AdaptFunParam' , adapt_fun_weight_param);
        delay_prop      = struct('AdaptFun' , adapt_fun_delay , 'AdaptFunParam' , adapt_fun_delay_param);
        threshold_prop  = struct('AdaptFun' , adapt_fun_threshold , 'AdaptFunParam' , adapt_fun_threshold_param);
        model_prop      = struct('AdaptFun' , adapt_fun_model , 'AdaptFunParam' , 'def');
        
    case 6
        weight_prop     = struct('AdaptFun' , adapt_fun_weight , 'AdaptFunParam' , adapt_fun_weight_param);
        delay_prop      = struct('AdaptFun' , adapt_fun_delay , 'AdaptFunParam' , adapt_fun_delay_param);
        threshold_prop  = struct('AdaptFun' , adapt_fun_threshold , 'AdaptFunParam' , adapt_fun_threshold_param);
        model_prop      = struct('AdaptFun' , 'none' , 'AdaptFunParam' , 'def');
        
    case 5
        weight_prop     = struct('AdaptFun' , adapt_fun_weight , 'AdaptFunParam' , adapt_fun_weight_param);
        delay_prop      = struct('AdaptFun' , adapt_fun_delay , 'AdaptFunParam' , adapt_fun_delay_param);
        threshold_prop  = struct('AdaptFun' , adapt_fun_threshold , 'AdaptFunParam' , 'def');
        model_prop      = struct('AdaptFun' , 'none' , 'AdaptFunParam' , 'def');
        
    case 4
        weight_prop     = struct('AdaptFun' , adapt_fun_weight , 'AdaptFunParam' , adapt_fun_weight_param);
        delay_prop      = struct('AdaptFun' , adapt_fun_delay , 'AdaptFunParam' , adapt_fun_delay_param);
        threshold_prop  = struct('AdaptFun' , 'none' , 'AdaptFunParam' , 'def');
        model_prop      = struct('AdaptFun' , 'none' , 'AdaptFunParam' , 'def');
        
    case 3
        weight_prop     = struct('AdaptFun' , adapt_fun_weight , 'AdaptFunParam' , adapt_fun_weight_param);
        delay_prop      = struct('AdaptFun' , adapt_fun_delay , 'AdaptFunParam' , 'def');
        threshold_prop  = struct('AdaptFun' , 'none' , 'AdaptFunParam' , 'def');
        model_prop      = struct('AdaptFun' , 'none' , 'AdaptFunParam' , 'def');
        
    case 2
        weight_prop     = struct('AdaptFun' , adapt_fun_weight , 'AdaptFunParam' , adapt_fun_weight_param);
        delay_prop      = struct('AdaptFun' , 'none' , 'AdaptFunParam' , 'def');
        threshold_prop  = struct('AdaptFun' , 'none' , 'AdaptFunParam' , 'def');
        model_prop      = struct('AdaptFun' , 'none' , 'AdaptFunParam' , 'def');
        
    case 1
        weight_prop     = struct('AdaptFun' , adapt_fun_weight , 'AdaptFunParam' , 'def');
        delay_prop      = struct('AdaptFun' , 'none' , 'AdaptFunParam' , 'def');
        threshold_prop  = struct('AdaptFun' , 'none' , 'AdaptFunParam' , 'def');
        model_prop      = struct('AdaptFun' , 'none' , 'AdaptFunParam' , 'def');
        
    case 0
        weight_prop     = struct('AdaptFun' , 'none' , 'AdaptFunParam' , 'def');
        delay_prop      = struct('AdaptFun' , 'none' , 'AdaptFunParam' , 'def');
        threshold_prop  = struct('AdaptFun' , 'none' , 'AdaptFunParam' , 'def');
        model_prop      = struct('AdaptFun' , 'none' , 'AdaptFunParam' , 'def');
end

AdaptModel = struct('Weight' , weight_prop , 'Delay' , delay_prop , 'Threshold' , threshold_prop , 'Model' , model_prop);

%---checking model
[CheckMessage CheckFlag]    = checkadapt(AdaptModel);
dispmessage(CheckMessage , 'text' , 'Adaptation Build Status');

AdaptModel.BuildStatus.Message = CheckMessage;
AdaptModel.BuildStatus.Flag    = CheckFlag;

return