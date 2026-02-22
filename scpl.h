struct Value {
        int nameLen;
        const char *name;

        int strLen;
        const char *str;

        enum {
                SCP_NUMBER,
                SCP_STRING,
                SCP_TABLE,
                SCP_TABLE_END,
        } type;
};

typedef struct Value Value;

int scpParse(void (*callback)(Value*, void*), void *data, const char *str);
