#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>

//TODO: write getline

char *strcatchr(const char *str, char c) {
    char buf[2];
    buf[0] = c; buf[1] = 0;
    return strcat(str, buf);
}

size_t getcnttokens(const char *line, size_t len) {
    size_t cnttokens = 1;
    bool inToken = false;
    for (int i=0; i<len; i++) {
        if (isspace(line[i]))  {
            inToken = false;
        } else {
            if (!inToken) {
                cnttokens++;
                inToken = true;
            }
        }
    }
    return cnttokens;
}

void syntax_error(const char *msg) {
    char buf[1000];
    sprintf(buf, "Syntax error near %s\n", msg);
    write(2, buf, strlen(buf));
}

char **gettokens(const char *line, size_t len) {
    char **tokens = malloc(sizeof(char*) * getcnttokens(line, len)); // it potentially allocates for more tokens
    bool inSingleQuotes = false;
    bool isAmpersand = false;
    int currtoken = 0;

    for (int i=0; i<len; i++) {
        if (line[i] == '&') {
            if (isAmpersand) {
                if (currtoken > 0 && strcmp(tokens[currtoken-1], "&&") == 0) {
                    syntax_error("unexpected token &");
                    return -1;
                }
                tokens[currtoken] = malloc(3);
                strcpy(tokens[currtoken], "&&");
            }
            isAmpersand = true;
        } else {
            if (isAmpersand) {
                tokens[currtoken] = malloc(2);
                strcpy(tokens[currtoken], "&");
            }
            isAmpersand = false;
            if (line[i] == '\'') {
                if (inSingleQuotes) {
                    inSingleQuotes = false;
                } else {
                    inSingleQuotes = true;
                    currtoken++;
                }
            } else if (line[i] == '"') {
            } else if (line[i] == ';') {
            } else if (isspace(line[i])) {
            } else {
                if (inSingleQuotes) {
                    strcatchr(tokens[currtoken], line[i]);
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    while (true) {
        write(1, "> ", 2);
        char *line = NULL;
        size_t len = 0;
        getline(&line, &len, stdin);

        char **tokens = gettokens(line, len);
        parse_tokens(tokens);

        free(line);
    }
    return 0;
}
