#include <pebble.h>

static Window *window;
static Layer *clock_layer;
static Layer *hour_hand_layer;
static GBitmap *clock_numbers;
static GBitmap *hour_hand;
static BitmapLayer *clock_numbers_layer;

static int s_current_angle = 0;

static void clock_update_proc(Layer *this_layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(this_layer);
  GPoint center = GPoint((bounds.size.w / 2), (bounds.size.h / 2));
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, 67);
}

static void hour_hand_update_proc(Layer *this_layer, GContext *ctx) {
  GPoint src_ic = GPoint(9,32);

  GRect ctx_bounds = layer_get_bounds(this_layer);
  GPoint ctx_ic = grect_center_point(&ctx_bounds);

  int angle = (s_current_angle * TRIG_MAX_ANGLE) / 360;

  graphics_context_set_compositing_mode(ctx, GCompOpSet);

  graphics_draw_rotated_bitmap(ctx, hour_hand, src_ic, angle, ctx_ic);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create white circle layer for clock
  clock_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  layer_set_update_proc(clock_layer, clock_update_proc);
  layer_add_child(window_layer, clock_layer);

  // Create layer for clock numbers
  clock_numbers = gbitmap_create_with_resource(RESOURCE_ID_CLOCK_NUMBERS);
  clock_numbers_layer = bitmap_layer_create(GRect((bounds.size.w / 2) - 66, (bounds.size.h / 2) - 66, 134, 134));
  bitmap_layer_set_bitmap(clock_numbers_layer, clock_numbers);
  bitmap_layer_set_compositing_mode(clock_numbers_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(clock_numbers_layer));

  // Create hour hand
  hour_hand = gbitmap_create_with_resource(RESOURCE_ID_HOUR_HAND);
  hour_hand_layer = layer_create(GRect(0,0, bounds.size.w, bounds.size.h));
  layer_set_update_proc(hour_hand_layer, hour_hand_update_proc);
  layer_add_child(window_layer, hour_hand_layer);
}

static void window_unload(Window *window) {
  layer_destroy(clock_layer);
  layer_destroy(hour_hand_layer);
  gbitmap_destroy(clock_numbers);
  gbitmap_destroy(hour_hand);
  bitmap_layer_destroy(clock_numbers_layer);
}

static void timer_handler(void *context) {
  s_current_angle++;
  if(s_current_angle == 360) {
    s_current_angle = 0;
  }

  layer_mark_dirty(hour_hand_layer);
  app_timer_register(100, timer_handler, NULL);
}

static void init(void) {
  window = window_create();

  window_set_fullscreen(window, true);

  // Set default background color to GColorTiffanyBlue
  window_set_background_color(window, GColorTiffanyBlue);

  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  window_stack_push(window, true);

  app_timer_register(100, timer_handler, NULL);

 // tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
