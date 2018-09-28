#include "procs.h"

int main()
{
	char *temp = "nibba";
	node *tempData = (node *)malloc(sizeof(node));
    tempData->word = (char *)malloc(sizeof(char) * strlen(temp) + 1);
    strcpy(tempData->word, temp);
	
        node* x = (node*) initializeSHM(sizeof(node));
	char* w = initializeSHM(sizeof(char) * strlen(temp) + 1);
	int totalMapsOrExtra = 1;

	strcpy(w, temp);
	memcpy(x,tempData, sizeof(node));

	x->word = w;

	printf("%s\n", x->word);
	fflush(stdout);

	

        //Doing doing this in here because we have to use exec
	int i;
	pid_t pid;
        for ( i = 0; i < totalMapsOrExtra; i++ )
        {
            	pid = fork();
		if ( pid == 0 ){
			printf("child created\n");
			temp = "child";
			memcpy(x->word, temp, strlen(temp)+1);
			printf("%s\n", x->word);
		} else {
			//wait(pid); //uncommenting->parent prints first with original temp string
			printf("%s\n", x->word);
			return 0;
		}
        }

	
}
