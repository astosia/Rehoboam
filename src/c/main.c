#include <pebble.h>
#include "main.h"
#include "weekday.h"
#include "effect_layer.h"

//#ifndef PBL_PLATFORM_APLITE
  #include <pebble-fctx/fctx.h>
  #include <pebble-fctx/fpath.h>
  #include <pebble-fctx/ffont.h>
  #define ROUND_VERTICAL_PADDING 15
//#endif

//Static and initial vars
static GFont
  FontDate,
//  FontDate2,
//  FontSunset,
//  FontWeather,
//  FontBattery,
//  FontIcon,
  FontIcon2;
//  FontIcon3,
//  FontCity,
//  FontWeatherIcons,
//  FontWind;
char
  sunsetstring[8],
  sunrisestring[8],
  freetextstring[16];

  FFont* time_font;
  #ifndef PBL_PLATFORM_APLITE
    FFont* info_font;
  #endif

static Window * s_window;

static Layer * s_picture_bitmap_layer;
static Layer * s_canvas_bt_icon;
static Layer * s_canvas_qt_icon;
  Layer * time_area_layer;
  #ifndef PBL_PLATFORM_APLITE
    Layer * info_area_layer;
    Layer * text_area_layer;
  #else
    static Layer * aplite_info_layer;
  #endif

static GBitmap *s_background_picture;

static int s_hours, s_minutes, s_day, s_month, s_loop, s_countdown;

//////Init Configuration///
//Init Clay
ClaySettings settings;
// Initialize the default settings
static void prv_default_settings(){
  settings.Back1Color = GColorWhite;
  settings.Back1ColorN = GColorWhite;
  settings.Text1Color = GColorBlack;
  settings.Text2Color = GColorBlack;
  settings.Text3Color = GColorBlack;
  settings.Text4Color = GColorBlack;
  settings.Text6Color = GColorBlack;
  settings.HourColor = GColorBlack;
  settings.MinColor = GColorBlack;
  settings.HourColorN = GColorBlack;
  settings.MinColorN = GColorBlack;
  settings.Text1ColorN = GColorBlack;
  settings.Text2ColorN = GColorBlack;
  settings.Text3ColorN = GColorBlack;
  settings.Text4ColorN = GColorBlack;
  settings.Text6ColorN = GColorBlack;
  settings.NightTheme = false;
  settings.Invert = false;
  settings.InvertNight = false;
  settings.Freetext ="DIVERGENCE";
}
int HourSunrise=700;
int HourSunset=2200;
int moonphase=0;
bool BTOn=true;
bool GPSOn=true;
bool IsNightNow=false;
bool IsInvertedNow=false;

//////End Configuration///
///////////////////////////


static GColor ColorSelect(GColor ColorDay, GColor ColorNight){
  if (settings.NightTheme && IsNightNow && GPSOn ){
    return ColorNight;
  }
  else{
    return ColorDay;
  }
}

// Callback for js request
void request_watchjs(){
  //Starting loop at 0
  s_loop = 0;
  // Begin dictionary
  DictionaryIterator * iter;
  app_message_outbox_begin( & iter);
  // Add a key-value pair
  dict_write_uint8(iter, 0, 0);
  // Send the message!
  app_message_outbox_send();
}

static void quiet_time_icon () {
  if(quiet_time_is_active()) {
  layer_set_hidden(s_canvas_qt_icon,false);
} else {
  layer_set_hidden(s_canvas_qt_icon,true);
}
}

///BT Connection
static void bluetooth_callback(bool connected){
  BTOn = connected;
}

static void bluetooth_vibe_icon (bool connected) {
  layer_set_hidden(s_canvas_bt_icon, connected);
//  layer_set_hidden(s_canvas_sunset_icon, !connected);

  if(!connected && !quiet_time_is_active()) {
    // Issue a vibrating alert
    vibes_double_pulse();
  }
}

static void update_background_picture() {
  if ((settings.InvertNight ==true) && (IsNightNow == true)){
      IsInvertedNow = true;
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Invert on at night");
            s_background_picture = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_BACKGROUNDINV);
    } else if ((settings.Invert == true) && (IsNightNow == false)){
      IsInvertedNow = true;
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Invert on in day");
            s_background_picture = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_BACKGROUNDINV);
    } else {
      IsInvertedNow = false;
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Invert off");
            s_background_picture = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_BACKGROUND);
    }

    APP_LOG(APP_LOG_LEVEL_DEBUG, "HourSunrise %d", HourSunrise);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "HourSunset %d", HourSunset);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "IsNightNow status %d", IsNightNow);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Invert day status %d", settings.Invert);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Invert night status %d", settings.InvertNight);

}

static void onreconnection(bool before, bool now){
  if (!before && now){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "BT reconnected, requesting weather at %d", s_minutes);
    request_watchjs();
  }
}


void layer_update_proc_background (Layer * back_layer, GContext * ctx){
  GRect bounds = layer_get_bounds(back_layer);
  GRect Backgroundrect = GRect(0, 0, bounds.size.w, bounds.size.h);
  graphics_context_set_fill_color(ctx,ColorSelect(settings.Back1Color, settings.Back1ColorN));
  graphics_fill_rect(ctx, Backgroundrect,0,GCornersAll);

  gbitmap_destroy(s_background_picture);
  update_background_picture();
  GRect bitmap_bounds = gbitmap_get_bounds(s_background_picture);
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, s_background_picture, bitmap_bounds);

///Underline colour
  graphics_context_set_stroke_color(ctx, ColorSelect(settings.Text2Color, settings.Text2ColorN));
  GPoint start = PBL_IF_ROUND_ELSE (GPoint (42,66),GPoint (31,62));
  GPoint end = PBL_IF_ROUND_ELSE (GPoint (89,66),GPoint (71,62));
  graphics_draw_line(ctx, start, end);
}

//#ifndef PBL_PLATFORM_APLITE
void update_time_area_layer(Layer *l, GContext* ctx7) {
  // check layer bounds
  GRect bounds = layer_get_unobstructed_bounds(l);

  #ifdef PBL_ROUND
 //   bounds = GRect(0, ROUND_VERTICAL_PADDING, bounds.size.w, bounds.size.h - ROUND_VERTICAL_PADDING * 2);
     bounds = GRect(0, 0,bounds.size.w, bounds.size.h);
  #else
     bounds = GRect(0,0,bounds.size.w, bounds.size.h);
  #endif

  // initialize FCTX, the fancy 3rd party drawing library that all the cool kids use
  FContext fctx;

  fctx_init_context(&fctx, ctx7);
  fctx_set_color_bias(&fctx, 0);
  fctx_set_fill_color(&fctx, ColorSelect(settings.HourColor, settings.HourColorN));

#ifdef PBL_ROUND
  //int font_size = bounds.size.h * 0.55;
  int font_size = 60;
 #elif PBL_PLATFORM_APLITE
  //int font_size = bounds.size.h * 0.65;
  int font_size = 54;
 #else
  int font_size = 54;
  #endif
  int h_adjust = 0;
  int v_adjust = 0;

    #ifdef PBL_COLOR
      fctx_enable_aa(true);
    #endif

  // if it's a round watch, EVERYTHING CHANGES
  #ifdef PBL_ROUND
//    v_adjust = ROUND_VERTICAL_PADDING;
    v_adjust = 0;

  #else
    // for rectangular watches, adjust X position based on sidebar position
  //    h_adjust -= ACTION_BAR_WIDTH / 2 + 1;
   h_adjust = 0;
  #endif

  FPoint time_pos;
  fctx_begin_fill(&fctx);
  fctx_set_text_em_height(&fctx, time_font, font_size);
  fctx_set_color_bias(&fctx,0);
//  fctx_set_text_em_height(&fctx, minutes_font, font_size);

  int hourdraw;
  char hournow[3];
  if (clock_is_24h_style()){
    hourdraw=s_hours;
    snprintf(hournow,sizeof(hournow),"%02d",hourdraw);
    }
  else {
    if (s_hours==0 || s_hours==12){
      hourdraw=12;
    }
    else hourdraw=s_hours%12;
  snprintf(hournow, sizeof(hournow), "%d", hourdraw);
   }

  int mindraw;
  mindraw = s_minutes;
  char minnow[3];
  snprintf(minnow, sizeof(minnow), "%02d", mindraw);

  char timedraw[6];
  snprintf(timedraw, sizeof(timedraw), "%s:%s",hournow,minnow);

  time_pos.x = INT_TO_FIXED(PBL_IF_ROUND_ELSE(40, 28) + h_adjust);
  time_pos.y = INT_TO_FIXED(PBL_IF_ROUND_ELSE(89, 82)  + v_adjust);

  fctx_set_offset(&fctx, time_pos);
  fctx_draw_string(&fctx, timedraw, time_font, GTextAlignmentLeft, FTextAnchorMiddle);
  fctx_end_fill(&fctx);

  fctx_deinit_context(&fctx);
}
//#endif

#ifndef PBL_PLATFORM_APLITE
void update_info_area_layer(Layer *a, GContext* ctx8) {
  // check layer bounds
 GRect bounds = layer_get_unobstructed_bounds(a);

  #ifdef PBL_ROUND
  bounds = GRect(0,0,bounds.size.w, bounds.size.h);
#else
 bounds = GRect(0,0,bounds.size.w,bounds.size.h);
  #endif
  // initialize FCTX, the fancy 3rd party drawing library that all the cool kids use
  FContext fctx1;

  fctx_init_context(&fctx1, ctx8);
  fctx_set_color_bias(&fctx1, 0);
  fctx_set_fill_color(&fctx1, ColorSelect(settings.MinColor, settings.MinColorN));


  // calculate font size
 #ifdef PBL_ROUND
//int font_size = bounds.size.h * 0.55;
int font_size2 = 18;
#else
//int font_size = bounds.size.h * 0.65;
int font_size2 = 16;
  #endif
  int h_adjust2 = 0;
  int v_adjust2 = 0;


    #ifdef PBL_COLOR
      fctx_enable_aa(true);
    #endif

  // if it's a round watch, EVERYTHING CHANGES
  #ifdef PBL_ROUND
 //   v_adjust2 = ROUND_VERTICAL_PADDING;
     v_adjust2 = 0;

  #else
    // for rectangular watches, adjust X position based on sidebar position
 //     h_adjust2 -= ACTION_BAR_WIDTH / 2 + 1;
     h_adjust2 = 0;
  #endif

  FPoint time_pos2;
  fctx_begin_fill(&fctx1);
  fctx_set_text_em_height(&fctx1, info_font, font_size2);

  int datedraw;
  datedraw = s_day;
  char datenow[3];
  snprintf(datenow, sizeof(datenow), "%02d", datedraw);

  int monthdraw;
  monthdraw = (s_month + 1);
  char monthnow[3];
  snprintf(monthnow, sizeof(monthnow), "%02d", monthdraw);

  int battery_level = battery_state_service_peek().charge_percent;
  char battperc[4];
  snprintf(battperc, sizeof(battperc), "%d", battery_level);

  char information[10];
  snprintf(information, sizeof(information), "%s.%s.%s",datenow,monthnow,battperc);

  time_pos2.x = INT_TO_FIXED(PBL_IF_ROUND_ELSE(41, 29) + h_adjust2);
  time_pos2.y = INT_TO_FIXED(PBL_IF_ROUND_ELSE(55 , 54)  + v_adjust2);

  fctx_set_offset(&fctx1, time_pos2);
  fctx_draw_string(&fctx1, information, info_font, GTextAlignmentLeft, FTextAnchorMiddle);
  fctx_end_fill(&fctx1);

  fctx_deinit_context(&fctx1);
}
#endif

#ifndef PBL_PLATFORM_APLITE
 void update_text_area_layer(Layer *a, GContext* ctx8) {
// check layer bounds
 GRect bounds = layer_get_unobstructed_bounds(a);

  #ifdef PBL_ROUND
 //   bounds = GRect(0, ROUND_VERTICAL_PADDING, bounds.size.w, bounds.size.h - ROUND_VERTICAL_PADDING * 2);
 bounds = GRect(0,0,bounds.size.w, bounds.size.h);
#else
 bounds = GRect(0,0,bounds.size.w,bounds.size.h);
  #endif
  // initialize FCTX, the fancy 3rd party drawing library that all the cool kids use
  FContext fctx1;

  fctx_init_context(&fctx1, ctx8);
  fctx_set_color_bias(&fctx1, 0);
  fctx_set_fill_color(&fctx1, ColorSelect(settings.Text1Color, settings.Text1ColorN));


  // calculate font size
#ifdef PBL_ROUND
//int font_size = bounds.size.h * 0.55;
int font_size2 = 18;
//#elif PBL_PLATFORM_APLITE
 //int font_size = bounds.size.h * 0.65;
// int font_size2 = 12;
#else
//int font_size = bounds.size.h * 0.65;
int font_size2 = 16;
  #endif
  int h_adjust2 = 0;
  int v_adjust2 = 0;


    #ifdef PBL_COLOR
      fctx_enable_aa(true);
    #endif

  // if it's a round watch, EVERYTHING CHANGES
  #ifdef PBL_ROUND
 //   v_adjust2 = ROUND_VERTICAL_PADDING;
     v_adjust2 = 0;

  #else
    // for rectangular watches, adjust X position based on sidebar position
     h_adjust2 = 0;
  #endif

  FPoint time_pos3;
  fctx_begin_fill(&fctx1);
  fctx_set_text_em_height(&fctx1, time_font, font_size2);

  time_pos3.x = INT_TO_FIXED(PBL_IF_ROUND_ELSE(42, 30) + h_adjust2);
  time_pos3.y = INT_TO_FIXED(PBL_IF_ROUND_ELSE(123 , 113)  + v_adjust2);

  fctx_set_offset(&fctx1, time_pos3);
  fctx_draw_string(&fctx1, "DIVERGENCE", time_font, GTextAlignmentLeft, FTextAnchorMiddle);
  fctx_end_fill(&fctx1);

  fctx_deinit_context(&fctx1);
}
#endif

////////////////////////////////////////////////
////////////////////////////////////////////////
#ifdef PBL_PLATFORM_APLITE
static void layer_update_proc_aplite_info(Layer * layer3, GContext * ctx){

GRect DivergenceRect =
  GRect (30,100,100,20);

GRect InfoRect =
  GRect (30,54-12,100,20);

  int datedraw;
  datedraw = s_day;
  char datenow[4];
  snprintf(datenow, sizeof(datenow), "%02d", datedraw);

  int monthdraw;
  monthdraw = (s_month + 1);
  char monthnow[4];
  snprintf(monthnow, sizeof(monthnow), "%02d", monthdraw);

  int battery_level = battery_state_service_peek().charge_percent;
  char battperc[5];
  snprintf(battperc, sizeof(battperc), "%d", battery_level);

  char information[12];
  snprintf(information, sizeof(information), "%s.%s.%s",datenow,monthnow,battperc);

graphics_context_set_text_color(ctx, ColorSelect(settings.Text1Color, settings.Text1ColorN));
graphics_draw_text(ctx, "DIVERGENCE", FontDate, DivergenceRect, GTextOverflowModeFill,GTextAlignmentLeft, NULL);
graphics_context_set_text_color(ctx, ColorSelect(settings.MinColor, settings.MinColorN));
graphics_draw_text(ctx, information, FontDate, InfoRect, GTextOverflowModeFill,GTextAlignmentLeft, NULL);

}
#endif
//////////////////////////////////////////////////
/////////////////////////////////////////////////


static void layer_update_proc_bt(Layer * layer3, GContext * ctx3){
   // Create Rects
  GRect BTIconRect =
    (PBL_IF_ROUND_ELSE(
      GRect(76,25,28,20),
      GRect(63,30,28,20)));

 onreconnection(BTOn, connection_service_peek_pebble_app_connection());
 bluetooth_callback(connection_service_peek_pebble_app_connection());

 graphics_context_set_text_color(ctx3, ColorSelect(settings.Text3Color, settings.Text3ColorN));
 graphics_draw_text(ctx3, "z", FontIcon2, BTIconRect, GTextOverflowModeFill,GTextAlignmentCenter, NULL);
}

static void layer_update_proc_qt(Layer * layer4, GContext * ctx4){

  quiet_time_icon();
   // Create Rects
  GRect QTIconRect =
    (PBL_IF_ROUND_ELSE(
      GRect(76,140,28,20),
      GRect(63,126,28,20)));



 graphics_context_set_text_color(ctx4, ColorSelect(settings.Text3Color, settings.Text3ColorN));
 graphics_draw_text(ctx4, "\U0000E061", FontIcon2, QTIconRect, GTextOverflowModeFill,GTextAlignmentCenter, NULL);
}

/////////////////////////////////////////
////Init: Handle Settings and Weather////
/////////////////////////////////////////
// Read settings from persistent storage
static void prv_load_settings(){
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, & settings, sizeof(settings));
}
// Save the settings to persistent storage
static void prv_save_settings(){
  persist_write_data(SETTINGS_KEY, & settings, sizeof(settings));
}
// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator * iter, void * context){
  s_loop = s_loop + 1;
  if (s_loop == 1){
  }
  ////////////
  Tuple * bg1_color_t = dict_find(iter, MESSAGE_KEY_Back1Color);
  if (bg1_color_t){
    settings.Back1Color = GColorFromHEX(bg1_color_t-> value -> int32);
  }
  Tuple * nbg1_color_t = dict_find(iter, MESSAGE_KEY_Back1ColorN);
  if (nbg1_color_t){
    settings.Back1ColorN = GColorFromHEX(nbg1_color_t-> value -> int32);
  }
  Tuple * tx1_color_t = dict_find(iter, MESSAGE_KEY_Text1Color);
  if (tx1_color_t){
    settings.Text1Color = GColorFromHEX(tx1_color_t-> value -> int32);
  }
  Tuple * ntx1_color_t = dict_find(iter, MESSAGE_KEY_Text1ColorN);
  if (ntx1_color_t){
    settings.Text1ColorN = GColorFromHEX(ntx1_color_t-> value -> int32);
  }
  ///////////////////////////////
  Tuple * tm_color_t = dict_find(iter, MESSAGE_KEY_HourColor);
  if (tm_color_t){
    settings.HourColor = GColorFromHEX(tm_color_t-> value -> int32);
  }
  Tuple * nttm_color_t = dict_find(iter, MESSAGE_KEY_HourColorN);
  if (nttm_color_t){
    settings.HourColorN = GColorFromHEX(nttm_color_t-> value -> int32);
  }
  Tuple * info_color_t = dict_find(iter, MESSAGE_KEY_MinColor);
  if (info_color_t){
    settings.MinColor = GColorFromHEX(info_color_t-> value -> int32);
  }
  Tuple * ntinfo_color_t = dict_find(iter, MESSAGE_KEY_MinColorN);
  if (ntinfo_color_t){
    settings.MinColorN = GColorFromHEX(ntinfo_color_t-> value -> int32);
  }
  ///////////////////////////////
  Tuple * tx2_color_t = dict_find(iter, MESSAGE_KEY_Text2Color);
  if (tx2_color_t){
    settings.Text2Color = GColorFromHEX(tx2_color_t-> value -> int32);
  }
  Tuple * ntx2_color_t = dict_find(iter, MESSAGE_KEY_Text2ColorN);
  if (ntx2_color_t){
    settings.Text2ColorN = GColorFromHEX(ntx2_color_t-> value -> int32);
  }
   Tuple * tx3_color_t = dict_find(iter, MESSAGE_KEY_Text3Color);
  if (tx3_color_t){
    settings.Text3Color = GColorFromHEX(tx3_color_t-> value -> int32);
  }
  Tuple * ntx3_color_t = dict_find(iter, MESSAGE_KEY_Text3ColorN);
  if (ntx3_color_t){
    settings.Text3ColorN = GColorFromHEX(ntx3_color_t-> value -> int32);
  }
  Tuple * tx4_color_t = dict_find(iter,MESSAGE_KEY_Text4Color);
  if (tx4_color_t){
    settings.Text4Color = GColorFromHEX(tx4_color_t-> value -> int32);
    }
  Tuple * ntx4_color_t = dict_find(iter, MESSAGE_KEY_Text4ColorN);
  if(ntx4_color_t){
    settings.Text4ColorN = GColorFromHEX(ntx4_color_t-> value -> int32);
  }
   Tuple * tx6_color_t = dict_find(iter,MESSAGE_KEY_Text6Color);
  if (tx6_color_t){
    settings.Text6Color = GColorFromHEX(tx6_color_t-> value -> int32);
    }
  Tuple * ntx6_color_t = dict_find(iter, MESSAGE_KEY_Text6ColorN);
  if(ntx6_color_t){
    settings.Text6ColorN = GColorFromHEX(ntx6_color_t-> value -> int32);
  }
   //Control of data from http
    //Hour Sunrise and Sunset
  Tuple * sunrise_it = dict_find(iter, MESSAGE_KEY_HourSunrise);
  if (sunrise_it){
    HourSunrise = (int) sunrise_it -> value -> int32;
  }
  Tuple * sunset_it = dict_find(iter, MESSAGE_KEY_HourSunset);
  if (sunset_it){
    HourSunset = (int) sunset_it -> value -> int32;
  }
  Tuple * sunset_dt = dict_find(iter, MESSAGE_KEY_WEATHER_SUNSET_KEY);
  if (sunset_dt){
     snprintf(sunsetstring, sizeof(sunsetstring), "%s", sunset_dt -> value -> cstring);
  }

   Tuple * sunrise_dt = dict_find(iter, MESSAGE_KEY_WEATHER_SUNRISE_KEY);
  if (sunrise_dt){
     snprintf(sunrisestring, sizeof(sunrisestring), "%s", sunrise_dt -> value -> cstring);
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "HourSunrise %d", HourSunrise);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "HourSunset %d", HourSunset);

  Tuple * inv_t = dict_find(iter, MESSAGE_KEY_Invert);
  if (inv_t){
    if (inv_t -> value -> int32 == 0){
      settings.Invert = false;
  //    APP_LOG(APP_LOG_LEVEL_DEBUG, "Invert off");
    } else
          settings.Invert = true;
  //        APP_LOG(APP_LOG_LEVEL_DEBUG, "Invert on");
  }
  Tuple * inv_nt = dict_find(iter, MESSAGE_KEY_InvertNight);
  if (inv_nt){
    if (inv_nt -> value -> int32 == 0){
      settings.InvertNight = false;
  //    APP_LOG(APP_LOG_LEVEL_DEBUG, "InvertNight off");
    } else
          settings.InvertNight = true;
  //        APP_LOG(APP_LOG_LEVEL_DEBUG, "InvertNight on");
  }
  Tuple * disntheme_t = dict_find(iter, MESSAGE_KEY_NightTheme);
  if (disntheme_t){
    if (disntheme_t -> value -> int32 == 0){
      settings.NightTheme = false;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "NTHeme off");
    } else settings.NightTheme = true;
  }
  //Update colors
  layer_mark_dirty(s_picture_bitmap_layer);
  layer_mark_dirty(s_canvas_bt_icon);
  layer_mark_dirty(s_canvas_qt_icon);
  layer_mark_dirty(time_area_layer);
#ifndef PBL_PLATFORM_APLITE
  layer_mark_dirty(info_area_layer);
  layer_mark_dirty(text_area_layer);
  #endif
  #ifdef PBL_PLATFORM_APLITE
  layer_mark_dirty(aplite_info_layer);
#endif
  // Save the new settings to persistent storage
  prv_save_settings();
}

//Load main layer
static void window_load(Window * window){
  Layer * window_layer = window_get_root_layer(window);
  GRect bounds4 = layer_get_bounds(window_layer);

  s_picture_bitmap_layer = layer_create(bounds4);
    layer_set_update_proc (s_picture_bitmap_layer, layer_update_proc_background);
    layer_add_child(window_layer,s_picture_bitmap_layer);

  time_area_layer = layer_create(bounds4);
     layer_add_child(window_layer, time_area_layer);
     layer_set_update_proc(time_area_layer, update_time_area_layer);

#ifndef PBL_PLATFORM_APLITE
 info_area_layer = layer_create(bounds4);
     layer_add_child(window_layer, info_area_layer);
     layer_set_update_proc(info_area_layer, update_info_area_layer);


 text_area_layer = layer_create(bounds4);
     layer_add_child(window_layer, text_area_layer);
     layer_set_update_proc(text_area_layer, update_text_area_layer);
#endif

#ifdef PBL_PLATFORM_APLITE

   aplite_info_layer = layer_create(bounds4);
     layer_set_update_proc(aplite_info_layer, layer_update_proc_aplite_info);
     layer_add_child(window_layer, aplite_info_layer);

#endif

  s_canvas_bt_icon = layer_create(bounds4);
    layer_set_update_proc (s_canvas_bt_icon, layer_update_proc_bt);
    layer_add_child(window_layer, s_canvas_bt_icon);

  s_canvas_qt_icon = layer_create(bounds4);
//    layer_set_hidden(s_canvas_qt_icon,true);
    layer_set_update_proc (s_canvas_qt_icon, layer_update_proc_qt);
    layer_add_child(window_layer, s_canvas_qt_icon);
}


static void window_unload(Window * window){
  layer_destroy (s_picture_bitmap_layer);
  gbitmap_destroy(s_background_picture);
  layer_destroy(time_area_layer);
  #ifndef PBL_PLATFORM_APLITE
    layer_destroy(info_area_layer);
    layer_destroy(text_area_layer);
  #endif
  #ifdef PBL_PLATFORM_APLITE
    layer_destroy(aplite_info_layer);
  #endif
  layer_destroy(s_canvas_bt_icon);
  layer_destroy(s_canvas_qt_icon);
  window_destroy(s_window);
//  fonts_unload_custom_font(FontIcon);
  fonts_unload_custom_font(FontIcon2);
//  fonts_unload_custom_font(FontIcon3);
//  fonts_unload_custom_font(FontWeatherIcons);
  ffont_destroy(time_font);
  #ifndef PBL_PLATFORM_APLITE
    ffont_destroy(info_font);
  #endif
}

void main_window_push(){
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);

}

void main_window_update(int hours, int minutes, int month, int day){
  s_hours = hours;
  s_minutes = minutes;
  s_day = day;
  s_month = month;

  layer_mark_dirty(s_picture_bitmap_layer);
  layer_mark_dirty(s_canvas_bt_icon);
  layer_mark_dirty(s_canvas_qt_icon);
  layer_mark_dirty(time_area_layer);
    #ifndef PBL_PLATFORM_APLITE
      layer_mark_dirty(info_area_layer);
      layer_mark_dirty(text_area_layer);
      #endif
      #ifdef PBL_PLATFORM_APLITE
  //    #else
      layer_mark_dirty(aplite_info_layer);
    #endif
}

static void tick_handler(struct tm * time_now, TimeUnits changed){

  main_window_update(time_now -> tm_hour, time_now -> tm_min, time_now  -> tm_mon, time_now -> tm_mday);
  //update_time();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Tick at %d", time_now -> tm_min);
  s_loop = 0;
  if (s_countdown == 0){
    //Reset
    s_countdown = 60;
  } else{
    s_countdown = s_countdown - 1;
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Countdown to update %d", s_countdown);
  // Evaluate if is day or night
  int nowthehouris = s_hours * 100 + s_minutes;
  if (HourSunrise <= nowthehouris && nowthehouris <= HourSunset){
    IsNightNow = false;
  } else{
    IsNightNow = true;
  }
  // Extra catch on sunrise and sunset
  if (nowthehouris == HourSunrise || nowthehouris == HourSunset){
    s_countdown = 1;
  }
  if (GPSOn && settings.NightTheme){
    //Extra catch around 1159 to gather information of today
    if (nowthehouris == 1159 && s_countdown > 5){
      s_countdown = 1;
    };

    // Change Colors & background when turns to nighttime
    layer_mark_dirty(s_picture_bitmap_layer);
    layer_mark_dirty(s_canvas_bt_icon);
    layer_mark_dirty(s_canvas_qt_icon);
    layer_mark_dirty(time_area_layer);
    #ifndef PBL_PLATFORM_APLITE
        layer_mark_dirty(info_area_layer);
        layer_mark_dirty(text_area_layer);
    #endif
    #ifdef PBL_PLATFORM_APLITE
        layer_mark_dirty(aplite_info_layer);
    #endif
  }
  // Get weather update every requested minutes and extra request 5 minutes earlier
  if (s_countdown == 0 || s_countdown == 5){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Update weather at %d", time_now -> tm_min);
      request_watchjs();
  }
  //If GPS was off request weather every 15 minutes
  if (!GPSOn){
          if (s_countdown % 15 == 0){
          APP_LOG(APP_LOG_LEVEL_DEBUG, "Attempt to request GPS on %d", time_now -> tm_min);
          request_watchjs();
        }

    }
 }

static void init(){

  prv_load_settings();
  // Listen for AppMessages
  //Starting loop at 0
  s_loop = 0;
  s_countdown = 60;

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  s_hours=t->tm_hour;
  s_minutes=t->tm_min;
  s_day=t->tm_mday;
  s_month=t->tm_mon;

  //Register and open
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(512, 512);
  // Load Resources (fonts & picutures)
  update_background_picture();
  time_font = ffont_create_from_resource(RESOURCE_ID_FONT_DINCONBOLD);
  #ifndef PBL_PLATFORM_APLITE
    info_font = ffont_create_from_resource(RESOURCE_ID_FONT_DINBOLD);
  #endif
  FontDate = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  //FontDate2 = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  //FontBattery= fonts_get_system_font(FONT_KEY_GOTHIC_14);
  //FontSunset = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
//  FontIcon = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHERICONS_20));
  FontIcon2 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DRIPICONS_16));
//  FontIcon3 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DRIPICONS_18));
  //FontWeather = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  //FontCity = PBL_IF_ROUND_ELSE(fonts_get_system_font(FONT_KEY_GOTHIC_14),fonts_get_system_font(FONT_KEY_GOTHIC_09));
  //FontWeatherIcons = fonts_load_custom_font(resource_get_handle(PBL_IF_ROUND_ELSE(RESOURCE_ID_FONT_WEATHERICONS_12,RESOURCE_ID_FONT_WEATHERICONS_16)));
  //FontWind = fonts_get_system_font(FONT_KEY_GOTHIC_18);

  main_window_push();
  // Register with Event Services
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  connection_service_subscribe((ConnectionHandlers){
    .pebble_app_connection_handler = bluetooth_vibe_icon
  });
  bluetooth_vibe_icon(connection_service_peek_pebble_app_connection());
}
static void deinit(){
  tick_timer_service_unsubscribe();
  app_message_deregister_callbacks(); //Destroy the callbacks for clean up
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
}
int main(){
  init();
  app_event_loop();
  deinit();
}
