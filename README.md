# Simple-Config-Parsing-Library
SCPL (Simple Config Parsing Library) - is a library for a simple, fast, zero-copy and flexible parsing of scpl-format in C-lanugage.

SCPL syntax example:
```scpl
theme = [
	number = 4,
	string = 'foo',
	arrays = [
		5,
		'string'
	],
	tables = [
		something = 'a'
	]
	// Comment
	/* Comment Block */
]
```

C-code example:
```c
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scpl.h"

typedef struct {
        int inTheme;
        int inArrays;
        int inTables;
} State;

void callback(Value *v, void *data) {
        State *state = data;

        if (v->type == SCPL_TABLE) {
                if (memcmp(v->name, "theme", v->nameLen) == 0) {
                        state->inTheme = 1;
                        printf("theme = [\n");
                } else if (memcmp(v->name, "arrays", v->nameLen) == 0) {
                        state->inTheme = 0;
                        state->inArrays = 1;
                        printf("arrays = [\n");
                } else if (memcmp(v->name, "tables", v->nameLen) == 0) {
                        state->inTheme = 0;
                        state->inTables = 1;
                        printf("tables = [\n");
                }
                return;
        } else if (v->type == SCPL_TABLE_END) {
                if (state->inArrays) {
                        state->inTheme = 1;
                        state->inArrays = 0;
                } else if (state->inTables) {
                        state->inTheme = 1;
                        state->inTables = 0;
                } else if (state->inTheme) {
                        state->inTheme = 0;
                }
                printf("]\n");
                return;
        }

        if (state->inTheme) {
                if (v->type == SCPL_NUMBER) {
                        if (memcmp(v->name, "number", v->nameLen) == 0) {
                                float number = atoi(v->str);
                                printf("number = %.2f\n", number);
                        }
                } else if (v->type == SCPL_STRING) {
                        if (memcmp(v->name, "string", v->nameLen) == 0) {
                                printf("string = \'%.*s\'\n", v->strLen, v->str);
                        }
                }
        } else if (state->inArrays) {
                if (v->type == SCPL_NUMBER) {
                        float number = atoi(v->str);
                        printf("_ = %.2f\n", number);
                } else if (v->type == SCPL_STRING) {
                        printf("_ = \'%.*s\'\n", v->strLen, v->str);
                }
        } else if (state->inTables) {
                if (v->type == SCPL_STRING) {
                        if (memcmp(v->name, "something", v->nameLen) == 0) {
                                printf("%.*s = \'%.*s\'\n", v->nameLen, v->name, v->strLen, v->str);
                        }
                }
        }
}

int main() {
        const char *path = "example.scpl";
        State state = { 0 };

        struct stat st;
        if (stat(path, &st) < 0) return 1;

        int fd = open(path, O_RDONLY);
        if (fd < 0) return 1;

        void *m = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
        close(fd);
        if (m < 0) return 1;

        if (!scplParse(callback, &state, m)) {
                printf("Error: Could not parse the file: %s.\n", "test");
                return 1;
        }

        munmap(m, st.st_size);
}
```
