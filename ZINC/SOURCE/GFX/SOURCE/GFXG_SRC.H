	   /**************************************************************/
	   /*  Copyright C Source, Inc. 1987 - 1993  All Rights Reserved */
	   /*      C Source Incorporated. Lee's Summit, Missouri USA     */
	   /**************************************************************/


#define REAL_MODE		0
#define PMODE_16		16
#define PMODE_32		32

#define PHAR_LAP		1
#define RATIONAL		2
#define ERGO			3
#define ZORTECH		4
#define INTEL			5

#if defined (PM)
#define PROT_MODE_SYS	PM
#endif

#if defined (DOSX16)
#define DOS16_EXTENDER	DOSX16
#endif

#if !defined (DOS16_EXTENDER)
#define DOS16_EXTENDER	0
#endif

#if !defined (PROT_MODE_SYS)
#define PROT_MODE_SYS    0
#endif

#if !defined (__GFXG_SRC_)
#define __GFXG_SRC_

#include "GFX.H"
#include "GFX_PRO.H"
#include "GFXLOPRO.H"

/* the pseudo storage classes
 */
#define FAST	register
#define GLOBAL extern
#define PUBLIC extern
#define IMPORT extern
#define LOCAL	static

#if defined (__HIGHC__)
#define LOCAL_FCN		static /*_near*/
#define LOCAL_FCN_PRO    extern /*_near*/
#else
#define LOCAL_FCN		static /*near*/
#define LOCAL_FCN_PRO    static /*near*/
#endif

/* the pseudo types
 */

typedef void * VOID_PTR;
typedef char TINY, TEXT;
typedef unsigned char UTINY;
typedef int BOOL, INT;
typedef unsigned int  UINT;
typedef long LONG;
typedef double DBL;

/* pseudo operators
*/
#define AND		&&
#define OR		||
#define then

/* system parameters
*/
#define NULL			0
#define EOF		   (-1)

#define TRUE			1
#define FALSE			0

#define YES			1
#define NO			0

#define SUCCESS		1
#define FAILURE		0

#define ON			1
#define OFF			0

#define POS			1
#define NEG		    -1

#define NULL_PTR		0
#define PTR_SHIFT		1
#define BUFSIZE		512


/*		 general macros
*/
#define abs(x) 		((x) < 0 ? -(x) : (x))
#define abs_flt(x)		((x) < 0.0 ? -(x) : (x))
#define inrange(a,x,b)	((a) <= (x) && (x) <= (b))
#define isdigit(c)		inrange('0', (c), '9')
#define LOOP			while(1)
#define max(x, y)		(((x) < (y)) ? (y) : (x))
#define min(x, y)		(((x) < (y)) ? (x) : (y))
#define swap(a,b,c)		(c=a, a=b, b=c)
#define toupper(c)		(inrange('a', c, 'z') ? (c) - ('a' - 'A') : (c))

#define MED_RES		0x100		/* Values for gfx.gfx_mode */
#define HIGH_RES		0x2
#define E_RES_MONO		0x4
#define E_RES_COLOR		0x204
#define HERCULES		0x6
#define _256_COLOR		0x8
#define MONO_BITMAP		0xA
#define UNDEFINED_GFX	0xFF08

#define MDA_MEM		0xB000		/* Values of gfx.screen_base */
#define CGA_MEM		0xB800
#define EGA_MEM		0xA000

#define FLOAT_SCALE		123
#define IS_ROUND		1
#define USES_ANGLES		2
#define OFF_SCREEN		0xFFFF

#define is_int_view()		(_gfx.xlat_scale != FLOAT_SCALE)
#define is_float_view()		(_gfx.xlat_scale == FLOAT_SCALE)
#define is_an_ega_mode()		(_gfx.gfx_mode & E_RES_MONO)
#define is_ega_color_mode()	(_gfx.gfx_mode == E_RES_COLOR)

#define MULTI_PLANAR		0x80			// _gfx.sys_flags

#define turn_off_color_flags() (_gfx.color_flags = 0)
#define fill_area()			(_gfx.color_flags & (FILL_PAT+FILL_SOLID))
#define set_ega_planes()		(_gfx.sys_flags & MULTI_PLANAR)
#define turn_on_ega()		if (set_ega_planes()) then (_gfx.color_flags&XOR_PEL) ? _gfx_set_ega_modify_type(3) : _gfx_set_w2_ega_mode();
#define turn_off_ega()		if (set_ega_planes()) then _gfx_clean_up_ega();

#define OPEN_FILE		0x3D02
#define OPEN_RW_FILE	0x3D02
#define OPEN_READ_FILE	0x3D00
#define CREATE_FILE		0x3C00
#define PIC_SIGNATURE	0xE3A5

IMPORT  INT _gfx_file_write_err;
#define gfx_write(buf,n_bytes,fd)	 gfx_file_io((UTINY_FAR *)buf,n_bytes,fd,0x4000)
#define gfx_read(buf,n_bytes,fd)	 gfx_file_io((UTINY_FAR *)buf,n_bytes,fd,0x3F00)
#define zfill(tp, len)			_gfx_fill(tp, len, 0)

#if (PROT_MODE_SYS == PMODE_32)
#define ptr_segment(ptr)		(0)
#define ptr_offset(ptr)		((UINT)ptr)
#define is_far_ptr(ptr)		(NO)
#if defined (__386__) && (defined (__WATCOMC__) || defined (__EXPRESSC__))
#define int86(a,b,c)		int386(a,b,c)
#endif
#define _gfx_int86x(a,b,c,d)	int86(a,b,c)
#else
#define ptr_segment(ptr)		(*((unsigned *)&(ptr) + 1))
#define ptr_offset(ptr)		((unsigned int) (ptr))
#define is_far_ptr(ptr)		(sizeof(ptr) > sizeof(short))
#define _gfx_int86x(a,b,c,d)	int86x(a,b,c,d)
#endif

#define _gfx_err_number(err_nmbr, fcn_nmbr)	_set_gfx_err_number(err_nmbr | fcn_nmbr)


#define SCREEN_FCN			0x100
#define FAST_FILL_FCN		0x200
#define LINE_FCN			0x300
#define DRAW_FCN			0x400
#define PAINT_FCN			0x500
#define GET_PIC_FCN			0x600
#define PUT_PIC_FCN			0x700
#define ELLIPSE_FCN			0x800
#define CIRCLE_FCN			0x900
#define ARC_FCN			0xA00
#define LOCATE_FCN			0xB00
#define OPEN_V_FCN			0xC00
#define CLOSE_V_FCN			0xD00
#define CLEAR_V_FCN			0xE00
#define SWITCH_V_FCN		0xF00
#define MOVE_V_FCN			0x1000
#define OPEN_FPIC_FCN		0x1100
#define CLOSE_FPIC_FCN		0x1200
#define GET_FPIC_FCN		0x1300
#define PUT_FPIC_FCN		0x1400
#define READ_FPIC_FCN		0x1500
#define WRITE_FPIC_FCN		0x1600
#define CREATE_FPIC_FCN		0x1700
#define DELETE_FPIC_FCN		0x1800
#define COMPRESS_FILE_FCN	0x1900
#define FOPEN_V_FCN			0x1A00
#define SET_VIDEO_PAGES_FCN	0x1B00

#define FONT_ALIGN_FCN		0x1C00
#define FONT_COLOR_FCN		0x1D00
#define FONT_MAGNIFY_FCN		0x1E20
#define FONT_ROTATE_FCN		0x1F00
#define FONT_UNDERLINE_FCN	0x2000
#define GET_FONT_ATTR_FCN	0x2000
#define GET_FONT_HEIGHT_FCN	0x2200
#define OPEN_FONT_FCN		0x2300
#define CLOSE_FONT_FCN		0x2400
#define CURR_FONT_FCN		0x2500
#define LINE_DIRECTION_FCN	0x2600
#define LINE_JUSTIFY_FCN		0x2700
#define FONT_SPACING_FCN		0x2800

#define IDENTIFY_SVGA_FCN	0x2C00
#define SET_SVGA_MODE_FCN	0x2D00

#define MAP_VGA_PAL_FCN		0x2E00
#define READ_VGA_PAL_FCN      0x2F00


#if (PROT_MODE_SYS == PMODE_32)
#define SOLID_LINE_PAT		0xFFFFFFFF
#else
#define SOLID_LINE_PAT		0xFFFF
#endif

typedef struct {INT x, y;} COOR;
/*
#define _gfx_read_vid_line(x, y, buf, x_bytes)		_gfx_move_vid_line(x, y, (UTINY_FAR *) buf, x_bytes, x_bytes, 0, 0)
#define _gfx_write_vid_line(x, y, buf, x_bytes, show_bytes, masks)	_gfx_move_vid_line(x, y, (UTINY_FAR *) buf, x_bytes, show_bytes, masks, 1)

#define _gfx_read_next_vid_line(buf, x_bytes)		_gfx_move_next_vid_line((UTINY_FAR *) buf, x_bytes, x_bytes, 0, 0)
#define _gfx_write_vid_line(buf, x_bytes, show_bytes, masks)	_gfx_move_next_vid_line((UTINY_FAR *) buf, x_bytes, show_bytes, masks, 1)
*/

#define L_TO_R		0x4

#define r_find_pel(x, y, color)	_gfx_find_pel(x, y, color, L_TO_R)
#define l_find_pel(x, y, color)	_gfx_find_pel(x, y, color, 0x0)
#define r_find_npel(x, y, color)	_gfx_find_pel(x, y, color, 0xFF00 | L_TO_R)
#define l_find_npel(x, y, color)	_gfx_find_pel(x, y, color, 0xFF00)


/*  The following structure/union definitions are equivalent to the structures
 *  used in the int86() and int86x() function calls. They are followed by
 *  prototypes for int86() and int86x() and then two macros that are used
 *  in the code.
 */

#if (PROT_MODE_SYS == PMODE_32)
typedef struct { UTINY al, ah, xax[sizeof(int)-2];
			  UTINY bl, bh, xbx[sizeof(int)-2];
			  UTINY cl, ch, xcx[sizeof(int)-2];
			  UTINY dl, dh, xdx[sizeof(int)-2];
			  UINT si, di, cflag, flags;
			} REG_HL;
#else
typedef struct { UTINY al, ah;
			  UTINY bl, bh;
			  UTINY cl, ch;
			  UTINY dl, dh;
			  UINT si, di, cflag, flags;
			} REG_HL;
#endif


typedef struct { UINT ax, bx, cx, dx, si, di, cflag, flags;
			} REG_X;

typedef union {  REG_HL _hl;
			  REG_X  _x;} REG;

typedef struct { UWORD es, cs, ss, ds;
			} SREGS;

INT int86(INT int_nmbr, REG *in_reg, REG *out_reg);
INT int86x(INT int_nmbr, REG *in_reg, REG *out_reg, SREGS *sregs);
INT _gfx_call_86x(INT int_nmbr, REG *reg, UTINY * ds_buf, UTINY * es_buf);
INT _gfx_farcall_86x(INT int_nmbr, REG *reg, UTINY_FAR * ds_buf, UTINY_FAR * es_buf);



#if (PROT_MODE_SYS == PMODE_16)
extern void _gfx_ns16_int86x(INT, REG_X *, INT, INT);
#define call_ns_crt(reg)	     _gfx_ns16_int86x(0x10, (REG_X *) &reg, 0, 0);
#else
#define call_ns_crt(reg)	     int86(0x10, (REG *)&reg, (REG *)&reg)
#endif

#define call_crt(reg)		int86(0x10, (REG *)&reg, (REG *)&reg)
#define call_dos(reg)		int86(0x21, (REG *)&reg, (REG *)&reg)
#define bad_dos_call(reg)	(reg.cflag != 0)
#define good_dos_call(reg)	(reg.cflag == 0)

#define call_crtx(reg, dsbuf, esbuf)	_gfx_call_86x(0x10, (REG *)&reg, (UTINY *) dsbuf, (UTINY *) esbuf)
#define call_dosx(reg, dsbuf, esbuf)	_gfx_call_86x(0x21, (REG *)&reg, (UTINY *) dsbuf, (UTINY *) esbuf)

	/*  The following prototypes are for internal graphics functions. */

char *malloc(int);
char *calloc(int, int);
void exit(int);
void free(char *);
double sin(double);
double cos(double);
double cos(double);
double sqrt(double);

extern  int  _gfx_get_color(int color);
extern  int  _gfx_in_viewport(void );
extern  int  _set_gfx_err_number(int err_code_name);
extern  int  _set_gfx_error(int err_code,char *err_fcn_name);
extern  void _gfx_puts(char *tp);
extern  void _gfx_get_card_type(void );
extern  int  _gfx_get_color(int color);
extern  int  _get_int_pt(int *arg);
extern  int  _gfx_xlat_coor(int type,int val);
extern  int  _gfx_malloc_coor_stack(int n_coor,int coor_elem_sz,int flags);
extern  void _gfx_free_coor_stack(void );
extern  int  _gfx_get_box_coor_pair(int check_type_stack,int *args,struct _box_coor *bcp);
extern  void _gfx_clip_box_coor_pair(struct _box_coor *bcp);
extern  void _gfx_fillbox(int x1,int y1,int x2,int y2,int color);
extern  int  _drawline(int x1,int y1,int x2,int y2,int color,unsigned int pattern,int start_bit);
extern  void gfx_fast_hline(int x,int y,int len,int n_lines,int color);
extern  void _gfx_fill_fig_with_1_line(int x1,int y1,int x2,int solid_color);
extern  void _gfx_set_pat_frame_delta(int delta);
extern  void _gfx_cls(struct _vu *view,int color);
extern  void _gfx_draw_oval(void );
extern  int  _gfx_get_arc_points(void );
extern  void _gfx_draw_pat_arc_points(int arc_pat_mask,int start_sp,int n_dots);
extern  void _gfx_get_oval_values_from_stack(int *args,int type);
extern  void _gfx_draw_curve(void );
extern  void _gfx_record_arc_coor(int dx,int dy);
extern  void _gfx_set_quad_bounds(void );
extern  void _gfx_oval_quad_pix(int dx,int dy);
extern  void _gfx_draw_oval_arc(void );
extern  void _gfx_set_arc_parameters(void );
extern  int  _gfx_clip_quads(void );
extern  int  _gfx_oval_is_clipped(void );
extern  int get_gfx_coor(int type,int *curr_log_coor,int N,int draw_scale);
extern  int scale_DRAW_coor(int coor,int scale,int type,int *curr_log_coor,int is_relative_point,int N);
extern  int get_gfx_numeric_arg(int type);
extern  void _gfx_skip_blanks(void );
extern  int  gfx_open(char *file_name,int type_open);
extern  int  gfx_close(unsigned int dos_handle);
extern  int  gfx_delete_file(char *filename);
extern  int  gfx_file_io(UTINY_FAR *bufptr,int n_bytes,int dos_handle,int fcn_type);
extern  long gfx_lseek(int dos_handle,long l_offset,int mode);
extern  char *_gfx_malloc(unsigned int n_bytes);
extern  void _gfx_free(void *data_ptr);
extern  int  _gfx_curr_view_nmbr(void );
extern  int  _gfx_set_view_coordinates(struct _vu *view,int x,int y);
extern  int  _gfx_swap_view_pic(struct _vu *view);
extern  void _gfx_restore_view_pic(VIEW *view);
extern  void _gfx_get_cursor_location(COOR_PT *, CRSR *);
extern  void _gfx_set_screen_base(unsigned int segment);
extern  void _gfx_force_on_planes(void);
extern  void _gfx_force_off_planes(void);

#if defined (USE_PAS_M2)

#define init_GFX_struct(a)	     	InitGFXStruct(a)		
#define set_video_resolution(a, b)	     SetVideoResolution(a, b)
#define screen(a)					Screen(a)
#define map_EGA_palette(a, b)		     MapEGAPalette(a, b)
#define convert_coor_pt(cpt, a)	     ConvertCoorPt(cpt, a)
#define set_fill_pat(a, b, c)		     SetFillPat(a, b, c)
#define close_fpic(a)			     CloseFpic(a)
#define open_fpic(a,b)			     OpenFpic(a,b)
#define create_fpic(a, b)		     CreateFpic(a, b)
#define cls()						Cls()
#define switch_view(a,b)			     SwitchView(a,b)
#define get_fpic(a,b,c,d,e,f)		     GetFpic(a,b,c,d,e,f)
#define draw(a)				     Draw(a)
#define get_super_VGA_info(a,b)         GetSuperVGAInfo(a,b)
#define get_pic(a,b,c,d)				GetPic(a,b,c,d)
#define get_image(a,b,c,d,e)			GetImage(a,b,c,d,e)
#define init_pic_struct(a,b)			InitPicStruct(a,b)
#define init_pic_struct_for_blob(a,b,c) InitPicStructForBlob(a,b,c)
#define delete_blob_item(a,b)			DeleteBlobItem(a,b)
#define put_pic(a,b,c,d)				PutPic(a,b,c,d)
#define put_fpic(a,b,c,d,e)			PutFpic(a,b,c,d,e)
#define free_pic(a)					FreePic(a)
#define create_blob_file(a,b,c)		CreateBlobFile(a,b,c)
#define open_blob_file(a,b)			OpenBlobFile(a,b)
#define close_blob_file(a)			CloseBlobFile(a)
#define free_GFX_workspace()			FreeGFXWorkspace()
#define alloc_GFX_workspace(a)		AllocGFXWorkspace(a)

#elif defined (USE_UPPER_C)

#define init_GFX_struct(a)			INIT_GFX_STRUCT(a)		
#define set_video_resolution(a, b)		SET_VIDEO_RESOLUTION(a, b)	     
#define screen(a)					SCREEN(a)
#define map_EGA_palette(a, b)		     MAP_EGA_PALETTE(a, b)
#define convert_coor_pt(cpt, a)	     CONVERT_COOR_PT(cpt, a)
#define set_fill_pat(a, b, c)		     SET_FILL_PAT(a, b, c)
#define close_fpic(a)			     CLOSE_FPIC(a)
#define open_fpic(a,b)			     OPEN_FPIC(a,b)
#define create_fpic(a, b)		     CREATE_FPIC(a, b)
#define cls()						CLS()
#define switch_view(a,b)			     SWITCH_VIEW(a,b)
#define get_fpic(a,b,c,d,e,f)		     GET_FPIC(a,b,c,d,e,f)
#define draw(a)				     DRAW(a)

#endif

#if defined (__HIGHC__)
#pragma Off(Prototype_override_warnings);
#endif

#endif

/* Version 3.0 Low level prototypes */

extern void _gfx_map_VGA_palette(int fcn_n, int first_color, int n_colors, UTINY_FAR *palette);
extern void _gfx_turn_on_grey_scale(void);
extern long _gfx_load_PCX_header(int fh, PCX_HDR *pcx_hdr, int *_256_pal_exists);
extern int  _gfx_lo_get_image_size(PIC *pic);

extern long _gfx_move_vidram(PIC *pic, INT action);

extern UTINY_FAR *_gfx_farmalloc(long n_bytes);
extern void _gfx_farfree(UTINY_FAR *buf);

extern long _gfx_drive_free_space(int disk_n);
extern int  _gfx_get_dflt_drive(void);

extern void _gfx_init_pmode32(void);
extern long _gfx_get_image_size(int arg, ...);
extern UINT _gfx_ptr_in_huge_buf(UTINY_FAR *buf, LONG offset, UTINY_FAR **norm_buf, UINT n_bytes);

extern INT _gfx_get_temp_workspace(UTINY_FAR **temp_buf, INT n_kbytes);
extern void _gfx_free_temp_workspace(void);

extern void _gfx_turn_off_abs_point(void);
extern void _gfx_turn_on_abs_point(void);


/* Version 3.0 macro definitions */

#define CLIP_COOR_PAIR		0x200
#define SET_WIDTH_HGT		0x400
#define GET_PIC_DIM			0x600

#define GET_IMAGE_SIZE		0x200
#define GET_BEST_IMAGE_SIZE	0x400


typedef struct _ws { UINT dflt_buf_sz, flags, buf_sz;
				 UTINY_FAR *buf;
			    } WORKSPACE;

typedef struct _memio  { long buf_offset, buf_size;
	                    UTINY_FAR *buf;
	                  } VMEMIO;  

#define _gfx_read_next_vid_line(buf, x_bytes)		_gfx_move_next_vid_line(buf, x_bytes, x_bytes, 0, 0)
#define _gfx_write_vid_line(buf, x_bytes, show_bytes, masks)	_gfx_move_next_vid_line(buf, x_bytes, show_bytes, masks, 1)




