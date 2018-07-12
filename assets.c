#include "assets.h"

TTF_Font *gFont = NULL;
ImageAsset gImageAssets[256];
Animation gPlayerAnimations[256];

bool load_font()
{
    bool success = true;

    //Open the font
    gFont = TTF_OpenFont("font.ttf", 28);
    if (gFont == NULL)
    {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        success = false;
    }

    return success;
}

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

bool load_assets()
{
    int asset_index = 0;
    ImageAsset playerSpriteSheet = load_image_asset("assets/player2.png");
    ImageAsset grovelSpriteSheet = load_image_asset("assets/iso_dirt_1.png");
    ImageAsset stoneSpriteSheet = load_image_asset("assets/iso_stone_1.png");
    ImageAsset selectionSpriteSheet = load_image_asset("assets/iso_selection.png");
    gImageAssets[asset_index++] = playerSpriteSheet;
    gImageAssets[asset_index++] = grovelSpriteSheet;
    gImageAssets[asset_index++] = stoneSpriteSheet;
    gImageAssets[asset_index++] = selectionSpriteSheet;
    load_animations();

    return true;
}