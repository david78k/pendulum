function SynapseModel = newsynapse(ext_psp_fun , ext_psp_fun_param , int_psp_fun , int_psp_fun_param)
% NEWSYNAPSE    Create a new synapse model for Biological Neural Network (BNN)
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

%---checking input arguments and creating with defaults
switch nargin
    case 4          % all arguments
        ext_psp_prop    = struct('ExtPSPFun' , ext_psp_fun , 'ExtPSPFunParam' , ext_psp_fun_param);
        int_psp_prop    = struct('IntPSPFun' , int_psp_fun , 'IntPSPFunParam' , int_psp_fun_param);
        
    case 3
        ext_psp_prop    = struct('ExtPSPFun' , ext_psp_fun , 'ExtPSPFunParam' , ext_psp_fun_param);
        int_psp_prop    = struct('IntPSPFun' , int_psp_fun , 'IntPSPFunParam' , 'def');
        
    case 2
        ext_psp_prop    = struct('ExtPSPFun' , ext_psp_fun , 'ExtPSPFunParam' , ext_psp_fun_param);
        int_psp_prop    = struct('IntPSPFun' , 'alpha_fun_int' , 'IntPSPFunParam' , 'def');
        
    case 1
        ext_psp_prop    = struct('ExtPSPFun' , ext_psp_fun , 'ExtPSPFunParam' , 'def');
        int_psp_prop    = struct('IntPSPFun' , 'alpha_fun_int' , 'IntPSPFunParam' , 'def');
        
    case 0
        ext_psp_prop    = struct('ExtPSPFun' , 'alpha_fun_ext' , 'ExtPSPFunParam' , 'def');
        int_psp_prop    = struct('IntPSPFun' , 'alpha_fun_int' , 'IntPSPFunParam' , 'def');
end

SynapseModel = struct('External' , ext_psp_prop , 'Internal' , int_psp_prop);

%---checking model
[CheckMessage CheckFlag]    = checksynapse(SynapseModel);
dispmessage(CheckMessage , 'text' , 'Synapse Build Status');

SynapseModel.BuildStatus.Message = CheckMessage;
SynapseModel.BuildStatus.Flag    = CheckFlag;

return