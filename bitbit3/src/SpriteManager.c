#include "SpriteManager.h"

#include "Scroll.h"

#include "SpritePrincess.h"
#include "SpriteZurrapa.h"
#include "SpriteParticle.h"
#include "SpriteAxe.h"

#include <string.h>

//Pool
struct Sprite sprite_manager_sprites[N_SPRITE_MANAGER_SPRITES];
DECLARE_STACK(sprite_manager_sprites_pool, N_SPRITE_MANAGER_SPRITES);

//Current sprites
DECLARE_VECTOR(sprite_manager_updatables, N_SPRITE_MANAGER_SPRITES);

UINT8 sprite_manager_removal_check;

void SpriteManagerReset() {
	UINT8 i;

	//place all sprites on the pool
	sprite_manager_sprites_pool[0] = N_SPRITE_MANAGER_SPRITES;
	for(i = 0; i != N_SPRITE_MANAGER_SPRITES; ++i) {
		sprite_manager_sprites_pool[i + 1] = i;

		sprite_manager_sprites[i].oam_idx = i << 1;
		move_sprite(i << 1, 200, 200);
		move_sprite((i << 1) + 1, 200, 200);
	}

	//Clear the list of updatable spritess
	sprite_manager_updatables[0] = 0;
	sprite_manager_removal_check = 0;
}

struct Sprite* SpriteManagerAdd(SPRITE_TYPE sprite_type) {
	UINT8 sprite_idx;
	struct Sprite* sprite;

	sprite_idx = StackPop(sprite_manager_sprites_pool);
	sprite = &sprite_manager_sprites[sprite_idx];
	sprite->type = sprite_type;
	sprite->marked_for_removal = 0;

	VectorAdd(sprite_manager_updatables, sprite_idx);

	switch((SPRITE_TYPE)sprite->type) {
		case SPRITE_TYPE_PRINCESS:      StartPrincess(sprite); break;
		case SPRITE_TYPE_ZURRAPA:       StartZurrapa(sprite);  break;
		case SPRITE_TYPE_DEAD_PARTICLE: StartParticle(sprite); break;
		case SPRITE_TYPE_AXE:           StartAxe(sprite);      break;
	}

	return sprite;
}

void SpriteManagerRemove(int idx) {
	sprite_manager_removal_check = 1;
	sprite_manager_sprites[sprite_manager_updatables[idx + 1]].marked_for_removal = 1;
}

void SpriteManagerRemoveSprite(struct Sprite* sprite) {
	UINT8 i;
	struct Sprite* s;
	for(i = 0u; i != sprite_manager_updatables[0]; ++i) {
		s = &sprite_manager_sprites[sprite_manager_updatables[i + 1]];
		if(s == sprite) {
			SpriteManagerRemove(i);
			break;
		}
	}
}

void SpriteManagerUpdate() {
	UINT8 i;
	struct Sprite* sprite;

	for(i = 0u; i != sprite_manager_updatables[0]; ++i) {
		sprite = &sprite_manager_sprites[sprite_manager_updatables[i + 1]];
		if(!sprite->marked_for_removal) {
			switch((SPRITE_TYPE)sprite->type) {
				case SPRITE_TYPE_PRINCESS:      UpdatePrincess(sprite, i); break;
				case SPRITE_TYPE_ZURRAPA:       UpdateZurrapa(sprite, i);  break;
				case SPRITE_TYPE_DEAD_PARTICLE: UpdateParticle(sprite, i); break;
				case SPRITE_TYPE_AXE:           UpdateAxe(sprite, i);      break;
			}

			if( (scroll_x + 10000u - sprite->x > 10032u) || (sprite->x + 10000u - scroll_x - SCREENWIDTH > 10032u) ||
					(scroll_y + 10000u - sprite->y > 10032u) || (sprite->y + 10000u - scroll_y - SCREENHEIGHT > 10032u)
				) {
				SpriteManagerRemove(i);
			} else {
				DrawSprite(sprite);
			}
		}
	}

	if(sprite_manager_removal_check) {
		for(i = 0u; i != sprite_manager_updatables[0]; ++i) {
			sprite = &sprite_manager_sprites[sprite_manager_updatables[i + 1u]];
			if(sprite->marked_for_removal) {
				StackPush(sprite_manager_sprites_pool, sprite_manager_updatables[i + 1u]);
				VectorRemovePos(sprite_manager_updatables, i);
				move_sprite(sprite->oam_idx, 200, 200);
				move_sprite(sprite->oam_idx + 1, 200, 200);
			}
		}
		sprite_manager_removal_check = 0;
	}
}