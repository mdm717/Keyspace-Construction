#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<dirent.h>
#include<errno.h>


typedef struct node { //Initializes node struct for the linked list

struct node* next;
char* tk;
int freq;
char* fileName;

} node;

char* outputFileName;
node* head;

void lowerCase(char* charText)
{
	int i;
        for(i=0; i<strlen(charText); i++) //converts characters to lowercase
        {
        	if(((int)charText[i] >= 65) && ((int)charText[i] <= 90))
                {
                	charText[i] = charText[i] +  32;
                }
        }
}

void alphaNum(char* charText)
{
    	int i;
        for(i=0; i<strlen(charText); i++) //removes nonalphanumeric characters
        {
            if(isalpha(charText[i]) || isdigit(charText[i]))
            {
                continue;
            }
            else
            {
                charText[i] = ' ';
            }
        }
}

void freeList(node* tmp)
{
    	if (tmp)
    	{
        	node* tofree = tmp;
        	while(tofree)
        	{
            		node* tmp2 = tofree;
            		tofree = tofree->next;
            		free(tmp2);
        	}
    	}
}

unsigned long long int fileSize(char* fileName) //finds the size of the file being read to help dynamically allocate memory for
{                                               //the buffer string

    	FILE* fin = fopen(fileName, "r");
    	fseek(fin, 0, SEEK_END);

    	int size = ftell(fin);

    	fseek(fin, 0, SEEK_SET);
    	fclose(fin);
    	return size;
}

char* removeNums(char* word) //removes numbers from file words
{
    	int i = 0;
    	int cnt = 0;
    	char* newWord;

    	for (i = 0; i < strlen(word); i++)
    	{
        	if (isdigit(word[i]) == 1)
        	{
            		cnt++;
        	}
        	else
        	{
            		break;
        	}
    	}

    	if (cnt == strlen(word))
    	{
        	return word;
    	}
        newWord = (char*)malloc(sizeof(word)-cnt);
        memset(newWord, '\0', strlen(word)-cnt);
        int newLength = strlen(word)-cnt+1;
        strncpy(newWord, word+cnt, newLength);
        return newWord;

}

char* takeInput(char* fName) //takes the input from the file and copies it to a single string buffer
{
    	FILE* fin;
    	fin = fopen(fName,"r");
	if(fin == NULL)
        {
                printf("Error, File Does Not Exist.");
                return NULL;
        }
    	unsigned long long int filesize = fileSize(fName) + 1;
    	char* buffer = (char*)malloc(sizeof(char) * filesize);

    	memset(buffer, '\0', filesize);

    	fread(buffer, sizeof(char), filesize, fin);
    	fclose(fin);
    	return buffer;
}

void printList(node* tmp)
{
    	while (tmp != NULL)
    	{
        	printf("%s \t\t file: %s \t\t freq: %d\n", tmp->tk, tmp->fileName, tmp->freq);
		tmp = tmp->next;
    	}
}

void sortInsert(node* newNode) //inserts the node into the correct sorted position in the list
{
    	node* curr;
    	if (head == NULL || strcmp(head->tk, newNode->tk) >= 0)
    	{
        	newNode->next = head;
        	head = newNode;
    	}
    	else
    	{
        	curr = head;
        	while (curr->next != NULL && strcmp(curr->next->tk, newNode->tk) < 0)
        	{
            		curr = curr->next;
        	}
        	newNode->next = curr->next;
        	curr->next = newNode;
    	}
}

void insert(char* word, char* fileName) //inserts a node into the linked list
{
	if(!(isdigit(word[0])))
	{
		node* newNode = (node*)malloc(sizeof(node));
		newNode->tk = word;
		newNode->fileName = fileName;
		sortInsert(newNode);
	}
}

void storeFreq(node* tmp) //stores the frequency
{
	node* temptwo = tmp;
	for(tmp=head; tmp != NULL; tmp=tmp->next)
	{
		int count = 0;
		char* word = tmp->tk;
		char* file = tmp->fileName;
		for(temptwo=head; temptwo != NULL; temptwo=temptwo->next)
		{
			if(strcmp(temptwo->tk, word) == 0 && strcmp(temptwo->fileName, file) == 0)
			{
				count++;
			}
		}
		tmp->freq = count;
	}	
}

void removeDoops(node* tmp) //removes duplicate nodes in the linked list
{
    	node* curr = tmp;
	node* after = (node*)malloc(sizeof(node));
	if(curr != NULL)
	{
    		while (curr->next != NULL)
    		{
    		    	if (strcmp(curr->tk, curr->next->tk) == 0 && strcmp(curr->fileName, curr->next->fileName) == 0)
    		    	{
    		    	    	after = curr->next->next;
    		    	    	free(curr->next);
    		    		curr->next = after;
    		    	}
			else
    		    	{
				curr = curr->next;
        		}
    		}
	}
}

node* mergeLists(node* L1, node* L2) //Merges two linked lists into one
{
    	node* finalList = NULL;

    	if (L1 == NULL){return(L2);}
    	else if(L2 == NULL){return(L1);}

    	if (strcmp(L1->tk, L2->tk) <= 0)
    	{
    	    finalList = L1;
    	    finalList->next = mergeLists(L1->next, L2);
    	}
    	else
    	{
    	    finalList = L2;
    	    finalList->next = mergeLists(L1, L2->next);
    	}
    	return (finalList);
}

node* createList(char* fileName) //Creates the linked list
{
        char* charText = takeInput(fileName);
        lowerCase(charText); //turn all chars to lowercase
        alphaNum(charText); //remove alphanumeric characters

        char *token = strtok(charText, " ");

        while(token)
            {
                if (isdigit(token[0]))
                {
                    token = removeNums(token);
                }
                insert(token, fileName);
                token = strtok(NULL, " ");
            }
        return head;
}

node* directories(char* fileName, node* result){
	DIR *din = opendir(fileName); //attempts to open a directory
	char currdirec[9999];
	if(errno == 2)
	{
		printf("File or Directory Does Not Exist.\n");
		errno = 0;
		return 0;
	}
	if(errno == 20) //if this returns ENOTDIR, fileName is a file
	{
		node* list = createList(fileName); //create a list of the words
		head = NULL;
		head = mergeLists(result, list); //merge the created list with the current list of other file word contents
		errno = 0; //reset errno to 0
	}	
	else //if opendir is successful
	{
		struct dirent *dir;
		getcwd(currdirec, sizeof(currdirec)); //get the current working directory
		char* temp = currdirec; //save the path in a temp char*
                strcat(currdirec, "/"); 
		strcat(currdirec, fileName); //add fileName to the end of the current file path with a /, making it a new file path
		chdir(currdirec); //change directories to the new file path		
                while((dir = readdir(din)) != NULL) //while there are contents are in the directory
                {
                        if(dir->d_type == DT_REG) //if dir is a file, pass the name of the file recursively along with the current list of words
                        {
				directories(dir->d_name, result);
                        }
			else if(dir->d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) //else if dir is another directory, pass the name of the directory recursively along with the current list of words
			{
				directories(dir->d_name, result);
				chdir(currdirec); //return to previous working directory
			}
                }
		chdir(temp); //return to previous working directory
	}
return result;
}


char* checkExist(char* dirName, char* check) //this should check if the file exists and will run until the new file name is not already in the directory, it should return the new legal file name
{ 
	char new[9999];
	char answer[9999];
	DIR *din = opendir(dirName);
        struct dirent* dir;
        while((dir = readdir(din)) != NULL) //run through this directory to check if argv[1] file already exists in the directory
        {
                if(strcmp(dir->d_name, check) == 0) //if dir->d_name is equal to the argv[1] file do this
                {
                        printf("%s already exists.  Would you like to overwrite it? (y/n)\n", check);
                        while(strcmp(answer, "y") != 0) //ask user for input until the type "y"
                        {
                                scanf("%s", answer);
                                lowerCase(answer);
                                if(strcmp(answer, "n") == 0) //however, if they do not wish to overwrite the file break from this loop
                                {
                                        break;
                                }
                        }
                        if(strcmp(answer, "n") == 0) //since they do not want to overwrite file, have user enter a new file name that does not exist
                        {
                                printf("What would you like to rename the file? (With file type '.xxx')\n");
                                scanf("%s", new);
                                check = new;
				checkExist(dirName, new);	
                        }
			
                }
        }
	return check;
}

void swap(node* a, node* b)
{
    	char* temp = a->tk;
    	int freq = a->freq;
    	char* fileName = a->fileName;

    	a->tk = b->tk;
    	a->freq = b->freq;
    	a->fileName = b->fileName;

    	b->tk = temp;
    	b->freq = freq;
    	b->fileName = fileName;
}

node* sortFreq(node* list)
{
	if(list == NULL)
	{
		return list;
	}
	int boo;
	node* n1;
	node* n2 = NULL;
	
	do
	{
		boo = 0;
        	n1 = list;

        	while (n1->next != n2)
        	{
        	    	if (strcmp(n1->tk, n1->next->tk) == 0 && n1->freq < n1->next->freq)
			{
                		swap(n1, n1->next);
                		boo = 1;
            		}
            		n1 = n1->next;
        	}
        	n2 = n1;
    	}
    while (boo);
    return list;
}

node* sortFile (node* list)
{
    	int boo;
    	node* n1;
    	node* n2 = NULL;

	if(list == NULL)
	{
		return list;
	}
    	do
    	{
    		boo = 0;
    		n1 = list;
	
	        while (n1->next != n2)
	        {
            		if (strcmp(n1->tk, n1->next->tk) == 0 && n1->freq == n1->next->freq &&
                	strcmp(n1->fileName, n1->next->fileName) > 0)
            		{
                		swap(n1, n1->next);
                		boo = 1;
            		}
            		n1 = n1->next;
        	}	
        	n2 = n1;
    	} while (boo);
    	return list;
}

void outputFile(char* outputFileName, node* result) //creates the output file from the created linked list of words in every file
{
	FILE *fout = fopen(outputFileName, "w");
	result = head;
        node* curr;
        node* prev = head;    
        fprintf(fout, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<fileIndex>\n");
        for(result=head; result!=NULL; result=result->next)
        {
                char* tempWord = result->tk;
                int count = 0;     
                if(strcmp(result->tk, prev->tk) == 0)
                {
                        fprintf(fout, "\t<word text =\"%s\">\n", result->tk);
                        count++;
                }
                for(curr=prev; curr!=NULL; curr=curr->next)
                {
                        if(strcmp(curr->tk, tempWord) == 0)
                        {
                                fprintf(fout, "\t\t<file name=\"%s\"> %d </file>\n", curr->fileName, curr->freq);
                                if(prev->next != NULL)
                                {
                                        prev = prev->next;
                                }
				else if(prev == NULL)
				{
					break;
				}
                                count++;
                        }
                }
                if(count != 0)
                {
                        fprintf(fout, "\t</word>\n");
                }
        }
                fprintf(fout, "</fileIndex>");
}

int main(int argc, char* argv[]){
	head = NULL;
	node* result = NULL;
	char current[9999];
	if(argc != 3)
	{
		printf("Wrong Number of Arguments.");
		return 0;
	}
	getcwd(current, sizeof(current)); //directory invertedIndex.c is being ran from
	result = directories(argv[2], result);
	chdir(current); //return to the directory invertedIndex.c is being ran from
	outputFileName = argv[1];
	outputFileName = checkExist(current, outputFileName); 
	storeFreq(head);
	sortFile(head);
	sortFreq(head);
	removeDoops(head);
	result = head;
	outputFile(outputFileName, result);
        freeList(head);
return 0;
}


