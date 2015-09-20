#include <pebble.h>

static Window *main_window;
static TextLayer *time_layer;
static TextLayer *date_layer;
static TextLayer *week_layer;
static Layer *battery_layer;
static Layer *chicken_layer;

static int battery_level;

typedef struct Chicken {
  int x;
  int y;
  int radius;
  int tail_height;
  int beak_size;
  int beak_angle;
} Chicken;

static Chicken chicken = {
  .x = 72,
  .y = 72,
  .radius = 40,
  .tail_height = 40,
  .beak_size = 15,
  .beak_angle = TRIG_MAX_ANGLE/20
};

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

static void battery_level_draw(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  int width = (int)(((float)battery_level / 100.0F) * (float) bounds.size.w);
  int x = (bounds.size.w - width) / 2;
  GRect bar = GRect(x, bounds.origin.y, width, bounds.size.h);

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bar, 1, GCornersAll);
}

static void battery_layer_create(){
  battery_layer = layer_create(GRect(0, 145, 144, 2));
  layer_set_update_proc(battery_layer, battery_level_draw);
}

static void battery_layer_destroy(){
  layer_destroy(battery_layer);
}

static void chicken_draw(Layer *layer, GContext *ctx){
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, GColorWhite);

  GPoint tail_start = GPoint(chicken.x + chicken.radius, chicken.y);
  GPoint tail_tip = GPoint(chicken.x + chicken.radius, chicken.y - chicken.tail_height);
  GPoint tail_end = GPoint(chicken.x, chicken.y);

  graphics_draw_line(ctx, tail_start, tail_tip);
  graphics_draw_line(ctx, tail_tip, tail_end);

  GPoint beak_start = GPoint(chicken.x - chicken.radius, chicken.y);
  GPoint beak_tip = GPoint(
    chicken.x - ((chicken.radius + chicken.beak_size) * cos_lookup(chicken.beak_angle))/TRIG_MAX_RATIO,
    chicken.y + ((chicken.radius + chicken.beak_size) * sin_lookup(chicken.beak_angle))/TRIG_MAX_RATIO);
  GPoint beak_end = GPoint(
    chicken.x - (chicken.radius * cos_lookup(2 * chicken.beak_angle))/TRIG_MAX_RATIO,
    chicken.y + (chicken.radius * sin_lookup(2 * chicken.beak_angle))/TRIG_MAX_RATIO);

  graphics_draw_line(ctx, beak_start, beak_tip);
  graphics_draw_line(ctx, beak_tip, beak_end);

  GPoint origin = GPoint(chicken.x, chicken.y);

  graphics_fill_circle(ctx, origin, chicken.radius);
  graphics_draw_circle(ctx, origin, chicken.radius);
}

static void chicken_layer_create(){
  chicken_layer = layer_create(GRect(0, 0, 144, 145));
  layer_set_update_proc(chicken_layer, chicken_draw);
}

static void chicken_layer_destroy(){
  layer_destroy(chicken_layer);
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

static void update_week(){
  time_t current_time = time(NULL);
  struct tm *tick_time = localtime(&current_time);

  static char buffer[] = "00-00";
  strftime(buffer, sizeof("00-00"), "%y-%W", tick_time);

  text_layer_set_text(week_layer, buffer);
}

static void update(){
  update_time();
  update_date();
  update_week();
}

static void main_window_load(){
  time_layer_create();
  date_layer_create();
  week_layer_create();
  battery_layer_create();
  chicken_layer_create();

  update();

  layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(time_layer));
  layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(date_layer));
  layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(week_layer));
  layer_add_child(window_get_root_layer(main_window), battery_layer);
  layer_add_child(window_get_root_layer(main_window), chicken_layer);
}

static void main_window_unload(){
  chicken_layer_destroy();
  battery_layer_destroy();
  week_layer_destroy();
  date_layer_destroy();
  time_layer_destroy();
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  update();
}

static void battery_handler(BatteryChargeState state){
  battery_level = state.charge_percent;
  layer_mark_dirty(battery_layer);
}

static void init(){
  main_window = window_create();

  window_set_window_handlers(main_window, (WindowHandlers) {
      .load   = main_window_load,
      .unload = main_window_unload
  });

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_handler);

  window_stack_push(main_window, true);

  battery_handler(battery_state_service_peek());
}

static void deinit(){
  window_destroy(main_window);
}

int main(void){
  init();
  app_event_loop();
  deinit();
}
