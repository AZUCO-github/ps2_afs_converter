#define	ADXK0	0x7298
#define	ADXK1	0x3350
#define	FOUT	4



typedef	struct	{
	int s1,s2;
}	PREV;



typedef	struct	{
	char	hriff[4];
	long	totalsize;

	char	hwave[8];
	long	hdrsize;
	short	format;
	short	channel;
	long	freq;
	long	byte_per_sec;
	short	blocksize;
	short	bits;

	char	hdata[4];
	long	datasize;
}	WAVHDR;



typedef	struct	{
	unsigned short	scale;					//%04X,		4+1		05
	unsigned long	sa;		//src	addr	//%08X,		8+1		14
	unsigned char	sb;		//src	bit		//%01X,		1+1		16
	unsigned char	b;		//		bit’l	//%01X,		1+1		18
	unsigned char	n;		//		not		//%01X,		1+1		20
	unsigned long	w;		//		weight	//%08X,		8+1		29
	unsigned long	da;		//dst	addr	//%08X,		8+1		38
	unsigned char	db;		//dst	bit		//%01X\n	1+1		40
}	COMPLEX_SCL;

#define	COMPLEX_SCL_SIZE	40

typedef	struct	{
	unsigned long	vote_0;
	unsigned long	vote_1;
	unsigned char	n;		//		not
	unsigned long	da;		//dst	addr
	unsigned char	db;		//dst	bit
}	COMPLEX_VOTE;

