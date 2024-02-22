#include <stdio.h>
#include <string.h>

#define MAX_KEY_LEN 100
#define MAX_VAL_LEN 100
#define MAX_LEVELS 10

typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VAL_LEN];
} KeyValue;

typedef struct {
    KeyValue kv;
    int level;
} ParsedLine;

char *trim_whitespace(char *str) {
    while (*str == ' ' || *str == '\t' || *str == '\n')
        str++;

    char *end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n'))
        end--;

    *(end + 1) = '\0';

    return str;
}

char *parse_hubuml(const char *filename, const char *target_key) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    char line[256];
    ParsedLine parsed_lines[MAX_LEVELS];
    int current_level = -1;
    char *result = NULL;
    while (fgets(line, sizeof(line), file)) {
        char *trimmed_line = trim_whitespace(line);

        if (trimmed_line[0] == '#' || trimmed_line[0] == '\n') {
            continue;
        }

        int level = 0;
        while (trimmed_line[level] == ' ') {
            level++;
        }

        while (current_level >= level) {
            current_level--;
        }

        KeyValue kv;
        if (sscanf(trimmed_line, "%99[^:]: %99[^\n]", kv.key, kv.value) == 2) {
            current_level++;
            parsed_lines[current_level].kv = kv;
            parsed_lines[current_level].level = level;

            if (strcmp(kv.key, target_key) == 0) {
                result = strdup(kv.value);
                break;
            }
        }
    }

    fclose(file);
    return result;
}