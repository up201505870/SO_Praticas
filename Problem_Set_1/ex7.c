#include <stdio.h>

int main() {


    int x = 256;
    
    if( *( (char *) &x ) == 0)
        printf("ISA of type Little Endian\n");
    else
        printf("ISA of type Big Endian\n");
        
    return 0;

}
