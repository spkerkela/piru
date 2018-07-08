#ifdef _WIN32
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL2/SDL.H>
#include <SDL2/SDL_render.H>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
TTF_Font *gFont = NULL;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

bool gGameRunning;
bool gGamePaused;
SDL_Rect gPlayerSprites[8 * 16];

enum CHARACTER_CLASS
{
  MAGE,
  ROGUE,
  WARRIOR,
  CHARACTER_CLASS_COUNT
};

char *character_class_str[CHARACTER_CLASS_COUNT] = {
    "Mage",
    "Rogue",
    "Warrior"};

enum ARMOR_CLASS
{
  LIGHT,
  MEDIUM,
  HEAVY,
  ARMOR_CLASS_COUNT
};

enum DIRECTION
{
  SOUTH,
  SOUTH_WEST_1,
  SOUTH_WEST_2,
  SOUTH_WEST_3,
  WEST,
  NORTH_WEST_1,
  NORTH_WEST_2,
  NORTH_WEST_3,
  NORTH,
  NORTH_EAST_1,
  NORTH_EAST_2,
  NORTH_EAST_3,
  EAST,
  SOUTH_EAST_1,
  SOUTH_EAST_2,
  SOUTH_EAST_3,
  DIRECTION_COUNT
};

typedef struct
{
  enum CHARACTER_CLASS character_class;
  enum ARMOR_CLASS armor_class;
  enum DIRECTION direction;
  int level;
  int current_game_level;
  int world_x;
  int world_y;
} Player;

Player gPlayer;

typedef struct
{
  char *assetName;
  SDL_Texture *texture;
} ImageAsset;

typedef struct
{
  SDL_Rect frames[1024];
  int columns;
  int rows;
  int currentFrame;
  ImageAsset image;
  int speed;
} Animation;

ImageAsset gImageAssets[256];
Animation gPlayerAnimations[256];

bool init_SDL()
{
  return SDL_Init(SDL_INIT_EVERYTHING) >= 0;
}

void start_menu_music() { printf("Ominous music playing..\n"); }

void stop_music() { printf("Music stopped..\n"); }

enum MAIN_MENU_SELECTIONS
{
  START_GAME,
  SHOW_INTRO,
  CREDITS,
  QUIT,
  MAIN_MENU_ITEM_COUNT
};

enum GAME_START_MODE
{
  NEW_GAME,
  LOAD_GAME
};

ImageAsset load_image_asset(char *fileName)
{
  SDL_Texture *texture = NULL;
  SDL_Surface *loadedSurface = IMG_Load(fileName);
  if (loadedSurface == NULL)
  {
    printf("Unable to load image %s! SDL_image Error: %s\n", fileName, IMG_GetError());
  }
  else
  {
    texture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (texture == NULL)
    {
      printf("Unable to optimize image %s! SDL Error: %s\n", fileName, SDL_GetError());
    }
    SDL_FreeSurface(loadedSurface);
  }
  ImageAsset asset;
  asset.assetName = fileName;
  asset.texture = texture;
  return asset;
}

bool load_animations()
{
  ImageAsset playerSpriteSheet = gImageAssets[0];
  int width;
  int height;
  SDL_QueryTexture(playerSpriteSheet.texture, NULL, NULL, &width, &height);
  printf("%d,%d\n", width, height);
  int animationColumns = 8;
  int animationRows = 16;
  int frameWidth = width / animationColumns;
  int frameHeight = height / animationRows;
  enum DIRECTION dir;
  for (dir = SOUTH; dir < DIRECTION_COUNT; dir++)
  {
    int x;
    for (x = 0; x < animationColumns; x++)
    {
      gPlayerAnimations[dir].currentFrame = 0;
      gPlayerAnimations[dir].columns = animationColumns;
      gPlayerAnimations[dir].rows = 1;
      gPlayerAnimations[dir].speed = 1;
      gPlayerAnimations[dir].image = playerSpriteSheet;
      gPlayerAnimations[dir].frames[x].x = x * frameWidth;
      gPlayerAnimations[dir].frames[x].y = dir * frameHeight;
      gPlayerAnimations[dir].frames[x].w = frameWidth;
      gPlayerAnimations[dir].frames[x].h = frameHeight;
    }
  }

  return true;
}

void draw_text(char *text, SDL_Color textColor, SDL_Rect *source, SDL_Rect *dest)
{
  SDL_Surface *textSurface = TTF_RenderText_Solid(gFont, text, textColor);
  if (textSurface == NULL)
  {
    printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
  }
  else
  {
    //Create texture from surface pixels
    SDL_Texture *mTexture = NULL;
    mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    if (mTexture == NULL)
    {
      printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
    }
    //Get rid of old surface
    SDL_RenderCopy(gRenderer, mTexture, source, dest);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(mTexture);
    mTexture = NULL;
  }
}
bool load_assets()
{
  ImageAsset playerSpriteSheet = load_image_asset("assets/player2.png");
  gImageAssets[0] = playerSpriteSheet;
  load_animations();

  return true;
}

bool load_file_exists()
{
  return false;
}

void create_new_character()
{
  printf("Create a new character\n");
  bool in_character_create = true;
  SDL_Event e;
  int selected = 0;
  while (in_character_create)
  {
    while (SDL_PollEvent(&e) != 0)
    {
      if (e.type == SDL_KEYDOWN)
      {
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
          selected--;
          if (selected < 0)
          {
            selected = CHARACTER_CLASS_COUNT - 1;
          }
          break;
        case SDLK_DOWN:
          selected++;
          if (selected > CHARACTER_CLASS_COUNT - 1)
          {
            selected = 0;
          }
          break;
        case SDLK_RETURN:
          in_character_create = false;
          break;
        }
      }
      SDL_RenderClear(gRenderer);

      SDL_Color color = {255, 255, 0};
      draw_text(character_class_str[selected], color, NULL, NULL);

      //Update screen
      SDL_RenderPresent(gRenderer);
    }
  }
  gPlayer.level = 1;
  gPlayer.character_class = selected;
  gPlayer.armor_class = selected;
}

void render_select_character_screen(int selected, char *character_names[], int char_count)
{
  //Clear screen
  SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
  SDL_RenderClear(gRenderer);
  SDL_Rect fillRect = {SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
  SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 0);
  SDL_RenderFillRect(gRenderer, &fillRect);

  //Render texture to screen
  SDL_RenderPresent(gRenderer);
}

void select_character_menu()
{
  printf("Select character\n");
  const int CHAR_COUNT = 5;
  char *temp_characters[5] = {
      "Char1",
      "Char2",
      "Char3",
      "Char4",
      "Create New"};
  int selected = 0;
  SDL_Event e;
  bool in_character_select = true;
  while (in_character_select)
  {
    while (SDL_PollEvent(&e) != 0)
    {
      if (e.type == SDL_KEYDOWN)
      {
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
          selected--;
          if (selected < 0)
          {
            selected = MAIN_MENU_ITEM_COUNT - 1;
          }
          break;
        case SDLK_DOWN:
          selected++;
          if (selected > CHAR_COUNT)
          {
            selected = 0;
          }
          break;
        case SDLK_RETURN:
          if (selected == CHAR_COUNT)
          {
            create_new_character();
          }
          else
          {
            // Load selected char
          }
          in_character_select = false;
          break;
        case SDLK_ESCAPE:
          return;
        default:
          break;
        }
        printf("%s\n", temp_characters[selected]);
      }
    }
    render_select_character_screen(selected, temp_characters, CHAR_COUNT);
  }
}

void init_player_position()
{
  gPlayer.world_x = 0;
  gPlayer.world_y = 0;
  gPlayer.current_game_level = 0;
  gPlayer.direction = SOUTH;
}

void draw_and_blit()
{
  //Clear screen
  SDL_RenderClear(gRenderer);

  //Render texture to screen
  SDL_Rect playerRenderQuad = {(SCREEN_WIDTH / 2) - 95,
                               (SCREEN_HEIGHT / 2) - 95,
                               150,
                               150};

  Animation currentPlayerAnimation = gPlayerAnimations[gPlayer.direction];

  SDL_RenderCopy(gRenderer, gImageAssets[0].texture,
                 &currentPlayerAnimation.frames[currentPlayerAnimation.currentFrame],
                 &playerRenderQuad);

  //Update screen
  SDL_RenderPresent(gRenderer);
}

void update_input()
{
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0)
  {
    if (e.type == SDL_QUIT)
    {
      gGameRunning = false;
      break;
    }
    if (e.type == SDL_KEYDOWN)
    {
      switch (e.key.keysym.sym)
      {
      case SDLK_RETURN:
        gGameRunning = false;
        break;
      case SDLK_ESCAPE:
        gGamePaused = !gGamePaused;
        break;
      case SDLK_LEFT:
        gPlayer.direction++;
        if (gPlayer.direction >= DIRECTION_COUNT)
        {
          gPlayer.direction = SOUTH;
        }
        break;
      case SDLK_RIGHT:
        if (gPlayer.direction == SOUTH)
        {
          gPlayer.direction = SOUTH_EAST_3;
        }
        else
        {
          gPlayer.direction--;
        }

        break;
      default:
        break;
      }
    }
  }
}

void update_animations()
{
  enum DIRECTION dir;
  for (dir = SOUTH; dir < DIRECTION_COUNT; dir++)
  {
    int animFrames = gPlayerAnimations[dir].columns;
    gPlayerAnimations[dir].currentFrame += 1;
    if (gPlayerAnimations[dir].currentFrame >= animFrames)
    {
      gPlayerAnimations[dir].currentFrame = 0;
    }
  }
}

void game_loop()
{
  if (!gGamePaused)
  {
    SDL_Delay(50);
    update_input();
    update_animations();
  }
  else
  {

    update_input();
  }
}

void run_game_loop(enum GAME_START_MODE start_mode)
{
  gGameRunning = true;
  gGamePaused = false;
  draw_and_blit();
  while (gGameRunning)
  {
    game_loop();
    draw_and_blit();
  }
}

bool start_game(enum GAME_START_MODE start_mode)
{
  switch (start_mode)
  {
  case LOAD_GAME:
    select_character_menu();
    break;
  case NEW_GAME:
    create_new_character();
    break;
  }
  printf("level: %d class: %d\n", gPlayer.level, gPlayer.character_class);
  load_assets();
  init_player_position();
  run_game_loop(start_mode);
  printf("Started game..\n");
  printf("Woah, that was quick, game over!\n");
  return false;
}

bool game_init()
{
  stop_music();
  if (load_file_exists())
  {
    return start_game(LOAD_GAME);
  }
  else
  {
    return start_game(NEW_GAME);
  }
}

void show_text_screen(char *text)
{
  bool show_credits = true;
  SDL_Event e;
  while (show_credits)
  {
    while (SDL_PollEvent(&e) != 0)
    {
      if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN)
      {
        return;
      }
    }
    SDL_RenderClear(gRenderer);

    SDL_Color color = {255, 0, 0};
    SDL_Rect dest = {10, 10, SCREEN_WIDTH, 200};
    draw_text(text, color, NULL, &dest);

    //Update screen
    SDL_RenderPresent(gRenderer);
  }
}

void show_intro()
{
  stop_music();
  show_text_screen("Amazing intro with cool effects.. Wow!");
  start_menu_music();
}

bool select_main_menu_item(enum MAIN_MENU_SELECTIONS selection)
{
  switch (selection)
  {
  case START_GAME:
    return game_init();
  case SHOW_INTRO:
    show_intro();
    return false;
  case CREDITS:
    show_text_screen("Piru was made by Simo-Pekka Kerkela!");
    return false;
  case QUIT:
    return true;
  default:
    return false;
  }
}

int main_menu()
{
  bool quit = false;
  SDL_Event e;
  int selected = START_GAME;
  char *menu_items[MAIN_MENU_ITEM_COUNT] = {"Start Game", "Show Intro", "Credits",
                                            "Quit"};

  start_menu_music();
  while (!quit)
  {
    while (SDL_PollEvent(&e) != 0)
    {
      // User requests quit
      if (e.type == SDL_QUIT)
      {
        quit = true;
      }
      if (e.type == SDL_KEYDOWN)
      {
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
          selected--;
          if (selected < 0)
          {
            selected = MAIN_MENU_ITEM_COUNT - 1;
          }
          break;
        case SDLK_DOWN:
          selected++;
          if (selected >= MAIN_MENU_ITEM_COUNT)
          {
            selected = 0;
          }
          break;
        case SDLK_RETURN:
          quit = select_main_menu_item(selected);
          break;
        default:
          break;
        }
      }
    }
    //Clear screen
    SDL_RenderClear(gRenderer);

    SDL_Color color = {255, 255, 0};
    draw_text(menu_items[selected], color, NULL, NULL);

    //Update screen
    SDL_RenderPresent(gRenderer);
  }
  stop_music();
  return 0;
}

bool load_font()
{
  bool success = true;

  //Open the font
  gFont = TTF_OpenFont("font.ttf", 28);
  if (gFont == NULL)
  {
    printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
    success = false;
  }

  return success;
}

int main(int argc, char const *argv[])
{
  if (!init_SDL())
  {
    printf("%s", SDL_GetError());
    return 1;
  }

  gWindow =
      SDL_CreateWindow("Piru", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  if (!gWindow)
  {
    printf("%s", SDL_GetError());
    return 1;
  }
  gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!gRenderer)
  {
    printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
    return 1;
  }
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags))
  {
    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
    return 1;
  }
  if (TTF_Init() == -1)
  {
    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    return 1;
  }
  if (!load_font())
  {
    printf("SDL_ttf could not load font! SDL_ttf Error: %s\n", TTF_GetError());
    return 1;
  }
  else
  {
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);

    srand(SDL_GetTicks());

    printf("Hello Piru\n");
    printf("%d\n", rand());

    main_menu();

    SDL_DestroyWindow(gWindow);
    SDL_Quit();
    return 0;
  }
}
