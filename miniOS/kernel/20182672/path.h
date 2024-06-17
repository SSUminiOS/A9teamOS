#ifndef PATH_H
#define PATH_H
#include "defs.h"
#include "queue.h"

//success : ret char*, fail : ret NULL
char* changeToAbPath(char* path){
    char* retPath=(char*)malloc(PATHNAME_SIZE);
    if(path[0]=='/'){
        strcpy(retPath,path);
        return retPath;
    }
        
    
    if(path[0]=='.'){
        if(path[1]=='.'){
            int count=1;
            char* searchPtr=strstr(path,"../");
            while((searchPtr=strstr(searchPtr+1,"../"))!=NULL)
                count++;

            char cwd[PATHNAME_SIZE]={0,};
            if(getcwd(cwd,PATHNAME_SIZE)<0){
                fprintf(stderr,"getcwd fail\n");
                free(retPath);
                return NULL;
            }

            char* temp=dirname(cwd);
            for(int i=1;i<count;i++){
                temp=dirname(temp);
            }
            sprintf(retPath,"%s/%s",temp,path+(3*count));
            return retPath;
        }
        else if(path[1]=='/'){
            char cwd[PATHNAME_SIZE]={0,};
            if(getcwd(cwd,PATHNAME_SIZE)<0){
                fprintf(stderr,"getcwd fail\n");
                free(retPath);
                return NULL;
            }
            sprintf(retPath,"%s%s",cwd,path+1);
            return retPath;
        } 
        else{
            fprintf(stderr,"not proper path %s\n",path);
            free(retPath);
            return NULL;
        }
    }
    else return NULL;
}


//recursive
void checkDirIsEmpty(char* dirPath){
	DIR* dirp;
	struct dirent* dentry;
	struct stat statbuf;
	char filename[PATHNAME_SIZE]={0,};
	char dirAbPath[PATHNAME_SIZE]={0,};

	int count=0;

	if((dirp=opendir(dirPath))==NULL){
        fprintf(stderr,"open dir fail %s\n",dirPath);
        return;
    }

	while((dentry=readdir(dirp))!=NULL){
		if(dentry->d_ino==0)
			continue;

		memcpy(filename,dentry->d_name,PATHNAME_SIZE);

		if(strcmp(filename,".")==0||strcmp(filename,"..")==0)
			continue;

		sprintf(dirAbPath,"%s/%s",dirPath,filename);
		
		if(lstat(dirAbPath,&statbuf)==-1){
			fprintf(stderr,"stat error %s\n",dirAbPath);
			closedir(dirp);
			return;
		}
		if(S_ISDIR(statbuf.st_mode)){
			checkDirIsEmpty(dirAbPath);
		}
		else if(S_ISREG(statbuf.st_mode)){
			count++;
		}
	}
    closedir(dirp);

	if(count==0){
		rmdir(dirPath);
	}
	return;
}


//same : ret 0, false : ret 1, fail : ret -1
int cmpFile(char* filepath1,char* filepath2){
    int fd1, fd2;

    if(((fd1=open(filepath1,O_RDONLY))<0)||((fd2=open(filepath2,O_RDONLY))<0)){
        fprintf(stderr,"open error %s or %s\n",filepath1,filepath2);
        return -1;
    }

    char buffer1[BUFFER_SIZE]={0,};
    char buffer2[BUFFER_SIZE]={0,};
    int length1=0;
    int length2=0;
    
    while(1){
        length1=read(fd1,buffer1,BUFFER_SIZE);
        length2=read(fd2,buffer2,BUFFER_SIZE);
        if(length1<0||length2<0){
            fprintf(stderr,"read error\n");
            close(fd1);
            close(fd2);
            return -1;
        }
        else if(length1!=length2){
            close(fd1);
            close(fd2);
            return 1;
        }
        else if(length1==0){
            break;
        }
        else{
            if(strcmp(buffer1,buffer2)==0){
                memset(buffer1,0,BUFFER_SIZE);
                memset(buffer2,0,BUFFER_SIZE);
                continue;
            }
            else{
                close(fd1);
                close(fd2);
                return 1;
            }
        }
    }
    return 0;
}


//success : ret 0, fail : ret -1
int copyFile(char* srcPath, char* desPath){
    int srcfd;
    int desfd;
    int length;
    char buffer[BUFFER_SIZE];
    if((srcfd=open(srcPath,O_RDONLY))<0){
        fprintf(stderr,"open fail %s\n",srcPath);
        return -1;
    }
    if((desfd=open(desPath,O_WRONLY|O_CREAT|O_TRUNC,0777))<0){
        fprintf(stderr,"open fail %s\n",desPath);
        close(srcfd);
        return -1;
    }
    while((length=read(srcfd,buffer,BUFFER_SIZE))>0){
        write(desfd,buffer,length);
        memset(buffer,0,BUFFER_SIZE);
    }
    close(desfd);
    close(srcfd);
    return 0;
}

//success: ret 0, fail :ret -1
int makeDirRepeatly(char* dirPath){
    if(access(dirPath,F_OK)==0){
        return 0;
    }
    char copyDirPath[PATHNAME_SIZE]={0,};
    strcpy(copyDirPath,dirPath);
    char* pathPtr=NULL;

    int length=strlen(copyDirPath);
    if(copyDirPath[length-1]=='/')
        copyDirPath[length-1]='\0';

    for(pathPtr=copyDirPath+1;*pathPtr!='\0';pathPtr++){
        if(*pathPtr=='/'){
            *pathPtr='\0';
            if(access(copyDirPath,F_OK)<0){
                if(mkdir(copyDirPath,0777)<0){
                    fprintf(stderr,"mkdir error %s\n",copyDirPath);
                    return -1;
                }
            }            
            *pathPtr='/';
        }
    }
    if(mkdir(copyDirPath,0777)<0){
        fprintf(stderr,"mkdir error %s\n",copyDirPath);
        return -1;
    }
    return 0;
}

//------------------------------------------------

//get filename from abpath
//success : ret filename, fail : ret NULL
char* getFileNameFromAbPath(char* abPath){
	char tempAbPath[PATHNAME_SIZE]={0,};
	strcpy(tempAbPath,abPath);

	char* prevPtr;
	char* curPtr=strtok(tempAbPath,"/");

	while(curPtr!=NULL){
		prevPtr=curPtr;
		curPtr=strtok(NULL,"/");
	}
	char* retStr=(char*)malloc(strlen(prevPtr)+1);
	strcpy(retStr,prevPtr);
	return retStr;
}

//change ab path to filelogname '/'=>'%'
//success : ret filelogname, faile : ret NULL
char* converToFileLogName(char* abPath){
	char tempAbPath[PATHNAME_SIZE]={0,};
	strcpy(tempAbPath,abPath);

	char* curPtr=strchr(tempAbPath,'/');

	while(curPtr!=NULL){
		//strcpy(curPtr,"%");
		*curPtr='%';
		curPtr=strchr(curPtr+1,'/');
	}

	char* retStr=(char*)malloc(strlen(tempAbPath)+1);
	strcpy(retStr,tempAbPath);

	return retStr;
}

//change filelogname '%'=>'/'
//success : ret abpath, fail : ret NULL
char* convertToAbPath(char* filelogname){
	char tempAbPath[PATHNAME_SIZE]={0,};
	strcpy(tempAbPath,filelogname);

	char* curPtr=strchr(tempAbPath,'%');

	while(curPtr!=NULL){
		//strcpy(curPtr,"/");
		*curPtr='/';
		curPtr=strchr(curPtr+1,'%');
	}

	char* retStr=(char*)malloc(strlen(tempAbPath)+1);
	strcpy(retStr,tempAbPath);

	return retStr;
}

//---------------------------------------------------------


//get backup file path, but get correctly garraunted sub dir path
//success : ret backupFilePath, fail : ret NULL
char* getBackupFilePath(char* originfilepath, char* backupDirPath){
	//dir save queue
    NAME_QUEUE queue;
    name_queueInit(&queue);

    //have to find backupFilePath
	char* backupFilePath;

    DIR* dirp;
    struct dirent* dentry;
    struct stat statbuf;
    char filename[PATHNAME_SIZE]={0,};
    char abPath[PATHNAME_SIZE]={0,};

    if((dirp=opendir(backupDirPath))==NULL){
        fprintf(stderr,"opendir fail %s\n",backupDirPath);
        return NULL;
    }

    while((dentry=readdir(dirp))!=NULL){
        if(dentry->d_ino==0)
            continue;

        //filename : relative path
        memcpy(filename,dentry->d_name,PATHNAME_SIZE);
        if(strcmp(filename,".")==0||strcmp(filename,"..")==0)
			continue;

        //rel path => absolute path
        sprintf(abPath,"%s/%s",backupDirPath,filename);
        
        //get stat info
        if(lstat(abPath,&statbuf)==-1){
            fprintf(stderr,"stat error %s\n",abPath);
            clearname_queue(&queue);
            closedir(dirp);
            return NULL;
        }

        if(S_ISDIR(statbuf.st_mode)){
            enname_queue(&queue,filename);
        }
    }
    
    closedir(dirp);

	//have to find backupFilePath
	backupFilePath=(char*)malloc(PATHNAME_SIZE);

    char* fname=getFileNameFromAbPath(originfilepath);
    if(isNameQueueEmpty(&queue)){
        //getBackupFilePath; backupDirPath+filename;
        sprintf(backupFilePath,"%s/%s",backupDirPath,fname);
    }
    else{
        char* dirName;
        char searchStr[PATHNAME_SIZE]={0,};
        char* partialPathPtr=NULL;
        while((dirName=dename_queue(&queue))!=NULL){
            sprintf(searchStr,"/%s/",dirName);
            free(dirName);
            if((partialPathPtr=strstr(originfilepath,searchStr))!=NULL)
                break;            
        }
        clearname_queue(&queue);

        if(partialPathPtr==NULL){
            // /dirname/backupfile
            sprintf(backupFilePath,"%s/%s",backupDirPath,fname);            
        }
        else{
            // /dirname/.../backupfile
            sprintf(backupFilePath,"%s%s",backupDirPath,partialPathPtr);
        }      
    }
    free(fname);
	return backupFilePath;
}
//ex) .../test/test2/c.txt
    // 1)/home/backup/timeStr/c.txt or
    // 2)/home/backup/timeStr/test2/c.txt or
    // 3)/home/backup/timeStr/test/test2/c.txt

    //if /home/backup/timeStr has no sub dir => 1)
    //2)3), get x, /home/backup/timeStr/x/.../c.txt
    //use x to find partial path
    //strstr(originpath,x) => /x/.../c.txt
    // /home/backup/timeStr+partial path => backupfilepath
    // /home/backup/timeStr+ /x/.../c.txt=> backupfilepath






//also need "get backup logs path" function
//success : ret backuplogs path, fail : ret NULL
char* getBackupLogsPath(char* originfilepath){
    char* logsPath="/home/backup/logs";
    char* filename=converToFileLogName(originfilepath);
    char* retPathName=(char*)malloc(PATHNAME_SIZE);
    sprintf(retPathName,"%s/%s",logsPath,filename);
    free(filename);
    return retPathName;
}

//----------------------------------------------------------

//get timeStr
//success : ret timestr, fail : ret NULL
char* getTimeStr(){
    char* buffer=(char*)malloc(TIMESTR_SIZE);
	time_t ct;
	struct tm tm;
	ct=time(NULL);
	tm=*localtime(&ct);
sprintf(buffer,"%02d%02d%02d%02d%02d%02d",tm.tm_year-100,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
    return buffer;
}

//success : ret new dir path, fail : ret null
char* makeBackupDirPath(char* timeStr){
    char* backupDirPath="/home/backup";
    char* tempPath=(char*)malloc(PATHNAME_SIZE);
    sprintf(tempPath,"%s/%s",backupDirPath,timeStr);
    if(mkdir(tempPath,0777)<0){
        printf("backuptimeDir make failed\n");
        free(tempPath);
        return NULL;
    }
    return tempPath;
}

#endif