/*
 * File: uniqsubstr.c
 *     : This code finds the longest unique substring in a given string
 *
 * Author: surojit-pathak@github / surojit.pathak@gmail.com
 *
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

void
check_unique (char *p, int *low, int *high)
{
    int i = *low, j = *high;
#define UNSIGNED_CHAR_RANGE 256
#define SCAN_MOVE_LEFT 1
#define SCAN_MOVE_RIGHT 2
    int assoc[UNSIGNED_CHAR_RANGE];
    unsigned char move[UNSIGNED_CHAR_RANGE];
    bool once = false;

    memset(assoc, -1, sizeof(int) * UNSIGNED_CHAR_RANGE);
    memset(move, 0, UNSIGNED_CHAR_RANGE);

    while (i <= j) {
        if (i == j) {
            once = true;
        }
        if (assoc[p[i]] == -1) {
            assoc[p[i]] = i;
            move[p[i]] = SCAN_MOVE_LEFT;
            i++;
        } else {
            if (move[p[i]] == SCAN_MOVE_LEFT) {
                *low = assoc[p[i]] + 1;
            } else {
                *high = assoc[p[i]] - 1;
            }
            check_unique(p, low, high);
            break;
        }

        if (once) break;

        if (assoc[p[j]] == -1) {
            assoc[p[j]] = j;
            move[p[j]] = SCAN_MOVE_RIGHT;
            j--;
        } else {
            if (move[p[j]] == SCAN_MOVE_RIGHT) {
                *high = assoc[p[j]] - 1;
            } else {
                *low = assoc[p[j]] + 1;
            }
            check_unique(p, low, high);
           break;
        }

    }
}

int main (int argc, char *argv[])
{
    char *charp = "abacbdadbc";

    if (argc == 2) {
        charp = strdup(argv[1]);
    }

    int i, j, k;
    int len = strlen(charp);

    i = 0;
    j = len -1;
    check_unique(charp, &i, &j);

    assert(i <= j);
    char *result = malloc(j - i + 2);
    k = 0;
    while (i <= j) {
       result[k++] = charp[i++];
    }

    result[k] = '\0';
    printf("The unique substring is %s\n", result);

    return 0;
}

