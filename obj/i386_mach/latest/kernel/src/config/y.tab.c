
# line 112 "../../../../../../src/30/kernel/src/config/config.y"
typedef union  {
	char	*str;
	int	val;
	struct	file_list *file;
	struct	idlst *lst;
} YYSTYPE;
# define ADDRMOD 257
# define AND 258
# define ANY 259
# define ARGS 260
# define AT 261
# define BIN 262
# define COMMA 263
# define CONFIG 264
# define CONTROLLER 265
# define CPU 266
# define CSR 267
# define DEVICE 268
# define DISK 269
# define DRIVE 270
# define DST 271
# define DUMPS 272
# define EQUALS 273
# define FLAGS 274
# define HZ 275
# define IDENT 276
# define MACHINE 277
# define MAJOR 278
# define MASTER 279
# define MAXUSERS 280
# define MAXDSIZ 281
# define MBA 282
# define MBII 283
# define MINOR 284
# define MINUS 285
# define NEXUS 286
# define ON 287
# define OPTIONS 288
# define MAKEOPTIONS 289
# define PRIORITY 290
# define PSEUDO_DEVICE 291
# define ROOT 292
# define SEMICOLON 293
# define SIZE 294
# define SLAVE 295
# define SWAP 296
# define TIMEZONE 297
# define TRACE 298
# define UBA 299
# define VECTOR 300
# define VME 301
# define VME16D16 302
# define VME24D16 303
# define VME32D16 304
# define VME16D32 305
# define VME24D32 306
# define VME32D32 307
# define LUN 308
# define SLOT 309
# define TAPE 310
# define ID 311
# define NUMBER 312
# define FPNUMBER 313

# line 194 "../../../../../../src/30/kernel/src/config/config.y"

#include "config.h"
#include <ctype.h>
#include <stdio.h>

struct	device cur;
struct	device *curp = 0;
char	*temp_id;
char	*val_id;
char	*malloc();

#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256

# line 779 "../../../../../../src/30/kernel/src/config/config.y"


yyerror(s)
	char *s;
{
	fprintf(stderr, "config: line %d: %s\n", yyline, s);
}

/*
 * return the passed string in a new space
 */
char *
ns(str)
	register char *str;
{
	register char *cp;

	cp = malloc((unsigned)(strlen(str)+1));
	(void) strcpy(cp, str);
	return (cp);
}

/*
 * add a device to the list of devices
 */
newdev(dp)
	register struct device *dp;
{
	register struct device *np;

	np = (struct device *) malloc(sizeof *np);
	*np = *dp;
	if (curp == 0)
		dtab = np;
	else
		curp->d_next = np;
	curp = np;
	curp->d_next = 0;
}

/*
 * note that a configuration should be made
 */
mkconf(sysname)
	char *sysname;
{
	register struct file_list *fl, **flp;

	fl = (struct file_list *) malloc(sizeof *fl);
	fl->f_type = SYSTEMSPEC;
	fl->f_needs = sysname;
	fl->f_rootdev = NODEV;
	fl->f_argdev = NODEV;
	fl->f_dumpdev = NODEV;
	fl->f_fn = 0;
	fl->f_next = 0;
	for (flp = confp; *flp; flp = &(*flp)->f_next)
		;
	*flp = fl;
	confp = flp;
}

struct file_list *
newswap()
{
	struct file_list *fl = (struct file_list *)malloc(sizeof (*fl));

	fl->f_type = SWAPSPEC;
	fl->f_next = 0;
	fl->f_swapdev = NODEV;
	fl->f_swapsize = 0;
	fl->f_needs = 0;
	fl->f_fn = 0;
	return (fl);
}

/*
 * Add a swap device to the system's configuration
 */
mkswap(system, fl, size)
	struct file_list *system, *fl;
	int size;
{
	register struct file_list **flp;
	char name[80];

	if (system == 0 || system->f_type != SYSTEMSPEC) {
		yyerror("\"swap\" spec precedes \"config\" specification");
		return;
	}
	if (size < 0) {
		yyerror("illegal swap partition size");
		return;
	}
	/*
	 * Append swap description to the end of the list.
	 */
	flp = &system->f_next;
	for (; *flp && (*flp)->f_type == SWAPSPEC; flp = &(*flp)->f_next)
		;
	fl->f_next = *flp;
	*flp = fl;
	fl->f_swapsize = size;
	/*
	 * If first swap device for this system,
	 * set up f_fn field to insure swap
	 * files are created with unique names.
	 */
	if (system->f_fn)
		return;
	if (eq(fl->f_fn, "generic"))
		system->f_fn = ns(fl->f_fn);
	else
		system->f_fn = ns(system->f_needs);
}

/*
 * find the pointer to connect to the given device and number.
 * returns 0 if no such device and prints an error message
 */
struct device *
connect(dev, num)
	register char *dev;
	register int num;
{
	register struct device *dp;
	struct device *huhcon();

	if (num == QUES)
		return (huhcon(dev));
	for (dp = dtab; dp != 0; dp = dp->d_next) {
		if ((num != dp->d_unit) || !eq(dev, dp->d_name))
			continue;
		if (dp->d_type != CONTROLLER && dp->d_type != MASTER) {
			(void) sprintf(errbuf,
			    "%s connected to non-controller", dev);
			yyerror(errbuf);
			return (0);
		}
		return (dp);
	}
	(void) sprintf(errbuf, "%s %d not defined", dev, num);
	yyerror(errbuf);
	return (0);
}

/*
 * connect to an unspecific thing
 */
struct device *
huhcon(dev)
	register char *dev;
{
	register struct device *dp, *dcp;
	struct device rdev;
	int oldtype;

	/*
	 * First make certain that there are some of these to wildcard on
	 */
	for (dp = dtab; dp != 0; dp = dp->d_next)
		if (eq(dp->d_name, dev))
			break;
	if (dp == 0) {
		(void) sprintf(errbuf, "no %s's to wildcard", dev);
		yyerror(errbuf);
		return (0);
	}
	oldtype = dp->d_type;
	dcp = dp->d_conn;
	/*
	 * Now see if there is already a wildcard entry for this device
	 * (e.g. Search for a "uba ?")
	 */
	for (; dp != 0; dp = dp->d_next)
		if (eq(dev, dp->d_name) && dp->d_unit == -1)
			break;
	/*
	 * If there isn't, make one because everything needs to be connected
	 * to something.
	 */
	if (dp == 0) {
		dp = &rdev;
		init_dev(dp);
		dp->d_unit = QUES;
		dp->d_name = ns(dev);
		dp->d_type = oldtype;
		newdev(dp);
		dp = curp;
		/*
		 * Connect it to the same thing that other similar things are
		 * connected to, but make sure it is a wildcard unit
		 * (e.g. up connected to sc ?, here we make connect sc? to a
		 * uba?).  If other things like this are on the NEXUS or
		 * if they aren't connected to anything, then make the same
		 * connection, else call ourself to connect to another
		 * unspecific device.
		 */
		if (dcp == TO_NEXUS || dcp == 0)
			dp->d_conn = dcp;
		else
			dp->d_conn = connect(dcp->d_name, QUES);
	}
	return (dp);
}

init_dev(dp)
	register struct device *dp;
{

	dp->d_name = "OHNO!!!";
	dp->d_type = DEVICE;
	dp->d_conn = 0;
	dp->d_vec = 0;
	dp->d_addr = dp->d_pri = dp->d_flags = dp->d_dk = 0;
	dp->d_slave = dp->d_drive = dp->d_unit = UNKNOWN;
	if (machine == MACHINE_SUN2 || machine == MACHINE_SUN3
	    || machine == MACHINE_SUN4){
		dp->d_addr = UNKNOWN;
		dp->d_mach = dp->d_bus = 0;
	}
	if (machine == MACHINE_MIPSY || machine == MACHINE_MIPS){
		dp->d_addrmod = 0;
	}
}

/*
 * make certain that this is a reasonable type of thing to connect to a nexus
 */
check_nexus(dev, num)
	register struct device *dev;
	int num;
{

	switch (machine) {

	case MACHINE_VAX:
		if (!eq(dev->d_name, "uba") && !eq(dev->d_name, "mba"))
			yyerror("only uba's and mba's should be connected to the nexus");
		if (num != QUES)
			yyerror("can't give specific nexus numbers");
		break;

	case MACHINE_SUN:
		if (!eq(dev->d_name, "mb"))
			yyerror("only mb's should be connected to the nexus");
		break;

	case MACHINE_ROMP:
		if (!eq(dev->d_name, "iocc"))
			yyerror("only iocc's should be connected to the nexus");
		break;
        case MACHINE_SUN2:
		if (!eq(dev->d_name, "virtual") &&
		    !eq(dev->d_name, "obmem") &&
		    !eq(dev->d_name, "obio") &&
		    !eq(dev->d_name, "mbmem") &&
		    !eq(dev->d_name, "mbio") &&
		    !eq(dev->d_name, "vme16d16") &&
		    !eq(dev->d_name, "vme24d16")) {
			(void)sprintf(errbuf,
			    "unknown bus type `%s' for nexus connection on %s",
			    dev->d_name, machinename);
			yyerror(errbuf);
		}

	case MACHINE_MMAX:
		yyerror("don't grok 'nexus' on mmax -- try 'slot'.");
		break;
        case MACHINE_SUN3:
        case MACHINE_SUN4:
		if (!eq(dev->d_name, "virtual") &&
		    !eq(dev->d_name, "obmem") &&
		    !eq(dev->d_name, "obio") &&
		    !eq(dev->d_name, "mbmem") &&
		    !eq(dev->d_name, "mbio") &&
		    !eq(dev->d_name, "vme16d16") &&
		    !eq(dev->d_name, "vme24d16") &&
                    !eq(dev->d_name, "vme32d16") &&
		    !eq(dev->d_name, "vme16d32") &&
		    !eq(dev->d_name, "vme24d32") &&
		    !eq(dev->d_name, "vme32d32")) {
			(void)sprintf(errbuf,
			    "unknown bus type `%s' for nexus connection on %s",
			    dev->d_name, machinename);
			yyerror(errbuf);
		}
		break;
	case MACHINE_MIPSY:
	case MACHINE_MIPS:
		if (!eq(dev->d_name, "vme") && !eq(dev->d_name, "mbii"))
			yyerror("only vme's and mbii's should be connected to the nexus");
		if (num != QUES)
			yyerror("can't give specific nexus numbers");
		break;
	}
}

/*
 * make certain that this is a reasonable type of thing to connect to a slot
 */

check_slot(dev, num)
	register struct device *dev;
	int num;
{

	switch (machine) {

	case MACHINE_MMAX:
		if (!eq(dev->d_name, "emc"))
			yyerror("only emc's plug into backplane slots.");
		if (num == QUES)
			yyerror("specific slot numbers must be given");
		break;

	case MACHINE_SQT:
		if (!eq(dev->d_name, "mbad") &&
		    !eq(dev->d_name, "zdc") &&
		    !eq(dev->d_name, "sec")) {
			(void)sprintf(errbuf,
			    "unknown bus type `%s' for slot on %s",
			    dev->d_name, machinename);
			yyerror(errbuf);
		}
		break;

	default:
		yyerror("don't grok 'slot' for this machine -- try 'nexus'.");
		break;
	}
}

/*
 * Check system specification and apply defaulting
 * rules on root, argument, dump, and swap devices.
 */
checksystemspec(fl)
	register struct file_list *fl;
{
	char buf[BUFSIZ];
	register struct file_list *swap;
	int generic;

	if (fl == 0 || fl->f_type != SYSTEMSPEC) {
		yyerror("internal error, bad system specification");
		exit(1);
	}
	swap = fl->f_next;
	generic = swap && swap->f_type == SWAPSPEC && eq(swap->f_fn, "generic");
	if (fl->f_rootdev == NODEV && !generic) {
		yyerror("no root device specified");
		exit(1);
	}
	/*
	 * Default swap area to be in 'b' partition of root's
	 * device.  If root specified to be other than on 'a'
	 * partition, give warning, something probably amiss.
	 */
	if (swap == 0 || swap->f_type != SWAPSPEC) {
		dev_t dev;

		swap = newswap();
		dev = fl->f_rootdev;
		if (minor(dev) & DEV_MASK) {
			(void) sprintf(buf,
"Warning, swap defaulted to 'b' partition with root on '%c' partition",
				(minor(dev) & DEV_MASK) + 'a');
			yyerror(buf);
		}
		swap->f_swapdev =
		   makedev(major(dev), (minor(dev) &~ DEV_MASK) | ('b' - 'a'));
		swap->f_fn = devtoname(swap->f_swapdev);
		mkswap(fl, swap, 0);
	}
	/*
	 * Make sure a generic swap isn't specified, along with
	 * other stuff (user must really be confused).
	 */
	if (generic) {
		if (fl->f_rootdev != NODEV)
			yyerror("root device specified with generic swap");
		if (fl->f_argdev != NODEV)
			yyerror("arg device specified with generic swap");
		if (fl->f_dumpdev != NODEV)
			yyerror("dump device specified with generic swap");
		return;
	}
	/*
	 * Default argument device and check for oddball arrangements.
	 */
	if (fl->f_argdev == NODEV)
		fl->f_argdev = swap->f_swapdev;
	if (fl->f_argdev != swap->f_swapdev)
		yyerror("Warning, arg device different than primary swap");
	/*
	 * Default dump device and warn if place is not a
	 * swap area or the argument device partition.
	 */
	if (fl->f_dumpdev == NODEV)
		fl->f_dumpdev = swap->f_swapdev;
	if (fl->f_dumpdev != swap->f_swapdev && fl->f_dumpdev != fl->f_argdev) {
		struct file_list *p = swap->f_next;

		for (; p && p->f_type == SWAPSPEC; p = p->f_next)
			if (fl->f_dumpdev == p->f_swapdev)
				return;
		(void) sprintf(buf, "Warning, orphaned dump device, %s",
			"do you know what you're doing");
		yyerror(buf);
	}
}

/*
 * Verify all devices specified in the system specification
 * are present in the device specifications.
 */
verifysystemspecs()
{
	register struct file_list *fl;
	dev_t checked[50], *verifyswap();
	register dev_t *pchecked = checked;

	for (fl = conf_list; fl; fl = fl->f_next) {
		if (fl->f_type != SYSTEMSPEC)
			continue;
		if (!finddev(fl->f_rootdev))
			deverror(fl->f_needs, "root");
		*pchecked++ = fl->f_rootdev;
		pchecked = verifyswap(fl->f_next, checked, pchecked);
#define	samedev(dev1, dev2) \
	((minor(dev1) &~ DEV_MASK) != (minor(dev2) &~ DEV_MASK))
		if (!alreadychecked(fl->f_dumpdev, checked, pchecked)) {
			if (!finddev(fl->f_dumpdev))
				deverror(fl->f_needs, "dump");
			*pchecked++ = fl->f_dumpdev;
		}
		if (!alreadychecked(fl->f_argdev, checked, pchecked)) {
			if (!finddev(fl->f_argdev))
				deverror(fl->f_needs, "arg");
			*pchecked++ = fl->f_argdev;
		}
	}
}

/*
 * Do as above, but for swap devices.
 */
dev_t *
verifyswap(fl, checked, pchecked)
	register struct file_list *fl;
	dev_t checked[];
	register dev_t *pchecked;
{

	for (;fl && fl->f_type == SWAPSPEC; fl = fl->f_next) {
		if (eq(fl->f_fn, "generic"))
			continue;
		if (alreadychecked(fl->f_swapdev, checked, pchecked))
			continue;
		if (!finddev(fl->f_swapdev))
			fprintf(stderr,
			   "config: swap device %s not configured", fl->f_fn);
		*pchecked++ = fl->f_swapdev;
	}
	return (pchecked);
}

/*
 * Has a device already been checked
 * for it's existence in the configuration?
 */
alreadychecked(dev, list, last)
	dev_t dev, list[];
	register dev_t *last;
{
	register dev_t *p;

	for (p = list; p < last; p++)
		if (samedev(*p, dev))
			return (1);
	return (0);
}

deverror(systemname, devtype)
	char *systemname, *devtype;
{

	fprintf(stderr, "config: %s: %s device not configured\n",
		systemname, devtype);
}

/*
 * Look for the device in the list of
 * configured hardware devices.  Must
 * take into account stuff wildcarded.
 */
/*ARGSUSED*/
finddev(dev)
	dev_t dev;
{

	/* punt on this right now */
	return (1);
}

/*
 * bi_info gives the magic number used to construct the token for
 * the autoconf code.  bi_max is the maximum value (across all
 * machine types for a given architecture) that a given "bus 
 * type" can legally have.
 */
struct bus_info {
	char    *bi_name;
	u_short bi_info;
	u_int   bi_max;
};

struct bus_info sun2_info[] = {
	{ "virtual",    0x0001, (1<<24)-1 },
	{ "obmem",      0x0002, (1<<23)-1 },
	{ "obio",       0x0004, (1<<23)-1 },
	{ "mbmem",      0x0010, (1<<20)-1 },
	{ "mbio",       0x0020, (1<<16)-1 },
	{ "vme16d16",   0x0100, (1<<16)-1 },
	{ "vme24d16",   0x0200, (1<<24)-(1<<16)-1 },
	{ (char *)0,    0,      0 }
};

struct bus_info sun3_info[] = {
	{ "virtual",    0x0001, (1<<32)-1 },
	{ "obmem",      0x0002, (1<<32)-1 },
	{ "obio",       0x0004, (1<<21)-1 },
	{ "vme16d16",   0x0100, (1<<16)-1 },
	{ "vme24d16",   0x0200, (1<<24)-(1<<16)-1 },
	{ "vme32d16",   0x0400, (1<<32)-(1<<24)-1 },
	{ "vme16d32",   0x1000, (1<<16) },
	{ "vme24d32",   0x2000, (1<<24)-(1<<16)-1 },
	{ "vme32d32",   0x4000, (1<<32)-(1<<24)-1 },
	{ (char *)0,    0,      0 }
};

struct bus_info sun4_info[] = {
	{ "virtual",    0x0001, 0xffffffff },
	{ "obmem",      0x0002, 0xffffffff },
	{ "obio",       0x0004, 0xffffffff },
	{ "vme16d16",   0x0100, (1<<16)-1 },
	{ "vme24d16",   0x0200, (1<<24)-(1<<16)-1 },
	{ "vme32d16",   0x0400, 0xfeffffff },
	{ "vme16d32",   0x1000, (1<<16) },
	{ "vme24d32",   0x2000, (1<<24)-(1<<16)-1 },
	{ "vme32d32",   0x4000, 0xfeffffff },
	{ (char *)0,    0,      0 }
};

bus_encode(addr, dp)
        u_int addr;
	register struct device *dp;
{
	register char *busname;
	register struct bus_info *bip;
	register int num;

	if (machine == MACHINE_SUN2)
		bip = sun2_info;
	else if (machine == MACHINE_SUN3)
		bip = sun3_info;
	else if (machine == MACHINE_SUN4)
		bip = sun4_info;
	else {
		yyerror("bad machine type for bus_encode");
		exit(1);
	}

        if (dp->d_conn == TO_NEXUS || dp->d_conn == 0) {
		yyerror("bad connection");
		exit(1);
	}

        busname = dp->d_conn->d_name;
        num = dp->d_conn->d_unit;

        for (; bip->bi_name != 0; bip++)
                if (eq(busname, bip->bi_name))
                        break;

        if (bip->bi_name == 0) {
                (void)sprintf(errbuf, "bad bus type '%s' for machine %s",
                        busname, machinename);
                yyerror(errbuf);
        } else if (addr > bip->bi_max) {
                (void)sprintf(errbuf,
                        "0x%x exceeds maximum address 0x%x allowed for %s",
                        addr, bip->bi_max, busname);
                yyerror(errbuf);
        } else {
                dp->d_bus = bip->bi_info;       /* set up bus type info */
                if (num != QUES)
                        /*
                         * Set up cpu type since the connecting
                         * bus type is not wildcarded.
                         */
                        dp->d_mach = num;
        }
}
short yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 104
# define YYLAST 255
short yyact[]={

 129,  35, 145, 162, 155, 132, 101, 102, 161, 157,
 126, 152, 151, 127, 150, 149, 148, 131, 147, 146,
   8, 142, 141, 111, 140,  60,  68, 136,  22,  13,
  16, 154,   9,  11, 135, 134, 122,  98, 128,  93,
  19,  15,  59,  10,  67,  61,  62,  63,  64,  65,
  66, 130,  17,  18,  69,  14,  35,   7,  35,  60,
  68, 158,   6,  92,  43,  87,  88, 139, 121,  26,
  60,  68,  25,  53,  12,  88,  59,  24,  67,  61,
  62,  63,  64,  65,  66,  52,  91,  59,  69,  67,
  61,  62,  63,  64,  65,  66,  23,  81, 159,  69,
  78,  76,  77,  75,  57,  51, 137, 110, 123,  50,
 106,  86,  58, 100, 109,  41,  85,  45,  38,  80,
 108,  55,  27,  34,  36,  39,  42,  28, 125,  89,
  54,  56, 105,  29,  30,  31,  32,  49,  48,  47,
  46,  44,  33,  21,  20,  40,  74,  37,   5,   4,
   3,  70,  71,  72,  73,   2,   1, 153, 107, 115,
 112, 118,  79, 138,   0,   0,   0,   0,   0,   0,
  90,  82,  83,  84,   0,   0,   0,   0,   0,   0,
   0,   0,   0,  39,   0,  42,   0,  94,  95,  96,
  97,   0, 104, 103,  99,   0, 124, 114, 117, 120,
   0,   0, 133, 113, 116, 119,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0, 124, 144, 143,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0, 156,   0,
   0,   0,   0, 124, 160 };
short yypact[]={

-1000,-1000,-236,-1000,-197,-216,-221,-1000,-224,-1000,
-1000,-1000,-1000,-1000,-1000,-253,-253,-253,-253,-247,
-1000,-187,-253,-1000,-1000,-1000,-1000,-157,-212,-157,
-157,-157,-157,-212,-1000,-1000,-1000,-160,-1000,-172,
-161,-1000,-173,-1000,-187,-1000,-1000,-1000,-1000,-1000,
-190,-190,-190,-190,-1000,-225,-1000,-223,-273,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-225,-225,-225,-225,-275,-253,-305,-253,-305,-1000,
-255,-1000,-255,-255,-255,-1000,-222,-276,-253,-257,
-277,-278,-285,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-152,-1000,-227,-1000,-1000,
-288,-290,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-291,-234,-1000,-310,-1000,-293,-294,-296,-297,
-298,-300,-301,-281,-1000,-1000,-1000,-255,-1000,-303,
-250,-186,-1000,-1000,-1000,-253,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-304,-1000,-1000,-1000,-309,
-1000,-1000,-1000 };
short yypgo[]={

   0, 107, 113, 112, 108, 163, 120, 114, 161, 160,
 159, 158, 157, 156, 155, 150, 149, 148, 147, 145,
 144, 143, 141, 117, 140, 139, 138, 137, 119, 132,
 110, 118, 115, 122, 121, 116, 127, 131, 129, 128,
 111 };
short yyr1[]={

   0,  13,  14,  14,  15,  15,  15,  15,  15,  17,
  17,  17,  17,  17,  17,  20,  21,  22,  22,  23,
  23,  23,  23,  24,  29,  29,  30,  11,  11,  25,
   9,   9,  26,  10,  10,  27,   8,   8,   7,  28,
  28,   5,   5,   6,   6,   6,  18,  18,  31,  31,
   2,   2,   2,   1,  19,  19,  32,   3,   3,   3,
   3,   3,   3,   3,   3,   3,   3,   3,  16,  16,
  16,  16,  16,  16,  16,  33,  36,  34,  34,  37,
  37,  37,  38,  38,  39,  39,  39,  39,  39,  39,
  39,  39,  12,  12,  35,  35,  35,  35,  35,  40,
   4,   4,   4,   4 };
short yyr2[]={

   0,   1,   2,   0,   2,   2,   2,   1,   2,   2,
   2,   2,   2,   2,   1,   2,   2,   2,   1,   1,
   1,   1,   1,   3,   3,   1,   2,   1,   1,   3,
   1,   1,   3,   1,   1,   3,   1,   1,   4,   1,
   0,   2,   0,   1,   2,   3,   3,   1,   1,   3,
   1,   1,   0,   1,   3,   1,   3,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   1,   4,   4,
   4,   4,   4,   3,   4,   3,   0,   2,   0,   3,
   3,   3,   2,   0,   2,   2,   2,   2,   2,   2,
   2,   2,   1,   2,   1,   2,   3,   3,   0,   2,
   1,   2,   2,   3 };
short yychk[]={

-1000, -13, -14, -15, -16, -17, 298, 293, 256, 268,
 279, 269, 310, 265, 291, 277, 266, 288, 289, 276,
 -20, -21, 264, 293, 293, 293, 293, -33, -36, -33,
 -33, -33, -33, -36,  -1, 311,  -1, -18, -31,  -1,
 -19, -32,  -1, 311, -22, -23, -24, -25, -26, -27,
 296, 292, 272, 260,  -1, -34, -37, 261,  -3, 299,
 282, 302, 303, 304, 305, 306, 307, 301, 283, 311,
 -34, -34, -34, -34,  -3, 263, 273, 263, 273, -23,
 -28, 287, -28, -28, -28, -35, -40, 290, 300, -38,
  -3, 309, 286, 312, -35, -35, -35, -35, 312, -31,
  -2, 311, 312, -32,  -2, -29, -30, -11,  -6,  -7,
  -1, 278,  -9,  -6,  -7, -10,  -6,  -7,  -8,  -6,
  -7, 290, 312,  -4,  -1, -39, 267, 270, 295, 257,
 308, 274, 262,  -3, 312, 312, 312, 258,  -5, 294,
 312, 312, 312, -40,  -4, 312, 312, 312, 312, 312,
 312, 312, 312, -12, 312, 285, -30, 312, 311, 284,
  -4, 312, 312 };
short yydef[]={

   3,  -2,   1,   2,   0,   0,   0,   7,   0,  76,
  76,  76,  76,  76,  76,   0,   0,   0,   0,   0,
  14,   0,   0,   4,   5,   6,   8,  78,   0,  78,
  78,  78,  78,   0,   9,  53,  10,  11,  47,  48,
  12,  55,   0,  13,  15,  18,  19,  20,  21,  22,
  40,  40,  40,  40,  16,  98,  83,   0,   0,  57,
  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,
  98,  98,  98,  98,  73,   0,  52,   0,  52,  17,
   0,  39,   0,   0,   0,  68,  94,   0,   0,  77,
   0,   0,   0,  75,  69,  70,  71,  72,  74,  46,
  49,  50,  51,  54,  56,  23,  25,  42,  27,  28,
  43,   0,  29,  30,  31,  32,  33,  34,  35,  36,
  37,   0,  95,  99, 100,  82,   0,   0,   0,   0,
   0,   0,   0,   0,  79,  80,  81,   0,  26,   0,
  44,   0,  97,  96, 101, 102,  84,  85,  86,  87,
  88,  89,  90,  91,  92,   0,  24,  41,  45,   0,
 103,  93,  38 };
#ifndef lint
static char yaccpar_sccsid[] = "@(#)yaccpar	4.1	(Berkeley)	2/11/83";
#endif not lint

#
# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

#ifdef YYDEBUG
int yydebug = 0; /* 1 for debugging */
#endif
YYSTYPE yyv[YYMAXDEPTH]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */

yyparse() {

	short yys[YYMAXDEPTH];
	short yyj, yym;
	register YYSTYPE *yypvt;
	register short yystate, *yyps, yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

#ifdef YYDEBUG
	if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
#endif
		if( ++yyps> &yys[YYMAXDEPTH] ) { yyerror( "yacc stack overflow" ); return(1); }
		*yyps = yystate;
		++yypv;
		*yypv = yyval;

 yynewstate:

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

	if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
		}

 yydefault:
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 ) {
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 ){
			if( *yyxi == yychar ) break;
			}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:   /* brand new error */

			yyerror( "syntax error" );
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

#ifdef YYDEBUG
			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
#endif
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

#ifdef YYDEBUG
			if( yydebug ) printf( "error recovery discards char %d\n", yychar );
#endif

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

		}

	/* reduction by production yyn */

#ifdef YYDEBUG
		if( yydebug ) printf("reduce %d\n",yyn);
#endif
		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		switch(yym){
			
case 1:
# line 209 "../../../../../../src/30/kernel/src/config/config.y"
 { verifysystemspecs(); } break;
case 4:
# line 220 "../../../../../../src/30/kernel/src/config/config.y"
 { newdev(&cur); } break;
case 6:
# line 224 "../../../../../../src/30/kernel/src/config/config.y"
 { do_trace = !do_trace; } break;
case 9:
# line 232 "../../../../../../src/30/kernel/src/config/config.y"
 {
		if (!strcmp(yypvt[-0].str, "vax")) {
			machine = MACHINE_VAX;
			machinename = "vax";
		} else if (!strcmp(yypvt[-0].str, "sun")) {
			/* default to Sun 3 */
			machine = MACHINE_SUN3;
			machinename = "sun3";
		} else if (!strcmp(yypvt[-0].str, "sun2")) {
			machine = MACHINE_SUN2;
			machinename = "sun2";
		} else if (!strcmp(yypvt[-0].str, "sun3")) {
			machine = MACHINE_SUN3;
			machinename = "sun3";
		} else if (!strcmp(yypvt[-0].str, "sun4")) {
			machine = MACHINE_SUN4;
			machinename = "sun4";
		} else if (!strcmp(yypvt[-0].str, "romp")) {
			machine = MACHINE_ROMP;
			machinename = "romp";
		} else if (!strcmp(yypvt[-0].str, "ca")) {
			machine = MACHINE_ROMP;
			machinename = "ca";
		} else if (!strcmp(yypvt[-0].str, "mmax")) {
			machine = MACHINE_MMAX;
			machinename = "mmax";
		} else if (!strcmp(yypvt[-0].str, "sqt")) {
			machine = MACHINE_SQT;
			machinename = "sqt";
		} else if (!strcmp(yypvt[-0].str, "i")) {
			machine = MACHINE_I386;
			machinename = "i386";
		} else if (!strcmp(yypvt[-0].str, "i386")) {
			machine = MACHINE_I386;
			machinename = "i386";
		} else if (!strcmp(yypvt[-0].str, "ix")) {
			machine = MACHINE_IX;
			machinename = "ix";
		} else if (!strcmp(yypvt[-0].str, "mipsy")) {
			machine = MACHINE_MIPSY;
			machinename = "mipsy";
		} else if (!strcmp(yypvt[-0].str, "mips")) {
			machine = MACHINE_MIPS;
			machinename = "mips";
		} else if (!strcmp(yypvt[-0].str, "i860")) {
			machine = MACHINE_I860;
			machinename = "i860";
		} else
			yyerror("Unknown machine type");
	      } break;
case 10:
# line 283 "../../../../../../src/30/kernel/src/config/config.y"
 {
		struct cputype *cp =
		    (struct cputype *)malloc(sizeof (struct cputype));
		cp->cpu_name = ns(yypvt[-0].str);
		cp->cpu_next = cputype;
		cputype = cp;
		free(temp_id);
	      } break;
case 13:
# line 296 "../../../../../../src/30/kernel/src/config/config.y"
 { ident = ns(yypvt[-0].str); } break;
case 15:
# line 302 "../../../../../../src/30/kernel/src/config/config.y"
 { checksystemspec(*confp); } break;
case 16:
# line 307 "../../../../../../src/30/kernel/src/config/config.y"
 { mkconf(yypvt[-0].str); } break;
case 26:
# line 333 "../../../../../../src/30/kernel/src/config/config.y"
 { mkswap(*confp, yypvt[-1].file, yypvt[-0].val); } break;
case 27:
# line 338 "../../../../../../src/30/kernel/src/config/config.y"
 {
			struct file_list *fl = newswap();

			if (eq(yypvt[-0].str, "generic"))
				fl->f_fn = yypvt[-0].str;
			else {
				fl->f_swapdev = nametodev(yypvt[-0].str, 0, 'b');
				fl->f_fn = devtoname(fl->f_swapdev);
			}
			yyval.file = fl;
		} break;
case 28:
# line 350 "../../../../../../src/30/kernel/src/config/config.y"
 {
			struct file_list *fl = newswap();

			fl->f_swapdev = yypvt[-0].val;
			fl->f_fn = devtoname(yypvt[-0].val);
			yyval.file = fl;
		} break;
case 29:
# line 361 "../../../../../../src/30/kernel/src/config/config.y"
 {
			struct file_list *fl = *confp;

			if (fl && fl->f_rootdev != NODEV)
				yyerror("extraneous root device specification");
			else
				fl->f_rootdev = yypvt[-0].val;
		} break;
case 30:
# line 373 "../../../../../../src/30/kernel/src/config/config.y"
 { yyval.val = nametodev(yypvt[-0].str, 0, 'a'); } break;
case 32:
# line 379 "../../../../../../src/30/kernel/src/config/config.y"
 {
			struct file_list *fl = *confp;

			if (fl && fl->f_dumpdev != NODEV)
				yyerror("extraneous dump device specification");
			else
				fl->f_dumpdev = yypvt[-0].val;
		} break;
case 33:
# line 392 "../../../../../../src/30/kernel/src/config/config.y"
 { yyval.val = nametodev(yypvt[-0].str, 0, 'b'); } break;
case 35:
# line 398 "../../../../../../src/30/kernel/src/config/config.y"
 {
			struct file_list *fl = *confp;

			if (fl && fl->f_argdev != NODEV)
				yyerror("extraneous arg device specification");
			else
				fl->f_argdev = yypvt[-0].val;
		} break;
case 36:
# line 410 "../../../../../../src/30/kernel/src/config/config.y"
 { yyval.val = nametodev(yypvt[-0].str, 0, 'b'); } break;
case 38:
# line 416 "../../../../../../src/30/kernel/src/config/config.y"
 { yyval.val = makedev(yypvt[-2].val, yypvt[-0].val); } break;
case 41:
# line 426 "../../../../../../src/30/kernel/src/config/config.y"
 { yyval.val = yypvt[-0].val; } break;
case 42:
# line 428 "../../../../../../src/30/kernel/src/config/config.y"
 { yyval.val = 0; } break;
case 43:
# line 433 "../../../../../../src/30/kernel/src/config/config.y"
 { yyval.str = yypvt[-0].str; } break;
case 44:
# line 435 "../../../../../../src/30/kernel/src/config/config.y"
 {
			char buf[80];

			(void) sprintf(buf, "%s%d", yypvt[-1].str, yypvt[-0].val);
			yyval.str = ns(buf); free(yypvt[-1].str);
		} break;
case 45:
# line 442 "../../../../../../src/30/kernel/src/config/config.y"
 {
			char buf[80];

			(void) sprintf(buf, "%s%d%s", yypvt[-2].str, yypvt[-1].val, yypvt[-0].str);
			yyval.str = ns(buf); free(yypvt[-2].str);
		} break;
case 48:
# line 458 "../../../../../../src/30/kernel/src/config/config.y"
 {
		struct opt *op = (struct opt *)malloc(sizeof (struct opt));
		op->op_name = ns(yypvt[-0].str);
		op->op_next = (struct opt *) 0;
		op->op_value = 0;
		if (opt == (struct opt *) 0)
			opt = op;
		else
			opt_tail->op_next = op;
		opt_tail = op;
		free(temp_id);
	      } break;
case 49:
# line 471 "../../../../../../src/30/kernel/src/config/config.y"
 {
		struct opt *op = (struct opt *)malloc(sizeof (struct opt));
		op->op_name = ns(yypvt[-2].str);
		op->op_next = (struct opt *) 0;
		op->op_value = ns(yypvt[-0].str);
		if (opt == (struct opt *) 0)
			opt = op;
		else
			opt_tail->op_next = op;
		opt_tail = op;
		free(temp_id);
		if (val_id)
			free(val_id);
	      } break;
case 50:
# line 488 "../../../../../../src/30/kernel/src/config/config.y"
 { yyval.str = val_id = ns(yypvt[-0].str); } break;
case 51:
# line 490 "../../../../../../src/30/kernel/src/config/config.y"
 { char nb[16];
	          (void) sprintf(nb, "%u", yypvt[-0].val);
	      	  yyval.str = val_id = ns(nb);
	      } break;
case 52:
# line 495 "../../../../../../src/30/kernel/src/config/config.y"
 { yyval.str = val_id = ns(""); } break;
case 53:
# line 500 "../../../../../../src/30/kernel/src/config/config.y"
 { yyval.str = temp_id = ns(yypvt[-0].str); } break;
case 56:
# line 511 "../../../../../../src/30/kernel/src/config/config.y"
 {
		struct opt *op = (struct opt *)malloc(sizeof (struct opt));
		op->op_name = ns(yypvt[-2].str);
		op->op_next =  (struct opt *) 0;
		op->op_value = ns(yypvt[-0].str);
		if (mkopt == (struct opt *) 0)
			mkopt = op;
		else
			mkopt_tail->op_next = op;
		mkopt_tail = op;
		free(temp_id);
		if (val_id)
			free(val_id);
	      } break;
case 57:
# line 528 "../../../../../../src/30/kernel/src/config/config.y"
 { yyval.str = ns("uba"); } break;
case 58:
# line 530 "../../../../../../src/30/kernel/src/config/config.y"
 { yyval.str = ns("mba"); } break;
case 59:
# line 532 "../../../../../../src/30/kernel/src/config/config.y"
 {
		if (machine != MACHINE_SUN2 && machine != MACHINE_SUN3
		    && machine != MACHINE_SUN4)
			yyerror("wrong machine type for vme16d16");
		yyval.str = ns("vme16d16");
		} break;
case 60:
# line 539 "../../../../../../src/30/kernel/src/config/config.y"
 {
		if (machine != MACHINE_SUN2 && machine != MACHINE_SUN3
		    && machine != MACHINE_SUN4)
			yyerror("wrong machine type for vme24d16");
			yyval.str = ns("vme24d16");
		} break;
case 61:
# line 546 "../../../../../../src/30/kernel/src/config/config.y"
 {
		if (machine != MACHINE_SUN3 && machine != MACHINE_SUN4)

                        yyerror("wrong machine type for vme32d16");
                yyval.str = ns("vme32d16");
                } break;
case 62:
# line 553 "../../../../../../src/30/kernel/src/config/config.y"
 {
                if (machine != MACHINE_SUN3  && machine != MACHINE_SUN4)
                        yyerror("wrong machine type for vme16d32");
                yyval.str = ns("vme16d32");
                } break;
case 63:
# line 559 "../../../../../../src/30/kernel/src/config/config.y"
 {
		if (machine != MACHINE_SUN3 && machine != MACHINE_SUN4)
			yyerror("wrong machine type for vme24d32");
		yyval.str = ns("vme24d32");
		} break;
case 64:
# line 565 "../../../../../../src/30/kernel/src/config/config.y"
 {
		if (machine != MACHINE_SUN3 && machine != MACHINE_SUN4)
			yyerror("wrong machine type for vme32d32");
		yyval.str = ns("vme32d32");
		} break;
case 65:
# line 571 "../../../../../../src/30/kernel/src/config/config.y"
 {
		if (machine != MACHINE_MIPSY && machine != MACHINE_MIPS)
			yyerror("wrong machine type for vme");
			yyval.str = ns("vme");
		} break;
case 66:
# line 577 "../../../../../../src/30/kernel/src/config/config.y"
 {
		if (machine != MACHINE_MIPSY && machine != MACHINE_MIPS)
			yyerror("wrong machine type for mbii");
			yyval.str = ns("mbii");
		} break;
case 67:
# line 583 "../../../../../../src/30/kernel/src/config/config.y"
 { yyval.str = ns(yypvt[-0].str); } break;
case 68:
# line 588 "../../../../../../src/30/kernel/src/config/config.y"
 { cur.d_type = DEVICE; } break;
case 69:
# line 590 "../../../../../../src/30/kernel/src/config/config.y"
 { cur.d_type = MASTER; } break;
case 70:
# line 592 "../../../../../../src/30/kernel/src/config/config.y"
 { cur.d_dk = 1; cur.d_type = DEVICE; } break;
case 71:
# line 595 "../../../../../../src/30/kernel/src/config/config.y"
 { cur.d_type = DEVICE; } break;
case 72:
# line 597 "../../../../../../src/30/kernel/src/config/config.y"
 { cur.d_type = CONTROLLER; } break;
case 73:
# line 599 "../../../../../../src/30/kernel/src/config/config.y"
 {
		cur.d_name = yypvt[-0].str;
		cur.d_type = PSEUDO_DEVICE;
		} break;
case 74:
# line 604 "../../../../../../src/30/kernel/src/config/config.y"
 {
		cur.d_name = yypvt[-1].str;
		cur.d_type = PSEUDO_DEVICE;
		cur.d_slave = yypvt[-0].val;
		} break;
case 75:
# line 612 "../../../../../../src/30/kernel/src/config/config.y"
 {
		cur.d_name = yypvt[-1].str;
		if (eq(yypvt[-1].str, "mba"))
			seen_mba = 1;
		else if (eq(yypvt[-1].str, "uba"))
			seen_uba = 1;
		else if (eq(yypvt[-1].str, "mbii"))
			seen_mbii = 1;
		else if (eq(yypvt[-1].str, "vme"))
			seen_vme = 1;
		cur.d_unit = yypvt[-0].val;
		} break;
case 76:
# line 627 "../../../../../../src/30/kernel/src/config/config.y"
 { init_dev(&cur); } break;
case 79:
# line 637 "../../../../../../src/30/kernel/src/config/config.y"
 {
		if (eq(cur.d_name, "mba") || eq(cur.d_name, "uba")
		    || eq(cur.d_name, "mbii") || eq(cur.d_name, "vme")) {
			(void) sprintf(errbuf,
			    "%s must be connected to a nexus", cur.d_name);
			yyerror(errbuf);
		}
		cur.d_conn = connect(yypvt[-1].str, yypvt[-0].val);
		if (machine == MACHINE_SQT)
			dev_param(&cur, "index", cur.d_unit);
		} break;
case 80:
# line 650 "../../../../../../src/30/kernel/src/config/config.y"
 { 
		check_slot(&cur, yypvt[-0].val);
		cur.d_addr = yypvt[-0].val;
		cur.d_conn = TO_SLOT; 
		 } break;
case 81:
# line 656 "../../../../../../src/30/kernel/src/config/config.y"
 { check_nexus(&cur, yypvt[-0].val); cur.d_conn = TO_NEXUS; } break;
case 84:
# line 666 "../../../../../../src/30/kernel/src/config/config.y"
 {
		cur.d_addr = yypvt[-0].val;
                if (machine == MACHINE_SUN2 || machine == MACHINE_SUN3
		    || machine == MACHINE_SUN4)
			bus_encode(yypvt[-0].val, &cur);
		if (machine == MACHINE_SQT) {
			dev_param(&cur, "csr", yypvt[-0].val);
		}
		} break;
case 85:
# line 676 "../../../../../../src/30/kernel/src/config/config.y"
 {
			cur.d_drive = yypvt[-0].val;
			if (machine == MACHINE_SQT) {
				dev_param(&cur, "drive", yypvt[-0].val);
			}
		} break;
case 86:
# line 683 "../../../../../../src/30/kernel/src/config/config.y"
 {
		if (cur.d_conn != 0 && cur.d_conn != TO_NEXUS &&
		    cur.d_conn->d_type == MASTER)
			cur.d_slave = yypvt[-0].val;
		else
			yyerror("can't specify slave--not to master");
		} break;
case 87:
# line 692 "../../../../../../src/30/kernel/src/config/config.y"
 { cur.d_addrmod = yypvt[-0].val; } break;
case 88:
# line 695 "../../../../../../src/30/kernel/src/config/config.y"
 {
		if ((cur.d_conn != 0) && (cur.d_conn != TO_SLOT) &&
			(cur.d_conn->d_type == CONTROLLER)) {
			cur.d_addr = yypvt[-0].val; 
		}
		else {
			yyerror("device requires controller card");
		    }
		} break;
case 89:
# line 705 "../../../../../../src/30/kernel/src/config/config.y"
 {
		cur.d_flags = yypvt[-0].val;
		if (machine == MACHINE_SQT) {
			dev_param(&cur, "flags", yypvt[-0].val);
		}
	      } break;
case 90:
# line 712 "../../../../../../src/30/kernel/src/config/config.y"
 { 
		 if (machine != MACHINE_SQT)
			yyerror("bin specification only valid on Sequent Balance");
		 if (yypvt[-0].val < 1 || yypvt[-0].val > 7)  
			yyerror("bogus bin number");
		 else {
			cur.d_bin = yypvt[-0].val;
			dev_param(&cur, "bin", yypvt[-0].val);
		}
	       } break;
case 91:
# line 723 "../../../../../../src/30/kernel/src/config/config.y"
 {
		if (machine != MACHINE_SQT)
			yyerror("bad device spec");
		dev_param(&cur, yypvt[-1].str, yypvt[-0].val);
		} break;
case 93:
# line 733 "../../../../../../src/30/kernel/src/config/config.y"
 { yyval.val = -(yypvt[-0].val); } break;
case 94:
# line 738 "../../../../../../src/30/kernel/src/config/config.y"
 { cur.d_pri = 0; } break;
case 95:
# line 740 "../../../../../../src/30/kernel/src/config/config.y"
 { cur.d_pri = yypvt[-0].val; } break;
case 96:
# line 742 "../../../../../../src/30/kernel/src/config/config.y"
 { cur.d_pri = yypvt[-1].val; } break;
case 97:
# line 744 "../../../../../../src/30/kernel/src/config/config.y"
 { cur.d_pri = yypvt[-0].val; } break;
case 99:
# line 750 "../../../../../../src/30/kernel/src/config/config.y"
 { cur.d_vec = yypvt[-0].lst; } break;
case 100:
# line 755 "../../../../../../src/30/kernel/src/config/config.y"
 {
		struct idlst *a = (struct idlst *)malloc(sizeof(struct idlst));
		a->id = yypvt[-0].str; a->id_next = 0; yyval.lst = a;
		a->id_vec = 0;
		} break;
case 101:
# line 760 "../../../../../../src/30/kernel/src/config/config.y"

		{
		struct idlst *a = (struct idlst *)malloc(sizeof(struct idlst));
	        a->id = yypvt[-1].str; a->id_next = yypvt[-0].lst; yyval.lst = a;
		a->id_vec = 0;
		} break;
case 102:
# line 767 "../../../../../../src/30/kernel/src/config/config.y"
 {
		struct idlst *a = (struct idlst *)malloc(sizeof(struct idlst));
		a->id_next = 0; a->id = yypvt[-1].str; yyval.lst = a;
		a->id_vec = yypvt[-0].val;
		} break;
case 103:
# line 773 "../../../../../../src/30/kernel/src/config/config.y"
 {
		struct idlst *a = (struct idlst *)malloc(sizeof(struct idlst));
		a->id_next = yypvt[-0].lst; a->id = yypvt[-2].str; yyval.lst = a;
		a->id_vec = yypvt[-1].val;
		} break;
		}
		goto yystack;  /* stack new state and value */

	}
