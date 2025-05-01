#ifndef PARSER_H
#define PARSER_H

struct command {
    char **argv;
    int background;
};

struct command *parse_line(const char *line);
void free_command(struct command *cmd);

#endif