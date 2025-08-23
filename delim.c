// C program to test the contiguous delimiter case in strsep() function
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main() {
    // Write C code here
    char str[] = "Hello ,, past delimiter";
    char* raw_str = (char*)malloc(sizeof(char) * strlen(str));
    strcpy(raw_str, str);
    char *token = strsep(&raw_str, ",");
    while( token != NULL ) {
        printf("%s\n", token);
        token = strsep(&raw_str, ",");
    }
    return 0;
}
