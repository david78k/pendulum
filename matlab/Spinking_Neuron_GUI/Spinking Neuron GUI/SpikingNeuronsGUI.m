function SpikingNeurons(varargin)
% function SpikingNeurons(varargin)
%
% This Graphical User Interface is generated with user-supplied menu definition data
% buildMainGUI.m version 1.0 has been used to generate this GUI
%
fh = figure('Visible','off', ...
            'NumberTitle','off', ...  % turns off MATLAB GUI window heading
            'Name','SpikingNeurons', ...    % now, define my own
            'Units','normalized', ...
            'Units','normalized', ...
            'Position',[1.000000e-001 1.000000e-001 6.000000e-001 6.000000e-001], ...
            'Color','white', ...   % match bg of MODE_new.jpg
            'Resize','off');
set(fh,'MenuBar','none'); % removes MATLAB default menu bar
% create custom menu bar items as defined by GID loaded above:
% "File", "Model", "Articles", "Tutorials", "Examples", "Run", "Code", "Help"
%
% The menu item handles used here are all local, the naming convention is
% mh_ijk where i, j, and k represent the menu item and sub menu items
% First, set up all the menu bars (but without any callbacks . . .
% The menu bars are: File, Model, Articles, Tutorial, Examples, Run, Code, Help
% For menu bar item: 'File'
mh1 = uimenu(fh,'Label', 'File');
mh(1,1,1,1) = uimenu(mh1,'Label', 'Exit');


% For menu bar item: 'Model'
mh2 = uimenu(fh,'Label', 'Model');
mh(2,1,1,1) = uimenu(mh2,'Label', 'System diagram');


% For menu bar item: 'Articles'
mh3 = uimenu(fh,'Label', 'Articles');
mh(3,1,1,1) = uimenu(mh3,'Label', 'Izhikevich (2003)');


% For menu bar item: 'Tutorial'
mh4 = uimenu(fh,'Label', 'Tutorial');
mh(4,1,1,1) = uimenu(mh4,'Label', 'Abstract');
mh(4,2,1,1) = uimenu(mh4,'Label', 'Tutorial');


% For menu bar item: 'Examples'
mh5 = uimenu(fh,'Label', 'Examples');
mh(5,1,1,1) = uimenu(mh5,'Label', 'Preset image 1: Gail');
mh(5,2,1,1) = uimenu(mh5,'Label', 'Preset image 2: Wedding');
mh(5,3,1,1) = uimenu(mh5,'Label', 'Preset image 3: Dog');
mh(5,4,1,1) = uimenu(mh5,'Label', 'Preset image 4: Optical illusion1');
mh(5,5,1,1) = uimenu(mh5,'Label', 'Preset image 5: Optical illusion2');
mh(5,6,1,1) = uimenu(mh5,'Label', 'Preset image 6: Robert');


% For menu bar item: 'Run'
mh6 = uimenu(fh,'Label', 'Run');
mh(6,1,1,1) = uimenu(mh6,'Label', 'Load an image and spike!');


% For menu bar item: 'Code'
mh7 = uimenu(fh,'Label', 'Code');
mh(7,1,1,1) = uimenu(mh7,'Label', 'Source code');


% For menu bar item: 'Help'
mh8 = uimenu(fh,'Label', 'Help');
mh(8,1,1,1) = uimenu(mh8,'Label', 'Contact');
mh(8,2,1,1) = uimenu(mh8,'Label', 'Credit');
mh(8,3,1,1) = uimenu(mh8,'Label', 'License');




% Next, setup the callbacks for all relevant menus

set(mh(1,1,1,1),'callback', {@exit_Callback});
set(mh(2,1,1,1),'callback', {@opendoc1_Callback 'System_diagram.pdf'});
set(mh(3,1,1,1),'callback', {@opendoc1_Callback 'Izh_spikes.pdf'});
set(mh(4,1,1,1),'callback', {@opendoc1_Callback 'Abstract.pdf'});
set(mh(4,2,1,1),'callback', {@opendoc1_Callback 'Tutorial.pdf'});
set(mh(5,1,1,1),'callback', {@run1_Callback 'reducedSpikingDemo(1)'});
set(mh(5,2,1,1),'callback', {@run1_Callback 'reducedSpikingDemo(2)'});
set(mh(5,3,1,1),'callback', {@run1_Callback 'reducedSpikingDemo(3)'});
set(mh(5,4,1,1),'callback', {@run1_Callback 'reducedSpikingDemo(4)'});
set(mh(5,5,1,1),'callback', {@run1_Callback 'reducedSpikingDemo(5)'});
set(mh(5,6,1,1),'callback', {@run1_Callback 'reducedSpikingDemo(6)'});
set(mh(6,1,1,1),'callback', {@run1_Callback 'reducedSpikingDemo(0)'});
set(mh(7,1,1,1),'callback', {@opendoc1_Callback 'HTML/Index.html'});
set(mh(8,1,1,1),'callback', {@opendoc1_Callback 'Contact.html'});
set(mh(8,2,1,1),'callback', {@opendoc1_Callback 'Credit.html'});
set(mh(8,3,1,1),'callback', {@opendoc1_Callback 'License.html'});
% Displays a user-provided image on this GUI
[X,map] = imread('izhik.gif');
imshow(X,map); % display image on front page


set(fh,'Visible','on');



% Functions needed by the output GUI will be generated in the following . . .


% generate callback function of the "opendoc1" type operation
function opendoc1_Callback(hObject,eventdata,varargin)
% hObject    handle to exit_callback (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
open(varargin{1})



function exit_Callback(hObject, eventdata)
% hObject    handle to exit_Callback (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
close all



% generate "run1" type callback function
function run1_Callback(hObject, eventdata, varargin)
% hObject    handle to RT_advanced_run (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
eval(varargin{1})



