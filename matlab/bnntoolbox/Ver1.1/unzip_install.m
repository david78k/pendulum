%% Decompressor program for Biological Neural Networks Toolbox
%   
%   Version:    1.0
%   ----------------------------------
%   Amir Reza Saffari Azar, August 2004
%   amir@ymer.org
%   http://www.ymer.org
%   http://ee.sut.ac.ir/faculty/saffari/main.index

disp('Enter the full or relative path to the toolbox zip file.')
disp('If this file is in the current directory just press Enter.')
toolbox_path    = input('Enter the downloaded zip file path:    ');

if isempty(toolbox_path)
    toolbox_path    = pwd;
end

if ~exist([toolbox_path , '\bnntoolbox.zip'])
    error('Toolbox file does not exist')
end

disp(' ')
disp('Enter the full or relative path to the current directory to install the toolbox.')
disp('If you want to install to the curent directory just press Enter.')
install_dir     = input('Enter the install directory:   ');

if ~exist(install_dir)
    create_flag = input('Installation directory does not exist. Do you want to create it? (0=No , 1=Yes)');
    if ~create_flag
        error('Installation directory does not exist.')
    end
else
    create_flag = 0;
end

if create_flag
    if ~isempty(install_dir)
        mkdir(install_dir)
    end
end

cd(toolbox_path)

if ~isempty(install_dir)
    unzip('bnntoolbox.zip' , install_dir)
    cd([install_dir , '/' , 'bnntoolbox'])
    install_toolbox
else
    unzip('bnntoolbox.zip')
    cd('bnntoolbox')
    install_toolbox
end