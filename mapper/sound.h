#ifndef   SOUND
#define   SOUND

#include <stdconst.h>

void SoundInit(void);
void SoundSync(ULONG *pattern, UBYTE length, UBYTE rate, UWORD duration);
void SoundASync(ULONG *pattern, UBYTE length, UBYTE rate, UWORD duration);
void SoundASyncHandler(void);
void SoundExit(void);

void SoundEnable(void);
void SoundDisable(void);
void SoundIntEnable(void(*handler)(void));
void SoundIntDisable(void);

#endif
