struct pcx_hdr{
	unsigned char manufac;          /* manufacturer */
	unsigned char version;          /* paintbrush version # */
	unsigned char encode;           /* equals 1 if run length encoded */
	unsigned char pix_bits;         /* bits per pixel */
   int           x0;               /* picture dimensions */
   int           y0;
   int           x1;
   int           y1;
   int           hres;             /* horiz. and vert. resolutions */
   int           vres;
   unsigned char palette[16][3];   /* palette info */
   unsigned char reserve;          /* reserved     */
   unsigned char planes;           /* number of color planes */
   int           linebyts;         /* number of bytes per image scan line */
   int           backgrd;          /* background color for cga */
   int           pal_num;          /* palette number for cga */
   int           vid_mode;         /* graphics mode for initgraf to use */
};

