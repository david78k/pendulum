function dispmessage(Message2Disp , DispType , MessageClass)
% DISPMESSAGE   Display message
% 
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index


switch nargin
    case 0
        Message2Disp    = 'Nothing !!!';
        DispType        = 'text';
        MessageClass    = 'Nothing !!!'
        
    case 1
        DispType        = 'text';
        MessageClass    = 'Network Build Status';        
        
    case 2
        MessageClass    = 'Network Build Status';
        
end

switch lower(DispType)
    case 'text'
        disp(' ')
        disp(['From ' , MessageClass , ':' , Message2Disp])
        
    case 'gui'
        
        
end

return