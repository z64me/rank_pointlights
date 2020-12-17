#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#define int32_t  s32
#define uint32_t u32
#define uint16_t u16
#define int16_t  s16
#define uint8_t  u8
#define int8_t   s8

#define s32 signed int
#define u32 unsigned int
#define u16 unsigned short
#define s16 signed short
#define u8  unsigned char
#define s8  signed char

#define NUM_SEGMENTS        (16)
#define SEGMENT_OFFSET(a)   ((u32)(a) & 0x00ffffff)
#define SEGMENT_NUMBER(a)   (((u32)(a) << 4) >> 28)
#define PHYSICAL_TO_VIRTUAL(addr) (void*)((u32)(addr) + 0x80000000)
#define SEGMENTED_TO_VIRTUAL(addr) PHYSICAL_TO_VIRTUAL(gSegments[SEGMENT_NUMBER(addr)] + SEGMENT_OFFSET(addr))
#define VIRTUAL_TO_PHYSICAL(addr) (u32)((u8*)(addr) - 0x80000000)

/* rankaisija structs */
typedef struct {
  unsigned char    col[3];  /* diffuse light value (rgba) */
  char             pad1;
  unsigned char    colc[3]; /* copy of diffuse light value (rgba) */
  char             pad2;
  signed char      dir[3];  /* direction of light (normalized) */
  char             pad3;
} Light_t;

typedef struct {
  unsigned char    col[3];  /* diffuse light value (rgba) */
  char             pad1;
  unsigned char    colc[3]; /* copy of diffuse light value (rgba) */
  char             pad2;
  signed short     pos[3];
  char             pad3;
} Light_Pos_t;

typedef struct {
  unsigned char    col[3];  /* ambient light value (rgba) */
  char         pad1;
  unsigned char    colc[3]; /* copy of ambient light value (rgba) */
  char         pad2;
} Ambient_t;

typedef struct {
  int        x1,y1,x2,y2;   /* texture offsets for highlight 1/2 */
} Hilite_t;

typedef union {
    Light_t    l;
    Light_Pos_t lPos;
    long long int    force_structure_alignment[2];
} Light;
/* end rankaisija structs */

/* decomp structs */
typedef struct {
    u8 r, g, b;
} Color_RGB8;

typedef struct {
    /* 0x0 */ s16 x;
    /* 0x2 */ s16 y;
    /* 0x4 */ s16 z;
    /* 0x6 */ u8 color[3];
    /* 0x9 */ u8 drawGlow;
    /* 0xA */ s16 radius;
} LightPoint; // size = 0xC

typedef struct {
    /* 0x0 */ s8 x;
    /* 0x1 */ s8 y;
    /* 0x2 */ s8 z;
    /* 0x3 */ u8 color[3];
} LightDirectional; // size = 0x6

typedef union {
    LightPoint point;
    LightDirectional dir;
} LightParams; // size = 0xC

typedef struct {
    /* 0x0 */ u8 type;
    /* 0x2 */ LightParams params;
} LightInfo; // size = 0xE

typedef struct Lights {
//    /* 0x00 */ u8 numLights;
//    /* 0x08 */ Lightsn l;
    char wow[0x80];
} Lights; // size = 0x80

typedef struct LightNode {
    /* 0x0 */ LightInfo* info;
    /* 0x4 */ struct LightNode* prev;
    /* 0x8 */ struct LightNode* next;
} LightNode; // size = 0xC

typedef struct {
    /* 0x0 */ LightNode* listHead;
    /* 0x4 */ Color_RGB8 ambient;
    /* 0x7 */ u8 unk_07;
    /* 0x8 */ u8 unk_08;
    /* 0x9 */ u8 unk_09;
    /* 0xA */ s16 unk_0A;
    /* 0xC */ s16 unk_0C;
} LightContext; // size = 0x10

typedef enum {
    /* 0x00 */ LIGHT_POINT_NOGLOW,
    /* 0x01 */ LIGHT_DIRECTIONAL,
    /* 0x02 */ LIGHT_POINT_GLOW
} LightType;

typedef struct {
	struct {
		u8 type;
	} polygon;
} Mesh;

typedef struct {
	/* 0x00 */ unsigned char wow[0x8];
	/* 0x08 */ Mesh *mesh;
	/* 0x0C */ void *segment;
	/* 0x10 */ char wow1[0x4];
} Room; /* size = 0x14 */

typedef struct {
	/* 0x00 */ Room  curRoom;
	/* 0x14 */ Room  prevRoom;
	/* 0x28 */ void* bufPtrs[2];
	/* 0x30 */ u8    unk_30;
	/* 0x31 */ s8    status;
	/* 0x34 */ void* unk_34;
	/* 0x38 */ char wow[0x74 - 0x38];
} RoomContext; // size = 0x74

// Game Info aka. Static Context (dbg ram start: 80210A10)
// Data normally accessed through REG macros (see regs.h)
#define BASE_REG(n, r) gGameInfo->data[n * REG_PER_GROUP + r]
#define REG_GROUPS 29 // number of REG groups, i.e. REG, SREG, OREG, etc.
#define REG_PAGES 6
#define REG_PER_PAGE 16
#define REG_PER_GROUP REG_PAGES * REG_PER_PAGE
#define HREG(r) BASE_REG(21, r)
typedef struct {
    /* 0x00 */ s32  regPage;   // 1 is first page
    /* 0x04 */ s32  regGroup;  // "register" group (R, RS, RO, RP etc.)
    /* 0x08 */ s32  regCur;    // selected register within page
    /* 0x0C */ s32  dpadLast;
    /* 0x10 */ s32  repeat;
    /* 0x14 */ s16  data[REG_GROUPS * REG_PER_GROUP]; // 0xAE0 entries
} GameInfo; // size = 0x15D4

struct GraphicsContext;
typedef struct GraphicsContext GraphicsContext;

typedef struct Mtx {
	char wow[0x40];
} Mtx;

typedef struct {
	/* 0x00000 */ struct {
		GraphicsContext *gfxCtx;
	} state;
	/* 0x00004 */ char wow[0x7A8 - 0x4];
	/* 0x007A8 */ LightContext lightCtx;
	/* 0x007B8 */ char wow1[0x11CBC - 0x7B8];
	/* 0x11CBC */ RoomContext roomCtx;
	/* 0x11D30 */ char wow2[0x11DE0 - 0x11D30];
	/* 0x11DE0 */ Mtx *billboard; /* known in decomp as unk_11DE0 */
	/* 0x11DE4 */ char wow3[0x11E15 - 0x11DE4];
	/* 0x11E15 */ s8 sceneLoadFlag; // "fade_direction"
} GlobalContext;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  num;
    /* 0x04 */ u32 segment;
} SCmdLightList;

typedef union {
	SCmdLightList         lightList;
} SceneCmd;

typedef struct {
    /* 0x00 */ char pos[0xC];
    /* 0x0C */ s32 yaw;
    /* 0x10 */ s32 playerParams;
    /* 0x14 */ s32 entranceIndex;
    /* 0x18 */ s32 roomIndex;
    /* 0x1C */ s32 set;
    /* 0x20 */ s32 tempSwchFlags;
    /* 0x24 */ s32 tempCollectFlags;
} FaroresWindData; // size = 0x28

typedef struct {
    /* 0x0000 */ char wow[0xE64];
    /* 0x0E64 */ FaroresWindData fw;
    /* 0x0E8C */ char unk_E8C[0x1360 - 0x0E8C];
    /* 0x1360 */ uint32_t sceneSetupIndex;
} SaveContext;
/* end decomp structs */

/* hacky new types */
typedef struct RoomPointLights
{
    void       *roomSegment;
    uint32_t    numlights;
    LightNode  *light[32];
} RoomPointLights; /* size = 0xD0 */
/* end hacky new types */

#endif /* TYPES_H_INCLUDED */
