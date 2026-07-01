#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 600;
#define MAX_TOKEN 100;

char* getField(const char *line,  int index){
    int curr;
    const char *start;
    const char *p;
    int len;
    char *res;

    curr = 0;
    start = line;
    p = line;

    while(*p){
        if (*p == '#') {
            if (curr == index) {
                len = (int)(p - start);
                res = malloc(len + 1);
                strncpy(res, start, len);
                res[len] = '\0';
                return res;
            }
            curr++;
            start = p + 1;
        }
        p++;
    }
    return NULL;
}