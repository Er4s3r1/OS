#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>

int main(){

    printf("0 - Change realization\n");
    printf("1 arg1 arg2 - GCF function\n");
    printf("2 arg - Translation function\n");
    printf("3 - Exit\n");

    int flag_lib = 1;      // номер текущей библиотеки
    void* library_handler; // указатель на начало динамической библиотеки

    library_handler = dlopen("libFirst.so",RTLD_LAZY);

    if (!library_handler){
        // если ошибка, то вывести ее на экран
        fprintf(stderr,"dlopen() error: %s\n", dlerror());
        exit(1); // в случае ошибки закончить работу программу
    };

    int c;
    int x,y;
    long a;

    printf("Enter the key number: ");
    scanf("%d",&c);
    
    while(c != 3){
        switch (c){
        case 0:
            
            dlclose(library_handler); // закрывается текущая библиотека

            if(flag_lib == 1){
                library_handler = dlopen("libSecond.so",RTLD_LAZY);
                flag_lib = 2;
            }
            else{
                library_handler = dlopen("libFirst.so",RTLD_LAZY);
                flag_lib = 1;
            }

            printf("\nRealization changed\n");

            printf("Enter the key number: ");
            scanf("%d",&c);

            break;

        case 1:

            scanf("%d %d",&x,&y);

            int (*gcffunc)(int, int); // указатель на функцию GCF
            char name1[] = "GCF";

            gcffunc = dlsym(library_handler,name1);

            if (!gcffunc){
                fprintf(stderr,"dlopen() error: %s\n", dlerror());
                exit(1);
            };
            
            if(flag_lib == 1) printf("\nGCF realization №1\n");
            else printf("\nGCF realization №2\n");

            printf("GCD(%d, %d) = %d\n",x,y,(*gcffunc)(x, y));;

            printf("\nEnter the key number: ");
            scanf("%d",&c);

            break;

        case 2:
            
            scanf("%ld",&a);

            char* (*tranfunc)(long); // указатель на функцию translation
            char name2[] = "translation";

            if(flag_lib == 1) printf("\nTranslation realization №1\n");
            else printf("\nTranslation realization №2\n");
            tranfunc = dlsym(library_handler,name2);
            char* res = (*tranfunc)(a);
            printf("Translation(%ld) = %s\n",a,res);
            free(res);

            printf("\nEnter the key number: ");
            scanf("%d",&c);

            break;
        case 3:
            break;
        default:
            printf("\nIncorrect Input\n");
            exit(-1);
            break;
        }
    }
    dlclose(library_handler);
    return 0;
}