#include <pebble.h>

#include "effects.h"

#ifdef PBL_COLOR
static void blur_(const GBitmap *fb, const GRect fb_bounds, uint16_t row, uint16_t x_start, uint16_t x_end, uint8_t *dest, uint8_t radius){
  uint16_t total[3];
  uint8_t  nb_points = 0;

  for (uint16_t x = x_start; x < x_end; ++x) {
    total[0] = total[1] = total[2] = 0;
    nb_points = 0;
    for (int16_t ky = row - radius; ky <= row + radius; ++ky){
      if(ky>=0 && ky<fb_bounds.size.h){
        GBitmapDataRowInfo row_info = gbitmap_get_data_row_info(fb, ky);
        for (int16_t kx = x - radius; kx <= x + radius; ++kx){
          if(row_info.min_x <= kx && kx <= row_info.max_x)
          {
            GColor8 color = (GColor8)*(row_info.data + kx);
            total[0] += color.r;
            total[1] += color.g;
            total[2] += color.b;
            nb_points++;
          }
        }
      }
    }
    total[0] = (total[0] * 0x55) / nb_points;
    total[1] = (total[1] * 0x55) / nb_points;
    total[2] = (total[2] * 0x55) / nb_points;
    dest[x-x_start] = GColorFromRGB(total[0], total[1], total[2]).argb; 
  }
}
#endif

#define max(a,b) a>b?a:b
#define min(a,b) a>b?b:a

void effect_blur(GContext* ctx, GRect position, void* param){
#ifdef PBL_COLOR
  //capturing framebuffer bitmap
  GBitmap *fb = graphics_capture_frame_buffer(ctx);
  GRect fb_bounds = gbitmap_get_bounds(fb);

  uint8_t radius = (uint8_t)(uint32_t)param; // Not very elegant... sorry
  uint16_t offset_x = position.origin.x;
  uint16_t offset_y = position.origin.y;
  uint16_t width    = position.size.w;
  uint16_t height   = position.size.h;
  
  uint8_t *buffer               = malloc(width * (radius + 1));
  GBitmapDataRowInfo* row_infos = malloc(sizeof(GBitmapDataRowInfo) * (radius + 1));
  uint8_t circular_index = 0;

  uint16_t h=0;
  for(; h<radius+1; h++){
    row_infos[h] = gbitmap_get_data_row_info(fb, offset_y+h);
    row_infos[h].min_x = max(row_infos[h].min_x, offset_x);
    row_infos[h].max_x = min(offset_x + width, row_infos[h].max_x);
    blur_(fb, fb_bounds, offset_y+h, row_infos[h].min_x, row_infos[h].max_x, buffer + h*width, radius);
  }

  for(; h<height; h++){
    memcpy(row_infos[circular_index].data + row_infos[circular_index].min_x, buffer + circular_index * width, row_infos[circular_index].max_x - row_infos[circular_index].min_x);
    row_infos[circular_index] = gbitmap_get_data_row_info(fb, offset_y+h);
    row_infos[circular_index].min_x = max(row_infos[circular_index].min_x, offset_x);
    row_infos[circular_index].max_x = min(offset_x + width, row_infos[circular_index].max_x);
    blur_(fb, fb_bounds, offset_y+h, row_infos[circular_index].min_x, row_infos[circular_index].max_x, buffer + circular_index*width, radius);
    circular_index = circular_index < radius ? circular_index + 1 : 0;
  }

  h=0;
  for(; h<radius; h++){
    memcpy(row_infos[circular_index].data + row_infos[circular_index].min_x, buffer + circular_index*width, row_infos[circular_index].max_x - row_infos[circular_index].min_x);
    circular_index = circular_index < radius ? circular_index + 1 : 0;
  }
  
  free(buffer);
  free(row_infos);
  
  graphics_release_frame_buffer(ctx, fb);
#endif
}