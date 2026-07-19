#include "cli.h"
#include "bridge.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static int theme_bg(Theme theme) { return theme == THEME_DARK ? 0x0c0c0c : 0xfffdf5; }
static int theme_fg(Theme theme) { return theme == THEME_DARK ? 0x33ff33 : 0x1a1a1a; }
static int theme_prompt(Theme theme) { return theme == THEME_DARK ? 0x00ffff : 0x0055aa; }
static int theme_error(Theme theme) { return theme == THEME_DARK ? 0xff3333 : 0xcc0000; }
static int theme_dim(Theme theme) { return theme == THEME_DARK ? 0x888888 : 0x555555; }

static int role_color(ColorRole role, Theme theme) {
    switch (role) {
        case ROLE_PROMPT: return theme_prompt(theme);
        case ROLE_DIM:    return theme_dim(theme);
        case ROLE_ERROR:  return theme_error(theme);
        default:          return theme_fg(theme);
    }
}

static void line_append(Terminal *t, const char *text, ColorRole role) {
    if (t->line_count >= CLI_MAX_LINES) {
        memmove(&t->lines[0], &t->lines[1], sizeof(Line) * (CLI_MAX_LINES - 1));
        t->line_count = CLI_MAX_LINES - 1;
    }
    Line *line = &t->lines[t->line_count++];
    strncpy(line->text, text, CLI_MAX_LINE_LEN - 1);
    line->text[CLI_MAX_LINE_LEN - 1] = '\0';
    line->role = role;
}

static void line_wrap_append(Terminal *t, const char *text, ColorRole role) {
    if (t->cols <= 0) return;
    const char *p = text;
    char buf[CLI_MAX_LINE_LEN];
    while (*p) {
        int n = 0;
        while (*p && n < t->cols - 1) {
            buf[n++] = *p++;
        }
        buf[n] = '\0';
        line_append(t, buf, role);
    }
}

static const char *cmd_trim(const char *s) {
    while (isspace((unsigned char)*s)) s++;
    return s;
}

static void cmd_execute(Terminal *t, const char *raw) {
    const char *cmd = cmd_trim(raw);
    if (cmd[0] == '\0') return;

    if (strcmp(cmd, "help") == 0) {
        line_wrap_append(t, "Available commands:", ROLE_PROMPT);
        line_wrap_append(t, "  help      - show this help", ROLE_FG);
        line_wrap_append(t, "  about     - about this demo", ROLE_FG);
        line_wrap_append(t, "  projects  - list sample projects", ROLE_FG);
        line_wrap_append(t, "  skills    - tech skills snapshot", ROLE_FG);
        line_wrap_append(t, "  contact   - how to reach out", ROLE_FG);
        line_wrap_append(t, "  theme     - toggle color theme", ROLE_FG);
        line_wrap_append(t, "  clear     - clear the screen", ROLE_FG);
    } else if (strcmp(cmd, "about") == 0) {
        line_wrap_append(t, "cweb - a pure C / WebAssembly terminal demo.", ROLE_FG);
        line_wrap_append(t, "Everything you see is rendered from C, compiled with Emscripten,", ROLE_DIM);
        line_wrap_append(t, "and drawn to an HTML5 canvas. No JavaScript frameworks involved.", ROLE_DIM);
    } else if (strcmp(cmd, "projects") == 0) {
        line_wrap_append(t, "* cweb (this site) - C + WASM CLI portfolio", ROLE_FG);
        line_wrap_append(t, "* tiny-term - minimal VT100 emulator", ROLE_DIM);
        line_wrap_append(t, "* wasm-2048 - 2048 clone in C for the browser", ROLE_DIM);
    } else if (strcmp(cmd, "skills") == 0) {
        line_wrap_append(t, "Languages: C, JavaScript, Python", ROLE_FG);
        line_wrap_append(t, "Tools:    Emscripten, Make, GitHub Actions", ROLE_FG);
        line_wrap_append(t, "Focus:    systems, embedded, web assembly", ROLE_FG);
    } else if (strcmp(cmd, "contact") == 0) {
        line_wrap_append(t, "Email: hello@example.com", ROLE_FG);
        line_wrap_append(t, "GitHub: github.com/example", ROLE_FG);
    } else if (strcmp(cmd, "theme") == 0) {
        t->theme = (t->theme == THEME_DARK) ? THEME_LIGHT : THEME_DARK;
        line_wrap_append(t, t->theme == THEME_DARK ? "Switched to dark theme." : "Switched to light theme.", ROLE_PROMPT);
    } else if (strcmp(cmd, "clear") == 0) {
        t->line_count = 0;
    } else {
        char err[CLI_MAX_LINE_LEN];
        snprintf(err, sizeof(err), "%s: command not found. Try 'help'.", cmd);
        line_wrap_append(t, err, ROLE_ERROR);
    }
}

void cli_init(Terminal *t) {
    memset(t, 0, sizeof(*t));
    t->theme = THEME_DARK;
    t->needs_redraw = 1;
    t->cursor_blink = 1;
    t->last_blink = 0.0;
    t->char_width = 10;
    t->line_height = 18;
    cli_resize(t, 800, 450);
    line_wrap_append(t, "Welcome to cweb. Type 'help' to get started.", ROLE_PROMPT);
}

void cli_resize(Terminal *t, int width, int height) {
    t->width = width;
    t->height = height;
    t->char_width = br_measure_text("M", t->line_height) + 1;
    if (t->char_width < 6) t->char_width = 8;
    t->cols = width / t->char_width;
    t->rows = height / t->line_height;
    t->needs_redraw = 1;
}

void cli_handle_char(Terminal *t, int codepoint) {
    if (codepoint < 32 || codepoint > 126) return;
    if (t->input_len >= CLI_MAX_INPUT - 1) return;
    memmove(&t->input[t->cursor_pos + 1], &t->input[t->cursor_pos], t->input_len - t->cursor_pos + 1);
    t->input[t->cursor_pos] = (char)codepoint;
    t->cursor_pos++;
    t->input_len++;
    t->needs_redraw = 1;
    t->cursor_blink = 1;
}

void cli_handle_key(Terminal *t, int key_code) {
    if (key_code == 8) { // Backspace
        if (t->cursor_pos > 0) {
            memmove(&t->input[t->cursor_pos - 1], &t->input[t->cursor_pos], t->input_len - t->cursor_pos + 1);
            t->cursor_pos--;
            t->input_len--;
            t->needs_redraw = 1;
        }
    } else if (key_code == 46) { // Delete
        if (t->cursor_pos < t->input_len) {
            memmove(&t->input[t->cursor_pos], &t->input[t->cursor_pos + 1], t->input_len - t->cursor_pos);
            t->input_len--;
            t->needs_redraw = 1;
        }
    } else if (key_code == 13) { // Enter
        char full[CLI_MAX_INPUT + 64];
        snprintf(full, sizeof(full), "%s%s", CLI_PROMPT, t->input);
        line_append(t, full, ROLE_PROMPT);
        cmd_execute(t, t->input);
        t->input[0] = '\0';
        t->input_len = 0;
        t->cursor_pos = 0;
        t->scroll_offset = 0;
        t->needs_redraw = 1;
    } else if (key_code == 37) { // Left
        if (t->cursor_pos > 0) { t->cursor_pos--; t->needs_redraw = 1; }
    } else if (key_code == 39) { // Right
        if (t->cursor_pos < t->input_len) { t->cursor_pos++; t->needs_redraw = 1; }
    } else if (key_code == 36) { // Home
        t->cursor_pos = 0; t->needs_redraw = 1;
    } else if (key_code == 35) { // End
        t->cursor_pos = t->input_len; t->needs_redraw = 1;
    } else if (key_code == 38) { // Up
        // no history in this demo
    } else if (key_code == 40) { // Down
        // no history in this demo
    }
    t->cursor_blink = 1;
}

void cli_render(Terminal *t) {
    br_get_canvas_size(&t->width, &t->height);
    int cw = br_measure_text("M", t->line_height) + 1;
    if (cw < 6) cw = 8;
    int cols = t->width / cw;
    int rows = t->height / t->line_height;
    if (cols != t->cols || rows != t->rows) {
        t->cols = cols;
        t->rows = rows;
        t->char_width = cw;
        t->needs_redraw = 1;
    }

    br_clear_screen(theme_bg(t->theme));

    int y = t->line_height;
    int start = t->line_count > rows - 2 ? t->line_count - (rows - 2) : 0;
    for (int i = start; i < t->line_count; i++) {
        int fg = role_color(t->lines[i].role, t->theme);
        br_draw_text(0, y, t->lines[i].text, fg, t->line_height);
        y += t->line_height;
    }

    char prompt_line[CLI_MAX_INPUT + 64];
    snprintf(prompt_line, sizeof(prompt_line), "%s%s", CLI_PROMPT, t->input);
    br_draw_text(0, t->height - t->line_height, prompt_line, theme_prompt(t->theme), t->line_height);

    if (t->cursor_blink) {
        int cx = (int)strlen(CLI_PROMPT) * t->char_width + t->cursor_pos * t->char_width;
        int cy = t->height - t->line_height + 2;
        br_draw_rect(cx, cy, t->char_width - 1, t->line_height - 4, theme_prompt(t->theme));
    }
}
