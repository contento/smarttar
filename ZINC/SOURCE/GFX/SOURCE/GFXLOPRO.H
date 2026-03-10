#define _Void	extern void AFdecl
#define _Int	extern int  AFdecl
#define _FarP	extern UTINY_FAR * AFdecl
#define _Uint	extern unsigned int AFdecl
#define _Long	extern long AFdecl

_Void _gfx_no_init(void);
_Void _gfx_clean_up_ega(void);
_Void _gfx_set_w0_ega_mode(void);
_Void _gfx_set_w1_ega_mode(void);
_Void _gfx_set_w2_ega_mode(void);
_Void _gfx_set_rd_cmp_mode(void);
_Void _gfx_set_ega_palette(char *);
_Void _gfx_set_hercules_mode(void);
_Void _gfx_set_herc_gfx_mode(void);
_Void _gfx_set_herc_text_mode(void);
_Int  _gfx_put_pel(int, int, int);
_Int  _gfx_set_vidram(int, int);
_Void _gfx_init_V7VGA(void);
_Void _gfx_init_ATI_18800(void);
_Void _gfx_init_TSENG_ET4000(void);
_Void _gfx_init_TSENG_ET3000(void);
_Void _gfx_init_TRIDENT_8800CS(void);
_Void _gfx_init_VESA(long, int, int);
_Void _gfx_init_WD(void);
_Void _gfx_init_OAK(void);
_Int  _gfx_init_no_open(void);

_Int  _gfx_find_pel(int, int, int, unsigned int);
_Void _gfx_asm_line(int, int, int, int, int, int, int, int, int, int, unsigned int);
_Int  _gfx_rev_pat_order(unsigned int);
_Int  _gfx_ror_pattern(int, int);
_Int  _gfx_arc_rot_pat_masks(int *);
_Void _gfx_copy_video_page(int, int, int, int);
_Int  _gfx_set_coor_pair(int *);
_Void _gfx_record_line_points(int, int, int, int, int, int, int, int, void*, int, int);
_Int  _gfx_idiv(int, int);
_Int  _gfx_get_pel(int, int);
_Int  _gfx_findb(char *, int, char);
_Int  _gfx_call_dos(int, int);
_Void _gfx_move(void *, void *, int);
_Void _gfx_far_move(UTINY_FAR *, UTINY_FAR *, int);
_Void _gfx_rw_screen_text(int, int, char *, int, int);
_Void _gfx_shift_block(UTINY_FAR *, UTINY_FAR *, int, int);
_Void _gfx_move_vid_line(int, int, UTINY_FAR *, int, int, int, int);
_Void _gfx_move_next_vid_line(UTINY_FAR *, int, int, int, int);
_Void _gfx_init_vid_line(int, int);
_Void _gfx_reverse_block(char *, int);
_Void _gfx_set_ega_modify_type(int);
_Int  _gfx_lpeek(int, int);
_Int  _gfx_check_hercules_card(void);
_Int  _gfx_check_crsr_reg(int);
_Void _gfx_dbl_pattern(unsigned char *, unsigned char *, int, int);
_Void _gfx_rol(int, int);
_Void _gfx_asm_pat_hline(int, int, int, int, unsigned char *, int, int, int, int);
_Void _gfx_asm_vline(int, int, int, int);
_Void _gfx_asm_hline(int, int, int,int, int);
_Int  _gfx_xlat_int_coor(int, int, int);
_Void _gfx_off_video_refresh(void);
_Void _gfx_on_video_refresh(void);
_Void _gfx_start_vert_retrace(void);
_Void _gfx_set_ega_video_addr(int);
_Void _gfx_fill(void * tp, int len, int c);
_Void _gfx_qsort(void *xye, int len, int sz);
_Void _gfx_4_pel(int, int, int, int, int);
_Int  _gfx_quick_4_pel(int, int, int, int, int);
_Int  _gfx_check_pix(int, int, int, CLIP_REGION *, int, int, CLIP_REGION *);
_Void _gfx_clip_gfx(int);
_Int  _gfx_inp(int);
_Void _gfx_outp(int, int);
_Void _gfx_lo_write_VGA_pal(int, int, UTINY_FAR *);
_Void _gfx_lo_read_VGA_pal(int, int, UTINY_FAR *);
_Int  _gfx_get_ds(void);
_Int  _gfx_get_es(void);
_Int  _gfx_get_cs(void);
_Void _gfx_p32_move(char *, int, char *, int, int);
_Void _gfx_init_move_vid_line(int , int);
_Void _gfx_xpand_bits(UTINY_FAR *, UTINY_FAR *, int, int);
_Void _gfx_promote_colors(UTINY_FAR *, UTINY_FAR *, int, int, int);
_Void _gfx_build_16_mask(UTINY_FAR *, UTINY_FAR *, int, int);


/** FONTS & MENUS **/

_Void  _gfx_write_char(int, int, int, int, int, int, int, UTINY_FAR *);
_FarP  _gfx_get_rom_font_ptr(int);
_Void  _gfx_386_far_move(int, int, int, int, int);
_Void  _gfx_rotate_90(UTINY_FAR *, int, int, int, char *, int, int);
_Void  _gfx_rotate_270(UTINY_FAR *, int, int, int, char *, int, int);
_Void  _gfx_rotate_180(UTINY_FAR *, int, int, int, char *, int, int);
_Void  _gfx_xpand_char(UTINY_FAR *, int, int, int, char *, int, int);
_Int   _gfx_findw(UWORD *, int, int);
_Void  _gfx_bump_event_handler(void);
_Void  _gfx_xy_vram256_to_buf(int, int, int, int, UTINY_FAR *);
_Void  _gfx_ptr_vram256_to_buf(int, int, int, int, UTINY_FAR *);
_Void  _gfx_buf_to_xy_vram256(UTINY_FAR *, int, int, int, int);
_Void  _gfx_buf_to_ptr_vram256(UTINY_FAR *, int, int, int, int);
_Uint  _gfx_rle_expand_row(UTINY_FAR *, UTINY_FAR *, int, int, int);
_Uint  _gfx_rle_compress_row(UTINY_FAR *, UTINY_FAR *, int, int, int);
_Uint  _gfx_rle_count_rows(UTINY_FAR *, unsigned int, int, int *, int);
_Void  _gfx_2_to_256(UTINY_FAR *, int, UTINY_FAR *, int, int);
_Void  _gfx_16_to_256(UTINY_FAR *, int, UTINY_FAR *, int, int);
_Uint  _gfx_pcx_expand_row(UTINY_FAR *, UTINY_FAR *, int, int, int);
_Uint  _gfx_pcx_compress_row(UTINY_FAR *, UTINY_FAR *, int, int, int);
_Uint  _gfx_pcx_count_rows(UTINY_FAR *, unsigned int, int, int *, int);
_Void  _gfx_set_gfx_event_handler(void);
_FarP  _gfx_get_dta(void);
_Void  _gfx_set_dta(UTINY_FAR *); 
_Int   _gfx_getkey(void);
_Int   _gfx_get_shift_flags(void);
_Long  _gfx_curr_bios_time(void);
_Long  _gfx_move_4plane_block(int, int, unsigned int, int, unsigned int, int);
_Int   _gfx_get_curr_256_page_n(void);
_Void  _gfx_set_curr_256_page_n(int);

