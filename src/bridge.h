#ifndef BRIDGE_H
#define BRIDGE_H

void br_init(void);
void br_get_canvas_size(int *w, int *h);
void br_clear_screen(int color);
void br_draw_rect(int x, int y, int w, int h, int color);
void br_draw_text(int x, int y, const char *text, int color, int font_size);
int br_measure_text(const char *text, int font_size);

#endif
