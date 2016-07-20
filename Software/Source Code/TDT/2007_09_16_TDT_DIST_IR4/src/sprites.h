#ifndef __SUMOBOT_SPRITES_H__
#define __SUMOBOT_SPRITES_H__

#define SPRITE_ALPHANUMERIC_WIDTH		5
#define SPRITE_ALPHANUMERIC_HEIGHT		7

static const USHORT sc_sprite_0[] = {
	0b01110,
	0b10001,
	0b10001,
	0b10001,
	0b10001,
	0b10001,
	0b01110
};

static const USHORT sc_sprite_1[] = {
	0b00100,
	0b01100,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b01110
};

static const USHORT sc_sprite_2[] = {
	0b01110,
	0b10001,
	0b00010,
	0b00100,
	0b01000,
	0b10000,
	0b11111
};

static const USHORT sc_sprite_3[] = {
	0b01110,
	0b10001,
	0b00001,
	0b00110,
	0b00001,
	0b10001,
	0b01110
};

static const USHORT sc_sprite_4[] = {
	0b00010,
	0b00110,
	0b01010,
	0b10010,
	0b11111,
	0b00010,
	0b00010
};

static const USHORT sc_sprite_5[] = {
	0b11111,
	0b10000,
	0b10000,
	0b11110,
	0b00001,
	0b10001,
	0b01110
};

static const USHORT sc_sprite_6[] = {
	0b01110,
	0b10001,
	0b10000,
	0b11110,
	0b10001,
	0b10001,
	0b01110
};

static const USHORT sc_sprite_7[] = {
	0b11111,
	0b00001,
	0b00010,
	0b00100,
	0b01000,
	0b10000,
	0b10000
};

static const USHORT sc_sprite_8[] = {
	0b01110,
	0b10001,
	0b10001,
	0b01110,
	0b10001,
	0b10001,
	0b01110
};

static const USHORT sc_sprite_9[] = {
	0b01110,
	0b10001,
	0b10001,
	0b01111,
	0b00001,
	0b10001,
	0b01110
};

static const USHORT sc_sprite_A[] = {
	0b01110,
	0b10001,
	0b10001,
	0b11111,
	0b10001,
	0b10001,
	0b10001
};

static const USHORT sc_sprite_B[] = {
	0b11110,
	0b10001,
	0b10001,
	0b11110,
	0b10001,
	0b10001,
	0b11110
};

static const USHORT sc_sprite_C[] = {
	0b01110,
	0b10001,
	0b10000,
	0b10000,
	0b10000,
	0b10001,
	0b01110
};

static const USHORT sc_sprite_D[] = {
	0b11110,
	0b10001,
	0b10001,
	0b10001,
	0b10001,
	0b10001,
	0b11110
};

static const USHORT sc_sprite_E[] = {
	0b11111,
	0b10000,
	0b10000,
	0b11111,
	0b10000,
	0b10000,
	0b11111
};

static const USHORT sc_sprite_F[] = {
	0b11111,
	0b10000,
	0b10000,
	0b11111,
	0b10000,
	0b10000,
	0b10000
};

static const USHORT sc_sprite_X[] = {
	0b10001,
	0b10001,
	0b01010,
	0b00100,
	0b01010,
	0b10001,
	0b10001
};

static const USHORT* sc_hexDigits[16] = {
	sc_sprite_0, sc_sprite_1, sc_sprite_2, sc_sprite_3,
	sc_sprite_4, sc_sprite_5, sc_sprite_6, sc_sprite_7,
	sc_sprite_8, sc_sprite_9, sc_sprite_A, sc_sprite_B,
	sc_sprite_C, sc_sprite_D, sc_sprite_E, sc_sprite_F
};

#define SPRITE_ALPHANUMERIC_MINI_WIDTH		3
#define SPRITE_ALPHANUMERIC_MINI_HEIGHT		5

static const USHORT sc_sprite_mini0[] = {
	0b111,
	0b101,
	0b101,
	0b101,
	0b111
};

static const USHORT sc_sprite_mini1[] = {
	0b010,
	0b110,
	0b010,
	0b010,
	0b111
};

static const USHORT sc_sprite_mini2[] = {
	0b111,
	0b001,
	0b111,
	0b100,
	0b111
};

static const USHORT sc_sprite_mini3[] = {
	0b111,
	0b001,
	0b111,
	0b001,
	0b111
};

static const USHORT sc_sprite_mini4[] = {
	0b101,
	0b101,
	0b111,
	0b001,
	0b001
};

static const USHORT sc_sprite_mini5[] = {
	0b111,
	0b100,
	0b111,
	0b001,
	0b111
};

static const USHORT sc_sprite_mini6[] = {
	0b111,
	0b100,
	0b111,
	0b101,
	0b111
};

static const USHORT sc_sprite_mini7[] = {
	0b111,
	0b001,
	0b001,
	0b001,
	0b001
};

static const USHORT sc_sprite_mini8[] = {
	0b111,
	0b101,
	0b111,
	0b101,
	0b111,
};

static const USHORT sc_sprite_mini9[] = {
	0b111,
	0b101,
	0b111,
	0b001,
	0b111
};

static const USHORT sc_sprite_miniA[] = {
	0b111,
	0b101,
	0b111,
	0b101,
	0b101
};

static const USHORT sc_sprite_miniB[] = {
	0b100,
	0b100,
	0b111,
	0b101,
	0b111
};

static const USHORT sc_sprite_miniC[] = {
	0b111,
	0b100,
	0b100,
	0b100,
	0b111
};

static const USHORT sc_sprite_miniD[] = {
	0b001,
	0b001,
	0b111,
	0b101,
	0b111
};

static const USHORT sc_sprite_miniE[] = {
	0b111,
	0b100,
	0b111,
	0b100,
	0b111
};

static const USHORT sc_sprite_miniF[] = {
	0b111,
	0b100,
	0b111,
	0b100,
	0b100
};

static const USHORT* sc_hexDigits_mini[16] = {
	sc_sprite_mini0, sc_sprite_mini1, sc_sprite_mini2, sc_sprite_mini3,
	sc_sprite_mini4, sc_sprite_mini5, sc_sprite_mini6, sc_sprite_mini7,
	sc_sprite_mini8, sc_sprite_mini9, sc_sprite_miniA, sc_sprite_miniB,
	sc_sprite_miniC, sc_sprite_miniD, sc_sprite_miniE, sc_sprite_miniF
};

#endif // __SUMOBOT_SPRITES_H__
