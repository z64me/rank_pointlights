Point lighting in OoT: a collaboration with rankaisija
```
Patching instructions:

OoT debug

func_80098B74.bin goes at 0xAD4EFC
Room_Draw.bin goes at 0xB19B5C

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

