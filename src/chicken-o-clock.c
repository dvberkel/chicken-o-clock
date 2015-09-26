#include <pebble.h>

static Window *main_window;
static TextLayer *time_layer;
static TextLayer *date_layer;
static TextLayer *week_layer;
static Layer *battery_layer;
static Layer *chicken_layer;
static Layer *egg_animation_layer;
static PropertyAnimation *egg_animation;
static GRect egg_origin;

static bool connection_status;
static int battery_level;

typedef struct Chicken {
  int x;
  int y;
  int radius;
  int tail_height;
  int beak_size;
  int beak_angle;
  int eye_size;
  int left_eye;
  int wing_size;
  int leg_size;
  int leg_angle;
  int egg_radius;
} Chicken;

static Chicken chicken = {
  .x = 72,
  .y = 50,
  .radius = 40,
  .tail_height = 40,
  .beak_size = 15,
  .beak_angle = TRIG_MAX_ANGLE/20,
  .eye_size = 3,
  .left_eye = 30,
  .wing_size = 15,
  .leg_size = 15,
  .leg_angle = TRIG_MAX_ANGLE/40,
  .egg_radius = 10
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
  GPoint tail_start = GPoint(chicken.x + chicken.radius, chicken.y);
  GPoint tail_tip = GPoint(chicken.x + chicken.radius, chicken.y - chicken.tail_height);

  int tail_angle = atan2_lookup(chicken.radius, chicken.tail_height);
  GPoint tail_end = GPoint(
    chicken.x + chicken.radius * cos_lookup(tail_angle)/TRIG_MAX_RATIO,
    chicken.y - chicken.radius * sin_lookup(tail_angle)/TRIG_MAX_RATIO);

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

  GPoint left_eye = GPoint(chicken.x - chicken.left_eye, chicken.y - chicken.eye_size);
  GPoint right_eye = GPoint(chicken.x - chicken.left_eye + 2 * chicken.eye_size, chicken.y - chicken.eye_size);

  graphics_fill_circle(ctx, left_eye, chicken.eye_size);
  graphics_fill_circle(ctx, right_eye, chicken.eye_size);

  GPoint wing_start = GPoint(chicken.x, chicken.y);
  GPoint wing_tip = GPoint(chicken.x + 2 * chicken.wing_size, chicken.y + chicken.wing_size);
  GPoint wing_end = GPoint(chicken.x + chicken.wing_size, chicken.y);

  graphics_draw_line(ctx, wing_start, wing_tip);
  graphics_draw_line(ctx, wing_tip, wing_end);

  int leg_offset = (chicken.radius * cos_lookup(chicken.leg_angle))/TRIG_MAX_RATIO;
  int leg_seperation = (chicken.radius * sin_lookup(chicken.leg_angle))/TRIG_MAX_RATIO;
  GPoint left_leg_start = GPoint(chicken.x - leg_seperation, chicken.y + leg_offset);
  GPoint left_leg_end = GPoint(chicken.x - leg_seperation, chicken.y + leg_offset + chicken.leg_size);
  GPoint right_leg_start = GPoint(chicken.x + leg_seperation, chicken.y + leg_offset);
  GPoint right_leg_end = GPoint(chicken.x + leg_seperation, chicken.y + leg_offset + chicken.leg_size);

  graphics_draw_line(ctx, left_leg_start, left_leg_end);
  graphics_draw_line(ctx, right_leg_start, right_leg_end);

  graphics_draw_circle(ctx, origin, chicken.radius);
}

static void chicken_layer_create(){
  chicken_layer = layer_create(GRect(0, 50, 144, 95));
  layer_set_update_proc(chicken_layer, chicken_draw);
}

static void chicken_layer_destroy(){
  layer_destroy(chicken_layer);
}

static void egg_animation_draw(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint origin = GPoint(bounds.origin.x + bounds.size.w/2, bounds.origin.y + bounds.size.h/2);

  GColor color;
  if (connection_status) {
    color = GColorBlack;
  } else {
    color = GColorWhite;
  }
  graphics_context_set_fill_color(ctx, color);
  graphics_fill_circle(ctx, origin, chicken.egg_radius - 1);
  graphics_draw_circle(ctx, origin, chicken.egg_radius - 1);
}

static void egg_animation_layer_create(){
  egg_origin = GRect(
      144,
      chicken.y + 2 * chicken.radius + chicken.leg_size - 2 * chicken.egg_radius,
      2 * chicken.egg_radius,
      2 * chicken.egg_radius);
  egg_animation_layer = layer_create(egg_origin);
  layer_set_update_proc(egg_animation_layer, egg_animation_draw);
}

static void egg_animation_layer_destroy(){
  layer_destroy(egg_animation_layer);
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
  egg_animation_layer_create();

  update();

  layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(time_layer));
  layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(date_layer));
  layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(week_layer));
  layer_add_child(window_get_root_layer(main_window), battery_layer);
  layer_add_child(window_get_root_layer(main_window), chicken_layer);
  layer_add_child(window_get_root_layer(main_window), egg_animation_layer);
}

static void main_window_unload(){
  chicken_layer_destroy();
  battery_layer_destroy();
  week_layer_destroy();
  date_layer_destroy();
  time_layer_destroy();
  egg_animation_layer_destroy();
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  update();
}

static void connection_handler(bool connected){
  connection_status = connected;
}

static void battery_handler(BatteryChargeState state){
  battery_level = state.charge_percent;
  layer_mark_dirty(battery_layer);
}

static void egg_animation_started(Animation *animation, void *data){
  /**/
}

static void egg_animation_stopped(Animation *animation, bool finished, void *data){
  /**/
  property_animation_destroy((PropertyAnimation *)animation);

  if (!finished){
    layer_set_frame(egg_animation_layer, egg_origin);
    layer_mark_dirty(egg_animation_layer);
  }
}

static void tap_handler(AccelAxisType axis, int32_t diretion){
  GRect from_frame = GRect(chicken.x, egg_origin.origin.y, egg_origin.size.w, egg_origin.size.h);
  GRect to_frame = egg_origin;

  egg_animation = property_animation_create_layer_frame(egg_animation_layer, &from_frame, &to_frame);
  animation_set_handlers((Animation *) egg_animation, (AnimationHandlers) {
      .started = (AnimationStartedHandler) egg_animation_started,
      .stopped = (AnimationStoppedHandler) egg_animation_stopped
  }, NULL);
  animation_set_duration((Animation *) egg_animation, 2000);

  animation_schedule((Animation*) egg_animation);
}

static void init(){
  main_window = window_create();

  window_set_window_handlers(main_window, (WindowHandlers) {
      .load   = main_window_load,
      .unload = main_window_unload
  });

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  bluetooth_connection_service_subscribe(connection_handler);
  battery_state_service_subscribe(battery_handler);
  accel_tap_service_subscribe(tap_handler);

  window_stack_push(main_window, true);

  connection_handler(bluetooth_connection_service_peek());
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
