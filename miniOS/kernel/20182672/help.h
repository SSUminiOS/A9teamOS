#include "defs.h"

void help(char* opt);
void help_backup();

void help(char* opt){
	printf("Usage: ");
	if(opt==NULL){
		printf("\n> ");
		help_backup();
		printf("help [command] : show commands for program\n");
	}
	else if(strcmp(opt,"backup")==0){
		help_backup();
	}
}

void help_backup(){
	printf("backup <path> [option]... : backup file if <path> is file\n");
	printf("  -d : backup files in directory if <path> is directory\n");
	printf("  -r : backup files in directory recursive if <path> is directory\n");
	printf("  -y : backup file although alread backuped\n");
}
