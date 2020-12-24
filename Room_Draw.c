#include "types.h"
#include "extern.h"
/*
0x800a29bc ENTRY_POINT
0xB19B5C   ROM_ADDR
*/

typedef struct {
	float x;
	s32 saveSpace;
	float z;
} Vec3fLimited;

static
inline
void Lights_HackyLightBind(Lights *lights, GlobalContext* globalCtx, Room *room) {
	LightNode *listHead = globalCtx->lightCtx.listHead;
	/* no point lighting in this room */
	if (!room_uses_pointlights(room->segment)) {
		/* run original lighting routine */
		Lights_BindAll(lights, listHead, 0);
		return;
	}
	
	/* point lighting magic courtesy of rankaisija */
	for ( ; listHead; listHead = listHead->next) {
		LightInfo *info = listHead->info;
		LightParams *params = &info->params;

		if (info->type != LIGHT_DIRECTIONAL) {
			Vec3fLimited lightPos = { params->point.x, 0, params->point.z };
			s32 dist = Math_Vec3f_DistXZ(&globalCtx->eye, (Vec3f*)&lightPos);
			s32 radiusF = params->point.radius;
			s16 yawCam = (s32)((Math_Vec3f_Yaw(&globalCtx->eye, &globalCtx->at) - Math_Vec3f_Yaw(&globalCtx->eye, (Vec3f*)&lightPos)));

			if (radiusF && dist < 1200 && !((yawCam > 13000 || yawCam < -13000) && dist > 400)) {
				Light *light = Lights_FindSlot(lights);

				if (!light)
					return;

				light->lPos.pos[0] = params->point.x;
				light->lPos.pos[1] = params->point.y;
				light->lPos.pos[2] = params->point.z;

				#ifdef WII_VC
				/**
				 * VC Pointlight radius is only half the precision of
				 * N64 pointlight. Also the pads are otherway around for
				 * these two.
				 * Pad1 = pointLightFlag, 0 reads the light as a
				 *        directional light. Also seems to work as a
				 *        light power value on VC?
				 *
				 * Pad2 = wiivc radius. Values go from smallest (0x80) to
				 *        biggest (0x10). 0xFF is OFF. Values larger than 0x80
				 *        do not make huge difference.
				 *
				 * Pad3 = Edge feather? Does not look good mostly. Might be
				 *        useful when used correctly?
				 *
				 */
				light->lPos.col[0] = params->point.color[0];
 				light->lPos.col[1] = params->point.color[1];
 				light->lPos.col[2] = params->point.color[2];

				radiusF = 4500000 / (radiusF * radiusF);
 				radiusF /= 2;
 				if (radiusF < 10)
 					radiusF = 10;
 				if (radiusF > 0x78)
 					radiusF = 0x78;
 				light->lPos.pad1 = 0x8;
 				light->lPos.pad2 = radiusF;
 				light->lPos.pad3 = 0xFF;

				#else /* N64 */
				/**
				 * Pad1 = pointLightFlag, 0 reads the light as a
				 *        directional light. No difference changing this
				 *        value for N64.
				 *
				 * Pad2 = Edge feather? Does not look good mostly. Might be
				 *        useful when used correctly?
				 *
				 * Pad3 = N64 radius. Values go from smallest (0xFF) to
				 *        biggest (0x20).
				 */
				light->lPos.col[0] = light->lPos.colc[0] = params->point.color[0];
 				light->lPos.col[1] = light->lPos.colc[1] = params->point.color[1];
 				light->lPos.col[2] = light->lPos.colc[2] = params->point.color[2];

				radiusF = 4500000 / (radiusF * radiusF);
 				if (radiusF < 20)
 					radiusF = 20;
 				if (radiusF > 255)
 					radiusF = 255;
 				light->lPos.pad1 = 0x8;
 				light->lPos.pad2 = 0xFF;
 				light->lPos.pad3 = radiusF;
				#endif
			}
		} else {
			Light *light = Lights_FindSlot(lights);
			if (!light)
				return;
			light->l.col[0] = light->l.colc[0] = params->dir.color[0];
			light->l.col[1] = light->l.colc[1] = params->dir.color[1];
			light->l.col[2] = light->l.colc[2] = params->dir.color[2];
			light->l.dir[0] = params->dir.x;
			light->l.dir[1] = params->dir.y;
			light->l.dir[2] = params->dir.z;
			light->l.pad1 = 0;
		}
	}
}

static
inline
void
destroy_expired_RoomPointLights(GlobalContext *globalCtx) {
	RoomPointLights *which;
	static char wow = 0;
	void *seg;
	int k;

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

	if (!gSaveContext.fw.set)
		D_8015BC10->info->params.point.radius = 0;
	wow = globalCtx->sceneLoadFlag;
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

	// if ((HREG(80) != 10) || (HREG(90) & 8)) // Does not seem to affect game in any way. But we'll keep these as commented out just in case.
	// {
		Lights *sp228;
		sp228 = LightContext_NewLights(&globalCtx->lightCtx, globalCtx->state.gfxCtx);
		Lights_HackyLightBind(sp228, globalCtx, room);
		Lights_Draw(sp228, globalCtx->state.gfxCtx);
	// }

#if 0
	if (room->mesh->polygon.type >= ARRAY_COUNTU(sRoomDrawHandlers))
	{
		__assert("this->ground_shape->polygon.type < number(Room_Draw_Proc)", "../z_room.c", 1125);
	}
#endif
	sRoomDrawHandlers[room->mesh->polygon.type](globalCtx, room, flags);
}
