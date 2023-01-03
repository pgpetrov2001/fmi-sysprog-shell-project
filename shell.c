#include <stdio.h>
  1 #include <stdlib.h>
  2 #include <fcntl.h>
  3 #include <ctype.h>
  4 #include <string.h>
  5
  6 //TODO: write getline
  7
  8 #define bool unsigned char
  9 #define true 1
 10 #define false 0
 11
 12 char *strcatchr(const char *str, char c) {
 13     char buf[2];
 14     buf[0] = c; buf[1] = 0;
 15     return strcat(str, buf);
 16 }
 17
 18 bool is_special_char(char c) {
 19     return isspace(c) || c == '&' || c == '\'' || c == ';';
 20 }
 21
 22 size_t getcnttokens(const char *line, size_t len) {
 23     size_t cnttokens = 1;
 24     bool inToken = false;
 25     for (int i=0; i<len; i++) {
 26         if (is_special_char(c))  {
 27             inToken = false;
 28         } else {
 29             if (!inToken) {
 30                 cnttokens++;
 31                 inToken = true;
 32             }
 33         }
 34     }
 35     return cnttokens;
 36 }
 37
 38 void syntax_error(const char *msg) {
 39     char buf[1000];
 40     sprintf(buf, "Syntax error near %s\n", msg);
 41     write(2, buf, strlen(buf));
 42 }
 43
 44 char **gettokens(const char *line, size_t len) {
 45     int cnt = getcnttokens(line, len);
 46     char **tokens = calloc(sizeof(char*) * (cnt + 1)); // it potentially allocates for more tokens
 47     bool *is_control = calloc(sizeof(bool) * cnt);
 48
 49     bool escaping = false;
 50     bool inSingleQuotes = false;
 51     int currtoken = 0;
 52
 53     int i;
 54     for (i=0; i<len; i++) {
 55         if (line[i] == '\'') {
 56             inSingleQuotes = !inSingleQuotes;
 57         } else {
 58             if (inSingleQuotes || escaping) {
 59                 strcatchr(tokens[currtoken], line[i]);
 60             } else {
 61                 if (lines[i] == '\\') {
                 inToken = true;
  1             }
  2         }
  3     }
  4     return cnttokens;
  5 }
  6
  7 void syntax_error(const char *msg) {
  8     char buf[1000];
  9     sprintf(buf, "Syntax error near %s\n", msg);
 10     write(2, buf, strlen(buf));
 11 }
 12
 13 char **gettokens(const char *line, size_t len) {
 14     int cnt = getcnttokens(line, len);
 15     char **tokens = calloc(sizeof(char*) * (cnt + 1)); // it potentially allocates for more tokens
 16     bool *is_control = calloc(sizeof(bool) * cnt);
 17
 18     bool escaping = false;
 19     bool inSingleQuotes = false;
 20     int currtoken = 0;
 21
 22     int i;
 23     for (i=0; i<len; i++) {
 24         if (line[i] == '\'') {
 25             inSingleQuotes = !inSingleQuotes;
 26         } else {
 27             if (inSingleQuotes || escaping) {
 28                 strcatchr(tokens[currtoken], line[i]);
 29             } else {
 30                 if (lines[i] == '\\') {
 31                     escaping = true;
 32                 } else if (line[i] == ';') {
 33                     is_control[currtoken] = 1;
 34                     tokens[currtoken] = malloc(2);
 35                     strcpy(tokens[currtoken], ";");
 36                 } else if (line[i] == '&') {
 37                     is_control[currtoken] = 1;
 38                     if (tokens[currtoken] && tokens[currtoken][0] == '&') {
 39                         if (strcmp(tokens[currtoken], "&&") == 0) {
 40                             syntax_error("unexpected character &");
 41                             return -1;
 42                         }
 43                         strcpy(tokens[currtoken], "&&");
 44                     } else {
 45                         tokens[currtoken] = malloc(3);
 46                         strcpy(tokens[currtoken], "&");
 47                     }
 48                 } else if (isspace(line[i])) {
 49                     if (tokens[currtoken]) currtoken++;
 50                 } else {
 51                     if (is_control[currtoken]) currtoken++;
 52                     strcatchr(tokens[currtoken], line[i]);
 53                 }
 54             }
 55
 56         }
 57     }
 58
 59     return tokens;
 60 }
 61
                                                         
