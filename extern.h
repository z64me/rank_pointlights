#ifndef EXTERN_H_INCLUDED
#define EXTERN_H_INCLUDED

/* decomp stuff */
void LightContext_RemoveLight(GlobalContext* globalCtx, LightContext* lightCtx, LightNode* node);
asm("LightContext_RemoveLight = 0x8007A7C4;");

LightNode* LightContext_InsertLight(GlobalContext* globalCtx, LightContext* lightCtx, LightInfo* info);
asm("LightContext_InsertLight = 0x8007A764;");

Light *Lights_FindSlot(Lights *lights);
asm("Lights_FindSlot = 0x8007A084;");

void Lights_BindAll(Lights *lights, LightNode *listHead, void *vec);
asm("Lights_BindAll = 0x8007A474;");

Lights *LightContext_NewLights(LightContext *lightCtx, GraphicsContext *gfxCtx);
asm("LightContext_NewLights = 0x8007A6C4;");

void Lights_Draw(Lights* lights, GraphicsContext* gfxCtx);
asm("Lights_Draw = 0x80079EFC;");

extern void (*sRoomDrawHandlers[])(GlobalContext* globalCtx, Room *room, u32 flags);
asm("sRoomDrawHandlers = 0x80127110;");

extern u32 gSegments[NUM_SEGMENTS];
asm("gSegments = 0x80166FA8;");

extern GameInfo *gGameInfo;
asm("gGameInfo = 0x8015FA90;");

extern SaveContext gSaveContext;
asm("gSaveContext = 0x8015E660;");

extern void bzero(void *dst, const int num);
asm("bzero = 0x80004450;");

extern void bcopy(const void *src, void *dst, const uint32_t num);
asm("bcopy = 0x80006F10;");

extern LightNode *D_8015BC10;
asm("D_8015BC10 = 0x8015BC10;");

extern int room_uses_pointlights(void *roomSegment);
asm("room_uses_pointlights = 0x8007A824;");

extern float Math_Vec3f_DistXZ(Vec3f* a, Vec3f* b);
asm("Math_Vec3f_DistXZ = 0x80078028;");

extern s16 Math_Vec3f_Yaw(Vec3f* a, Vec3f* b);
asm("Math_Vec3f_Yaw = 0x80078068;");
/* end decomp stuff */

/* hacky stuff */

/* overwrites unused function CollisionCheck_SetAT_SAC */
/* write 00000000 00000000 at 0xAD4A4C in rom */
extern RoomPointLights RoomPointLightsA;
asm("RoomPointLightsA = 0x8005D8AC;");

/* overwrites unused function CollisionCheck_SetAC_SAC */
/* write 00000000 00000000 at 0xAD4CA4 in rom */
extern RoomPointLights RoomPointLightsB;
asm("RoomPointLightsB = 0x8005DB04;");

/* end hacky stuff */

#endif /* EXTERN_H_INCLUDED */

