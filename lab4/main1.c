#include <stdio.h>
#include <stdlib.h>
#include "first_real.c"

int main(){
    
    printf("1 arg1 arg2 - GCF function\n");
    printf("2 arg - Translation function\n");
    printf("3 - Exit\n");

    int c;
    int x,y;
    long a;

    printf("Enter the key number: ");
    scanf("%d", &c);

    while(c != 3){
        switch (c){
        case 1:

            scanf("%d %d",&x,&y);

            printf("GCD(%d, %d) = %d\n",x,y,GCF(x,y));

            printf("\nEnter the key number: ");
            scanf("%d",&c);

            break;
        case 2:
            scanf("%ld" ,&a);
            char* binary = translation(a);
            printf("Translation(%ld) = %s\n",a,binary);
            free(binary);
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
}