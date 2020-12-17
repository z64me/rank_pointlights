#include "types.h"
#include "extern.h"

/*
0x80801E0C ENTRY_POINT
0xBD0D3C   ROM_ADDR
*/

extern void osSyncPrintf(const char* fmt, ...);
asm("osSyncPrintf = 0x80002130;");

void Select_Destroy(void* thisx) {
    osSyncPrintf("%c", 7);
    // "view_cleanup will hang, so it won't be called"
    osSyncPrintf("*** view_cleanupはハングアップするので、呼ばない ***\n");
}
