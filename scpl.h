struct Value {
        int nameLen;
        const char *name;

        int strLen;
        const char *str;

        enum {
                SCPL_NUMBER,
                SCPL_STRING,
                SCPL_TABLE,
                SCPL_TABLE_END,
        } type;
};

typedef struct Value Value;

int scplParse(void (*callback)(Value*, void*), void *data, const char *str);
