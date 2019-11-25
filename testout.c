#include <stdlib.h>
#include <stdio.h>

/* Opens 2 file pointers, one to an input file and one to an
output file. In order to create a lot of traffic for the I/O
scheduler, the for loop executes 20 times, each time randomly
choosing to generate a read or write */
int main(){
	FILE *fptr;
	FILE *fptr2;
	char str[500];
	fptr = fopen("output.txt", "w");
	fptr2 = fopen("testinput.txt", "r");
	int i;
	for(i = 0; i < 20; i++){
		int r = rand() % 2;
		if(r == 0){
			fprintf(fptr, "output\n");
		}
		else{
			fscanf(fptr2, "%s", str);
		}
	}
	fclose(fptr);
	fclose(fptr2);
	return 0;
}
