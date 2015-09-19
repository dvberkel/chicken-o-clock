#include <pebble.h>

static Window *main_window;

static void main_window_load(){
}

static void main_window_unload(){
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
