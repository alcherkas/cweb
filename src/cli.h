#ifndef CLI_H
#define CLI_H

#define CLI_MAX_LINES 256
#define CLI_MAX_INPUT 256
#define CLI_MAX_LINE_LEN 512
#define CLI_PROMPT "guest@cweb:~$ "

typedef enum {
    THEME_DARK,
    THEME_LIGHT
} Theme;

typedef struct {
    char text[CLI_MAX_LINE_LEN];
    int fg;
} Line;

typedef struct {
    Line lines[CLI_MAX_LINES];
    int line_count;
    int scroll_offset;
    char input[CLI_MAX_INPUT];
    int input_len;
    int cursor_pos;
    Theme theme;
    int width;
    int height;
    int char_width;
    int line_height;
    int cols;
    int rows;
    int needs_redraw;
    int cursor_blink;
    double last_blink;
} Terminal;

void cli_init(Terminal *t);
void cli_render(Terminal *t);
void cli_handle_char(Terminal *t, int codepoint);
void cli_handle_key(Terminal *t, int key_code);
void cli_resize(Terminal *t, int width, int height);

#endif
