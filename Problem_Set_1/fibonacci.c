#include <stdio.h>

int main(int argc, char** argv) {

    int hi = 1;
    int lo = 0;
  
    for(int i = 0; i < 10; i++) {
        
        int extra = hi;
        hi = hi + lo;
        lo = extra;
        
        printf("%d\n", lo);
  
    }
    return 0;
}
