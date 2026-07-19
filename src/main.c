#include <emscripten.h>
#include "cli.h"
#include "bridge.h"

static Terminal term;

static double now_ms(void) {
    return emscripten_get_now();
}

void app_handle_char(int codepoint) {
    cli_handle_char(&term, codepoint);
}

void app_handle_key(int key_code) {
    cli_handle_key(&term, key_code);
}

static void loop(void) {
    double t = now_ms();
    if (t - term.last_blink > 500.0) {
        term.last_blink = t;
        term.cursor_blink = !term.cursor_blink;
        term.needs_redraw = 1;
    }

    int w, h;
    br_get_canvas_size(&w, &h);
    if (w != term.width || h != term.height) {
        cli_resize(&term, w, h);
    }

    cli_render(&term);
}

int main(void) {
    br_init();
    cli_init(&term);
    emscripten_set_main_loop(loop, 0, 1);
    return 0;
}
