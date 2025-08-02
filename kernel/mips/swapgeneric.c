/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log:	swapgeneric.c,v $
 * Revision 2.7  91/02/05  17:51:43  mrt
 * 	Added author notices
 * 	[91/02/04  11:24:57  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:29:43  mrt]
 * 
 * Revision 2.6  90/09/10  16:04:37  rpd
 * 	Apparently, with the new prom the only reliable way to get 
 * 	the boot image name is to query the prom environment vars.
 * 	[90/09/10  15:13:56  af]
 * 
 * Revision 2.5  90/09/09  23:21:20  rpd
 * 	Mods for new syntax used in new DEC proms for 3maxen.
 * 	[90/09/09  19:02:17  af]
 * 
 * Revision 2.4  90/08/07  22:30:25  rpd
 * 	Subtle bug: rz(0,2) turned out just rz2, but the U*x server really
 * 	wants rz2a.
 * 	[90/08/07  15:18:21  af]
 * 
 * Revision 2.3.2.1  90/06/11  11:26:35  af
 * 	Ok, so I did rewrite it.  We only work on a string basis, and it
 * 	does not really matter what we say anyways: the kernel will ask
 * 	for the swapfile later if not found.  What we pass on to the U*x
 * 	server is settable as a command line option, so there really is
 * 	not much we have to do here besides mapping from the prom syntax
 * 	into our own syntax for the name of the boot device.
 * 
 * Revision 2.3  89/12/08  19:48:28  rwd
 * 	I know, I should have rewritten it. I just fixed bugs for now.
 * 	We did not understand rz(0,1,0) properly.
 * 	[89/12/05            af]
 * 
 * Revision 2.2  89/11/29  14:15:23  af
 * 	Checked in temporarily, until I get around to rewrite this code.
 * 	[89/11/16  14:48:03  af]
 * 
 * Revision 2.3  89/08/08  21:49:45  jsb
 * 	A few errors in the code that guesses the root device iff
 * 	you had more than one disk.
 * 	[89/07/31            rvb]
 * 
 * Revision 2.2  89/05/31  12:32:13  rvb
 * 	Define "asmke" here. [af]
 * 
 * Revision 2.1  89/05/30  12:56:14  rvb
 * Created.
 * 
 */
/*
 *	File: swapgeneric.c
 * 	Author: Alessandro Forin and Robert V. Baron,
 *		at Carnegie Mellon University
 *	Date:	5/89
 *
 *	Define boot device for kernel, and consequently where
 *	the root filesystem and swapfile lives by default.
 */

#include <sys/varargs.h>

#define LPAR '('
#define RPAR ')'
#define SLASH '/'

#define LEN 80
static char root_name_space[LEN];
char *root_name = root_name_space;	/* the name of the bootdevice */

int askme;				/* maybe set by parse_args, or by
					 * patching the kernel */

set_root_name(name)
	char *name;
{
	extern char    *index(), *prom_getenv();
	char           *cp;
	int		ret;

	/*
	 *	The prom gets us something like "rz(0,2,0)mach",
	 *	which we turn into "rz2a"
	 *
	 *	Syntax: dev '(' [ctlr ','] [unit ','] [part] ')' file
	 *	Becomes: dev unit part1
	 *	Where:
	 *		part1 == 'a' + part
	 */

	if (strcmp(name, "boot") == 0)
		name = prom_getenv("boot");

	mig_strncpy(root_name, name, LEN);	/* copy and terminate arg */
	prom_map_bootdev();

	/*
	 *	Weeeell, that's before DEC "fixed" the proms.  The joyful
	 *	motivation was to make them "less DEC-ish".  Sigh.
	 *	Anyways, now we get something like "5/rz2/mach" for slot5
	 *	of the TURBOchannel, driver "rz" unit 2 file "mach".
	 *	Which is just peachy, but we must cope with both old&new,
	 *	'course.
	 */
#define isdigit(c)	((c) >= '0' && (c) <= '9')

	if (isdigit(root_name[0]))
		/* New wave prom */
		ret = set_rootname_newstyle();
	else
		ret = set_rootname_oldstyle();
	if (ret)
		askme = 1;
}

static set_rootname_oldstyle()
{
	char 	       *cp;
	char           *c1p, *c2p, part;
	int		unit;

	cp = index(root_name, RPAR);
	if (cp++)
		*cp = 0;

	if ((c1p = index(root_name, LPAR)) == 0)
		return 1;	/* sanity */
	cp = c1p + 1;
	while (*cp != RPAR && *cp++ != ',');		/* skip controller */
	c2p = cp;
	while (*cp != RPAR && *cp != ',') cp++;		/* skip unit */
	if (*cp == ',')					/* skip partition */
		*cp++ = 0;
	else
		*cp = 0;
	if ((part = *cp) == 0)				/* part missing ? */
		part = 'a';
	if (isdigit(part))				/* map to letters */
		part += 'a' - '0';

	if (!*c2p)					/* set unit */
		unit = '0';
	else
		unit = *c2p;
	*c1p++ = unit;

	*c1p++ = part;					/* set partition */
	*c1p = 0;
	return 0;
}

static set_rootname_newstyle()
{
	char 	       *cp;
	char           *c1p, *c2p, part;
	int		unit;

	if ((cp = index(root_name, SLASH)) == 0)
		return 1;
	*cp++ = 0;

	if ((c1p = index(cp, SLASH)) == 0)
		return 1;	/* sanity */
	*c1p = 0, c1p[1] = 0;
	root_name = cp;

	/*
	 * Find unit and partition
	 */
	for (c2p = cp; c2p < c1p && !isdigit(*c2p); c2p++) ;

	cp = c2p;
	unit = (isdigit(*c2p) ? *c2p++ : '0');

	part = (c2p < c1p ? *c2p : 'a');
	if (isdigit(part))
		part += 'a' - '0';

	/*
	 * Set unit and partition in root_name
	 */
	*cp++ = unit;
	*cp++ = part;
	*cp = 0;
	return 0;
}


static
prom_map_bootdev()
{
	if (strcmp("dkip", root_name) == 0)
		root_name += 2;		/* --> "ip" */
}

/*
 * If the variable "askme" is set, enter a dialogue with the
 * user to get the name of the root device.  We only check
 * for a legal syntax here, too bad if he screws up.
 */
static char *examples[] = {	/* some examples of legal device names */
	"rz", "ip", 0
};

int askme;	/* set by parse_args, or by patching the kernel */

get_root_name()
{
	register char **gc, c;

	if (!askme)
		return;
retry:
	root_name = root_name_space;
	bzero(root_name,LEN);

	printf("root device? ");

	gets(root_name);
	prom_map_bootdev();

	c = root_name[2];		/* check unit num */
	if (c >= '0' && c <= '7') {
		c = root_name[3];	/* check part num */
		if (c && c >= 'a' && c <= 'h')
			goto found;
		printf("bad partition number\n");
	} else
		printf("bad/missing unit number\n");

	printf("bad root specification, use something like:");
	for (gc = examples; *gc; gc++)
		printf(" %s%%d[a-h]", *gc);
	printf("\n");
	goto retry;
found:
	set_root_name(root_name);
}
