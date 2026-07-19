EMCC ?= emcc
BUILD_DIR := build
PUBLIC_DIR := public
SRC_DIR := src

SRCS := $(SRC_DIR)/main.c $(SRC_DIR)/cli.c $(SRC_DIR)/bridge.c

EMFLAGS := -O2 \
  -s EXPORTED_FUNCTIONS='["_main","_app_handle_char","_app_handle_key"]' \
  -s EXPORTED_RUNTIME_METHODS='["ccall","setValue","UTF8ToString"]' \
  -s MODULARIZE=0 \
  -s NO_EXIT_RUNTIME=1 \
  -s ALLOW_MEMORY_GROWTH=1

.PHONY: all clean serve

all: $(BUILD_DIR)/index.html

$(BUILD_DIR)/index.html: $(SRCS) $(PUBLIC_DIR)/index.html $(PUBLIC_DIR)/style.css
	@mkdir -p $(BUILD_DIR)
	$(EMCC) $(SRCS) $(EMFLAGS) -o $(BUILD_DIR)/cli.js
	cp $(PUBLIC_DIR)/index.html $(BUILD_DIR)/index.html
	cp $(PUBLIC_DIR)/style.css $(BUILD_DIR)/style.css

clean:
	rm -rf $(BUILD_DIR)

serve: all
	cd $(BUILD_DIR) && python3 -m http.server 8000
