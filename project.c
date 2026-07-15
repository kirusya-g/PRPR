#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 600
#define MAX_TOKEN 100

typedef struct Vysledok {
    char SID[10];
    char NarHry;
    char GID[10];
    char NarSut;
    char DatHry[12];
    int Trvanie;
    struct Vysledok *next;
} Vysledok;

typedef struct Hrac {
    char PID[10];
    char Identita[100];
    char Krajina[100];
    int RokNar;
    Vysledok *vysledky;
    struct Hrac *next;
} Hrac;

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

void v3(Hrac *zoznam){
    Hrac *h;
    Vysledok *v;
    int first;

    first = 1;
    h = zoznam;

    while (h != NULL){
        if (!first)
            printf("\n");
        first = 0;

        printf("PID: %s\n", h ->PID);
        printf("Identita: %s\n", h->Identita);
        printf("Krajina: %s\n", h->Krajina);
        if (h->RokNar != 0){
            printf("RokNar: %d\n", h->RokNar);
        } else {
            printf("RokNar: \n");
        }

        v = h->vysledky;
        if(v == NULL){
            printf("Vysledky: \n");
        } else {
            printf("Vysledky: %s / %c / %s / %c / %s / %d\n",
                    v->SID, v->NarHry, v->GID, v->NarSut, v->DatHry, v->Trvanie);
                v = v->next;
                while (v != NULL){
                    printf("%s / %c / %s / %c / %s / %d\n",
                    v->SID, v->NarHry, v->GID, v->NarSut, v->DatHry, v->Trvanie);
                v = v->next;
                }
        }
        h = h->next;
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

void q(char ***pRiesGID, char ***pRiesPID, char ***pRiesSID,
         char ***pRiesDatum, char ***pRiesMin, char ***pRiesSek,
         int *pRiesCount, int polNaplnene){
            int y;
            int pos;
            char bufGID[MAX_TOKEN];
            char bufPID[MAX_TOKEN];
            char bufSID[MAX_TOKEN];
            int datum;
            int min;
            int sek;
            int parsed;
            char **newGID;
            char **newSID;
            char **newPID;
            char **newDatum;
            char **newMin;
            char **newSek;
            int i;
            char datumStr[20];
            char minStr[20];
            char sekStr[20];

            if (!polNaplnene){
                printf("Q: Polia nie su vytvorene\n");
                return;
            }

            scanf("%d", &y);
            while(getchar() != '\n') {}

            for(;;){
                scanf("%99s", bufGID);
                while(getchar() != '\n') {}
                if (isValidGID(bufGID)) break;
                printf("Q: nespravny format vstupu, zadaj znova: ");
            }

            for(;;){
                scanf("%99s", bufPID);
                while(getchar() != '\n') {}
                if (isValidPID(bufPID)) break;
                printf("Q: nespravny format vstupu, zadaj znova: ");
            }

            for(;;){
                scanf("%99s", bufSID);
                while(getchar() != '\n') {}
                if (isValidSID(bufSID)) break;
                printf("Q: nespravny format vstupu, zadaj znova: ");
            }

            for (;;) {
                parsed = scanf("%d %d %d", &datum, &min, &sek);
                while (getchar() != '\n') {}
                if (parsed == 3) break;
                printf("Q: nespravny format vstupu, zadaj znova:");
            }

            sprintf(datumStr, "%d", datum);
            sprintf(minStr, "%d", min);
            sprintf(sekStr, "%d", sek);

            pos  = y - 1;
            if (pos < 0) pos = 0;
            if (pos > *pRiesCount) pos = *pRiesCount;

            newGID = malloc(sizeof(char*) * (*pRiesCount + 1));
            newPID = malloc(sizeof(char*) * (*pRiesCount + 1));
            newSID = malloc(sizeof(char*) * (*pRiesCount + 1));
            newDatum = malloc(sizeof(char*) * (*pRiesCount + 1));
            newMin = malloc(sizeof(char*) * (*pRiesCount + 1));
            newSek = malloc(sizeof(char*) * (*pRiesCount + 1));

            for (i = 0; i < pos; i++) {
                newGID[i] = (*pRiesGID)[i];
                newPID[i] = (*pRiesPID)[i];
                newSID[i] = (*pRiesSID)[i];
                newDatum[i] = (*pRiesDatum)[i];
                newMin[i] = (*pRiesMin)[i];
                newSek[i] = (*pRiesSek)[i];
            }

            newGID[pos] = duplicateString(bufGID);
            newPID[pos] = duplicateString(bufPID);
            newSID[pos] = duplicateString(bufSID);
            newDatum[pos] = duplicateString(datumStr);
            newMin[pos] = duplicateString(minStr);
            newSek[pos] = duplicateString(sekStr);

            for (i = pos; i < *pRiesCount; i++) {
                newGID[i+1] = (*pRiesGID)[i];
                newPID[i+1] = (*pRiesPID)[i];
                newSID[i+1] = (*pRiesSID)[i];
                newDatum[i+1] = (*pRiesDatum)[i];
                newMin[i+1] = (*pRiesMin)[i];
                newSek[i+1] = (*pRiesSek)[i];
            }

            free(*pRiesGID);
            free(*pRiesPID);
            free(*pRiesSID);
            free(*pRiesDatum);
            free(*pRiesMin);
            free(*pRiesSek);

            *pRiesGID = newGID;
            *pRiesPID = newPID;
            *pRiesSID = newSID;
            *pRiesDatum = newDatum;
            *pRiesMin = newMin;
            *pRiesSek = newSek;
            (*pRiesCount)++;

}

void w(char ***pRiesGID, char ***pRiesPID, char ***pRiesSID,
         char ***pRiesDatum, char ***pRiesMin, char ***pRiesSek,
         int *pRiesCount, int polNaplnene){
            char pidInput[MAX_TOKEN];
            int i;
            int newCount;
            int deleted;
            char **newGID;
            char **newPID;
            char **newSID;
            char **newDatum;
            char **newMin;
            char **newSek;

            if (!polNaplnene){
                printf("W: POlia nie su vytvorene.\n");
                return;
            }

            scanf("%99s", pidInput);
            while (getchar() != '\n'){}

            if (*pRiesCount == 0){
                printf("W: Vymazalo sa : 0 zaznamov!\n");
                return;
            }

            newGID = malloc(sizeof(char*) * (*pRiesCount));
            newPID = malloc(sizeof(char*) * (*pRiesCount));
            newSID = malloc(sizeof(char*) * (*pRiesCount));
            newDatum = malloc(sizeof(char*) * (*pRiesCount));
            newSek = malloc(sizeof(char*) * (*pRiesCount));
            newMin = malloc(sizeof(char*) * (*pRiesCount));

            newCount = 0;
            deleted = 0;

            for (i = 0; i < *pRiesCount; i++){
                if (strcmp((*pRiesPID)[i],pidInput) == 0){
                    free((*pRiesGID)[i]);
                    free((*pRiesPID)[i]);
                    free((*pRiesSID)[i]);
                    free((*pRiesDatum)[i]);
                    free((*pRiesMin)[i]);
                    free((*pRiesSek)[i]);
                    deleted++;
                } else {
                    newGID[newCount] = (*pRiesGID)[i];
                    newPID[newCount] = (*pRiesPID)[i];
                    newSID[newCount] = (*pRiesSID)[i];
                    newDatum[newCount] = (*pRiesDatum)[i];
                    newMin[newCount] = (*pRiesMin)[i];
                    newSek[newCount] = (*pRiesSek)[i];
                    newCount++;
                }
            }

            free(*pRiesGID);
            free(*pRiesPID);
            free(*pRiesSID);
            free(*pRiesDatum);
            free(*pRiesMin);
            free(*pRiesSek);

            if (newCount == 0) {
                free(newGID);
                free(newPID);
                free(newSID);
                free(newDatum);
                free(newMin);
                free(newSek);
                *pRiesGID = NULL;
                *pRiesPID = NULL;
                *pRiesSID = NULL;
                *pRiesDatum = NULL;
                *pRiesMin = NULL;
                *pRiesSek = NULL;
            } else {
                *pRiesGID = newGID;
                *pRiesPID = newPID;
                *pRiesSID = newSID;
                *pRiesDatum = newDatum;
                *pRiesMin = newMin;
                *pRiesSek = newSek;
            }
            *pRiesCount = newCount;

            printf("W: Vymazalo sa : %d zaznamov.\n", deleted);
         }

void solutionToGrid(const char *sol, char grid[9][9]){
    int quadRow;
    int quadCol;
    int r;
    int c;
    int quadIndex;
    int base;

    for (quadRow = 0; quadRow < 3; quadRow++){
        for (quadCol = 0; quadCol < 3; quadCol++) {
            quadIndex = quadRow * 3 + quadCol;
            base = quadIndex * 9;
            for (r = 0; r < 3; r++) {
                for (c = 0; c < 3; c++) {
                    grid[quadRow * 3 + r][quadCol * 3 + c] = sol[base + r * 3 + c];
                }
            }
        }
    }
}

void e(char **sudokuSID, char **sudokuSol, int sudokuCount, int polNaplnene){
    char sidInput[MAX_TOKEN];
    int x;
    int parsed;
    int idx;
    int i;
    int j;
    int k;
    char grid[9][9];
    FILE *out;
    int removedPositions[9];
    int removedCount;
    int pos;
    int already;

    parsed = scanf("%99s %d", sidInput, &x);
    while (getchar() != '\n'){ }

    if (parsed != 2 || !isValid(sidInput) || x < 1 || x > 5) {
        printf("E: Nespravny vstup.\n");
        return;
    }

    if (!polNaplnene) {
        printf("E: Polia nie su vytvorene.\n");
        return;
    }

    idx = -1;
    for (i = 0; i < sudokuCount; i++){
        if (strcmp(sudokuSID[i], sidInput) == 0){
            idx = i;
            break;
        }
    }

    if (idx == -1){
        printf ("E: Nespravny vstup.\n");
        return;
    }

    solutionToGrid(sudokuSol[idx], grid);

    out = fopen("Vystup_E.txt", "w");
    if (!out) {
        printf("E: Nespravny vstup.\n");
        return;
    }

    for (i = 0; i < 9; i++){
        removedCount = 0;
        while (removedCount < x) {
            pos = rand() % 9;
            already = 0;
            for (k = 0; k < removedCount; k++){
                if (removedPositions[k] == pos) {
                    already = 1;
                    break;
                }
            }
            if (!already){
                removedPositions[removedCount] = pos;
                removedCount++;
            }
        }

        fprintf(out, "|");
        for (j = 0; j < 9; j++){
            already = 0;
            for (k = 0; k < removedCount; k++){
                if (removedPositions[k] == j){
                    already = 1;
                    break;
                }
            }
            if (already){
                fprintf(out, " |");
            } else {
                fprintf(out, "%c|", grid[i][j]);
            }
        }
        fprintf(out, "\n");
    }
    fclose(out);
}

Hrac* findPlayer(Hrac *zoznam, const char *pid){
    Hrac *cur;
    cur = zoznam;
    while (cur != NULL) {
        if (strcmp(cur->PID, pid) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void freeVysledky(Vysledok *v){
    Vysledok *tmp;
    while (v != NULL){
        tmp = v;
        v = v->next;
        free(tmp);
    }
}

void freeZoznam(Hrac *h){
    Hrac *tmp;
    while (h != NULL){
        freeVysledky(h->vysledky);
        tmp = h;
        h = h->next;
        free(tmp);
    }
}

void m(FILE *fH, FILE *fR, Hrac **pZoznam, int *pZoznamNaplneny){
    char lineH[MAX_LINE];
    char lineR[MAX_LINE];
    Hrac *head;
    Hrac *tail;
    Hrac *newNode;
    Hrac *found;
    Vysledok *newVys;
    Vysledok *vTail;
    char *pid;
    char *meno;
    char *krajina;
    char *rok;
    char *gid;
    char *pidR;
    char *sid;
    char *datum;
    char *minS;
    char *sekS;
    int hracCount;
    int riesCount;

    head = NULL;
    tail = NULL;
    hracCount = 0;
    riesCount = 0;

    if(*pZoznamNaplneny){
        freeZoznam(*pZoznam);
        *pZoznam = NULL;
        *pZoznamNaplneny = 0;
    }

    rewind(fH);
    while (fgets(lineH, sizeof(lineH), fH)) {
        trim(lineH);
        if (strlen(lineH) == 0) continue;

        pid = getField(lineH, 0);
        meno = getField(lineH, 1);
        krajina = getField(lineH, 2);
        rok = getField (lineH, 3);

        newNode = malloc(sizeof(Hrac));
        strncpy(newNode->PID, pid, sizeof(newNode->PID) - 1);
        newNode->PID[sizeof(newNode->PID) - 1] = '\0';
        strncpy(newNode->Identita, meno ? meno : "", sizeof(newNode->Identita) - 1);
        newNode->Identita[sizeof(newNode->Identita) - 1] = '\0';
        strncpy(newNode->Krajina, krajina ? krajina : "", sizeof(newNode->Krajina) - 1);
        newNode->Krajina[sizeof(newNode->Krajina) - 1] = '\0';
        newNode->RokNar = rok ? atoi(rok) : 0;
        newNode->vysledky = NULL;
        newNode->next = NULL;

        if (head == NULL){
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        hracCount++;

        if (pid) free(pid);
        if (meno) free(meno);
        if (krajina) free(krajina);
        if (rok) free(rok);
    }

    rewind(fR);
    while (fgets(lineR, sizeof(lineR), fR)){
        trim(lineR);
        if (strlen(lineR) == 0) continue;

        gid = getField(lineR, 0);
        pidR = getFieldTrimmed(lineR, 1);
        sid = getFieldTrimmed(lineR, 2);
        datum = getField(lineR, 3);
        minS = getField(lineR, 4);
        sekS = getField(lineR, 5);

        found = findPlayer(head, pidR);
        if (found != NULL && gid != NULL && sid != NULL){
            newVys = malloc(sizeof(Vysledok));
            strncpy(newVys->SID, sid, sizeof(newVys->SID) - 1);
            newVys->SID[sizeof(newVys->SID) - 1] = '\0';
            newVys->NarHry = sid[3];
            strncpy(newVys->GID, gid, sizeof(newVys->GID) - 1);
            newVys->GID[sizeof(newVys->GID) - 1] = '\0';
            newVys->NarSut = gid[3];
            strncpy(newVys->DatHry, datum ? datum : "", sizeof(newVys->DatHry) - 1);
            newVys->DatHry[sizeof(newVys->DatHry) - 1] = '\0';
            newVys->Trvanie = (minS ? atoi(minS) : 0) * 60 + (sekS ? atoi(sekS) : 0);
            newVys->next = NULL;

            if (found->vysledky == NULL) {
                found->vysledky = newVys;
            } else {
                vTail = found->vysledky;
                while (vTail->next != NULL) vTail = vTail->next;
                vTail->next = newVys;
            }
            riesCount++;
        }
        if (gid) free(gid);
        if (pidR) free(pidR);
        if (sid) free(sid);
        if (datum) free(datum);
        if (minS) free(minS);
        if (sekS) free(sekS);
    }

    *pZoznam = head;
    *pZoznamNaplneny = 1;

    return hracCount + riesCount;
}

void a(Hrac **pZoznam, int *pZoznamNaplneny){
    int y;
    char meno[100];
    char krajina[100];
    int rok;
    Hrac *newNode;
    Hrac *prev;
    Hrac *check;
    int pos;
    int maxNum;
    int num;
    char newPID[10];

    scanf("%d", &y);
    while (getchar() != '\n') { }

    if (fgets(meno, sizeof(meno), stdin) == NULL) meno[0] = '\0';
    trim(meno);

    if (fgets(krajina, sizeof(krajina), stdin) == NULL) krajina[0] = '\0';
    trim(krajina);

    scanf("%d", &rok);
    while(getchar() != '\n') { }

    check = *pZoznam;
    while (check != NULL) {
        if (strcmp(check->Identita, meno) == 0 && check->RokNar == rok){
            printf("A: Duplicita zaznamu.\n");
            return;
        }
        check = check->next;
    }

    maxNum = 0;
    check = *pZoznam;
    while (check != NULL) {
        if (check->PID[3] == 'a'){
            num = atoi(check->PID + 4);
            if (num > maxNum) maxNum = num;
        }
        check = check->next;
    }
    sprintf(newPID, "PIDa%05d", maxNum + 1);

    newNode = malloc(sizeof(Hrac));
    strcpy(newNode->PID, newPID);
    strncpy(newNode->Identita, meno, sizeof(newNode->Identita) - 1);
    newNode->Identita[sizeof(newNode->Identita) - 1] = '\0';
    strncpy(newNode->Krajina, krajina, sizeof(newNode->Krajina) - 1);
    newNode->Krajina[sizeof(newNode->Krajina) - 1] = '\0';
    newNode->RokNar = rok;
    newNode->vysledky = NULL;
    newNode->next = NULL;

    if (*pZoznam == NULL || y <= 1){
        newNode->next = *pZoznam;
        *pZoznam = newNode;
        pos = 1;
    } else {
        prev = *pZoznam;
        pos = 1;
        while(prev->next != NULL && pos < y - 1){
            prev = prev->next;
            pos++;
        }
        newNode->next = prev->next;
        prev->next = newNode;
        pos = pos + 1;
    }
    *pZoznamNaplneny = 1;

    printf("A: Uspesne pridany zaznam na poziciu %d.\n", pos);
}

void swapResultData(Vysledok *a, Vysledok *b){
    char tmpSID[10];
    char tmpGID[10];
    char tmpDat[12];
    char tmpNarHry;
    char tmpNarSut;
    int tmpTrv;

    strcpy(tmpSID, a->SID); strcpy(a->SID, b->SID); strcpy(b->SID, tmpSID);
    strcpy(tmpGID, a->GID); strcpy(a->GID, b->GID); strcpy(b->GID, tmpGID);
    strcpy(tmpDat, a->DatHry); strcpy(a->DatHry, b->DatHry); strcpy(b->DatHry, tmpDat);

    tmpNarHry = a->NarHry; a->NarHry = b->NarHry; b->NarHry = tmpNarHry;
    tmpNarSut = a->NarSut; a->NarSut = b->NarSut; b->NarSut = tmpNarSut;
    tmpTrv = a->Trvanie; a->Trvanie = b->Trvanie; b->Trvanie = tmpTrv;
}

void d(Hrac *zoznam){
    Hrac *h;
    Vysledok *a;
    int swapped;

    h = zoznam;
    while(h != NULL) {
        do {
            swapped = 0;
            a = h->vysledky;
            while(a != NULL && a->next != NULL){
                if (a->Trvanie > a->next->Trvanie){
                    swapResultData(a, a->next);
                    swapped = 1;
                }
                a = a->next;
            }
        } while (swapped);
        h = h->next;
    }
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

    char **sudokuSID;
    char **sudokuSol;
    int sudokuCount;

    char **hracPID;
    char **hracMeno;
    char **hracKrajina;
    char **hracRok;
    int hracCount;

    char **riesGID;
    char **riesPID;
    char **riesSID;
    char **riesDatum;
    char **riesMin;
    char **riesSek;
    int riesCount;

    fSudoku = NULL;
    fHracov = NULL;
    fRieseni = NULL;
    polNaplnene = 0;
    
    sudokuSID = NULL;
    sudokuSol = NULL;
    sudokuCount = 0;

    hracPID = NULL;
    hracMeno = NULL;
    hracKrajina = NULL;
    hracRok = NULL;
    hracCount = 0;

    riesGID = NULL;
    riesPID = NULL;
    riesSID = NULL;
    riesDatum = NULL;
    riesMin = NULL;
    riesSek = NULL;
    riesCount = 0;

    srand((unsigned int)time(NULL));

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
        }else if (c == 'n') {
            if (fSudoku == NULL || fHracov == NULL || fRieseni == NULL){
                printf("N: Neotvoreny subor.\n");
            } else {
                n(fSudoku, fHracov, fRieseni, &sudokuSID, &sudokuSol, &sudokuCount, &hracPID, &hracMeno, &hracKrajina, &hracRok, &hracCount, &riesGID, &riesPID, &riesSID, &riesDatum, &riesMin, &riesSek, &riesCount, &polNaplnene);
            }
        } else if (c == 'q'){
            q(&riesGID, &riesPID, &riesSID, &riesDatum, &riesMin, &riesSek, &riesCount, polNaplnene);
        } else if (c == 'w'){
            w(&riesGID, &riesPID, &riesSID, &riesDatum, &riesMin, &riesSek, &riesCount, polNaplnene);
        } else if (c == 'e'){
            e(sudokuSID, sudokuSol, sudokuCount, polNaplnene);
        }else if (c == 'k') {
            if (fSudoku) fclose(fSudoku);
            if (fHracov) fclose(fHracov);
            if (fRieseni) fclose(fRieseni);
            if (polNaplnene){
                freeStringArray(sudokuSID, sudokuCount);
                freeStringArray(sudokuSol, sudokuCount);
                freeStringArray(hracPID, hracCount);
                freeStringArray(hracMeno, hracCount);
                freeStringArray(hracKrajina, hracCount);
                freeStringArray(hracRok, hracCount);
                freeStringArray(riesGID, riesCount);
                freeStringArray(riesPID, riesCount);
                freeStringArray(riesSID, riesCount);
                freeStringArray(riesDatum, riesCount);
                freeStringArray(riesMin, riesCount);
                freeStringArray(riesSek, riesCount);
            }
            break;
        }
        
    }
    return 0;
}