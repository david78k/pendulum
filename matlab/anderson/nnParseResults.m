function [nnE, nnEp, nnOuts, nnWh1, nnWh2, nnWo, nnCurve, nnCorr] = nnParseResults(nnFid);
%  Copyright (c) 1996 by Charles W. Anderson

word = fscanf(nnFid,'%s',1);
if strcmp(word,'ld.so.1:') == 1
  fskipwords(nnFid,9); %to skip rest of lib older version warning
  word = fscanf(nnFid,'%s',1);  %get first word after warning line
end

while strcmp(word,'i')~=1,
  word = fscanf(nnFid,'%s',1);
  if feof(nnFid) == 1,
    %error('Can''t read past end of file');
    return;
  end
end
%fskipwords(nnFid,1);
numInputs = fscanf(nnFid,'%d',1);
fskipwords(nnFid,1);
numHidden1 = fscanf(nnFid,'%d',1);
fskipwords(nnFid,1);
numHidden2 = fscanf(nnFid,'%d',1);
fskipwords(nnFid,1);
numOutputs = fscanf(nnFid,'%d',1);
fskipwords(nnFid,1);
hrate = fscanf(nnFid,'%f',1);
fskipwords(nnFid,1);
orate = fscanf(nnFid,'%f',1);
fskipwords(nnFid,1);
mom = fscanf(nnFid,'%f',1);
fskipwords(nnFid,1);
eps = fscanf(nnFid,'%d',1);
fskipwords(nnFid,1);
linearOut = fscanf(nnFid,'%d',1);
fskipwords(nnFid,1);
hammer = fscanf(nnFid,'%d',1);
fskipwords(nnFid,5);

first = fscanf(nnFid,'%s',1);
if strcmp(first,'epoch') ~= 1
  error('Error reading results file.  expected ''epoch'' but not found.');
end

nnCurve = [];
while strcmp(first,'epoch') == 1,
  rst = [];
  rst = [rst fscanf(nnFid,'%f',1)];
  fskipwords(nnFid,1);
  rst = [rst fscanf(nnFid,'%f',1)];
  fskipwords(nnFid,1);
  rst = [rst fscanf(nnFid,'%f',1)];
  fskipwords(nnFid,1);
  rst = [rst fscanf(nnFid,'%f',1)];
  fskipwords(nnFid,1);
  rst = [rst fscanf(nnFid,'%f',1)];
  fskipwords(nnFid,1);
  rst = [rst fscanf(nnFid,'%f',1)];
  nnCurve = [nnCurve ; rst];
  first = fscanf(nnFid,'%s',1);
end

while strcmp(first,'pat') ~= 1,
  first = fscanf(nnFid,'%s',1);
end
%first = 'pat';
nnOuts = [];
while strcmp(first,'pat') == 1,
  fskipwords(nnFid,2);
  targets = fscanf(nnFid,'%f',numOutputs);
  fskipwords(nnFid,1);
  outputs = fscanf(nnFid,'%f',numOutputs);
  nnOuts = [nnOuts ; [targets' outputs']];
  fskipwords(nnFid,1+numOutputs); % to skip errors
  first = fscanf(nnFid,'%s',1);
end

%Now should be pointing right after 'Best'

fskipwords(nnFid,1);
nnEp = fscanf(nnFid,'%d',1);
fskipwords(nnFid,3);
nnE = fscanf(nnFid,'%f',1);
fskipwords(nnFid,1);
nnCorr = fscanf(nnFid,'%f',1);

fskipwords(nnFid,3);

%Now should be at first weight
%  Assumes inputs do not go to output net.

nnWh1 = fscanf(nnFid,'%f',[numInputs+1,numHidden1]);
if numHidden2 > 0
  nnWh2 = fscanf(nnFid,'%f',[numHidden1+1,numHidden2]);
  nnWo = fscanf(nnFid,'%f',[numHidden2+1,numOutputs]);
else
  nnWh2 = [];
  nnWo = fscanf(nnFid,'%f',[numHidden1+1,numOutputs]);
end  


