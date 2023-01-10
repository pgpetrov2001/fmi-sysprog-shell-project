#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>

//TODO: write getline

#define bool unsigned char
#define true 1
#define false 0

char *strcatchr(char *str, char c) {
    if (!str) {
        char *buf = malloc(2);
        buf[0] = c; buf[1] =0;
        return buf;
    }
    int len = strlen(str);
 
    char *newstr = malloc(len+2);
    strcpy(newstr, str);
    free(str);
    newstr[len] = c;
    newstr[len+1] = 0;
    return newstr;
}

bool is_special_char(char c) {
    return isspace(c) || c == '&' || c == '\'' || c == ';';
}

size_t getcnttokens(const char *line, size_t len) {
    size_t cnttokens = 1;
    bool inToken = false;
    int i;
    for (i=0; i<len; ++i) {
        if (is_special_char(line[i]))  {
            inToken = false;
            if (!isspace(line[i])) ++cnttokens;
        } else {
            if (!inToken) {
                ++cnttokens;
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

char **gettokens(const char *line, size_t len, bool **is_control_out) {
    int cnt = getcnttokens(line, len);
    //calloc fills values with 0s
    char **tokens = calloc(cnt + 1, sizeof(char*));
    bool *is_control = calloc(cnt, sizeof(bool));

    bool escaping = false;
    bool inSingleQuotes = false;
    int currtoken = 0;

    int i;
    for (i=0; i<len; i++) {
        if (line[i] == '\'') {
            inSingleQuotes = !inSingleQuotes;
        } else {
            if (inSingleQuotes || escaping) {
                tokens[currtoken] = strcatchr(tokens[currtoken], line[i]);
                escaping = false;
            } else {
                if (line[i] == '\\') {
                    escaping = true;
                } else if (line[i] == ';') {
                    if (tokens[currtoken]) ++currtoken;
                    is_control[currtoken] = 1;
                    tokens[currtoken] = malloc(2);
                    strcpy(tokens[currtoken], ";");

} else if (line[i] == '&') {
                    if (tokens[currtoken] && tokens[currtoken][0] == '&') {
                        if (strcmp(tokens[currtoken], "&&") == 0) {
                            syntax_error("unexpected character &");
                            free(tokens);
                            free(is_control);
                            return NULL;
                        }
                        strcpy(tokens[currtoken], "&&");
                    } else {
                        if (tokens[currtoken]) ++currtoken;
                        is_control[currtoken] = 1;
                        tokens[currtoken] = malloc(3);
                        strcpy(tokens[currtoken], "&");
                    }
                } else if (isspace(line[i])) {
                    if (tokens[currtoken]) currtoken++;
                } else {
                    if (is_control[currtoken]) currtoken++;
                    tokens[currtoken] = strcatchr(tokens[currtoken], line[i]);
                }
            }

        }
    }

    *is_control_out = is_control;
    return tokens;
}

void parse_command(char **tokens, bool *is_control) {
    if (tokens[0] == '&') {
        syntax_error("unexpected token starting with &");
        return;
    }
    char **it;
    char *cmd;
    for (it=tokens; *it; ++it) {
        char *token = *it;
        if (is_control[it-tokens]) {
        } else {
        }
    }
}

int main(int argc, char *argv[]) {
    while (true) {
        write(1, "> ", 2);
        char *line = NULL;
        size_t sz = 0;
        size_t len = getline(&line, &sz, stdin);

        bool *is_control;
        char **tokens = gettokens(line, len-1, &is_control); // newline is not needed
        if (!tokens) {
            free(line);
            continue;
        }
        parse_tokens(tokens);
    }
    return 0;
}
