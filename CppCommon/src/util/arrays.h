/*
 arrays.h
 Katsuki Ohto
 */

#ifndef UTIL_ARRAYS_H_
#define UTIL_ARRAYS_H_

static int sortedDAsearch(double *dar,int sn,int ln,double ans){
	//単調増加な配列の内容がansを超える最小のインデックス値を
	//2分探索で探す
	int n=(sn+ln)/2;
	if(dar[n]>ans){
		if(n-sn<=1)return sn;
		return sortedDAsearch(dar,sn,n,ans);
	}else{
		if(ln-n<=1)return ln-1;
		return sortedDAsearch(dar,n,ln,ans);
	}
}

#endif // UTIL_ARRAYS_H_