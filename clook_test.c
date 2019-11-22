#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE * fp; 
FILE *ptr; 
char name [FILENAME_MAX];

void testOperations()
{
	int total = 10;
	char readString [50];
	int i = 0;

	fp = fopen("/test/bigfile.txt", "w");
	i =0;
	for(; i <1000 ; i++)
	{
		fprintf(fp, "testing line %d\n", i);
	}
	fclose (fp);

	fp = fopen("/test/bigfile.txt", "r");
	fscanf(fp, "%s", readString);
	fclose(fp);

	i=0;
	for(; i<total; i++)
	{
		snprintf(name, sizeof(name), "/test/%d.txt", i);
		fp = fopen( name, "w");
		fclose (fp);
	}

	i =0;
	for (; i<total; i++)
	{
		snprintf(name, sizeof(name), "/test/%d.txt", i);
		fp = fopen(name, "a");
		fprintf(fp, "hooking testing %d", i);
		fclose (fp);
	}

	i =0;
	for (; i<total ; i++)
	{
		snprintf(name, sizeof(name), "/test/%d.txt", i);
		fp = fopen(name, "r");
		fscanf(fp, "%s", readString);
		fclose (fp);
	}

	i=0;
	for(; i<total ; i++)
	{
		snprintf(name, sizeof(name), "/test/%d.txt", i);
		remove(name);
	}

	fp = fopen("/test/hello.c", "r");
	fscanf(fp, "%s", readString);
	fclose (fp);
}

int main()
{
	testOperations();
}
