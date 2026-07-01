#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 600
#define MAX_TOKEN 100

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

void v1(FILE *fH, FILE *fR) {
    char lineH[MAX_LINE];
    char lineR[MAX_LINE];
    int first;
    char *pid;
    char *name;
    char *krajina;
    char *rok;
    char copy[MAX_LINE];
    char *pidR;
    char *pTrim;
    int count;

    first = 1;

    rewind(fH);

    while (fgets(lineH, sizeof(lineH), fH)) {
        trim(lineH);
        if (strlen(lineH) == 0) {
            continue;
        }

        pid = getField(lineH, 0);
        name = getField(lineH, 1);
        krajina = getField(lineH, 2);
        rok = getField(lineH, 3);

        if (!first) printf("\n");

        first = 0;

        printf("Identifikator: %s\n", pid ? pid : " ");
        printf("Meno a priezvisko: %s\n", name ? name : " ");
        printf("Krajina: %s\n", krajina ? krajina : " ");
        printf("Rok: %s\n", rok ? rok : " ");
        printf("Riesenia: \n");

        rewind(fR);
        count = 0;
        while( count < 10 && fgets(lineR, sizeof(lineR), fR)) {
            srtncpy(copy, lineR, MAX_LINE - 1);
            copy[MAX_LINE - 1] = '\0';
            trim(copy);
            if (strlen(copy) == 0) continue;

            pidR = getField(copy, 0);
            if (pidR) {
                pTrim = skipSpaces(pidR);
                if (pid && strcmp(pTrim, pid) == 0) {
                    printf("\t%s\n", copy);
                    count++;
                }
                free(pidR);
            }
        }
        if (pid) free(pid);
        if (name) free(name);
        if (krajina) free(krajina);
        if (rok) free(rok);
    }
}

