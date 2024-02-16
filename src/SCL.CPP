/******************************************************************************
	ADX scale file
******************************************************************************/
#include	<windows.h>
#include	<stdio.h>
#include	<omp.h>
#include	"le.h"
#include	"adx.h"



		unsigned long		scl_sso;			//scale save order
extern	unsigned long		adx_sc;				//scale cycle



		void	scale_save			(	COMPLEX_SCL *,
										HANDLE,
										unsigned long	);

		void	scale_load			(	COMPLEX_SCL *,
										HANDLE,
										unsigned long	);

		void	scale_bit_search	(	unsigned long,
										COMPLEX_SCL*,
										unsigned long,
										unsigned long,
										unsigned long,
										unsigned long,
										unsigned long	);

		void	vote				(	COMPLEX_VOTE *,
										COMPLEX_SCL *,
										unsigned long	);

		void	dbm					(	COMPLEX_SCL *,
										COMPLEX_VOTE *	);

		void	scale_text2bin(			unsigned short *,
										COMPLEX_SCL *,
										unsigned long,
										unsigned long	);

		void	ps2_rio_paradise(		COMPLEX_SCL *	);



void	scale_bit_search(		unsigned long	ssiz,
								COMPLEX_SCL		*src,
								unsigned long	scl_sfl,
								unsigned long	scl_ssb,
								unsigned long	scl_seb,
								unsigned long	scl_not,
								unsigned long	scl_skp)
{
	if(scl_not)	printf("bit search not %01X - %01X\n",scl_ssb,scl_seb);
	else		printf("bit search     %01X - %01X\n",scl_ssb,scl_seb);
	scl_seb++;

#pragma omp parallel num_threads(omp_get_num_procs()*4)
{
	long			dx,dy,dz;
	long			sx,sy,sz;
	unsigned long	   cy=ssiz/16;
	unsigned short	dv;
	unsigned short	sv; 

	if(scl_skp){
		for(sx=scl_ssb;sx<scl_seb;sx++){
#pragma omp for
			for(sy=1;sy<cy;sy++){
				if(src[sy*16+sx].w==0xffffffff)			//ä˘Ç…àÍívÇµÇƒÇÈèÍçá
					goto next_search_a;
				sz=cy-1-sy;
				for(dz=0;dz<sz;dz++){
					dv=scl_not^	src[(0 +dz)*16+sx].b;
					sv=			src[(sy+dz)*16+sx].b;

					if(dv!=sv){
						if(dz>=scl_sfl){				//ï~ãèílÇí¥Ç¶ÇÈ
							for(sz=0;sz<dz;sz++){
								if(	src[(0 +sz)*16+sx].w!=0xffffffff){
									src[(0 +sz)*16+sx].w	=0xffffffff;
									src[(0 +sz)*16+sx].da	=sy+sz;
									src[(0 +sz)*16+sx].db	=sx;	}

								if(	src[(sy+sz)*16+sx].w!=0xffffffff){
									src[(sy+sz)*16+sx].w	=0xffffffff;
									src[(sy+sz)*16+sx].da	=0 +sz;
									src[(sy+sz)*16+sx].db	=sx;	}

									src[(sy+sz)*16+sx].n	=scl_not;
							}
			printf("SRC(bit%02d:%08X)=DST(bit%02d:%08X) LEN=%08X Cyc=%08X\n"
										,sx,sy,sx,0,dz,sy);
						}
						goto next_search_a;
					}else{
						if((dz+1==sz)&&(dz>=scl_sfl)){	//Ç‡Ç§èIÇÌÇÈ
							for(sz=0;sz<dz;sz++){
								if(	src[(0 +sz)*16+sx].w!=0xffffffff){
									src[(0 +sz)*16+sx].w	=0xffffffff;
									src[(0 +sz)*16+sx].da	=sy+sz;
									src[(0 +sz)*16+sx].db	=sx;	}

								if(	src[(sy+sz)*16+sx].w!=0xffffffff){
									src[(sy+sz)*16+sx].w	=0xffffffff;
									src[(sy+sz)*16+sx].da	=0 +sz;
									src[(sy+sz)*16+sx].db	=sx;	}

									src[(sy+sz)*16+sx].n	=scl_not;
							}
			printf("SRC(bit%02d:%08X)=DST(bit%02d:%08X) LEN=%08X Cyc=%08X\n"
										,sx,sy,sx,0,dz,sy);
							goto next_search_a;
						}
					}
				}
next_search_a:
				NULL;
			}
		}
	}else{
		for(sx=scl_ssb;sx<scl_seb;sx++){
#pragma omp for
			for(sy=0;sy<cy;sy++){
				if(src[sy*16+sx].w==0xffffffff)			//ä˘Ç…àÍívÇµÇƒÇÈèÍçá
					goto next_search_c;
		for(dx=0;dx<16;dx++){
			for(dy=0;dy<cy;dy++){
				if((sx==dx)&&(sy==dy))					//é©ï™é©êgÇÕèúäO
					goto next_search_b;

				sz=min(cy-1-sy,cy-1-dy);
				for(dz=0;dz<sz;dz++){
					dv=scl_not^	src[(dy+dz)*16+dx].b;
					sv=			src[(sy+dz)*16+sx].b;

					if(dv!=sv){
						if(dz>=scl_sfl){				//ï~ãèílÇí¥Ç¶ÇÈ
							for(sz=0;sz<dz;sz++){
								if(	src[(dy+sz)*16+dx].w!=0xffffffff){
									src[(dy+sz)*16+dx].w	=0xffffffff;
									src[(dy+sz)*16+dx].da	=sy+sz;
									src[(dy+sz)*16+dx].db	=sx;	}

								if(	src[(sy+sz)*16+sx].w!=0xffffffff){
									src[(sy+sz)*16+sx].w	=0xffffffff;
									src[(sy+sz)*16+sx].da	=dy+sz;
									src[(sy+sz)*16+sx].db	=dx;	}

									src[(sy+sz)*16+sx].n	=scl_not;
							}
			printf("SRC(bit%02d:%08X)=DST(bit%02d:%08X) LEN=%08X Cyc=%08X\n"
										,sx,sy,dx,dy,dz,max(sy,dy)-min(sy,dy));
							goto next_search_c;

						}
						goto next_search_b;
					}else{
						if((dz+1==sz)&&(dz>=scl_sfl)){	//Ç‡Ç§èIÇÌÇÈ
							for(sz=0;sz<dz;sz++){
								if(	src[(dy+sz)*16+dx].w!=0xffffffff){
									src[(dy+sz)*16+dx].w	=0xffffffff;
									src[(dy+sz)*16+dx].da	=sy+sz;
									src[(dy+sz)*16+dx].db	=sx;	}

								if(	src[(sy+sz)*16+sx].w!=0xffffffff){
									src[(sy+sz)*16+sx].w	=0xffffffff;
									src[(sy+sz)*16+sx].da	=dy+sz;
									src[(sy+sz)*16+sx].db	=dx;	}

									src[(sy+sz)*16+sx].n	=scl_not;
							}
			printf("SRC(bit%02d:%08X)=DST(bit%02d:%08X) LEN=%08X Cyc=%08X\n"
										,sx,sy,dx,dy,dz,max(sy,dy)-min(sy,dy));
							goto next_search_c;
						}
					}
				}
next_search_b:
				NULL;
			}
		}
next_search_c:
				NULL;
			}
		}
	}
}

}



void	scale_load(	COMPLEX_SCL		*scl,
					HANDLE			hscl,
					unsigned long	lscl)
{	unsigned long	tx,tz;
	char			tb[COMPLEX_SCL_SIZE+4];
	COMPLEX_SCL		ts;

	memset(scl,0,sizeof(COMPLEX_SCL)*lscl);

	for(tx=0;tx<lscl;tx++){
		ReadFile(hscl,tb,COMPLEX_SCL_SIZE,&tz,NULL);
		tb[COMPLEX_SCL_SIZE+3]=0;
		sscanf(tb,"%04X,%08X,%01X,%01X,%01X,%08X,%08X,%01X",
					&ts.scale,
					&ts.sa,
					&ts.sb,
					&ts.b,
					&ts.n,
					&ts.w,
					&ts.da,
					&ts.db);
		memcpy(&scl[ts.sa*16+ts.sb],&ts,sizeof(COMPLEX_SCL));
	}
}



void	scale_save(	COMPLEX_SCL		*scl,
					HANDLE			hscl,
					unsigned long	lscl)
{	unsigned long	tx,ty,tz;
	char			tb[COMPLEX_SCL_SIZE+4];

	for(tx=0;tx<lscl;tx++){
		ty=tx&0xfffffff0;

		scl[tx].scale=	scl[ty+ 0].b<<15|
						scl[ty+ 1].b<<14|
						scl[ty+ 2].b<<13|
						scl[ty+ 3].b<<12|
						scl[ty+ 4].b<<11|
						scl[ty+ 5].b<<10|
						scl[ty+ 6].b<< 9|
						scl[ty+ 7].b<< 8|
						scl[ty+ 8].b<< 7|
						scl[ty+ 9].b<< 6|
						scl[ty+10].b<< 5|
						scl[ty+11].b<< 4|
						scl[ty+12].b<< 3|
						scl[ty+13].b<< 2|
						scl[ty+14].b<< 1|
						scl[ty+15].b<< 0;
	}

	if(scl_sso){
		for(ty=0;ty<16;ty++){
			for(tx=0;tx<lscl;tx++){
				if(scl[tx].sb==ty){

					sprintf(tb,"%04X,%08X,%01X,%01X,%01X,%08X,%08X,%01X\n",
								scl[tx].scale,
								scl[tx].sa,
								scl[tx].sb,
								scl[tx].b,
								scl[tx].n,
								scl[tx].w,
								scl[tx].da,
								scl[tx].db);
					WriteFile(hscl,tb,strlen(tb),&tz,NULL);

				}
			}
		}
	}else{
			for(tx=0;tx<lscl;tx++){
					sprintf(tb,"%04X,%08X,%01X,%01X,%01X,%08X,%08X,%01X\n",
								scl[tx].scale,
								scl[tx].sa,
								scl[tx].sb,
								scl[tx].b,
								scl[tx].n,
								scl[tx].w,
								scl[tx].da,
								scl[tx].db);
					WriteFile(hscl,tb,strlen(tb),&tz,NULL);
			}
	}
}



void	vote(	COMPLEX_VOTE *vot,
				COMPLEX_SCL *scl,
				unsigned long max	)
{
	unsigned long	tul1,tul2;

	for(tul1=0;tul1<max;tul1++){
		if(	(vot[tul1%(adx_sc*16)].vote_1!=0xffffffff)&&
			(vot[tul1%(adx_sc*16)].vote_0!=0xffffffff)	){

			if(scl[tul1].b==1){
					vot[tul1%(adx_sc*16)].vote_1+=scl[tul1].w;
				if(	vot[tul1%(adx_sc*16)].vote_1< scl[tul1].w)
					vot[tul1%(adx_sc*16)].vote_1= 0xffffffff;
			}else{
					vot[tul1%(adx_sc*16)].vote_0+=scl[tul1].w;
				if(	vot[tul1%(adx_sc*16)].vote_0< scl[tul1].w)
					vot[tul1%(adx_sc*16)].vote_0= 0xffffffff;
			}

			if(	(	0xffffffff			==scl[tul1].w )&&
				(	vot[tul1%(adx_sc*16)].da	< scl[tul1].da)		){
					vot[tul1%(adx_sc*16)].n		= scl[tul1].n;
					vot[tul1%(adx_sc*16)].da	= scl[tul1].da;
					vot[tul1%(adx_sc*16)].db	= scl[tul1].db;
			}
		}
	}
}



void	dbm(	COMPLEX_SCL		*msc,
				COMPLEX_VOTE	*vot	)
{	unsigned long	tul1;

	for(tul1=0;tul1<(adx_sc*16);tul1++){
		if(	(	vot[tul1].vote_0==0xffffffff)||
			(	vot[tul1].vote_1==0xffffffff)	){

			if(	vot[tul1].vote_0==0xffffffff){
				msc[tul1].sa=tul1/16;
				msc[tul1].sb=tul1%16;
				msc[tul1].b=0;
				msc[tul1].n=vot[tul1].n;
				msc[tul1].w=0xffffffff;
				msc[tul1].da=vot[tul1].da;
				msc[tul1].db=vot[tul1].db;
			}else{
				msc[tul1].sa=tul1/16;
				msc[tul1].sb=tul1%16;
				msc[tul1].b=1;
				msc[tul1].n=vot[tul1].n;
				msc[tul1].w=0xffffffff;
				msc[tul1].da=vot[tul1].da;
				msc[tul1].db=vot[tul1].db;
			}
		}else{
			if(	vot[tul1].vote_0>vot[tul1].vote_1){
				msc[tul1].sa=tul1/16;
				msc[tul1].sb=tul1%16;
				msc[tul1].b=0;
				msc[tul1].n=vot[tul1].n;
				msc[tul1].w=vot[tul1].vote_0;
				msc[tul1].da=vot[tul1].da;
				msc[tul1].db=vot[tul1].db;
			}else{
				msc[tul1].sa=tul1/16;
				msc[tul1].sb=tul1%16;
				msc[tul1].b=1;
				msc[tul1].n=vot[tul1].n;
				msc[tul1].w=vot[tul1].vote_1;
				msc[tul1].da=vot[tul1].da;
				msc[tul1].db=vot[tul1].db;
			}
		}
	}
}



void	scale_text2bin(	unsigned short	*adx_scl,
						COMPLEX_SCL		*scl,
						unsigned long	adx_sc,
						unsigned long	scl_len		)
{	unsigned long	tul1;

	for(tul1=0;tul1<scl_len;tul1++)
		if(scl[tul1].sa<adx_sc)
			adx_scl[scl[tul1].sa]|=(scl[tul1].b<<(15-scl[tul1].sb));
}



void	ps2_rio_paradise(	COMPLEX_SCL		*scl	)
{	unsigned long	tx,tz;

	for(tx=0;tx<0x8000;tx++){					//addr / bit	set
		for(tz=0;tz<16;tz++){
				scl[tx*16+tz].sa=tx;
				scl[tx*16+tz].sb=tz;
		}
	}

	for(tx=0;tx<0x8000;tx++)					//bit	0	clear
				scl[tx*16+ 0].b=0;
	for(tx=0;tx<0x8000;tx++)					//bit	F	clear
				scl[tx*16+15].b=0;

	for(tx=0;tx<0x4000;tx++){					//bit	1	not shadow
				scl[(tx+0x4000)*16+ 1].b
			=~	scl[(tx+0x0000)*16+ 1].b&1;	}
	for(tx=0;tx<0x2000;tx++){					//bit	2	not shadow
				scl[(tx+0x2000)*16+ 2].b
			=~	scl[(tx+0x0000)*16+ 2].b&1;	}
	for(tx=0;tx<0x1000;tx++){					//bit	3	not shadow
				scl[(tx+0x1000)*16+ 3].b
			=~	scl[(tx+0x0000)*16+ 3].b&1;	}
	for(tx=0;tx<0x0800;tx++){					//bit	4	not shadow
				scl[(tx+0x0800)*16+ 4].b
			=~	scl[(tx+0x0000)*16+ 4].b&1;	}
	for(tx=0;tx<0x0400;tx++){					//bit	5	not shadow
				scl[(tx+0x0400)*16+ 5].b
			=~	scl[(tx+0x0000)*16+ 5].b&1;	}
	for(tx=0;tx<0x0200;tx++){					//bit	6	not shadow
				scl[(tx+0x0200)*16+ 6].b
			=~	scl[(tx+0x0000)*16+ 6].b&1;	}
	for(tx=0;tx<0x0100;tx++){					//bit	7	not shadow
				scl[(tx+0x0100)*16+ 7].b
			=~	scl[(tx+0x0000)*16+ 7].b&1;	}
	for(tx=0;tx<0x0080;tx++){					//bit	8	not shadow
				scl[(tx+0x0080)*16+ 8].b
			=~	scl[(tx+0x0000)*16+ 8].b&1;	}
	for(tx=0;tx<0x0040;tx++){					//bit	9	not shadow
				scl[(tx+0x0040)*16+ 9].b
			=~	scl[(tx+0x0000)*16+ 9].b&1;	}
	for(tx=0;tx<0x0020;tx++){					//bit	A	not shadow
				scl[(tx+0x0020)*16+10].b
			=~	scl[(tx+0x0000)*16+10].b&1;	}
	for(tx=0;tx<0x0010;tx++){					//bit	B	not shadow
				scl[(tx+0x0010)*16+11].b
			=~	scl[(tx+0x0000)*16+11].b&1;	}
	for(tx=0;tx<0x0008;tx++){					//bit	C	not shadow
				scl[(tx+0x0008)*16+12].b
			=~	scl[(tx+0x0000)*16+12].b&1;	}
	for(tx=0;tx<0x0004;tx++){					//bit	D	not shadow
				scl[(tx+0x0004)*16+13].b
			=~	scl[(tx+0x0000)*16+13].b&1;	}
	for(tx=0;tx<0x0002;tx++){					//bit	E	not shadow
				scl[(tx+0x0002)*16+14].b
			=~	scl[(tx+0x0000)*16+14].b&1;	}

	for(tx=0x4000;tx<0x8000;tx++){				//bit	2	shadow
				scl[(tx+0x0000)*16+ 2].b
			=	scl[(tx%0x4000)*16+ 2].b;	}
	for(tx=0x2000;tx<0x8000;tx++){				//bit	3	shadow
				scl[(tx+0x0000)*16+ 3].b
			=	scl[(tx%0x2000)*16+ 3].b;	}
	for(tx=0x1000;tx<0x8000;tx++){				//bit	4	shadow
				scl[(tx+0x0000)*16+ 4].b
			=	scl[(tx%0x1000)*16+ 4].b;	}
	for(tx=0x0800;tx<0x8000;tx++){				//bit	5	shadow
				scl[(tx+0x0000)*16+ 5].b
			=	scl[(tx%0x0800)*16+ 5].b;	}
	for(tx=0x0400;tx<0x8000;tx++){				//bit	6	shadow
				scl[(tx+0x0000)*16+ 6].b
			=	scl[(tx%0x0400)*16+ 6].b;	}
	for(tx=0x0200;tx<0x8000;tx++){				//bit	7	shadow
				scl[(tx+0x0000)*16+ 7].b
			=	scl[(tx%0x0200)*16+ 7].b;	}
	for(tx=0x0100;tx<0x8000;tx++){				//bit	8	shadow
				scl[(tx+0x0000)*16+ 8].b
			=	scl[(tx%0x0100)*16+ 8].b;	}
	for(tx=0x0080;tx<0x8000;tx++){				//bit	9	shadow
				scl[(tx+0x0000)*16+ 9].b
			=	scl[(tx%0x0080)*16+ 9].b;	}
	for(tx=0x0040;tx<0x8000;tx++){				//bit	A	shadow
				scl[(tx+0x0000)*16+10].b
			=	scl[(tx%0x0040)*16+10].b;	}
	for(tx=0x0020;tx<0x8000;tx++){				//bit	B	shadow
				scl[(tx+0x0000)*16+11].b
			=	scl[(tx%0x0020)*16+11].b;	}
	for(tx=0x0010;tx<0x8000;tx++){				//bit	C	shadow
				scl[(tx+0x0000)*16+12].b
			=	scl[(tx%0x0010)*16+12].b;	}
	for(tx=0x0008;tx<0x8000;tx++){				//bit	D	shadow
				scl[(tx+0x0000)*16+13].b
			=	scl[(tx%0x0008)*16+13].b;	}
	for(tx=0x0004;tx<0x8000;tx++){				//bit	E	shadow
				scl[(tx+0x0000)*16+14].b
			=	scl[(tx%0x0004)*16+14].b;	}

}



void	xb360_imas_l4u(	COMPLEX_SCL		*scl	)
{	unsigned long	tx,tz;

	for(tx=0;tx<0x8000;tx++){					//addr / bit	set
		for(tz=0;tz<16;tz++){
				scl[tx*16+tz].sa=tx;
				scl[tx*16+tz].sb=tz;
		}
	}

	for(tx=0;tx<0x8000;tx++)					//bit	0	clear
				scl[tx*16+ 0].b=0;

	for(tx=0;tx<0x4000;tx++){					//bit	1	not shadow
				scl[(tx+0x4000)*16+ 1].b
			=~	scl[(tx+0x0000)*16+ 1].b&1;	}
	for(tx=0;tx<0x2000;tx++){					//bit	2	not shadow
				scl[(tx+0x2000)*16+ 2].b
			=~	scl[(tx+0x0000)*16+ 2].b&1;	}
	for(tx=0;tx<0x1000;tx++){					//bit	3	not shadow
				scl[(tx+0x1000)*16+ 3].b
			=~	scl[(tx+0x0000)*16+ 3].b&1;	}
	for(tx=0;tx<0x0800;tx++){					//bit	4	not shadow
				scl[(tx+0x0800)*16+ 4].b
			=~	scl[(tx+0x0000)*16+ 4].b&1;	}
	for(tx=0;tx<0x0400;tx++){					//bit	5	not shadow
				scl[(tx+0x0400)*16+ 5].b
			=~	scl[(tx+0x0000)*16+ 5].b&1;	}
	for(tx=0;tx<0x0200;tx++){					//bit	6	not shadow
				scl[(tx+0x0200)*16+ 6].b
			=~	scl[(tx+0x0000)*16+ 6].b&1;	}
	for(tx=0;tx<0x0100;tx++){					//bit	7	not shadow
				scl[(tx+0x0100)*16+ 7].b
			=~	scl[(tx+0x0000)*16+ 7].b&1;	}
	for(tx=0;tx<0x0080;tx++){					//bit	8	not shadow
				scl[(tx+0x0080)*16+ 8].b
			=~	scl[(tx+0x0000)*16+ 8].b&1;	}
	for(tx=0;tx<0x0040;tx++){					//bit	9	not shadow
				scl[(tx+0x0040)*16+ 9].b
			=~	scl[(tx+0x0000)*16+ 9].b&1;	}
	for(tx=0;tx<0x0020;tx++){					//bit	A	not shadow
				scl[(tx+0x0020)*16+10].b
			=~	scl[(tx+0x0000)*16+10].b&1;	}
	for(tx=0;tx<0x0010;tx++){					//bit	B	not shadow
				scl[(tx+0x0010)*16+11].b
			=~	scl[(tx+0x0000)*16+11].b&1;	}
	for(tx=0;tx<0x0008;tx++){					//bit	C	not shadow
				scl[(tx+0x0008)*16+12].b
			=~	scl[(tx+0x0000)*16+12].b&1;	}
	for(tx=0;tx<0x0004;tx++){					//bit	D	not shadow
				scl[(tx+0x0004)*16+13].b
			=~	scl[(tx+0x0000)*16+13].b&1;	}
	for(tx=0;tx<0x0002;tx++){					//bit	E	not shadow
				scl[(tx+0x0002)*16+14].b
			=~	scl[(tx+0x0000)*16+14].b&1;	}
	for(tx=0;tx<0x0001;tx++){					//bit	E	not shadow
				scl[(tx+0x0001)*16+15].b
			=~	scl[(tx+0x0000)*16+15].b&1;	}

	for(tx=0x4000;tx<0x8000;tx++){				//bit	2	shadow
				scl[(tx+0x0000)*16+ 2].b
			=	scl[(tx%0x4000)*16+ 2].b;	}
	for(tx=0x2000;tx<0x8000;tx++){				//bit	3	shadow
				scl[(tx+0x0000)*16+ 3].b
			=	scl[(tx%0x2000)*16+ 3].b;	}
	for(tx=0x1000;tx<0x8000;tx++){				//bit	4	shadow
				scl[(tx+0x0000)*16+ 4].b
			=	scl[(tx%0x1000)*16+ 4].b;	}
	for(tx=0x0800;tx<0x8000;tx++){				//bit	5	shadow
				scl[(tx+0x0000)*16+ 5].b
			=	scl[(tx%0x0800)*16+ 5].b;	}
	for(tx=0x0400;tx<0x8000;tx++){				//bit	6	shadow
				scl[(tx+0x0000)*16+ 6].b
			=	scl[(tx%0x0400)*16+ 6].b;	}
	for(tx=0x0200;tx<0x8000;tx++){				//bit	7	shadow
				scl[(tx+0x0000)*16+ 7].b
			=	scl[(tx%0x0200)*16+ 7].b;	}
	for(tx=0x0100;tx<0x8000;tx++){				//bit	8	shadow
				scl[(tx+0x0000)*16+ 8].b
			=	scl[(tx%0x0100)*16+ 8].b;	}
	for(tx=0x0080;tx<0x8000;tx++){				//bit	9	shadow
				scl[(tx+0x0000)*16+ 9].b
			=	scl[(tx%0x0080)*16+ 9].b;	}
	for(tx=0x0040;tx<0x8000;tx++){				//bit	A	shadow
				scl[(tx+0x0000)*16+10].b
			=	scl[(tx%0x0040)*16+10].b;	}
	for(tx=0x0020;tx<0x8000;tx++){				//bit	B	shadow
				scl[(tx+0x0000)*16+11].b
			=	scl[(tx%0x0020)*16+11].b;	}
	for(tx=0x0010;tx<0x8000;tx++){				//bit	C	shadow
				scl[(tx+0x0000)*16+12].b
			=	scl[(tx%0x0010)*16+12].b;	}
	for(tx=0x0008;tx<0x8000;tx++){				//bit	D	shadow
				scl[(tx+0x0000)*16+13].b
			=	scl[(tx%0x0008)*16+13].b;	}
	for(tx=0x0004;tx<0x8000;tx++){				//bit	E	shadow
				scl[(tx+0x0000)*16+14].b
			=	scl[(tx%0x0004)*16+14].b;	}
	for(tx=0x0002;tx<0x8000;tx++){				//bit	E	shadow
				scl[(tx+0x0000)*16+15].b
			=	scl[(tx%0x0002)*16+15].b;	}

}



