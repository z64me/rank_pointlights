#include "types.h"
#include "extern.h"

/*
0x8005DD5C ENTRY_POINT
0xAD4EFC   ROM_ADDR
*/

static
inline
RoomPointLights *
get_RoomPointLights(void *segment)
{
	if (RoomPointLightsA.roomSegment == segment)
		return &RoomPointLightsA;
	
	if (RoomPointLightsB.roomSegment == segment)
		return &RoomPointLightsB;
	
	if (!RoomPointLightsA.roomSegment)
		return &RoomPointLightsA;
	
	if (!RoomPointLightsB.roomSegment)
		return &RoomPointLightsB;
	
	return 0;
}

static
inline
void
register_room_light(RoomPointLights *wow, LightNode *light)
{
	if (!wow || !light)
		return;
	
	wow->light[wow->numlights] = light;
	wow->numlights++;
}

// Scene Command 0x0C: Light List
void func_80098B74(GlobalContext* globalCtx, SceneCmd* cmd) {
	s32 i;
	LightInfo* lightInfo;
	LightNode *node;
	RoomPointLights *wow = get_RoomPointLights(globalCtx->roomCtx.curRoom.segment);

	// if (SREG(30))
	// {
	//     RoomPointLightsA.roomSegment = RoomPointLightsB.roomSegment = 0;
	// 	RoomPointLightsA.numlights = RoomPointLightsB.numlights = 0;
	// }

	if (!wow)
		return;
	
	wow->roomSegment = globalCtx->roomCtx.curRoom.segment;
	wow->numlights = 0;

	lightInfo = SEGMENTED_TO_VIRTUAL(cmd->lightList.segment);
	for (i = 0; i < cmd->lightList.num; i++) {
		node = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, lightInfo);
		register_room_light(wow, node);
		lightInfo++;
	}
}

