#ifndef FILELOG_H
#define FILELOG_H

#include "defs.h"
#include "path.h"


typedef struct backuptimenode{
    char timeStr[TIMESTR_SIZE];
    struct backuptimenode* next;
}BACKUPTIMENODE;

typedef struct filelog{
    int count;
    char filepath[PATHNAME_SIZE];
    BACKUPTIMENODE dummyhead;
    BACKUPTIMENODE* tailptr;
}FILELOG;
// FILELOG A -> BACKUPTIMENODE -> BACKUPTIMENODE -> ...

BACKUPTIMENODE* makeBACKUPTIMENODE(char* timeStr){
    BACKUPTIMENODE* retNode;
    if((retNode=(BACKUPTIMENODE*)malloc(sizeof(BACKUPTIMENODE)))==NULL){
        return NULL;
    }

    retNode->next=NULL;
    strcpy(retNode->timeStr,timeStr);
    return retNode;
}

//write(append) to filelog file
//success : ret 0, fail : ret -1
int writeFileLog(char* originfilepath,char* backupDirPath){
    int filelogfd;
    char* backupLogsFilePath;
    if((backupLogsFilePath=getBackupLogsPath(originfilepath))==NULL){
        fprintf(stderr,"getBackupLogsPath from %s fail\n",originfilepath);
        return -1;
    }
    
    char timeStr[TIMESTR_SIZE]={0,};
    sscanf(backupDirPath,"/home/backup/%[^'/']/%*s",timeStr);
    

    if((filelogfd=open(backupLogsFilePath,O_WRONLY|O_CREAT|O_APPEND))<0){
        fprintf(stderr,"open fail for %s\n",backupLogsFilePath);
        return -1;
    }

    if(write(filelogfd,timeStr,TIMESTR_SIZE)<=0){
        fprintf(stderr,"something wrong with write into %s\n",backupLogsFilePath);
        close(filelogfd);
        return -1;
    }

    close(filelogfd);
    free(backupLogsFilePath);
    return 0;
}

//bring file to inmem filelog
//success : ret FILELOG*, fail : ret NULL
FILELOG* bringFileLog(char* originfilepath){
    char* backupLogsFilePath;
    if((backupLogsFilePath=getBackupLogsPath(originfilepath))==NULL){
        fprintf(stderr,"getBackupLogsPath from %s fail\n",originfilepath);
        return NULL;
    }

    FILELOG* inmemfilelog=(FILELOG*)malloc(sizeof(FILELOG));
    strcpy(inmemfilelog->filepath,backupLogsFilePath);
    inmemfilelog->tailptr=&(inmemfilelog->dummyhead);
    inmemfilelog->count=0;

    int filelogfd;

    if((filelogfd=open(backupLogsFilePath,O_RDONLY))<0){
        fprintf(stderr,"failed to open %s\n",backupLogsFilePath);
        return NULL;
    }

    int timeStrCount=0;
    char timeBuffer[TIMESTR_SIZE]={0,};
    while(read(filelogfd,timeBuffer,TIMESTR_SIZE)>0){
        
        BACKUPTIMENODE* newNode=makeBACKUPTIMENODE(timeBuffer);
        inmemfilelog->tailptr->next=newNode;
        inmemfilelog->tailptr=newNode;
        timeStrCount++;
    }

    inmemfilelog->count=timeStrCount;
    close(filelogfd);
    free(backupLogsFilePath);
    return inmemfilelog;
}

//show all timelines in inmemfilelog
void showTimeLines(FILELOG* filelog){
    BACKUPTIMENODE* node=(filelog->dummyhead.next);
    int count=0;
    printf("%d. exit\n",count);

    while(node!=NULL){
        printf("%d. %s\n",++count,node->timeStr);
        node=node->next;
    }
}

//get indexth timeStr from inmem filelog
//success : ret timeStr, failed : ret NULL
char* getTimeStrFromLogs(FILELOG* filelog, int index){
    BACKUPTIMENODE* node=(filelog->dummyhead.next);
    int count=0;

    while(node!=NULL){
        ++count;
        if(index==count){
            char* retStr=(char*)malloc(TIMESTR_SIZE);
            strcpy(retStr,node->timeStr);
            return retStr;
        }
        node=node->next;
    }
    return NULL;
}

//remove info in inmem filelog
//success : ret 0, fail : ret -1
int removeBACKUPTIMENODE(FILELOG* filelog, char* timeStr){
    BACKUPTIMENODE* prevNode=&(filelog->dummyhead);
    BACKUPTIMENODE* searchNode=filelog->dummyhead.next;
    while(searchNode!=NULL){
        if(strcmp(searchNode->timeStr,timeStr)==0){
            filelog->count--;
            prevNode->next=searchNode->next;
            free(searchNode);
            if(prevNode->next==NULL)
                filelog->tailptr=prevNode;
            return 0;
        }
        prevNode=searchNode;
        searchNode=searchNode->next;
    }
    printf("there is no %s timelog in %s filelog\n",timeStr,filelog->filepath);
    return -1;
}

//save inmem filelog data to file
//success : ret 0, fail : ret -1
int saveFileLog(FILELOG* inmemfilelog){
    int filelogfd;
    
    if((filelogfd=open(inmemfilelog->filepath,O_WRONLY|O_CREAT|O_TRUNC))<0){
        fprintf(stderr,"faile to open %s\n",inmemfilelog->filepath);
        return -1;
    }

    int timeStrCount=0;
    BACKUPTIMENODE* recordNode=inmemfilelog->dummyhead.next;
    while(recordNode!=NULL){
        timeStrCount++;
        if(write(filelogfd,recordNode->timeStr,TIMESTR_SIZE)<=0){
            printf("weird saving inmemory filelog\n");
            close(filelogfd);
            return -1;
        }
        recordNode=recordNode->next;
    }
    close(filelogfd);

    if(timeStrCount==0){
        if(remove(inmemfilelog->filepath)<0){
            printf("remove error %s\n",inmemfilelog->filepath);
            return -1;
        }
    }
    return 0;
}

//close inmem filelog
//success : ret 0, fail : ret -1
int closeFileLog(FILELOG* filelog){
    BACKUPTIMENODE* headNode=&(filelog->dummyhead);
    BACKUPTIMENODE* tempNode;
    while(headNode->next!=NULL){
        tempNode=headNode->next;
        headNode->next=headNode->next->next;
        free(tempNode);
    }
    free(filelog);
    return 0;
}


#endif