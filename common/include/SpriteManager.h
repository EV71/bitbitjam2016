#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include "Vector.h"
#include "Stack.h"
#include "Sprite.h"

#define N_SPRITE_MANAGER_SPRITES 20

//Pool
extern struct Sprite* sprite_manager_sprites[];
extern UINT8 sprite_manager_sprites_pool[];

//Current sprites
extern UINT8 sprite_manager_updatables[];

extern UINT8 sprite_manager_current_index;
extern struct Sprite* sprite_manager_current_sprite;

void SpriteManagerReset();

struct Sprite* SpriteManagerAdd(UINT8 sprite_type);
void SpriteManagerRemove(int idx);
void SpriteManagerRemoveSprite(struct Sprite* sprite);

void SpriteManagerUpdate();

#endif