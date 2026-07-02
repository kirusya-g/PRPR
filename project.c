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
            strncpy(copy, lineR, MAX_LINE - 1);
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

void doV(FILE **fS, FILE **fH, FILE **fR, int volba, int polNaplnene, int lzNaplnene) {
    if (volba == 1) {
        if(!openFiles(fS, fH, fR)) {
            printf("V1: Neotvorene txt subory\n");
            return;
        }
        v1(*fH, *fR);
    } else if (volba == 2) {
        if (!polNaplnene) {
            printf("V2: Nenanplnene polia\n");
        }
    } else {
        printf("V: Nespravna volba vypisu\n");
    }
}

int correctSID(const char *sid) {
    int i;
    if (strlen(sid) != 8) return 0;
    if (strncmp(sid, "SID", 3) != 0) return 0;
    if (sid[3] < 'A' || sid[3] > 'Z') return 0;
    for (i = 4; i < 8; i++) {
        if (sid[i] < '0' || sid[i] > '9') return 0;
    }
    return 1;
}

void h (FILE **fS, FILE **fH, FILE **fR, const char *sidInput) {
    char sid[MAX_TOKEN];
    FILE *out;
    char line[MAX_LINE];
    char copy[MAX_LINE];
    char *sidLine;
    char *sTrim;

    strncpy(sid, sidInput, MAX_TOKEN - 1);
    sid[MAX_TOKEN - 1] = '\0';
    trim(sid);

    if (!correctSID(sid)) {
        printf("H: Nespravny vstup\n");
        return;
    }

    if (*fS == NULL || *fH == NULL || *fR == NULL) {
        printf("H: Neotvorene txt subory\n");
        return;
    }

    out = fopen("Vystup_H.txt", "w");
    if (!out) {
        printf("H: Neotvoreny subor Vystup_H\n");
        return;
    }

    rewind(*fR);
    while (fgets(line, sizeof(line), *fR)) {
        strncpy(copy, line, MAX_LINE - 1);
        copy[MAX_LINE - 1] = '\0';
        trim(copy);
        if (strlen(copy) == 0) continue;

        sidLine = getField(copy, 2);
        if (sidLine) {
            sTrim = skipSpaces(sidLine);
            if (strcmp(sTrim, sid) == 0) {
                fprintf(out, "%s\n", copy);
        }
        free(sidLine);
        }
    }
    fclose(out);
    printf("H: Uspesne vytvoreny subor Vystup_H.txt\n");
}

int main(void) {
    FILE *fSudoku;
    FILE *fHracov;
    FILE *fRieseni;
    int polNaplnene;
    int lzNaplnene;
    char cmdLine[MAX_LINE];
    char c;
    int volba;
    char sidInput[MAX_LINE];

    fSudoku = NULL;
    fHracov = NULL;
    fRieseni = NULL;
    polNaplnene = 0;
    lzNaplnene = 0;

    while (fgets(cmdLine, sizeof(cmdLine), stdin)) {
        trim(cmdLine);
        if (strlen(cmdLine) == 0) continue;
    
        c = cmdLine[0];
        
        if (c == 'v') {
            volba  = -1;
            sscanf(cmdLine + 1, "%d", &volba);
            doV(&fSudoku, &fHracov, &fRieseni, volba, polNaplnene, lzNaplnene);
        } else if (c == 'h') {
            if (fgets(sidInput, sizeof(sidInput), stdin)) {
                h(&fSudoku, &fHracov, &fRieseni, sidInput);
            }
        } else if (c == 'k') {
            if (fSudoku) fclose(fSudoku);
            if (fHracov) fclose(fHracov);
            if (fRieseni) fclose(fRieseni);
            break;
        }
        
    }
    return 0;
}