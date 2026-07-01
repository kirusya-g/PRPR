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

void trim(char *s){
    int len;
    len = (int)strlen(s);
    while (len > 0 && (s[len-1] == '\n' || s[len-1] == '\r')){
        s[len-1] = '\0';
        len--;
    }
}

char* skipSpaces(char *s){
    while (*s == ' ') {
        s++;
    }
    return s;
}

int openFiles(FILE **fS, FILE **fH, FILE **fR){
    FILE *s;
    FILE *h;
    FILE *r;

    if (*fS != NULL && *fH != NULL && *fR != NULL) {
        return 1;
    }

    s = fopen("Sudoku.txt", "r");
    h = fopen("RegisterHracov.txt", "r");
    r = fopen("RegisterRieseni.txt", "r");

    if (s == NULL || h == NULL || r == NULL) {
        if (s) fclose(s);
        if (h) fclose(h);
        if (r) fclose(r);
        return 0;
    }

    *fS = s;
    *fH = h;
    *fR = r;    
    return 1;
}

