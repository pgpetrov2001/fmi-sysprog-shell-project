
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

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
    return isspace(c) || c == '&' || c == '\'' || c == ';' || c == '>' || c == '<';
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
    *is_control_out = NULL;
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
                } else if (is_special_char(line[i])) {
                    if (tokens[currtoken]) ++currtoken;
                    is_control[currtoken] = 1;
                    tokens[currtoken] = malloc(2);
                    char buf[2];
                    buf[0] = line[i]; buf[1] = 0;
                    strcpy(tokens[currtoken], buf);
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

void signal_error(const char *msg) {
    write(2, msg, strlen(msg));
}

void general_exec(const char **cmd, char *inredir, char *outredir) {
    if (inredir) {
        int fdr = open(inredir, O_RDONLY);
        if (fdr == -1) {
            write(2, "Could not open file", strlen("Could not open file"));
            write(2, inredir, strlen(inredir));
            write(2, " for writing\n", strlen(" for writing\n"));
            return;
        }
        close(0); dup(fdr);
    }
    if (outredir) {
        int fdw = open(outredir, O_WRONLY|O_CREAT|O_TRUNC, 0777);
        if (fdr == -1) {
            write(2, "Could not open file", strlen("Could not open file"));
            write(2, inredir, strlen(inredir));
            write(2, " for writing\n", strlen(" for writing\n"));

return;
        }
        close(1); dup(fdw);
    }
    if (execvp(cmd[0], cmd) == -1) {
        write(2, "Command ", strlen("Command "));
        write(2, cmd[0], strlen(cmd[0]));
        write(2, " not found in path");
    }
}

int execute_singleton_command(const char **cmd, char *inredir, char *outredir) {
    int pid;
    if (pid = fork()) {
        int status;
        waitpid(pid, &status, 0);
        fflush(stdout);
        return status;
    } else {
        general_exec(cmd, *inredir, *outredir);
    }
}

void execute_singleton_command_async(const char **cmd, char *inredir, char *outredir) {
    int pid;
    if (pid = fork()) {
        char msg[20];
        sprintf(msg, "[%d]\n", pid);
        write(1, msg, strlen(msg));
    } else {
        general_exec(cmd, *inredir, *outredir);
    }
}

void execute_command(char **tokens, bool *is_control) {
    int num_control = 0;
    int num_tokens = 0;
    {
        char **it;
        for (it=tokens, num_tokens=0; *it; ++it, ++num_tokens)
        {
            if (is_control[num_tokens]) {
                if (num_tokens > 0 && is_control[num_tokens-1]) {
                    syntax_error("syntax error with 2 consecutive control tokens");
                    return;
                }
                ++num_control;
            }
        }
    }
    if (num_tokens == 0) return;
    if (is_control[0]) {
        syntax_error("unexpected token at start");
        return;
    }
    if (is_control[num_tokens-1] && strcmp(tokens[num_tokens-1], "&&") == 0) {
        syntax_error("unexpected token at end of input");
        return;
    }
    char **cmd         = calloc(num_tokens+1, sizeof(void*));
    char **cmd_end = cmd;
    //char ***pipe_chain = calloc(num_control * sizeof(void*));

char **it             = tokens;
    char **cmd_it         = cmd;
    //char ***pipe_chain_it = pipe_chain;
    bool reading_out_redir = 0;
    bool reading_in_redir = 0;
    char *inredir = NULL;
    char *outredir = NULL;
    for (; *it; ++it) {
        char *token = *it;
        if (is_control[it-tokens]) {
            if (strcmp(token, "&&") == 0) {
                if (execute_singleton_command(cmd, inredir, outredir)) return;
            } else if (strcmp(token, ";") == 0) {
                execute_singleton_command(cmd, inredir, outredir);
            } else if (strcmp(token, "&") == 0) {
                execute_singleton_command_async(cmd);
            } else if (strcmp(token, ">") == 0) {
                reading_out_redir = 1;
            } else if (strcmp(token, "<") == 0) {
                reading_in_redir = 1;
            } else {
                assert(false);
            }
            cmd = cmd_end;
        } else {
            if (reading_in_redir) {
                inredir = token;
                reading_in_redir = 0;
            } else if (reading_out_redir) {
                outredir = token;
                reading_out_redir = 0;
            } else {
                *(cmd_end++) = token;
            }
        }
    }

    if (cmd[0]) execute_singleton_command(cmd);
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
        char **it = tokens;
        for (; *it; ++it) {
            printf("token %s\n", *it);
        }
        fflush(stdout);
        execute_command(tokens, is_control);
    }
    return 0;
}
