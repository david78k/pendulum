# Each line of the short output has this format:
# 20  20   0.100   0.010 0.000     0.1473   581     0.2266     0.2444 0.925
#         Copyright (c) 1996 by Charles W. Anderson

NF == 10 && $1 != "ld.so.1:" { 
  ind = "h1 " $1 " h2 " $2 " rh " $3 " ro " $4 " m " $5;
  if (notin(ind))
    indices[numindices++] = ind;
  
  corr_sum[ind] += $10;
  corr_sqsum[ind] += $10*$10;
  epoch_sum[ind] += $7;
  epoch_sqsum[ind] += $7 * $7;
  rms_sum[ind] += $9;
  rms_sqsum[ind] += $9 * $9;
  num[ind]++;
}


END {
  for (i=0; i<numindices; i++) {
    ind = indices[i];
    printf("%33s %d fc %.3f %.3f RMS %.4f %.4f ep %.1f %.1f\n",
	   ind,num[ind],
	   corr_sum[ind]/num[ind],
	   confint(corr_sum[ind],corr_sqsum[ind],num[ind]),
	   rms_sum[ind]/num[ind],
	   confint(rms_sum[ind],rms_sqsum[ind],num[ind]),
	   epoch_sum[ind]/num[ind], 
	   confint(epoch_sum[ind],epoch_sqsum[ind],num[ind]));
  }
}


function notin(ind) {
  for (i=0; i<numindices; i++) {
    if (indices[i] == ind)
      return 0;
  }
  return 1;}

function stdev(sum, sumsq, n) {
  if (n > 2)
    return sqrt((n * sumsq - sum * sum) / (n * (n - 1)));
  else
    return 0.
}

function confint(sum, sumsq, n) {
  s = stdev(sum,sumsq,n);
  return 1.6449 * s / sqrt(n);
}

function fraction_correct(thresh) {
  n = 0;
  numc = 0.;
  while ($1 == "pat") {
    n++;
    if (($4 < thresh && $6 < thresh) ||
	($4 > thresh && $6 >= thresh))      numc++;
    getline;
  }

  return numc / n;
}
