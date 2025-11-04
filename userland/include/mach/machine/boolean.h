/*
 * mach/machine/boolean.h - i386 boolean type definition
 */

#ifndef _MACH_MACHINE_BOOLEAN_H_
#define _MACH_MACHINE_BOOLEAN_H_

/*
 * Boolean type for i386 - simple int-based boolean
 */
typedef int boolean_t;

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#endif /* _MACH_MACHINE_BOOLEAN_H_ */
