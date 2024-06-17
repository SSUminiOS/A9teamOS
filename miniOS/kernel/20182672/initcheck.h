#ifndef INITCHECK_H
#define INITCHECK_H
#include "defs.h"
//success : ret0 fail : ret -1
int initchecklog(){
	if(access("/home/backup",F_OK)<0){
		printf("no backup dir. making ...\n");
		if(mkdir("/home/backup",0777)<0){
			printf("back dir make failed\n");
			return -1;
		}
		if(mkdir("/home/backup/logs",0777)<0){
			printf("backup logs dir make failed\n");
			return -1;
		}
	}
	return 0;
}

#endif