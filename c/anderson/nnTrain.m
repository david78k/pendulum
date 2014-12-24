function [teste,bestep,testouts,wh,wo,errorCurve] = nnTrain(...
  matrices,nrows,ninputs,nhiddens1,nhiddens2,noutputs,hrates,orates,moms,epochs,options)
%nnTrain:
%[teste,bestep,testouts,wh,wo,errorCurve] = nnTrain(...
%  matrices,nrows,ninputs,nhiddens1,nhiddens2,noutputs,...
%  hrates,orates,moms,epochs,options);
%
% Output Arguments:
%  teste     is error on test set using best network.
%  bestep    is epoch at which net produced lowest error on validation set.
%  testouts  is output of network for each test pattern.
%  wh,wo     are weight matrices, for hidden and output layers.
%  errCurve  is train,validation,and test errors per epoch (20 values spaced
%            evenly over entire training run.
%
% Input Arguments:
%  matrices  patterns, by row.  Each row has inputs then targets.
%  nrows     number of rows in each piece appearing sequentially in matrices.
%  ninputs   number of components in each input vector, not counting constant.
%  nhiddens1  number of hidden units.  If a vector, all values will be run.
%            If running hammer, this vector set to a nondecreasing order
%            to work around bnlib bugs.
%  nhiddens2  number of hidden units in second layer. Can be empty.
%            If a vector, all values will be run.
%            If running hammer, this vector set to a nondecreasing order
%            to work around bnlib bugs.
%  noutputs  number of components in target vector.
%  hrates    learning rate for hidden units. If a vector, all will be run.
%  orates    learning rate for output units.  If a vector, all will be run.
%  moms      momentum rates for all units.  If a vector, all will be run.
%  epochs    number of epochs to train.
%  options   optional string containing any subset of the following arguments:
%    'cv=yes' of 'cv=no' perform or don't perform cross-validation.  Default
%                        is cv=yes.
%    'c=1'      do this many combinations of test and validate choices
%               If not specified, will perform all.  If output arguments
%               given, will perform only one.
%    'f=nn.results'   Results will be written to this file, unless output
%                       arguments are given. Default is nn.results.
%    'o=short' or 'o=long'  Form of the results.  Default is short.  If output
%                       arguments are given, form is long.
%    'm=hammer'   Name of machine to run on.  If missing, use current
%                 machine.
%    'bg=yes' or 'bg=no'   Run in background or foreground. Default is
%                          bg=yes. If output arguments given, then in fg.
%    'd=nn.newone'  Name of directory to use.  If it exists, all existing
%                   file names are replaced by new ones.
%    's=runem'   Useful for performing a sequence of runs.
%                     Doesn't run them immediately.  Sets up directory and
%                     files as usual, then writes command to end of script
%                     file named runem.  To run, exit matlab and run the
%                     script runem in the background.  Avoids leaving matlab
%                     running in the background!
%
% THESE EXAMPLES ARE OUT OF DATE!!
%Examples (in nnTrainExample.m):
% d1 = [0 0 .1; 0 1 .9; 1 0 .9; 1 1 .1]   %2-bit exclusive-or
% d2 = d1 + randn(4,3)*.1;
% d3 = d1 + randn(4,3)*.1;
% nnTrain([d1;d2;d3],[4 4 4],2,3,0,1,.5,0.1,0.9,2000);
% or, without cross validation
% nnTrain([d1;d2;d3],[4 4 4],2,3,0,1,.5,0.1,0.9,2000,'nocv');
%  Results are in file nn.results.
%
%To do several runs in sequence, say with different noise in the data
%
% d1 = [0 0 .1; 0 1 .9; 1 0 .9; 1 1 .1]   %2-bit exclusive-or
% d2 = d1 + randn(4,3)*.1;
% d3 = d1 + randn(4,3)*.1;
% nnTrain([d1;d2;d3],[4 4 4],2,3,0,1,.5,0.1,0.9,2000,...,
%    'cv f=xor.results short m=hammer script=runxor');
% d2 = d1 + randn(4,3)*.1;
% d3 = d1 + randn(4,3)*.1;
% nnTrain([d1;d2;d3],[4 4 4],2,3,0,1,.5,0.1,0.9,2000,...,
%    'cv f=xor.results short m=hammer script=runxor');
% To run script in background:
% !runxor &
%
% Analysis:
%----------
% Example:
%nnPlotRuns(nnRuns(nnResults('s1ham.results'),[1 2 3 4],[10],[1 2],'max'),4)
%
% For more help, read the code. (Sorry)  You can also retrieve a short
%  tutorial from http://www.cs.colostate.edu/~anderson
%
%       Copyright (c) 1996 by Charles W. Anderson
       
if (nargin < 10)
  error('train requires at least 10 input arguments.  Type  help train');
end

%%% Unique symbol for this run.  Used in data file and exp file names.
unique = tempname;
nfiles = size(nrows,2);

%Process optional arguments
%if nargin < 11
  % defaults for optional arguments
  nCombinations = nfiles * (nfiles-1);
  resultsForm = 'short';
  bgfg = 'bg';
  resultsfile = 'nn.results';
  machine = [];
  scriptName = [];
  subdir = ['nn.dir' unique(8:size(unique,2))];
%else  
if nargin == 11
  c = findstr(options,'cv='); 
  if c
    if strcmp(strtok(options(c+3:length(options))),'yes')
      crossValid = 1;
    elseif strcmp(strtok(options(c+3:length(options))),'no')
      crossValid = 0;
    else
      error('Error specifying cv, must be cv=yes or cv=no');
    end
  else 
    crossValid = 1;
  end
  c = findstr(options,'c=');
  if c
    nCombinations = str2num(strtok(options(c+2:length(options))));
  end
  c = findstr(options,'f=');
  if c
    resultsfile = strtok(options(c+2:length(options)));
  end
  c = findstr(options,'o=');
  if c
    if strcmp(strtok(options(c+2:length(options))),'short')
      resultsForm = 'short';
    elseif strcmp(strtok(options(c+2:length(options))),'long')
      resultsForm = 'long';
    else
      error('Error specifying output form. Must be short or long.');
    end
  else
    resultsForm = 'short';   %default
  end
  c = findstr(options,'m=');
  if c
    machine = strtok(options(c+2:length(options)));
  end
  c = findstr(options,'bg=');
  if c
    if strcmp(strtok(options(c+3:length(options))),'yes')
      bgfg = 'bg';
    elseif strcmp(strtok(options(c+3:length(options))),'no')
      bgfg = 'fg';
    end
  else
    bgfg = 'bg';   %default
  end
  c = findstr(options,'d=');
  if c
    subdir = strtok(options(c+2:length(options)));
  end
  c = findstr(options,'s=');
  if c
    scriptName = strtok(options(c+2:length(options)));
  end
end        

%%%This must be after bgfg has been assigned.
if (nargout == 0) | strcmp(bgfg,'bg')
  if ~exist('resultsfile')
     error('When running in bg or with no output arguments, you must specify a filename.');
  end
end

%%%Make the subdir if it doesn't already exist.

fid = fopen(subdir,'r');
if fid == -1
  eval(['!mkdir ' subdir]);
else
  fclose(fid);
end

%%% Save all matrices into nn.data1, nn.data2, etc.
first = 1;
for m = 1:nfiles,
  num = nrows(m);
  tempMat = matrices(first:(first+num-1),:);
  % Data files are named nn.342.data.1, nn.342.data.2, etc
  % where 342 is a unique symbol automatically generated by MATLAB tempname.
  eval(['save ' subdir '/nn.data.' num2str(m) ' tempMat -ascii']);
  first = first + num;
end;

%%% Prepare experiment file for writing.  Name will be nn.exp
expFileName = [subdir '/nn.exp'];
fid = fopen(expFileName,'w');
if fid == -1,
   error(['Couldn''t open ' expFileName]);
end

%%%If result form not specified or given as short, add -summarize
if strcmp(resultsForm,'short')
   fprintf(fid,'-summarize\n');
end

%Work around bnlib bug
if strcmp(machine,'hammer') | strcmp(machine,'hammerhi')
 ascending = sort(nhiddens1);
 nhiddens1 = [];
 for i=length(ascending):-1:1
  nhiddens1 = [nhiddens1 ascending(i)];
 end
 ascending = sort(nhiddens2);
 nhiddens2 = [];
 for i=length(ascending):-1:1
  nhiddens2 = [nhiddens2 ascending(i)];
 end
end

fprintf(fid,'-ninputs %d -noutputs %d\n',ninputs,noutputs);

%%% Loop through all combinations of training, validation, and testing files.

ncombos = 0;

for testi = nfiles:-1:1,
  for vali = nfiles:-1:1,
    if vali ~= testi
      fprintf(fid,'-train ');
      for i = 1:nfiles
	if (i~=testi) & (i~=vali) 
	  fprintf(fid,'%s\n',['nn.data.' num2str(i)]);
	end
      end
      fprintf(fid,'-validate %s ',['nn.data.' num2str(vali)]);
      fprintf(fid,'-test %s\n',['nn.data.' num2str(testi)]);

      %%% Loop through all combinations of parameters

      for nh1 = nhiddens1,
	fprintf(fid,'-nhiddens1 %d ',nh1);
	for nh2 = nhiddens2,
	  if ~isempty(nh2)
	    fprintf(fid,'-nhiddens2 %d ',nh2);
	  end
	  for or = orates,
	    fprintf(fid,'-orate %f ',or);
	    for hr = hrates,
	      fprintf(fid,'-hrate %f ',hr);
	      for mom = moms,
		fprintf(fid,'-mom %f ',mom);
		for ep = epochs,
		  fprintf(fid,'-epochs %d\n',ep);
		  %        for i = 1:nruns
		  fprintf(fid,'-end ');  %One -end for each repetition.
		  %        end
		  fprintf(fid,'\n');
		end
	      end
	    end
	  end
	end
      end
      ncombos = ncombos + 1;
    end % end of if vali ~= testi
    if ncombos >= nCombinations, break; end
  end % loop through validation set choices
  if ncombos >= nCombinations, break; end
end  % loop through test set choices

fclose(fid);


%%% Redirect output to specified file or to unique temp file.
if ~exist('resultsfile') 
  resultsfile = [subdir '.results']; %subdir used in file name, not as dir
end
out = ['>>& ../' resultsfile];  % up a dir because being run in subdir

if strcmp(bgfg,'bg')
  amp = ' &';
else
  amp = ' ';
end

% If saving commands in script, then don't put in background.  Want them to
% run sequentially.
if ~ isempty(scriptName)
  amp = [];
end

if isempty(machine)
  str = ['( cd ' pwd '/' subdir '; train nn.exp' out amp ')'];
elseif strcmp(machine,'hammer')
  str = ['( cd ' pwd '/' subdir '; train nn.exp hammer' out amp ')'];
elseif strcmp(machine,'hammerhi')
  str = ['( cd ' pwd '/' subdir '; train nn.exp hammerhi' out amp ')'];
else
  str = ['rsh ' machine ' '' cd ' pwd '/' subdir '; train nn.exp ' out amp ' '' ' amp];
end

if isempty(scriptName)
  disp('Training with this command:');
  disp(['! ' str]);
  eval(['! ' str]);
else
  fscript = fopen(scriptName,'r');
  if fscript == -1
    fscript = fopen(scriptName,'a');
    fprintf(fscript,'#!/bin/csh\n');
  else
    fclose(fscript);
    fscript = fopen(scriptName,'a');
  end
  fprintf(fscript,'%s\n',str);
  fclose(fscript);
  eval(['!chmod +x ' scriptName]);
  disp(['This command appended to script ' scriptName]);
  disp(str);
end
