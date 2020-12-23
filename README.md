Point lighting in OoT: a collaboration with **rankaisija**
```
Patching instructions:

NOTE: OoT for Wii VC implements point lighting differently
than N64 hardware, so when releasing your mod for Wii VC,
build for Wii VC and/or use the provided Wii VC patch!

OoT debug

short version:
Use the patches provided under 'Releases'

long version:

func_80098B74.bin goes at 0xAD4EFC
Room_Draw.bin goes at 0xB19B5C
room_uses_pointlights.bin goes at 0xAF19C4

/* this ensures our custom structs are zero-initialized */
write 00000000 00000000 at 0xAD4A4C
write 00000000 00000000 at 0xAD4CA4

/* this redirects old Room_Draw to new Room_Draw */
write 08028A6F 00000000 at 0xB0E640

/* this redirects old func_80098B74 to new func_80098B74 */
write 8005DD5C at 0xB9E670

/* this change allocates an extra matrix for cylindrical billboards */
write 0080 at 0xB362DE
```

### Building
Run `make clean && make` to build. To build for Wii VC, run `make clean && make wiivc`

### Known issues
* Because of the limitation of N64, this supports only drawing 7 lights to room mesh at once. If there are more than 7, some lights will stop from drawing. For this reason the code is adjusted to draw lights based on distance to camera. It hasn't yet been researched that much but actors spawned in the room seem to take highest priority in the order they are set in. Do note that Navi light is one of the last ones in priority. This can be avoided by not having too many light sources in small area.
* Env A/B lights are not drawn on to room. For directional lights, add one in Room Env tab in SO.
