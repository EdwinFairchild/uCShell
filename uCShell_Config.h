#ifndef uCSHELL_CONFIG_H
#define uCSHELL_CONFIG_H

// bugs still exist when runing uCShell in a PC environment
#define USING_DESKTOP 0
// When retargeting printf on mcu some extra code is needed
// NOTE: this does not retatget printf for you, it just lets uCShell know that you did
#define RETARGET_PRINTF 1
// not all terminals support color codes (Mobaterm and Putty do)
#define USE_COLORS 1
/* TODO :
 *
 * BUG : not checking for max message size
         say what?

 *
 */

#endif
