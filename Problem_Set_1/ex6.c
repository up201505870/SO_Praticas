#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

int no_months(char *mon1, char *mon2) {

    int x = 0;
    int y = 0;

    for(int i = 0; i < 12 && x == 0 && y == 0; i++) {
    
        if(strncasecmp(mon1, *(months + i), 3) == 0)
            x = i;
        
        if(strncasecmp(mon2, *(months + i), 3) == 0)
            y = i;
    }

    if(y > x)
        return y - x;
    else if (y == x)
        return 0;
    else
        return 12 - x - y;

}

int main (int argc, char **argv) {

    for(int i = 0; i < 12; i++) {

        printf("%s\n", *(months+i));
    
    }

    printf("%d\n", no_months("Jun", "Jan"));

    return 0;
}