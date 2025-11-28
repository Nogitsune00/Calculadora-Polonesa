#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "expressao.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Converte graus para radianos (para sen, cos, tg)
static float grausParaRad(float ang) {
    return ang * M_PI / 180.0;
}

float getValorPosFixa(char *StrPosFixa) {
    if (StrPosFixa == NULL || strlen(StrPosFixa) == 0)
        printf("Erro: dados invalidos\n");
        return 0;

    float pilha[256];
    int topo = -1;
    char token[64];
    int pos = 0;

    for (int i = 0; ; i++) {

        // Encerra token quando encontra espaço ou final de string
        if (StrPosFixa[i] == ' ' || StrPosFixa[i] == '\0') {

            if (pos > 0) {
                token[pos] = '\0';
                pos = 0;

                if (strcmp(token, "+") == 0 ||
                    strcmp(token, "-") == 0 ||
                    strcmp(token, "*") == 0 ||
                    strcmp(token, "/") == 0 ||
                    strcmp(token, "%") == 0 ||
                    strcmp(token, "^") == 0)
                {
                    if (topo < 1){
                        printf("Erro: verifique os dados e tente novamente");
                        return 0;
                    }   // Erro de operandos insuficientes

                    float b = pilha[topo--];
                    float a = pilha[topo--];

                    if (strcmp(token, "+") == 0) pilha[++topo] = a + b;
                    else if (strcmp(token, "-") == 0) pilha[++topo] = a - b;
                    else if (strcmp(token, "*") == 0) pilha[++topo] = a * b;
                    else if (strcmp(token, "/") == 0) pilha[++topo] = a / b;
                    else if (strcmp(token, "%") == 0) pilha[++topo] = fmod(a, b);
                    else if (strcmp(token, "^") == 0) pilha[++topo] = pow(a, b);
                }

                else if (strcmp(token, "sen") == 0 ||
                         strcmp(token, "cos") == 0 ||
                         strcmp(token, "tg") == 0 ||
                         strcmp(token, "log") == 0 ||
                         strcmp(token, "raiz") == 0)
                {
                    if (topo < 0) {
                        printf("Erro: verifique os dados e tente novamente");
                        return 0;
                    }
                        

                    float a = pilha[topo--];

                    if (strcmp(token, "sen") == 0)
                        pilha[++topo] = sin(grausParaRad(a));

                    else if (strcmp(token, "cos") == 0)
                        pilha[++topo] = cos(grausParaRad(a));

                    else if (strcmp(token, "tg") == 0)
                        pilha[++topo] = tan(grausParaRad(a));

                    else if (strcmp(token, "log") == 0)
                        pilha[++topo] = log10(a);

                    else if (strcmp(token, "raiz") == 0)
                        pilha[++topo] = sqrt(a);
                }

                else {
                    // converte token para float
                    pilha[++topo] = atof(token);
                }
            }

            if (StrPosFixa[i] == '\0') break;
        }
        else {
            token[pos++] = StrPosFixa[i];
        }
    }

    if (topo != 0)
        printf("Erro: verifique os dados e tente novamente");
        return 0;

    return pilha[0];
}

char * getFormaInFixa(char *Str) {
    if (!Str) return NULL;

    char *pilhaExpr[256];
    int pilhaPrec[256];
    int topo = -1;
    char token[64];
    int pos = 0;

    #define PREC(op) ( \
        (op=='+'||op=='-') ? 1 : \
        (op=='*'||op=='/'||op=='%') ? 2 : \
        (op=='^') ? 3 : \
        10 \
    )

    for (int i = 0; ; i++) {
        if (Str[i] == ' ' || Str[i] == '\0') {
            if (pos > 0) {
                token[pos] = '\0';
                pos = 0;

                if (strcmp(token, "+")==0 || strcmp(token, "-")==0 ||
                    strcmp(token, "*")==0 || strcmp(token, "/")==0 ||
                    strcmp(token, "%")==0 || strcmp(token, "^")==0)
                {
                    if (topo < 1) return NULL;

                    char *bExpr = pilhaExpr[topo];
                    int   bPrec = pilhaPrec[topo];
                    topo--;

                    char *aExpr = pilhaExpr[topo];
                    int   aPrec = pilhaPrec[topo];
                    topo--;

                    int p = PREC(token[0]);

                    char A[512], B[512];

                    if (aPrec < p)
                        sprintf(A, "(%s)", aExpr);
                    else
                        sprintf(A, "%s", aExpr);

                    if (bPrec < p ||
                       (bPrec == p && (token[0]=='-' || token[0]=='/')))
                        sprintf(B, "(%s)", bExpr);
                    else
                        sprintf(B, "%s", bExpr);

                    char buf[1024];
                    sprintf(buf, "%s%s%s", A, token, B);

                    free(aExpr);
                    free(bExpr);

                    pilhaExpr[++topo] = strdup(buf);
                    pilhaPrec[topo] = p;
                }
                else if (!strcmp(token,"sen") || !strcmp(token,"cos") ||
                         !strcmp(token,"tg")  || !strcmp(token,"log") ||
                         !strcmp(token,"raiz"))
                {
                    if (topo < 0) return NULL;

                    char *aExpr = pilhaExpr[topo];
                    int   aPrec = pilhaPrec[topo];
                    topo--;

                    char buf[1024];
                    sprintf(buf, "%s(%s)", token, aExpr);

                    free(aExpr);

                    pilhaExpr[++topo] = strdup(buf);
                    pilhaPrec[topo] = 4;
                }

                else {
                    pilhaExpr[++topo] = strdup(token);
                    pilhaPrec[topo] = 10;
                }
            }

            if (Str[i] == '\0') break;
        }
        else {
            token[pos++] = Str[i];
        }
    }

    if (topo != 0)
        return NULL;

    return pilhaExpr[0];
}