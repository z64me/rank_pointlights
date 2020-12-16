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
	
	if (setup && *((char*)header) == 0x18)
	{
		uint32_t *list = (void*)SEGMENTED_TO_VIRTUAL(header[1]);
		
		for (int i = setup - 1; i >= 0; --i)
			if (list[i])
				return (void*)SEGMENTED_TO_VIRTUAL(list[i]);
	}
	
	return header;
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

