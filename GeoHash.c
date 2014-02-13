/*
 * GeoHash.c
 *
 * Author: MTD
 *
 */
#include <stdio.h>


const char CHARACTERS[] = {
    // 0   1   2   3   4   5   6   7
      '0','1','2','3','4','5','6','7',
    // 8   9  10  11  12  13  14  15
      '8','9','b','c','d','e','f','g',
    //16  17  18  19  20  21  22  23
      'h','j','k','m','n','p','q','r',
    //24  25  26  27  28  29  30  31
      's','t','u','v','w','x','y','z'
};
char MAP['z'+1];

void initMap() {
    char i;
    for (i = 0; i < 32; i++){
        MAP[(int)CHARACTERS[i]] = i;
        printf("%i %c " , i, CHARACTERS[i]);
    }
}

void p(char* s) {
    printf("%s\n", s);
}
int main(int argc, char **argv) {
    initMap();
    printf("work! %c %c\n",MAP['0'], CHARACTERS[21]);
    printf("kagemand\n");
}
