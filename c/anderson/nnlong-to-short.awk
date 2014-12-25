#       Copyright (c) 1996 by Charles W. Anderson

$3 == "h1" {
  h1 = $4; h2 = $6; hr = $10; or = $12; mr = $14; maxep = $16;
}
$1 == "epoch" && $2 == maxep {
  trerror = $4;
}
$1 == "Best" {
  ep = $3; valerror = $5; testerror = $7; frcor = $9;
  printf("%3d%4d%8.3f%8.3f%6.3f%11.4f%6d%11.4f%11.4f%6.3f\n",
	  h1,h2,hr,or,mr,trerror,ep,valerror,testerror,frcor);
}

  
