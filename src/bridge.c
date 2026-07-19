#include "bridge.h"
#include <emscripten.h>

EM_JS(void, br_init, (void), {
    if (!window.cwebCanvas) {
        window.cwebCanvas = document.getElementById("cweb-canvas");
        window.cwebCtx = window.cwebCanvas.getContext("2d");
        window.cwebFontSize = 16;
        window.cwebFontFamily = "'JetBrains Mono', 'Fira Code', monospace";

        function resize() {
            const rect = window.cwebCanvas.parentElement.getBoundingClientRect();
            window.cwebCanvas.width = Math.floor(rect.width);
            window.cwebCanvas.height = Math.floor(rect.height);
        }
        window.addEventListener("resize", resize);
        resize();

        document.addEventListener("keydown", function(e) {
            if (e.ctrlKey || e.altKey || e.metaKey) return;
            if (e.key.length === 1) {
                e.preventDefault();
                Module.ccall('app_handle_char', 'null', ['number'], [e.key.codePointAt(0)]);
            } else if (e.key === "Backspace") {
                e.preventDefault();
                Module.ccall('app_handle_key', 'null', ['number'], [8]);
            } else if (e.key === "Enter") {
                e.preventDefault();
                Module.ccall('app_handle_key', 'null', ['number'], [13]);
            } else if (e.key === "ArrowLeft") {
                e.preventDefault();
                Module.ccall('app_handle_key', 'null', ['number'], [37]);
            } else if (e.key === "ArrowRight") {
                e.preventDefault();
                Module.ccall('app_handle_key', 'null', ['number'], [39]);
            } else if (e.key === "ArrowUp") {
                e.preventDefault();
                Module.ccall('app_handle_key', 'null', ['number'], [38]);
            } else if (e.key === "ArrowDown") {
                e.preventDefault();
                Module.ccall('app_handle_key', 'null', ['number'], [40]);
            } else if (e.key === "Delete") {
                e.preventDefault();
                Module.ccall('app_handle_key', 'null', ['number'], [46]);
            } else if (e.key === "Home") {
                e.preventDefault();
                Module.ccall('app_handle_key', 'null', ['number'], [36]);
            } else if (e.key === "End") {
                e.preventDefault();
                Module.ccall('app_handle_key', 'null', ['number'], [35]);
            }
        });
    }
});

EM_JS(void, br_get_canvas_size, (int *w, int *h), {
    const canvas = window.cwebCanvas;
    Module.setValue(w, canvas.width, 'i32');
    Module.setValue(h, canvas.height, 'i32');
});

EM_JS(void, br_clear_screen, (int color), {
    const ctx = window.cwebCtx;
    const canvas = window.cwebCanvas;
    const hex = (color & 0xFFFFFF).toString(16).padStart(6, '0');
    ctx.fillStyle = '#' + hex;
    ctx.fillRect(0, 0, canvas.width, canvas.height);
});

EM_JS(void, br_draw_rect, (int x, int y, int w, int h, int color), {
    const ctx = window.cwebCtx;
    const hex = (color & 0xFFFFFF).toString(16).padStart(6, '0');
    ctx.fillStyle = '#' + hex;
    ctx.fillRect(x, y, w, h);
});

EM_JS(void, br_draw_text, (int x, int y, const char *text, int color, int font_size), {
    const ctx = window.cwebCtx;
    const hex = (color & 0xFFFFFF).toString(16).padStart(6, '0');
    ctx.fillStyle = '#' + hex;
    ctx.font = font_size + 'px ' + window.cwebFontFamily;
    ctx.textBaseline = 'top';
    const str = UTF8ToString(text);
    ctx.fillText(str, x, y);
});

EM_JS(int, br_measure_text, (const char *text, int font_size), {
    const ctx = window.cwebCtx;
    ctx.font = font_size + 'px ' + window.cwebFontFamily;
    ctx.textBaseline = 'top';
    const str = UTF8ToString(text);
    return Math.ceil(ctx.measureText(str).width);
});
