#include "scp.h"

inline int scpIsAlpha(const char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
inline int scpIsDigit(const char c) { return c >= '0' && c <= '9'; }

static int skip(const char **s) {
        while (**s)
                if (**s <= ' ') ++*s;
                else if (**s == '#') {
                        while (*++*s != '#')
                                if (!**s) return 0;
                        ++*s;
                }
                else return 1;
        return 0;
}

static int parseAny(ScpUserData *sud, const char **s);

int scpParseTable(ScpUserData *sud, const char **s) {
        if (**s == '[') {
                sud->callback((*s)++, 1, sud->data);

                while (skip(s)) {
                        if (!parseAny(sud, s) || !skip(s))
                                return 0;

                        if (**s == '=') {
                                ++*s;
                                if (!skip(s) || !parseAny(sud, s))
                                        return 0;
                        }

                        if (!skip(s))
                                return 0;

                        if (**s == ',') ++*s;
                        else if (**s == ']') { sud->callback((*s)++, 1, sud->data); return 1; }
                        else return 0;
                }
        } else
                while (skip(s))
                        if (!parseAny(sud, s) || !skip(s) || *(*s)++ != '=' || !skip(s) || !parseAny(sud, s))
                                return 0;
        return 1;
}

static int parseAny(ScpUserData *sud, const char **s) {
        const char *str = *s;

        if (scpIsAlpha(**s)) while (scpIsAlpha(*++*s) || scpIsDigit(**s));
        else if (scpIsDigit(**s)) while (scpIsDigit(*++*s));
        else if (**s == '\'') while (*++*s != '\'') { if (**s == '\0') return 0; }
        else if (**s == '[') return scpParseTable(sud, s);
        else return 0;

        unsigned int len = *s - str;

        sud->callback(str, len, sud->data);

        return 1;
}
