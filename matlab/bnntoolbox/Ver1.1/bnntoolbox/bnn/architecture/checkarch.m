function [CheckMessage , CheckFlag] = checkarch(NetArch)
% CHECKARCH     Check network architecture for errors
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
        %---neuron num
        if (NetArch.NeuronNum == 0)
            CheckMessage    = 'No neuron';
            CheckFlag       = 2;
        end
        
        %---checking input type
        if length(NetArch.InputType) ~= NetArch.InputNum
            CheckMessage    = 'Input types does not match with number of inputs';
            CheckFlag       = 3;
        end
        
        %---checking input weight
        if size(NetArch.InputWeight , 1) ~= NetArch.NeuronNum
            CheckMessage    = 'Number of neurons in input weight matrix does not match with number of neurons in network';
            CheckFlag       = 4;
        elseif size(NetArch.InputWeight , 2) ~= NetArch.InputNum
            CheckMessage    = 'Number of inputs in input weight matrix does not match with number of inputs in network';
            CheckFlag       = 5;
        end
        
        %---checking input delay
        if size(NetArch.InputDelay , 1) ~= NetArch.NeuronNum
            CheckMessage    = 'Number of neurons in input delay matrix does not match with number of neurons in network';
            CheckFlag       = 6;
        elseif size(NetArch.InputDelay , 2) ~= NetArch.InputNum
            CheckMessage    = 'Number of inputs in input delay matrix does not match with number of inputs in network';
            CheckFlag       = 7;
        end
        
        if ~isempty(find(NetArch.InputDelay < 0))
            CheckMessage    = 'Negative delays are not allowed, check input delay matrix';
            CheckFlag       = 8;
        end
        
        %---checking neuron weight
        if size(NetArch.NeuronWeight , 1) ~= NetArch.NeuronNum
            CheckMessage    = 'Number of neurons in neurons weight matrix rows does not match with number of neurons in network';
            CheckFlag       = 9;
        elseif size(NetArch.NeuronWeight , 2) ~= NetArch.NeuronNum
            CheckMessage    = 'Number of neurons in neurons weight matrix columns does not match with number of neurons in network';
            CheckFlag       = 10;
        end
        
        %---checking neuron delay
        if size(NetArch.NeuronDelay , 1) ~= NetArch.NeuronNum
            CheckMessage    = 'Number of neurons in neurons delay matrix rows does not match with number of neurons in network';
            CheckFlag       = 11;
        elseif size(NetArch.NeuronDelay , 2) ~= NetArch.NeuronNum
            CheckMessage    = 'Number of neurons in neurons delay matrix rows does not match with number of neurons in network';
            CheckFlag       = 12;
        end
        
        if ~isempty(find(NetArch.NeuronDelay < 0))
            CheckMessage    = 'Negative delays are not allowed, check neurons delay matrix';
            CheckFlag       = 13;
        end
        
end

return