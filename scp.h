typedef struct {
        void (*callback)(const char *, unsigned int, void*);
        void *data;
} ScpUserData;

int scpParseTable(ScpUserData *sud, const char **s);
inline int scpIsAlpha(const char c);
inline int scpIsDigit(const char c);
