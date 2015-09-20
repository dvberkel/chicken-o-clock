#include <pebble.h>

static Window *main_window;
static TextLayer *time_layer;
static TextLayer *date_layer;
static TextLayer *week_layer;

static void time_layer_create(){
  time_layer = text_layer_create(GRect(48, 148, 48, 20));
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_text_color(time_layer, GColorBlack);
  text_layer_set_text(time_layer, "00:00");

  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
}

static void time_layer_destroy(){
  text_layer_destroy(time_layer);
}

static void date_layer_create(){
  date_layer = text_layer_create(GRect(96, 148, 48, 20));
  text_layer_set_background_color(date_layer, GColorClear);
  text_layer_set_text_color(date_layer, GColorBlack);
  text_layer_set_text(date_layer, "00/00");

  text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(date_layer, GTextAlignmentRight);
}

static void date_layer_destroy(){
  text_layer_destroy(date_layer);
}

static void week_layer_create(){
  week_layer = text_layer_create(GRect(0, 148, 48, 20));
  text_layer_set_background_color(week_layer, GColorClear);
  text_layer_set_text_color(week_layer, GColorBlack);
  text_layer_set_text(week_layer, "00-00");

  text_layer_set_font(week_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(week_layer, GTextAlignmentLeft);
}

static void week_layer_destroy(){
  text_layer_destroy(week_layer);
}

static void update_time(){
  time_t current_time = time(NULL);
  struct tm *tick_time = localtime(&current_time);

  static char buffer[] = "00:00";
  strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);

  text_layer_set_text(time_layer, buffer);
}

static void update_date(){
  time_t current_time = time(NULL);
  struct tm *tick_time = localtime(&current_time);

  static char buffer[] = "00/00";
  strftime(buffer, sizeof("00/00"), "%d/%m", tick_time);

  text_layer_set_text(date_layer, buffer);
}

static void main_window_load(){
  time_layer_create();
  date_layer_create();
  week_layer_create();

  update_time();
  update_date();

  layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(time_layer));
  layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(date_layer));
  layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(week_layer));
}

static void main_window_unload(){
  week_layer_destroy();
  date_layer_destroy();
  time_layer_destroy();
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  update_time();
  update_date();
}

static void init(){
  main_window = window_create();

  window_set_window_handlers(main_window, (WindowHandlers) {
      .load   = main_window_load,
      .unload = main_window_unload
  });

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

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
