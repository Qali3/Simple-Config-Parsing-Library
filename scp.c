#include "scp.h"

typedef struct {
        void (*callback)(Value*, void*);
        void *data;
} UserData;

static inline int isVoid(const char c)  { return c != '\0' && c <= ' '; }
static inline int isAlpha(const char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
static inline int isDigit(const char c) { return c >= '0' && c <= '9'; }

int skip(const char **s) {
        for (;;) {
                if (**s == '/') {
                        if (*++*s == '/') {
                                while (*++*s != '\n' && **s);
                        } else if (**s == '*') {
                                while (**s) {
                                        ++*s;
                                        if (**s == '*' && *(*s + 1) == '/') {
                                                *s += 2;
                                                break;
                                        }
                                }
                        }
                } else if (isVoid(**s)) {
                        ++*s;
                } else {
                        break;
                }
        }
        return **s;
}

int parseNumber(Value *v, const char **s) {
        v->str = *s;

        if (**s == '-' && *(*s+1) == '.')
                return 0;

        while (isDigit(*++*s) || **s == '.');

        if (*(*s-1) == '.')
                return 0;

        v->strLen = *s - v->str;
        v->type = SCP_NUMBER;

        return 1;
}

int parseString(Value *v, const char **s) {
        v->str = ++*s;

        while (*(*s)++ != '\'') if (!**s) return 0;

        v->strLen = *s - v->str - 1;
        v->type = SCP_STRING;

        return 1;
}

int parseKey(UserData *ud, Value *v, const char **s);
int parseValue(UserData *ud, Value *v, const char **s);

int parseTable(UserData *ud, Value *v, const char **s) {
        v->type = SCP_TABLE;

        v->strLen = 0;
        v->str = 0;

        ++*s;

        while (skip(s)) {
                ud->callback(v, ud->data);

                v->nameLen = 0;
                v->name = 0;

                if (!parseValue(ud, v, s) && !parseKey(ud, v, s))
                        return 0;

                if (!skip(s))
                        return 0;

                if (**s == ',') {
                        ++*s;

                        if (!skip(s))
                                return 0;
                } else if (**s == ']') {
                        ud->callback(v, ud->data);

                        v->type = SCP_TABLE_END;

                        v->nameLen = 0;
                        v->name = 0;

                        v->strLen = 0;
                        v->str = 0;

                        ++*s;

                        return 1;
                } else
                        return 0;
        }

        return 0;
}

int parseValue(UserData *ud, Value *v, const char **s) {
        if (isDigit(**s) || **s == '-')
                return parseNumber(v, s);
        else if (**s == '\'')
                return parseString(v, s);
        else if (**s == '[')
                return parseTable(ud, v, s);

        return 0;
}

int parseAssignment(UserData *ud, Value *v, const char **s) {
        if (*(*s)++ != '=')
                return 0;

        if (!skip(s))
                return 1;

        return parseValue(ud, v, s);
}

int parseKey(UserData *ud, Value *v, const char **s) {
        if (isAlpha(**s)) {
                v->name = *s;

                while (isAlpha(*++*s) || isDigit(**s));

                v->nameLen = *s - v->name;
        }

        if (!skip(s))
                return 1;

        return parseAssignment(ud, v, s);
}

int scpParse(void (*callback)(Value*, void*), void *data, const char *s) {
        UserData ud = { callback, data };
        Value v = { 0 };

        while (*s) {
                if (!skip(&s))
                        return 1;

                if (!parseKey(&ud, &v, &s))
                        return 0;

                callback(&v, data);
        }
        return 1;
}
