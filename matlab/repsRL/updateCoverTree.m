function updateCoverTree(x, maxDepth, X)
global gN gvm gpar gchild ginfreg gvreg gdepth;

N = size(X, 1);
K = size(X, 2);
log2K = max(round(log2(K)), 1);

if (gN == 0)
  gN = 1;
  gvm(1) = 1;
  gpar(1) = 0;
  gchild{1, maxDepth + 1} = [];
  ginfreg(1, 1 : maxDepth + 1) = 1 ./ (2 .^ (0 : maxDepth));
  gvreg(1, 1 : maxDepth + 1) = 0;
  gdepth(1) = 0;
else
  v = 1;
  dp = 0;
  
  pari = 0;
  pardepth = Inf;
  
  depth = 0;
  while ((depth <= maxDepth) && (~isempty(v)))
    % compute the distance between x and new vertices
    ginfregp = ginfreg(v, depth + 1)' .^ 2;
    
    sel = (dp == 0);
    nsel = sum(sel);
    if (nsel > 0)
      if ((K > 4) && (nsel * K > 256))
        dp(sel) = dp(sel) + (((ones(nsel, 1) * x(1 : log2K) - ...
          X(v(sel), 1 : log2K)) .^ 2) * ones(log2K, 1))';
        sel = sel & (dp < ginfregp);
        nsel = sum(sel);
      end
      dp(sel) = ...
        (((ones(nsel, 1) * x - X(v(sel), :)) .^ 2) * ones(K, 1))';
    end
    
    % remove vertices that cannot produce a parent of x
    sel = (dp < ginfregp);
    v = v(sel);
    dp = dp(sel);
    
    % update the best candidate for the parent of x
    [val, ndx] = min(dp);
    if (val < (1 / (2 ^ depth)) ^ 2)
      pari = v(ndx);
      pardepth = depth;
    end
    
    % expand the next level of the tree
    childv = [gchild{v, depth + 1}];
    nchild = length(childv);
    v(end + (1 : nchild)) = childv;
    dp(end + (1 : nchild)) = 0;
    
    depth = depth + 1;
  end
  
  % double the size of data structures
  gN = gN + 1;
  if (gN > length(gvm))
    gvm(2 * (gN - 1)) = 0;
    gpar(2 * (gN - 1)) = 0;
    gchild{2 * (gN - 1), maxDepth + 1} = [];
    ginfreg(2 * (gN - 1), maxDepth + 1) = 0;
    gvreg(2 * (gN - 1), maxDepth + 1) = 0;
    gdepth(2 * (gN - 1)) = 0;
  end
  
  % append x
  if (pardepth < maxDepth)
    gvm(gN) = 1;
    gpar(gN) = pari;
    gchild{pari, pardepth + 1} = [gchild{pari, pardepth + 1}, gN];
    ginfreg(gN, pardepth + 2 : maxDepth + 1) = ...
      1 ./ (2 .^ (pardepth + 1 : maxDepth));
    gdepth(gN) = pardepth + 1;
  else
    gvm(gN) = 1;
    gpar(gN) = pari;
    gdepth(gN) = pardepth + 1;
  end
  
  % update vertex multiplicities and influence regions
  depth = pardepth;
  i = pari;
  while (i ~= 0)
    gvm(i) = gvm(i) + 1;
    
    dx = sqrt(sum((x - X(i, :)) .^ 2));
    sub = (gdepth(i) : depth) + 1;
    ginfreg(i, sub) = max(ginfreg(i, sub), dx + 1 / (2 ^ (pardepth + 1)));
    gvreg(i, sub) = max(gvreg(i, sub), dx);
    
    depth = gdepth(i) - 1;
    i = gpar(i);
  end
end
