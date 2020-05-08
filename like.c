#include "pch.h"

#include "like.h"


bool mkLike(char *pTameText, char *pWildText, bool bCaseSensitive, const char cAltTerminator) {
    bool bMatch = true;
    char * pAfterLastWild = NULL; // The location after the last '%', if we�ve encountered one
    char * pAfterLastTame = NULL; // The location in the tame string, from which we started after last wildcard
    char t, w;

    // Walk the text strings one character at a time.
    while (1) {
        t = *pTameText;
        w = *pWildText;

        // How do you match a unique text string?
        if (!t || t == cAltTerminator) {
            // Easy: unique up on it!
            if (!w || w == cAltTerminator) {
                break;                                   // "x" matches "x"
            }
            else if (w == '%') {
                pWildText++;
                continue;                           // "x*" matches "x" or "xy"
            }
            else if (pAfterLastTame) {
                if (!(*pAfterLastTame) || *pAfterLastTame == cAltTerminator) {
                        bMatch = false;
                        break;
                }
                pTameText = pAfterLastTame++;
                pWildText = pAfterLastWild;
                continue;
            }

            bMatch = false;
            break;                                           // "x" doesn't match "xy"
        }
        else {

            if (!bCaseSensitive) {
                // Lowercase the characters to be compared.
                if (t >= 'A' && t <= 'Z') {
                    t += ('a' - 'A');
                }

                if (w >= 'A' && w <= 'Z') {
                    w += ('a' - 'A');
                }
            }

            // How do you match a tame text string?
            if (t != w) {
                // The tame way: unique up on it!
                if (w == '%') {
                    pAfterLastWild = ++pWildText;
                    pAfterLastTame = pTameText;
                    w = *pWildText;

                    if (!w || w == cAltTerminator) {
                        break;                           // "*" matches "x"
                    }
                    continue;                           // "*y" matches "xy"
                }
                else if (pAfterLastWild) {
                    if (pAfterLastWild != pWildText) {
                        pWildText = pAfterLastWild;
                        w = *pWildText;

                        if (!bCaseSensitive && w >= 'A' && w <= 'Z') {
                            w += ('a' - 'A');
                        }

                        if (t == w) {
                            pWildText++;
                        }
                    }
                    pTameText++;
                    continue;                           // "*sip*" matches "mississippi"
                }
                else {
                    bMatch = false;
                    break;                                   // "x" doesn't match "y"
                }
            }
        }

        pTameText++;
        pWildText++;
    }

    return bMatch;
}
