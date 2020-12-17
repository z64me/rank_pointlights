Point lighting in OoT: a collaboration with **rankaisija**
```
Patching instructions:

OoT debug

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

### Known issues
* Entering a point-lit scene via map select, going back to map select, and re-entering the scene prevents lights from spawning. Work around this by re-re-entering from map select.
* There are no known issues in normal gameplay, so we found it suitable for release.
