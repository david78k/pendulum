function IOMode = newiomode(input_fun_name , input_spike_time , analog_input , auto_save , save_file_name , file_format)
% NEWIOMODE     Create input/output model for Biological Neural Network (BNN)
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

%---checking input arguments and creating with defaults
switch nargin
    case 6          % all arguments
        
    case 5
        file_format     = 'MAT';
        
    case 4
        file_format     = 'MAT';
        save_file_name  = 'bnnet';
        
    case 3
        file_format     = 'MAT';
        save_file_name  = 'bnnet';
        auto_save       = 0;
        
    case 2
        file_format     = 'MAT';
        save_file_name  = 'bnnet';
        auto_save       = 0;
        analog_input    = 0;
        
    case 1
        file_format         = 'MAT';
        save_file_name      = 'bnnet';
        auto_save           = 0;
        analog_input        = 0;
        input_spike_time    = 0;
        
    case 0
        file_format         = 'MAT';
        save_file_name      = 'bnnet';
        auto_save           = 0;
        analog_input        = 0;
        input_spike_time    = 0;
        input_fun_name      = 'none';
        
end

input_prop  = struct('InputFunName' , input_fun_name , 'InputSpikeTime' , {input_spike_time} , 'AnalogInput' , analog_input);
output_prop = struct('SpikeTimes' , [] , 'State' , [] , 'Time' , [] , 'AutoSave' , auto_save , 'SaveFileName' , save_file_name , 'FileFormat' , file_format);
IOMode      = struct('Input' , input_prop , 'Output' , output_prop);

%---checking model
[CheckMessage CheckFlag]    = checkiomode(IOMode);
dispmessage(CheckMessage , 'text' , 'InputOutput Build Status');

IOMode.BuildStatus.Message = CheckMessage;
IOMode.BuildStatus.Flag    = CheckFlag;

return