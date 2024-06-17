#include "defs.h"
#include "filelog.h"
#include "queue.h"

// originpath/originfile => backup/time/copyfile


///////////////////////////////////impl///////////////////////////////
//success : ret 0, fail : ret -1
int backup_file(char* srcFilePath, char* desDirPath, int cmpflag){
    if(access(desDirPath,F_OK)<0){
        fprintf(stderr,"desDirPath %s not exsist\n",desDirPath);
        return -1;
    }

    if(cmpflag==1){
        char* backupLogsFilePath;
        if((backupLogsFilePath=getBackupLogsPath(srcFilePath))==NULL){
            fprintf(stderr,"getBackupLogsPath from %s fail\n",srcFilePath);
            return -1;
        }

        if(access(backupLogsFilePath,F_OK)<0){
            printf("no filelogs record. this is first backup\n");
        }
        else{
            //open that filelog
            FILELOG* filelog;
            if((filelog=bringFileLog(srcFilePath))==NULL){
                fprintf(stderr,"bringFileLog fail %s\n",srcFilePath);
                return -1;
            }
            
            //find recent timeline in filelog
            char recentTimeStr[TIMESTR_SIZE]={0,};
            strcpy(recentTimeStr,filelog->tailptr->timeStr);

            //get new desDirPath
            char newBackupDirPath[PATHNAME_SIZE]={0,};
            strcpy(newBackupDirPath,desDirPath);
            strncpy(newBackupDirPath+13,recentTimeStr,TIMESTR_SIZE-1);

            //bring recent timeline backupfile path
            char* recentBackupFilePath;
            if((recentBackupFilePath=getBackupFilePath(srcFilePath,newBackupDirPath))==NULL){
                fprintf(stderr,"bring backupfilepath fail %s\n",srcFilePath);
                closeFileLog(filelog);
                return -1;
            }///home/backup/YYMMDDHHMMSS

            //maybe /home/backup/YYMMDDHHMMSS/SOMETHING

            //get compare result
            int result=cmpFile(srcFilePath,recentBackupFilePath);
            if(result<0){
                fprintf(stderr,"cmp fail %s, %s\n",srcFilePath,recentBackupFilePath);
                free(recentBackupFilePath);
                closeFileLog(filelog);
                return -1;
                
            }
            
            if(result==0){
                printf("%s already backuped to %s\n",srcFilePath,recentBackupFilePath);
                free(recentBackupFilePath);
                closeFileLog(filelog);
                return -1;
            }
            else if(result==1){
                printf("not same. proceed...\n");
                free(recentBackupFilePath);
                closeFileLog(filelog);
            }
        }
    }

    char* filename;
    char desAbPath[PATHNAME_SIZE]={0,};

    if((filename=getFileNameFromAbPath(srcFilePath))==NULL){
        fprintf(stderr,"getFileNameFromAbPath failed %s\n",srcFilePath);
        return -1;
    }

    sprintf(desAbPath,"%s/%s",desDirPath,filename);

    if(copyFile(srcFilePath,desAbPath)<0){
        fprintf(stderr,"copyfile err %s to %s\n",srcFilePath,desAbPath);
        free(filename);
        return -1;
    }
    
    //copy over finish
    //lets record to file log
    if(writeFileLog(srcFilePath,desDirPath)<0){
        fprintf(stderr,"writeFileLog fail %s\n",filename);
        free(filename);
        return -1;
    }

    free(filename);
    
    char* timeStr=getFileNameFromAbPath(desDirPath);
        int logfd=initcheck_openbaklog();
        char logbuf[PATHNAME_SIZE]={0,};
			sprintf(logbuf,"%s: %s backuped to %s\n",timeStr,srcFilePath,desAbPath);
			write(logfd,logbuf,PATHNAME_SIZE);
			close(logfd);
            free(timeStr);
    return 0;
}

//success : ret 0, fail : ret -1
int backup_shallow(char* srcDirPath, char* backupDirPath, int cmpflag){
    DIR* dirp;
    struct dirent* dentry;
    struct stat statbuf;
    char filename[PATHNAME_SIZE]={0,};
    char srcAbPath[PATHNAME_SIZE]={0,};

    if((dirp=opendir(srcDirPath))==NULL){
        fprintf(stderr,"open dir fail %s\n",srcDirPath);
        return -1;
    }

    while((dentry=readdir(dirp))!=NULL){
        if(dentry->d_ino==0)
            continue;

        //filename : relative path
        memcpy(filename,dentry->d_name,PATHNAME_SIZE);

        if(strcmp(filename,".")==0||strcmp(filename,"..")==0)
			continue;

        //rel path => absolute path
        sprintf(srcAbPath,"%s/%s",srcDirPath,filename);
        
        if(lstat(srcAbPath,&statbuf)==-1){
            fprintf(stderr,"stat error %s\n",srcAbPath);
            closedir(dirp);
            return -1;
        }

        if(S_ISDIR(statbuf.st_mode)){
            continue;//different at backup_deep();
        }
        else if(S_ISREG(statbuf.st_mode)){
            if(backup_file(srcAbPath,backupDirPath,cmpflag)<0){
                fprintf(stderr,"backup_file fail %s\n",srcAbPath);
            }
        }
    }    

    closedir(dirp);
    return 0;
}

//success : ret 0, fail : ret -1
//no recursive
int backup_deep(char* srcDirPath, char* backupDirPath, int cmpflag){
    DIR* dirp;
    struct dirent* dentry;
    struct stat statbuf;
    char filename[PATHNAME_SIZE]={0,};
    char srcAbPath[PATHNAME_SIZE]={0,};

    NAME_QUEUE queue;
    name_queueInit(&queue);
    enname_queue(&queue,srcDirPath);
    char* remainDirPath;

    NAME_QUEUE backupQueue;
    name_queueInit(&backupQueue);
    enname_queue(&backupQueue,backupDirPath);

    while((remainDirPath=dename_queue(&queue))!=NULL){
        char* tempBackupDirPath=dename_queue(&backupQueue);

        if((dirp=opendir(remainDirPath))==NULL){
            fprintf(stderr,"open dir fail %s\n",remainDirPath);
            free(tempBackupDirPath);
            free(remainDirPath);
            clearname_queue(&queue);
            return -1;
        }

        while((dentry=readdir(dirp))!=NULL){
            if(dentry->d_ino==0)
                continue;

            //filename : relative path
            memcpy(filename,dentry->d_name,PATHNAME_SIZE);

            if(strcmp(filename,".")==0||strcmp(filename,"..")==0)
			    continue;

            //rel path => absolute path
            sprintf(srcAbPath,"%s/%s",remainDirPath,filename);
            
            if(lstat(srcAbPath,&statbuf)==-1){
                fprintf(stderr,"stat error %s\n",srcAbPath);
                free(tempBackupDirPath);
                free(remainDirPath);
                closedir(dirp);
                clearname_queue(&queue);
                return -1;
            }

            if(S_ISDIR(statbuf.st_mode)){
                enname_queue(&queue,srcAbPath);
                char tempPath[PATHNAME_SIZE]={0,};
                sprintf(tempPath,"%s/%s",backupDirPath,filename);
                enname_queue(&backupQueue,tempPath);
            }
            else if(S_ISREG(statbuf.st_mode)){
                if(access(tempBackupDirPath,F_OK)<0){
                    mkdir(tempBackupDirPath,0777);
                }

                if(backup_file(srcAbPath,tempBackupDirPath,cmpflag)<0){
                    fprintf(stderr,"backup_file fail %s\n",srcAbPath);
                }
            }
        }
        free(tempBackupDirPath);
        closedir(dirp);
        free(remainDirPath);
    }

    clearname_queue(&queue);

    return 0;
}
