#ifndef GPU_H
   #define GPU_H
   #define PIXELS_H 144
   #define PIXELS_W 160
   typedef struct GPU {
      int window_width;
      int window_height;
      uint32_t *pixels;
      SDL_Texture* framebuffer;
      SDL_Window*  window;
      SDL_Renderer* rend;
   } GPU;
   void update_window(GPU *gpu);
   GPU init_gpu();
   void destroy_gpu(GPU *gpu);
#endif
