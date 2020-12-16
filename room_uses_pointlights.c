#include "types.h"
#include "extern.h"

/*
0x8007A824 ENTRY_POINT
0xAF19C4   ROM_ADDR
*/

/* evaluate 'alt headers' command, courtesy of Skawo */
static
inline
void *
zh_evaluate_alt_headers(uint32_t *header)
{
	int setup = gSaveContext.sceneSetupIndex;
	uint32_t listLoc = 0;
	uint32_t offset = 0;

	if (setup && *((char*)header) == 0x18)
	{
		listLoc = ((( (uint32_t*)header)[1] ) & 0x00FFFFFF);
		offset = ((( (uint32_t*)header)[(listLoc / 4) + (setup - 1)] ) & 0x00FFFFFF);;
	}
	return header + (offset / 4);
}

int room_uses_pointlights(void *roomSegment)
{
	char *header = zh_evaluate_alt_headers(roomSegment);
	
	/* locate 'end header' command */
	while (*header != 0x14)
		header += 8;

	/* command's second byte says whether to use point lights */
	return header[1];
}
