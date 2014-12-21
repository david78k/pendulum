function savebnn(net)
% SAVEBNN   Save a Biological Neural Network (BNN)
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

switch nargin
    case 0
        error('Nothing to save')
        
end

save_file_name  = net.InputOutput.Output.SaveFileName;
file_format     = net.InputOutput.Output.FileFormat;

switch upper(file_format)
    case 'MAT'
        save(save_file_name , 'net')
        
    case 'EXCEL'
        
end

return