
#include "defs.h"
#include "help.h"
#include "initcheck.h"
#include "backup.h"
#include "path.h"
#include "filelog.h"

/***********/
//rules
//every path -> converts to absolute path -> process function
//dirname => "/.../.../..." always

//problem filelog naming 
//home/kkh980729/hey/hk/kh.log
//

int main(int argc, char* argv[]){
	if(argc<2){
		fprintf(stderr,"error,wrong input\n");
		printf("%s help : show commands for program",argv[0]);
		exit(1);
	}

	//check /home/backup dir check	
	initchecklog();
	
	extern int optind;
    extern char* optarg;
    extern int optopt;
    int c;

	if(strcmp(argv[1],"help")==0){
		help(argv[2]);
		exit(0);
	}
	else if(strcmp(argv[1],"backup")==0){//argv[2] file or directory
		if(argc==2){
			fprintf(stderr,"error no path\n");
			exit(1);
		}
		if(argc>=3){
			char realPath[PATHNAME_SIZE]={0,};
			if(realpath(argv[2],realPath)==NULL){
				fprintf(stderr,"convert to realpath fail %s\n",argv[2]);
				exit(1);
			}

			char* timeStr=getTimeStr();
			char* desDirPath;
			if((desDirPath=makeBackupDirPath(timeStr))==NULL){
				fprintf(stderr,"makeBackupDirPath error\n");
				free(timeStr);
				exit(1);
			}
			////////////////////////////////////////////////////////////
			int result;
			int d=0;
			int r=0;
			int y=0;
			
			while((c=getopt(argc,argv,"dry"))!=-1){
				switch(c){
					case 'd':
						d=1;
					break;
					case 'r':
						r=1;
					break;
					case 'y':
						y=1;
					break;
					case '?':
						fprintf(stderr,"option err\n");
						free(timeStr);
						free(desDirPath);
						exit(1);
					break;
                }
        	}
				
			if(r==1){
				if(y==0)
					result=backup_deep(realPath,desDirPath,1);

				if(y==1)
					result=backup_deep(realPath,desDirPath,0);
			}
			else{
				if(d==1){
					if(y==0)
							result=backup_shallow(realPath,desDirPath,1);

					if(y==1)
							result=backup_shallow(realPath,desDirPath,0);
				}
				if(d==0){
						if(y==0)
							result=backup_file(realPath,desDirPath,1);

						if(y==1)
							result=backup_file(realPath,desDirPath,0);
				}
						
			}

			if(result<0){
				fprintf(stderr,"backup error\n");
				free(timeStr);
				free(desDirPath);
				exit(1);
			}
			/////////////////////////////////////////////////////////////
			//realpath, desDirPath, timeStr  -> ssubak.log record
			
			checkDirIsEmpty(desDirPath);
			free(timeStr);
			free(desDirPath);
		}
				
	}
	else{
		fprintf(stderr,"error : wrong command --%s\n",argv[1]);
		printf("%s help : show commands for program",argv[0]);
		exit(1);
	}

}














