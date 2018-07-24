#include "assets.h"

TTF_Font *gFont = NULL;
ImageAsset gImageAssets[MAX_SPRITES];
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

bool load_animations(ImageAsset spriteSheet, int columns, int rows, enum ANIMATION animationIndex, int offset_x, int offset_y)
{
    int width;
    int height;
    SDL_QueryTexture(spriteSheet.texture, NULL, NULL, &width, &height);
    int animationColumns = columns;
    int animationRows = rows;
    int frameWidth = width / animationColumns;
    int frameHeight = height / animationRows;
    if (animationIndex == ANIM_WARRIOR_WALK || animationIndex == ANIM_WARRIOR_IDLE || animationIndex == ANIM_WARRIOR_ATTACK) // Player animation
    {
        enum PLAYER_DIRECTION dir;
        for (dir = PLAYER_SOUTH; dir < PLAYER_DIRECTION_COUNT; dir++)
        {
            int x;
            for (x = 0; x < animationColumns; x++)
            {
                animations[animationIndex][dir].columns = animationColumns;
                animations[animationIndex][dir].rows = 1;
                animations[animationIndex][dir].speed = 1;
                animations[animationIndex][dir].image = spriteSheet;
                animations[animationIndex][dir].frames[x].x = x * frameWidth;
                animations[animationIndex][dir].frames[x].y = dir * frameHeight;
                animations[animationIndex][dir].frames[x].w = frameWidth;
                animations[animationIndex][dir].frames[x].h = frameHeight;
                animations[animationIndex][dir].offset_x = offset_x;
                animations[animationIndex][dir].offset_y = offset_y;
            }
        }
    }
    else
    {
        enum MONSTER_DIRECTION dir;
        for (dir = MONSTER_SOUTH_WEST; dir < MONSTER_DIRECTION_COUNT; dir++)
        {
            int x;
            for (x = 0; x < animationColumns; x++)
            {
                animations[animationIndex][dir].columns = animationColumns;
                animations[animationIndex][dir].rows = 1;
                animations[animationIndex][dir].speed = 1;
                animations[animationIndex][dir].image = spriteSheet;
                animations[animationIndex][dir].frames[x].x = x * frameWidth;
                animations[animationIndex][dir].frames[x].y = dir * frameHeight;
                animations[animationIndex][dir].frames[x].w = frameWidth;
                animations[animationIndex][dir].frames[x].h = frameHeight;
                animations[animationIndex][dir].offset_x = offset_x;
                animations[animationIndex][dir].offset_y = offset_y;
            }
        }
    }

    return true;
}

bool load_assets()
{
    // load
    ImageAsset warriorMoveSpriteSheet = load_image_asset("assets/player_walk.png");
    ImageAsset warriorIdleSpriteSheet = load_image_asset("assets/player_idle.png");
    ImageAsset warriorAttackSpriteSheet = load_image_asset("assets/player_attack.png");
    ImageAsset grovelSpriteSheet = load_image_asset("assets/iso_dirt_1.png");
    ImageAsset stoneSpriteSheet = load_image_asset("assets/iso_stone_1.png");
    ImageAsset selectionSpriteSheet = load_image_asset("assets/iso_selection.png");
    ImageAsset cursorSword = load_image_asset("assets/sword.png");
    ImageAsset skeletonIdleSpriteSheet = load_image_asset("assets/skeleton_idle.png");
    ImageAsset skeletonWalkSpriteSheet = load_image_asset("assets/skeleton_walk.png");
    ImageAsset skeletonAttackSpriteSheet = load_image_asset("assets/skeleton_attack.png");

    // allocate images
    gImageAssets[SPRITE_WARRIOR_WALK] = warriorMoveSpriteSheet;
    gImageAssets[SPRITE_WARRIOR_ATTACK] = warriorAttackSpriteSheet;
    gImageAssets[SPRITE_WARRIOR_IDLE] = warriorIdleSpriteSheet;
    gImageAssets[SPRITE_GROVEL] = grovelSpriteSheet;
    gImageAssets[SPRITE_STONE] = stoneSpriteSheet;
    gImageAssets[SPRITE_SELECTION] = selectionSpriteSheet;
    gImageAssets[SPRITE_CURSOR] = cursorSword;
    gImageAssets[SPRITE_SKELETON_IDLE] = skeletonIdleSpriteSheet;
    gImageAssets[SPRITE_SKELETON_WALK] = skeletonWalkSpriteSheet;
    gImageAssets[SPRITE_SKELETON_ATTACK] = skeletonAttackSpriteSheet;

    // allocate animations
    load_animations(warriorMoveSpriteSheet, 8, 16, ANIM_WARRIOR_WALK, -96, -96);
    load_animations(warriorIdleSpriteSheet, 8, 16, ANIM_WARRIOR_IDLE, -80, -88);
    load_animations(warriorAttackSpriteSheet, 9, 16, ANIM_WARRIOR_ATTACK, -174, -88);
    load_animations(skeletonIdleSpriteSheet, 8, 8, ANIM_SKELETON_IDLE, -80, -56);
    load_animations(skeletonWalkSpriteSheet, 8, 8, ANIM_SKELETON_WALK, -80, -56);
    load_animations(skeletonAttackSpriteSheet, 16, 8, ANIM_SKELETON_ATTACK, -118, -80);
    return true;
}