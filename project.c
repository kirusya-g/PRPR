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

char* trimLeadingAndFree(char *s) {
    char *start;
    char *res;
    int len;

    if (s == NULL) return NULL;
    start = skipSpaces(s);
    len = (int)strlen(start);
    res = malloc(len + 1);
    strcpy(res, start);
    free(s);
    return res;
}

char* getFieldTrimmed(const char *line, int index) {
    char *res;
    res = getField(line, index);
    return trimLeadingAndFree(res); 
}

char* duplicateString(const char *s) {
    char *res;
    res = malloc(strlen(s) + 1);
    strcpy(res, s);
    return res;
}

int isValidSID(const char *sid) {
    int i;
    if (strlen(sid) != 8) return 0;
    if (strncmp(sid, "SID", 3) != 0) return 0;
    if (sid[3] < 'A' || sid[3] > 'Z') return 0;
    for (i = 4; i < 8; i++) {
        if (sid[i] < '0' || sid[i] > '9') return 0;
    }
    return 1;
}

int isValidGID(const char *gid) {
    int i;
    if (strlen(gid) != 7) return 0;
    if (strncmp(gid, "GID", 3) != 0) return 0;
    if (gid[3] < 'A' || gid[3] > 'Z') return 0;
    for(i = 4; i < 7; i++) {
        if(gid[i] < '0' || gid[i] > '9') return 0;
    }
    return 1;
}

int isValidPID(const char *pid) {
    int i;
    if (strlen(pid) != 6) return 0;
    if (strncmp(pid, "PID", 3) != 0) return 0;
    if (!((pid[3] >= 'a' && pid[3] <= 'z') || (pid[3] >= 'A' && pid[3] <= 'Z'))) return 0;
    for (i = 4; i < 6; i++) {
        if (pid[i] < '0' || pid[i] > '9') return 0;
    }
    return 1;
}

void freeStringArray(char **arr, int size) {
    int i;
    if (arr == NULL) return;
    for (i = 0; i < size; i++) {
        if (arr[i] != NULL) free(arr[i]);
    }
    free(arr);
}

int countNonEmptyLines(FILE *f) {
    char line[MAX_LINE];
    int count;

    count = 0;
    rewind(f);
    while (fgets(line, sizeof(line), f)) {
        trim(line);
        if (strlen(line) > 0) {
            count++;
        }
    }
    return count;
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

            pidR = getField(copy, 1);
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

void v2(char **hracPID, char **hracMeno, char **hracKrajina, char **hracRok, int hracCount,
          char **riesGID, char **riesPID, char **riesSID, char **riesDatum,
          char **riesMin, char **riesSek, int riesCount) {
            int i;
            int j;
            int count;
            int sec;

            for(i = 0; i < hracCount; i++) {
                if (i > 0) printf("\n");

                printf("PID: %s / %s / %s/ \n", hracPID[i], hracRok[i], hracKrajina[i]);
                printf("Identita:  %s\n", hracMeno[i]);
                printf("Vysledok: \n");

                count = 0;
                for(j = 0; j < riesCount && count < 10; j++){
                    if (strcmp(riesPID[j], hracPID[i]) == 0) {
                        sec = atoi(riesMin[j]) * 60 + atoi(riesSek[j]);
                        printf("\t%s / %s / %s / %s / %c / %c / %d\n", riesGID[j], riesPID[j], riesSID[j], riesDatum[j], riesGID[j][3], riesSID[j][3], sec);
                        count ++;
                    }
                }
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

void n(FILE *fS, FILE *fH, FILE *fR, char ***pSudokuSID, char ***pSudokuSol, int *pSudokuCount,
         char ***pHracPID, char ***pHracMeno, char ***pHracKrajina, char ***pHracRok, int *pHracCount,
         char ***pRiesGID, char ***pRiesPID, char ***pRiesSID, char ***pRiesDatum,
         char ***pRiesMin, char ***pRiesSek, int *pRiesCount,
         int *pPolNaplnene) {
            char line[MAX_LINE];
            int count;
            int i;
            char **sudokuSID;
            char **sudokuSol;
            char **hracPID;
            char **hracMeno;
            char **hracKrajina;
            char **hracRok;
            char **riesGID;
            char **riesPID;
            char **riesSID;
            char **riesDatum;
            char **riesMin;
            char **riesSek;

            if (fS == NULL || fH == NULL || fR == NULL) {
                printf("N: Neotvorene txt subory\n");
                return;
            }

            if (*pPolNaplnene) {
                freeStringArray(*pSudokuSID, *pSudokuCount);
                freeStringArray(*pSudokuSol, *pSudokuCount);
                freeStringArray(*pHracPID, *pHracCount);
                freeStringArray(*pHracMeno, *pHracCount);
                freeStringArray(*pHracKrajina, *pHracCount);
                freeStringArray(*pHracRok, *pHracCount);
                freeStringArray(*pRiesGID, *pRiesCount);
                freeStringArray(*pRiesPID, *pRiesCount);
                freeStringArray(*pRiesSID, *pRiesCount);
                freeStringArray(*pRiesDatum, *pRiesCount);
                freeStringArray(*pRiesMin, *pRiesCount);
                freeStringArray(*pRiesSek, *pRiesCount);
            }

            count = countNonEmptyLines(fS);
            sudokuSID = malloc(sizeof(char*) * count);
            sudokuSol = malloc(sizeof(char*) * count);
            rewind(fS);
            i = 0;
            while (i < count && fgets(line, sizeof(line), fS)) {
                trim(line);
                if (strlen(line) == 0) continue;
                sudokuSID[i] = getField(line, 0);
                sudokuSol[i] = getField(line, 1);
                i++;
            }
            *pSudokuSID = sudokuSID;
            *pSudokuSol = sudokuSol;
            *pSudokuCount = count;


            count = countNonEmptyLines(fH);
            hracPID = malloc(sizeof(char*) * count);
            hracMeno = malloc(sizeof(char*) * count);
            hracKrajina = malloc(sizeof(char*) * count);
            hracRok = malloc(sizeof(char*) * count);
            rewind(fH);
            i = 0;
            while (i < count && fgets(line, sizeof(line), fH)) {
                trim(line);
                if (strlen(line) == 0) continue;
                hracPID[i] = getField(line, 0);
                hracMeno[i] = getField(line, 1);
                hracKrajina[i] = getField(line, 2);
                hracRok[i] = getField(line, 3);
                i++;
         }
        *pHracPID = hracPID;
        *pHracMeno = hracMeno;
        *pHracKrajina = hracKrajina;
        *pHracRok = hracRok;
        *pHracCount = count;


        count = countNonEmptyLines(fR);
        riesGID = malloc(sizeof(char*) * count);
        riesPID = malloc(sizeof(char*) * count);
        riesSID = malloc(sizeof(char*) * count);
        riesDatum = malloc(sizeof(char*) * count);
        riesMin = malloc(sizeof(char*) * count);
        riesSek = malloc(sizeof(char*) * count);
        rewind(fR);
        i = 0;
        while(i < count && fgets(line, sizeof(line), fR)) {
            trim(line);
            if (strlen(line) == 0) continue;
            riesGID[i] = getField(line, 0);
            riesPID[i] = getField(line, 1);
            riesSID[i] = getField(line, 2);
            riesDatum[i] = getField(line, 3);
            riesMin[i] = getField(line, 4);
            riesSek[i] = getField(line, 5);
            i++;
        }
        *pRiesGID = riesGID;
        *pRiesPID = riesPID;
        *pRiesSID = riesSID;
        *pRiesDatum = riesDatum;
        *pRiesMin = riesMin;
        *pRiesSek = riesSek;
        *pRiesCount = count;
        *pPolNaplnene = 1;


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