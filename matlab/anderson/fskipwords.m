function fskipwords(fid,n)
  for i = 1:n,
    fscanf(fid,'%s',1);
  end
