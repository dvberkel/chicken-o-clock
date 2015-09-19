#include <pebble.h>

static Window *main_window;
static TextLayer *time_layer;

static void time_layer_create(){
  time_layer = text_layer_create(GRect(0, 55, 144, 50));
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_text_color(time_layer, GColorBlack);
  text_layer_set_text(time_layer, "00:00");

  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
}

static void time_layer_destroy(){
  text_layer_destroy(time_layer);
}

static void main_window_load(){
  time_layer_create();

  layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(time_layer));
}

static void main_window_unload(){
  time_layer_destroy();
}

static void init(){
  main_window = window_create();

  window_set_window_handlers(main_window, (WindowHandlers) {
      .load   = main_window_load,
      .unload = main_window_unload
  });

  window_stack_push(main_window, true);
}

static void deinit(){
  window_destroy(main_window);
}

int main(void){
  init();
  app_event_loop();
  deinit();
}
