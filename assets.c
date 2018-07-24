#include "assets.h"

TTF_Font *gFont = NULL;
ImageAsset gImageAssets[256];
Animation animations[256][256];

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

bool load_animations(ImageAsset spriteSheet, int columns, int rows, int animationIndex)
{
    int width;
    int height;
    SDL_QueryTexture(spriteSheet.texture, NULL, NULL, &width, &height);
    int animationColumns = columns;
    int animationRows = rows;
    int frameWidth = width / animationColumns;
    int frameHeight = height / animationRows;
    enum DIRECTION dir;
    for (dir = SOUTH; dir < DIRECTION_COUNT; dir++)
    {
        int x;
        for (x = 0; x < animationColumns; x++)
        {
            animations[animationIndex][dir].currentFrame = 0;
            animations[animationIndex][dir].columns = animationColumns;
            animations[animationIndex][dir].rows = 1;
            animations[animationIndex][dir].speed = 1;
            animations[animationIndex][dir].image = spriteSheet;
            animations[animationIndex][dir].frames[x].x = x * frameWidth;
            animations[animationIndex][dir].frames[x].y = dir * frameHeight;
            animations[animationIndex][dir].frames[x].w = frameWidth;
            animations[animationIndex][dir].frames[x].h = frameHeight;
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
    ImageAsset cursorSword = load_image_asset("assets/sword.png");
    ImageAsset skeletonIdleSpriteSheet = load_image_asset("assets/skeleton_idle.png");
    gImageAssets[asset_index++] = playerSpriteSheet;
    gImageAssets[asset_index++] = grovelSpriteSheet;
    gImageAssets[asset_index++] = stoneSpriteSheet;
    gImageAssets[asset_index++] = selectionSpriteSheet;
    gImageAssets[asset_index++] = cursorSword;
    gImageAssets[asset_index++] = skeletonIdleSpriteSheet;
    load_animations(playerSpriteSheet, 8, 16, 0);

    return true;
}