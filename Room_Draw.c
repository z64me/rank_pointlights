#include "types.h"
#include "extern.h"

/*
0x800a29bc ENTRY_POINT
0xB19B5C   ROM_ADDR
*/

extern int room_uses_pointlights(void *roomSegment);
asm("room_uses_pointlights = 0x8007A824;");

static
inline
void Lights_HackyLightBind(Lights *lights, LightNode *listHead, Room *room)
{
	/* no point lighting in this room */
	if (!room_uses_pointlights(room->segment))
	{
		/* run original lighting routine */
		Lights_BindAll(lights, listHead, 0);
		return;
	}
	
	/* point lighting magic courtesy of rankaisija */
	for ( ; listHead; listHead = listHead->next)
	{
		LightInfo *info = listHead->info;
		LightParams *params = &info->params;

		if (info->type == LIGHT_DIRECTIONAL)
		{
			Light *light = Lights_FindSlot(lights);

			if (light)
			{
				light->l.col[0] = light->l.colc[0] = params->dir.color[0];
				light->l.col[1] = light->l.colc[1] = params->dir.color[1];
				light->l.col[2] = light->l.colc[2] = params->dir.color[2];
				light->l.dir[0] = params->dir.x;
				light->l.dir[1] = params->dir.y;
				light->l.dir[2] = params->dir.z;
				light->l.pad1 = 0;
			}
		}
		
		else if (info->type == LIGHT_POINT_NOGLOW || info->type == LIGHT_POINT_GLOW)
		{
			float radiusF = params->point.radius;

			if (radiusF > 0)
			{
				Light *light = Lights_FindSlot(lights);

				if (light)
				{
					radiusF = 4500000.0f / (radiusF * radiusF);

					if (radiusF > 255)
						radiusF = 255;
					else if (radiusF < 20)
						radiusF = 20;

					light->lPos.col[0] = params->point.color[0];
					light->lPos.colc[0] = light->lPos.col[0];
					light->lPos.col[1] = params->point.color[1];
					light->lPos.colc[1] = light->lPos.col[1];
					light->lPos.col[2] = params->point.color[2];
					light->lPos.colc[2] = light->lPos.col[2];
					light->lPos.pos[0] = params->point.x;
					light->lPos.pos[1] = params->point.y;
					light->lPos.pos[2] = params->point.z;
					light->lPos.pad1 = 0x8;
					light->lPos.pad2 = 0xff;
					light->lPos.pad3 = (unsigned char)radiusF;
				}
			}
		}
	}
}

static
inline
void
destroy_expired_RoomPointLights(GlobalContext *globalCtx)
{
	void *seg;
	RoomPointLights *which;
	int k;
	static char wow = 0;
	static char wow_fw = 0;

	for (k = 0; k < 2; ++k)
	{
		switch (k)
		{
			case 0:
				which = &RoomPointLightsA;
				break;
			
			default:
				which = &RoomPointLightsB;
				break;
		}
		if (((seg = which->roomSegment)
			&& seg != globalCtx->roomCtx.curRoom.segment
			&& seg != globalCtx->roomCtx.prevRoom.segment)
			|| (globalCtx->sceneLoadFlag == 0 && wow > 0)
		)
		{
			while (which->numlights)
				LightContext_RemoveLight(
					globalCtx
					, &globalCtx->lightCtx
					, which->light[--which->numlights]
				);
			which->roomSegment = 0;
		}
	}

	if (!gSaveContext.fw.set && wow_fw)
		LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, D_8015BC10);

	wow = globalCtx->sceneLoadFlag;
	wow_fw = gSaveContext.fw.set;
}

/* create cylindrical billboard matrix */
static
inline
void
billboard_cylinder(GlobalContext *globalCtx)
{
	uint16_t *cyl = (void*)(globalCtx->billboard + 1);
	
	/* cylinder = copy of sphere */
	bcopy(globalCtx->billboard, cyl, 0x40);
	
	/* revert up vector to identity */
	cyl[0x08 / 2] = 0; /* x */
	cyl[0x0A / 2] = 1; /* y */
	cyl[0x0C / 2] = 0; /* z */
	
	cyl[0x28 / 2] = 0; /* x */
	cyl[0x2A / 2] = 0; /* y */
	cyl[0x2C / 2] = 0; /* z */
}

void Room_Draw(GlobalContext *globalCtx, Room *room, u32 flags)
{
	/* don't process rooms that point to nothing */
	if (!room->segment)
		return;
		
	gSegments[3] = VIRTUAL_TO_PHYSICAL(room->segment);

	/* room may have despawned: destroy expired point lights */
	destroy_expired_RoomPointLights(globalCtx);
	
	/* generate cylindrical billboard */
	billboard_cylinder(globalCtx);
	
	if ((HREG(80) != 10) || (HREG(90) & 8))
	{
		Lights *sp228;
		sp228 = LightContext_NewLights(&globalCtx->lightCtx, globalCtx->state.gfxCtx);
		Lights_HackyLightBind(sp228, globalCtx->lightCtx.listHead, room);
		Lights_Draw(sp228, globalCtx->state.gfxCtx);
	}

#if 0
	if (room->mesh->polygon.type >= ARRAY_COUNTU(sRoomDrawHandlers))
	{
		__assert("this->ground_shape->polygon.type < number(Room_Draw_Proc)", "../z_room.c", 1125);
	}
#endif
	sRoomDrawHandlers[room->mesh->polygon.type](globalCtx, room, flags);
}
