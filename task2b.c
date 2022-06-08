#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

typedef struct link link;

struct link {
    link *nextVirus;
    virus *vir;
};

// task 0
void PrintHex(unsigned char* buffer, int lenght) {
    for(int i=0; i<lenght; i++)
        fprintf(stdout, "%02X ", (unsigned char)buffer[i]);
    printf("\n");
}

void printVirus(virus* virus, FILE* output){
    fprintf(output, "Virus name: %s\n", virus->virusName);
    fprintf(output, "Virus size: %d\n", virus->SigSize);
    fprintf(output, "signature:\n");
    for(int i=0; i<virus->SigSize; i++)
        fprintf(output, "%02X ", (unsigned char)virus->sig[i]);
    fprintf(output, "\n\n");
}
void printv(virus* virus){
    fprintf(stdout, "Virus name: %s\n", virus->virusName);
    fprintf(stdout, "Virus size: %d\n", virus->SigSize);
    PrintHex(virus->sig, virus->SigSize);
}

//task 1a
virus* readVirus(FILE* file) {
    virus* v = (virus*)calloc(1, sizeof(virus));
    fread(v, 18, 1, file);
    if(v->SigSize != 0)
        v->sig = (unsigned char*)calloc(1, v->SigSize);
    fread(v->sig, v->SigSize, 1, file);
    return v;

}

//task1b
void list_print(link *virus_list, FILE* file){
    virus* virus1 = virus_list->vir;
    printVirus(virus1, file);
    struct link* list = virus_list;
    while(list->nextVirus != NULL)
    {
        list = list->nextVirus;
        virus1 = list->vir;
        printVirus(virus1, file);
    }
}

link* list_append(link* virus_list, virus* data){

    if(virus_list == NULL) {
        virus_list = (link*)calloc(1, sizeof(link));
        virus_list->vir = data;
    }
    else
    {
        struct link* l = NULL;
        l = virus_list;
        while(l->nextVirus != NULL) {
            l = l->nextVirus;
        }
        l->nextVirus = (link*)calloc(1, sizeof(link));
        l->nextVirus->nextVirus = NULL;
        l->nextVirus->vir = data;
    }

    return virus_list;
}

void list_free(link *virus_list){
    link* list = virus_list;
    link* temp = list;
    while(list!=NULL && list->vir != NULL && list->vir->SigSize != 0)
    {
        free(list->vir->sig);
        free(list->vir);
        temp = list;
        list = list->nextVirus;
        free(temp);
    }
    //free(list);
}

// Task1c
void detect_virus(char *buffer, unsigned int size, link *virus_list){
    link* l = NULL;
    int index = 0;
    bool isVirus = true;
    for( int i=index; i<size; i++){
        l = virus_list;
        for ( int j=0; j<l->vir->SigSize; j++)
        {
            if(memcmp(&buffer[index], &(l->vir->sig[j]), 1) == 0){
                index++;
            } else{
                if(l->nextVirus != NULL) {
                    j=-1;
                    l = l->nextVirus;
                    index = i;
                }
                else{
                    j=(int)l->vir->sig;
                    index = i+1;
                    isVirus = false;
                }
            }
        }
        if(isVirus)
        {
            fprintf(stdout, "Starting byte: %d\n", i);
            fprintf(stdout, "Virus name: %s\n", l->vir->virusName);
            fprintf(stdout, "Virus size: %d\n", l->vir->SigSize);
            printf("\n");
            i=index;
            isVirus = false;
        }
        isVirus = true;
    }
}

int min( int a, int b ){
    if(a > b)
        return b;
    return a;
}

//Task2b
void kill_virus(char *fileName, int signitureOffset, int signitureSize){
    FILE * infected = fopen(fileName, "rb+");
    char byte[signitureSize];
    for (int i=0; i<signitureSize; i++)
    {
        byte[i] = '\x90';
    }
    fseek(infected, signitureOffset, SEEK_CUR);
    fwrite(byte, 1, signitureSize, infected);
    fclose(infected);
}

int main(int argc, char **argv) {
    char index[256];
    FILE * test = fopen("test", "w");
    FILE * file;
    FILE * detect;
    link* list = NULL;

    while(1){
        printf("%s", "1) Load signatures\n2) Print signatures\n3) Detect viruses\n4) Fix file\n5) Quit\n");
        scanf("%s", index);
        int t = atoi(index);
        if( t == 1 ){
            fprintf(stdout, "Enter signature file name: ");
            char arr[256];
            scanf("%s", arr);
            file = fopen(arr, "r");
            virus* virus1 = readVirus(file);
            while(virus1 != NULL && virus1->SigSize != 0) {
                list = list_append(list, virus1);
                virus1 = readVirus(file);

            }
            free(virus1);

        }
        else if( t == 2 ){
            if(file != NULL){
                list_print(list, test);
            }
        }
        else if( t == 3 ){
            fprintf(stdout, "Enter detect file name: ");
            char arr[256];
            scanf("%s", arr);
            detect = fopen(arr, "r");
            char buffer[10000];
            fread(buffer, 10000, 1, detect);
            fseek(detect, 0L, SEEK_END);
            int size = ftell(detect);
            fseek(detect, 0L, SEEK_END);
            detect_virus(buffer, min(size, 10000), list);
        }
	else if ( t == 4 ) {
            char *name = argv[1];
            printf("%s\n", name);
            fprintf(stdout, "Enter the stating byte location: ");
            char bytearr[256];
            scanf("%s", bytearr);
            int location = atoi(bytearr);
            fprintf(stdout, "Enter the size of the virus signature: ");
            char sizearr[256];
            scanf("%s", sizearr);
            int size = atoi(sizearr);
            kill_virus(name, location, size);
        }
        else if( t == 5 ){
            fclose(file);
            fclose(test);
            fclose(detect);
            list_free(list);
            exit(0);
        }
        printf("\n");
    }
}
