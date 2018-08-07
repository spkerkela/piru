#include "damage_text.h"
DamageText damage_text[MAX_DAMAGE_TEXT];

void init_damage_text() {
  int i;
  for (i = 0; i < MAX_DAMAGE_TEXT; i++) {
    memset(&damage_text[i], 0, sizeof(DamageText));
  }
}

SDL_Texture *get_damage_text_texture(char *text, SDL_Color color, int width) {
  SDL_Surface *textSurface = TTF_RenderText_Solid(gFont, text, color);
  SDL_Texture *mTexture = NULL;
  if (textSurface == NULL) {
    printf("Unable to render text surface! SDL_ttf Error: %s\n",
           TTF_GetError());
  } else {
    // Create texture from surface pixels
    mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    if (mTexture == NULL) {
      printf("Unable to create texture from rendered text! SDL Error: %s\n",
             SDL_GetError());
    }
    // this is so we can fade out the damage text
    SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);

    SDL_Rect dest = {0, 0, 16 * width, 16};
    SDL_RenderCopy(gRenderer, mTexture, NULL, &dest);
    SDL_FreeSurface(textSurface);
  }
  return mTexture;
}

void create_damage_text(Point point, int damage, RGB_Color color) {
  char *str = calloc(10, sizeof(char));
  if (!str) {
    printf("str alloc failed\n");
    return;
  }
  sprintf(str, "%d", damage);
  int width = strlen(str);
  DamageText dt = {point.x, point.y, 0, 0, true, NULL};
  SDL_Color sdl_color;
  sdl_color.r = color.r;
  sdl_color.g = color.g;
  sdl_color.b = color.b;
  dt.texture = get_damage_text_texture(str, sdl_color, width);
  dt.width = width;
  free(str);
  push_damage_text(dt);
}

void free_damage_text(int i) {
  if (damage_text[i].texture) {
    free(damage_text[i].texture);
  }
}

void push_damage_text(DamageText damage) {
  int i;
  for (i = 0; i < MAX_DAMAGE_TEXT; i++) {
    if (!damage_text[i].alive) {
      free_damage_text(i);
      damage_text[i] = damage;
      break;
    }
  }
}

bool update_damage_text(int id) {
  static int frames_to_show = 700;
  if (damage_text[id].alive) {
    if (damage_text[id].frames_alive < frames_to_show) {
      int alpha = (int)(255 * (1 - (double)damage_text[id].frames_alive /
                                       (double)frames_to_show));
      damage_text[id].frames_alive += gClock.delta;
      damage_text[id].y_offset -= 3;
      SDL_SetTextureAlphaMod(damage_text[id].texture, alpha);
    } else {
      damage_text[id].alive = false;
    }

    return true;
  }
  return false;
}