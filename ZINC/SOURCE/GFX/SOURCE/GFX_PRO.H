	   /**************************************************************/
	   /*  Copyright C Source, Inc. 1987 - 1993  All Rights Reserved */
	   /*      C Source Incorporated. Lee's Summit, Missouri USA     */
	   /**************************************************************/

#if !defined (__GFX_PRO_)
#define __GFX_PRO_

#if defined (__cplusplus)
extern "C" {
#endif

#if defined (USE_PAS_M2)

			/*******************************************
			 *	  PROTOTYPES FOR PAS/M2 INTERFACE	   *
			 *******************************************/

			/*******************************************
			 *	   GFX GRAPHICS LIBRARY FUNCTIONS	   *
			 *******************************************/


#if defined (USE_GFX_GRAPHICS)

	/* Setting the video 'playing field' */

extern  void Color(int fgnd, int bkgnd, int palette);
extern  int  CopyVideoPages(int from_page, int to_page);
extern  void InitGFXStruct(int bios_nmbr);
extern  int  Screen(int mode);
extern  void SetVideoResolution(int res_x, int res_y);
extern  int  SetExtendedMode(int x_res, int y_res, int is_color, int mode_n);
extern  int  SetVideoPages(int vpage, int wpage);
extern  unsigned int  GetCardMonitor(void);
extern  int MapEGAPalette(int index, int color);

	/* Functions getting various screen coordinates. */

extern  void ConvertCoorPt(COOR_PT *cpt, int type);
extern  void GetCAP(COOR_PT *cpt);
extern  void ViewOrigin(COOR_PT *cpt);
extern  void ViewLrCorner(COOR_PT *cpt);
extern  void ViewUlCorner(COOR_PT *cpt);

	/* Pattern setting functions */

extern  void SetLinePat(unsigned int pat);
extern  void SetFillPat(unsigned char *pat, int color, int bkgnd_color);
extern  void SetFillSolid(int color);

	/* General purpose */

extern  void Cls( void );
extern  void MoveTo(int, int);
extern  void MoveToRel(int, int);
extern  void MoveToF(double, double);
extern  void MoveToRelF(double, double);
extern  int  AFdecl Point(int, int);
extern  int  PointRel(int, int);
extern  int  PointF(double, double);
extern  int  PointRelF(double, double);
extern  void SetVideoAspectRatio(int, int);

	/* Drawing functions */

extern  void AFdecl Box(int, int, int, int, int);
extern  void BoxTo(int, int, int);
extern  void BoxToRel(int, int, int);
extern  void BoxF(double, double, double, double, int);
extern  void BoxToF(double, double, int);
extern  void BoxToRelF(double, double, int);

extern  void Circle(int, int, int, int);
extern  void CircleRel(int, int, int, int);
extern  void CircleF(double, double, double, int);
extern  void CircleRelF(double, double, double, int);

extern  void CircleArc(int, int, int, int, int, int);
extern  void CircleArcRel(int, int, int, int, int, int);
extern  void CircleArcF(double, double, double, int, int, int);
extern  void CircleArcRelF(double, double, double, int, int, int);

extern  int  Draw(char *fmt, ... );

extern  void AFdecl DrawLine(int, int, int, int, int);
extern  void DrawLineTo(int, int, int);
extern  void DrawLineToRel(int, int, int);
extern  void DrawLineF(double, double, double, double, int);
extern  void DrawLineToF(double, double, int);
extern  void DrawLineToRelF(double, double, int);

extern  void AFdecl Oval(int, int, int, int, int);
extern  void OvalRel(int, int, int, int, int);
extern  void OvalF(double, double, double, double, int);
extern  void OvalRelF(double, double, double, double, int);

extern  void AFdecl OvalArc(int, int, int, int, int, int, int);
extern  void OvalArcRel(int, int, int, int, int, int, int);
extern  void OvalArcF(double, double, double, double, int, int, int);
extern  void OvalArcRelF(double, double, double, double, int, int, int);

extern  void PieSlice(int, int, int, int, int, int);
extern  void PieSliceRel(int, int, int, int, int, int);
extern  void PieSliceF(double, double, double, int, int, int);
extern  void PieSliceRelF(double, double, double, int, int, int);

extern  void AFdecl OvalPieSlice(int, int, int, int, int, int, int);
extern  void OvalPieSliceRel(int, int, int, int, int, int, int);
extern  void OvalPieSliceF(double, double, double, double, int, int, int);
extern  void OvalPieSliceRelF(double, double, double, double, int, int, int);

extern  int  AFdecl PolyLine(int, int, int *, int, int);
extern  int  PolyLineRel(int, int, int *, int, int);
extern  int  PolyLineF(double, double, double *, int, int);
extern  int  PolyLineRelF(double, double, double *, int, int);

extern  int  PolyPoint(int, int, int *, int, int);
extern  int  PolyPointRel(int, int, int *, int, int);
extern  int  PolyPointF(double, double, double *, int, int);
extern  int  PolyPointRelF(double, double, double *, int, int);

extern  void AFdecl Pset(int, int, int);
extern  void PsetRel(int, int, int);
extern  void PsetF(double, double, int);
extern  void PsetRelF(double, double, int);

	/* Seed fill functions */

extern  int  Fastfill(int, int, int, int);
extern  int  FastfillRel(int, int, int, int);
extern  int  FastfillF(double, double, int, int);
extern  int  FastfillRelF(double, double, int, int);
extern  int  Paint(int, int, int, int);
extern  int  PaintRel(int, int, int, int);
extern  int  PaintF(double, double, int, int);
extern  int  PaintRelF(double, double, int, int);

	/* Pic & FPic functions */

extern  int  CloseFpic(int pic_handle);
extern  int  CompressFpicFile(char * file_name, char *temp_file_name);
extern  int  CreateFpic(char *fname, int max_fpics);
extern  int  DeleteFpic(int fpic_index, int fpic_handle);
extern  void FreePic(PIC *);
extern  int  OpenFpic(char *fname, int update_file);
extern  PIC *ReadFpic(int fpic_index, int fpic_handle);
extern  int  WriteFpic(PIC *pic, int fpic_index, int fpic_handle);

extern  int  GetFpic(int, int, int, int, int, int);
extern  int  GetFpicTo(int, int, int, int);
extern  int  GetFpicToRel(int, int, int, int);
extern  int  GetFpicF(double, double, double, double, int, int);
extern  int  GetFpicToF(double, double, int, int);
extern  int  GetFpicToRelF(double, double, int, int);

extern  PIC  AFdecl *GetPic(int, int, int, int);
extern  PIC  *GetPicTo(int, int);
extern  PIC  *GetPicToRel(int, int);
extern  PIC  *GetPicF(double, double, double, double);
extern  PIC  *GetPicToF(double, double);
extern  PIC  *GetPicToRelF(double, double);

extern  int  PutFpic(int, int, int, int, int);
extern  int  PutFpicRel(int, int, int, int, int);
extern  int  PutFpicF(double, double, int, int, int);
extern  int  PutFpicRelF(double, double, int, int, int);

extern  int  AFdecl PutPic(int, int, PIC *, int);
extern  int  PutPicRel(int, int, PIC *, int);
extern  int  PutPicF(double, double, PIC *, int);
extern  int  PutPicRelF(double, double, PIC *, int);

	/* Viewport control functions */

extern  int  ClearView(int view_n, int color);
extern  int  CloseView(int view_n, int hide_view);
extern  int  CloseViewFile(void );
extern  int  CreateViewFile(char *fpic_name);
extern  int  DeleteViewFile(void );
extern  void LogicalIntView(int x_ul, int y_ul, int x_lr, int y_lr);
extern  void LogicalFloatView(double x_ul, double y_ul, double x_lr, double y_lr);
extern  int  SwitchView(int new_view_n, int hide_view);

extern  int  MoveView(int, int);
extern  int  MoveViewRel(int, int);
extern  int  MoveViewF(double, double);
extern  int  MoveViewRelF(double, double);

extern  int  OpenView(int, int, int, int, int, int);
extern  int  OpenViewTo(int, int, int, int);
extern  int  OpenViewToRel(int, int, int, int);
extern  int  OpenViewF(double, double, double, double, int, int);
extern  int  OpenViewToF(double, double, int, int);
extern  int  OpenViewToRelF(double, double, int, int);


	/* Now obsolete functions */
extern  int  Line(int, int, int, int, int, int);
extern  int  Locate(int, int );
extern  void ScaleIntView(int x_scale, int y_scale, int x_org, int y_org, int x_dir, int y_dir);
extern  void ScaleFloatView(double x_scale, double y_scale, double x_org, double y_org, int x_dir, int y_dir);

	/* NEW GFX 3.0 FUNCTIONS */

extern  int  IdentifySuperVGA(void);
extern  int  SetSuperVGAMode(int);
extern  int  GetSuperVGAInfo(int **svga_modes, int vram);

extern  int  ReadPCXHeader(char *, PCX_HDR *);        
extern  int  OpenPCXFile(char *, PIC *);

extern  int  GetPCXFile(int, int, int, int, char *, int);
extern  int  GetPCXFileTo(int, int, char *, int);
extern  int  GetPCXFileToRel(int, int, char *, int);
extern  int  GetPCXFileF(double, double, double, double, char *, int);
extern  int  GetPCXFileToF(double, double, char *, int);
extern  int  GetPCXFileToRelF(double, double, char *, int);

extern  int  PutPCXFile(int, int, char *, int);
extern  int  PutPCXFileRel(int, int, char *, int);
extern  int  PutPCXFileF(double, double, char *, int);
extern  int  PutPCXFileRelF(double, double, char *, int);

extern  int  MapVGAPalette(int, int, unsigned char *);
extern  int  ReadVGAPalette(int, int, unsigned char *);
extern  void MapVGAGreyScale(int brightness);

extern  unsigned int AllocGFXVideoRam(unsigned int);
extern  unsigned int AllocGFXWorkspace(unsigned int);
extern  unsigned int AllocGFXSystemRam(unsigned int);
extern  unsigned int AllocGFXDiskMem(int, unsigned int);

extern  void FreeGFXWorkspace(void);
extern  void FreeGFXSystemRam(void);
extern  void FreeGFXDiskMem(void);


extern  int  OpenBlobFont(int, int);
extern  int  GetBlobNameIndex(char *, int);
extern  int  InitBlobNameList(char *, int, int, int *);
extern  int  GetBlobNameList(char *, int, int, char **, int);
//extern  void SortBlobNameList(char *[], int, int);
extern  int  GetBlobItemName(char *, int, int);
extern  int  SetBlobItemName(char *, int, int);
extern  int  SliceFileFromBlob(char *, int, int, int);
extern  int  AddFileToBlob(char *, int, int, int, int);
extern  UTINY_FAR *LoadBlobItem(UTINY_FAR *, long *, int, int);

extern  int  DeleteBlobItem(int, int);
extern  int  GetBlobItemInfo(BLOB_ITEM *, int, int);
extern  int  CloseBlobFile(int);
extern  int  OpenBlobFile(char *, int);
extern  int  CreateBlobFile(char *, int, int);

extern  PIC *GetDefaultPicStructPtr(void);
extern  int  InitPicStructForBlob(PIC *, int, int);
extern  void InitPicStruct(PIC *, int);

extern  int  PutImage(int, int, PIC *, int);
extern  int  PutImageRel(int, int, PIC *, int);
extern  int  PutImageF(double, double, PIC *, int);
extern  int  PutImageRelF(double, double, PIC *, int);

extern  int  GetImage(int, int, int, int, PIC *);
extern  int  GetImageTo(int, int, PIC *);
extern  int  GetImageToRel(int, int, PIC *);
extern  int  GetImageF(double, double, double, double, PIC *);
extern  int  GetImageToF(double, double, PIC *);
extern  int  GetImageToRelF(double, double, PIC *);

extern  long GetImageSize(int, int, int, int, PIC *);
extern  long GetImageSizeTo(int, int, PIC *);
extern  long GetImageSizeToRel(int, int, PIC *);
extern  long GetImageSizeF(double, double, double, double, PIC *);
extern  long GetImageSizeToF(double, double, PIC *);
extern  long GetImageSizeToRelF(double, double, PIC *);

#endif

			/*******************************************
			 *	GFX FONTS & MENUS LIBRARY FUNCTIONS   *
			 *******************************************/

#if defined (USE_GFX_FONTS_MENUS)

	/* font functions */

int CloseFont(int fh);
int CurrFont(int fh);
int FontAlign(int fh, int alignment);
int FontColor(int fh, int fgnd, int bkgnd);
int FontMagnify(int fh, int width, int height);
int FontRotate(int fh, int rotation_angle);
int FontSpacing(int fh, int delta_space);
int FontUnderline(int fh, int location, int thickness, int color);
void FprintFont(char *, ... );
struct _fontattr *GetFontAttr(int fh);
struct _lineattr *GetLineAttr(void);
int	GetFontHeight(int fh);
int	GetLineLen(int fh, char * text, int max_n_char);
int	LineDirection(int text_dir);
int	LineJustify(int justify);
int	LineUnderline(int on_or_off);
int	OpenFont(char * text);
int	OpenMemFont(FONT *);
void AFdecl PrintFont(int x, int y, char * text, int max_len);
void PrintFontRel(int dx, int dy, char * text, int max_len);
void PrintFontF(double x, double y, char * text, int max_len);
void PrintFontRelF(double dx, double dy, char * text, int max_len);
void GetFontCursor(COOR_PT *cpt);

	/* menu functions */

void SetEventTextMacro(char *tp);
void SetDoubleClick(int n_ticks);
void SetAutoRepeat(int p1, int p2);
void SetMouseEvent(int mouse_event_mask);
void SetExitHook(EVENT * (*exit_hook_fcn)(), unsigned int mask_for_exit_hook);
void SetLoopHook(void (*event_vec)());
void SetEventTimer(int n_ticks, int auto_repeat);
EVENT *GetEvent(int event_mask);
void PositionRootMenu(int root_top);
void SetRootMenu(ROOT_MENU *new_root);
void ClearMenus(void);
EVENT *SetBoxWrtCenter(int dx, int dy, GFX_BOX *gbox);
EVENT *SetBox(int x, int y, GFX_BOX *gbox);
EVENT *SetBoxToRel(int dx, int dy, GFX_BOX *gbox);
EVENT *SetBoxF(double x, double y, GFX_BOX *gbox);
EVENT *SetBoxToRelF(double dx, double dy, GFX_BOX *gbox);
EVENT *_set_box(int x, int y, GFX_BOX *gbox);
void InitGFXMenu(int n_box_layers, int dflt_fh, int orig_x_res, int orig_y_res);
void ClearBox();
EVENT *RevisitBox();

EVENT *PeekEvent(void);
int	PushEvent(EVENT *event);
void FlushEvent_q(void);
void HaltQ(void);
void SetCursorRegions(int n_regions, CURSOR_REGION *crsr_regions);
int	PushHotSpot(HOT_SPOT *hs);
int	PopHotSpots(int n_hot_spots);
void SleepAllHotSpots(void);
void WakeHotSpots(void);
void ModHotSpotsStatus(int first_hs_n, int n_hot_spots, int type, int status);
int	Qprint(int x, int y, int fh, int color, int justify, char *tp, int max_n_char);
int	QstringLen(int  fh, char  *tp);
void ShadeBox(int x, int y, int x_width, int y_height, int pattern, int color);
void ResetLayout(int n_layouts, LAYOUT *layouts[]);
int	ScaleDim(int val, int x_or_y);
void ResetColorScheme(int n_cs, COLOR_SCHEME *csa[], int n_items, COLOR_MAP *color_map);

int	InitAutoBox(int type, GFX_BOX *dflt_gbox);
GFX_BOX *BuildAutoBox(int type, char *box_title, int button_n, int n_items, char **items);
EVENT   *SetBoxWrtMenu(int justify, int delta_x, int delta_y, GFX_BOX *gbox);
int	GetDirectory(char *asciiz, int flags, unsigned char ***items);
int	InitMouse(void);

void InitAutoGFXMenu(int used_video_pages, int n_sys_para, int n_box_layers, int dflt_fh, int x_res, int y_res);
void ResetAutoCsLayout(int save_orig_settings);

void  _gfx_color_box(int x1, int y1, int x2, int y2, int color);
void  _gfx_solid_box(int x1, int y1, int x2, int y2, int color);

/* Low level mouse functions */

int	MouseReset(void);
void MouseShowCursor(void);
void MouseHideCursor(void);
void MouseGetCursor(int crsr[3]);
void MouseSetCursor(int x, int y);
int	MouseGetButtonPressInfo(int button_n, int crsr[3]);
int	MouseGetButtonReleaseInfo(int button_n, int crsr[3]);
void MouseSetBounds(int min_x, int min_y, int max_x, int max_y);
void MouseDefineCursorShape(CURSOR_SHAPE *crsr_shape);
void MouseReadMotionCounters(int *h_count, int *v_count);
void MouseSetMickeyToPixelRatio(int x_ratio, int y_ratio);
void MouseAreaToHideCursor(int min_x, int min_y, int max_x, int max_y);
void MouseSetSpeedThreshold(int mickeys_per_sec);

/* The lower case names are also available */

int	AFdecl mouse_reset(void);
void AFdecl mouse_show_cursor(void);
void AFdecl mouse_show_crsr(void);
void AFdecl mouse_hide_cursor(void);
void AFdecl mouse_hide_crsr(void);
void AFdecl mouse_get_cursor(int crsr[3]);
void AFdecl mouse_set_cursor(int x, int y);
int	AFdecl mouse_get_button_press_info(int button_n, int crsr[3]);
int	AFdecl mouse_get_button_release_info(int button_n, int crsr[3]);
void AFdecl mouse_set_bounds(int min_x, int min_y, int max_x, int max_y);
void AFdecl mouse_define_cursor_shape(CURSOR_SHAPE *crsr_shape);
void AFdecl mouse_read_motion_counters(int *h_count, int *v_count);
void AFdecl mouse_set_mickey_to_pixel_ratio(int x_ratio, int y_ratio);
void AFdecl mouse_area_to_hide_cursor(int min_x, int min_y, int max_x, int max_y);
void AFdecl mouse_set_speed_threshold(int mickeys_per_sec);

#endif

			/*******************************************
			 *	 GFX SCREEN DUMP LIBRARY FUNCTIONS    *
			 *******************************************/

#if defined (USE_GFX_SCREEN_DUMP)

int	SetMemPrinterRec(int dev_id, char *port_name);
int	SetDiskPrinterRec(int dev_id, char *port_name);
void SetPrxOptions(int pic, int clear, int white, int eject, int reset);
int	SetPrxMonitorAspect(int x_asp, int y_asp);
int	GetPrinterStatus(int n);
void SetPrxSwathHook(int (*hook_fcn)(int, int, int, int));
void SetPrxSolidOutput(unsigned char solid_pat[16]);
void SetPrxShading(unsigned char *usr_pat_data);
void SetPrxColorTable(unsigned char *usr_color_data);
int	SetPrxOutputScale(int type);
void SetPrxTableName(char *table_name);
int	PrintScreen(int width, int height, int lmargin, int tmargin);
int	PrintArea(int x1, int y1, int x2, int y2, int width, int height, int lm, int tm);
int	PrintAreaTo(int x2, int y2, int width, int height, int lm, int tm);
int	PrintAreaToRel(int dx, int dy, int width, int height, int lm, int tm);
void LinkAllPrinterTypes(void);
void LinkDotMatrixPrinters(void);
void LinkLaserPrinters(void);

#endif


#elif defined (USE_UPPER_C)

			/*******************************************
			 *	PROTOTYPES FOR UPPER CASE INTERFACE   *
			 *******************************************/

			/*******************************************
			 *	   GFX GRAPHICS LIBRARY FUNCTIONS	   *
			 *******************************************/


#if defined (USE_GFX_GRAPHICS)

	/* Setting the video 'playing field' */

extern  void COLOR(int fgnd, int bkgnd, int palette, ...);
extern  int  COPY_VIDEO_PAGES(int from_page, int to_page);
extern  void INIT_GFX_STRUCT(int bios_nmbr);
extern  int  SCREEN(int mode);
extern  void SET_VIDEO_RESOLUTION(int res_x, int res_y);
extern  int  SET_EXTENDED_MODE(int x_res, int y_res, int is_color, int mode_n);
extern  int  SET_VIDEO_PAGES(int vpage, int wpage);
extern  unsigned int  GET_CARD_MONITOR(void);
extern  int MAP_EGA_PALETTE(int index, int color);

	/* Functions getting various screen coordinates. */

extern  void CONVERT_COOR_PT(COOR_PT *cpt, int type);
extern  void GET_CURR_PT(COOR_PT *cpt);
extern  void VIEW_ORIGIN(COOR_PT *cpt);
extern  void VIEW_LR_CORNER(COOR_PT *cpt);
extern  void VIEW_UL_CORNER(COOR_PT *cpt);

	/* Pattern setting functions */

extern  void SET_LINE_PAT(unsigned int pat);
extern  void SET_FILL_PAT(unsigned char *pat, int color, int bkgnd_color);
extern  void SET_FILL_SOLID(int color);

	/* General purpose */

extern  void CLS( void );
extern  void MOVE_TO(int, ... );
extern  int  POINT(int, ... );
extern  void SET_VIDEO_ASPECT_RATIO(int, int);

	/* Drawing functions */

extern  void BOX(int, ... );
extern  void CIRCLE(int, ... );
extern  void CIRCLE_ARC(int, ... );
extern  int  DRAW(char *fmt, ... );
extern  int  DRAW_LINE(int, ... );
extern  void OVAL(int, ... );
extern  void OVAL_ARC(int, ... );
extern  int  PIE_SLICE(int, ... );
extern  int  OVAL_PIE_SLICE(int, ... );
extern  void POLY_POINT(int, ... );
extern  int  POLY_LINE(int, ... );
extern  int  PSET(int, ... );

	/* Seed fill functions */

extern  int  FASTFILL(int, ... );
extern  int  PAINT(int, ... );

	/* Pic & FPic functions */

extern  int  CLOSE_FPIC(int pic_handle);
extern  int  COMPRESS_FPIC_FILE(char *file_name, char *temp_file_name);
extern  int  CREATE_FPIC(char *fname, int max_fpics);
extern  int  DELETE_FPIC(int fpic_index, int fpic_handle);
extern  int  GET_FPIC(int, ... );
extern  void FREE_PIC(PIC *);
extern  PIC *GET_PIC(int, ... );
extern  int  OPEN_FPIC(char *fname, int update_file);
extern  int  PUT_FPIC(int, ... );
extern  int  PUT_PIC(int, ... );
extern  PIC *READ_FPIC(int fpic_index, int fpic_handle);
extern  int  WRITE_FPIC(PIC *pic, int fpic_index, int fpic_handle);

	/* Viewport control functions */

extern  int  CLEAR_VIEW(int view_n, int color);
extern  int  CLOSE_VIEW(int view_n, int hide_view);
extern  int  CLOSE_VIEW_FILE(void );
extern  int  CREATE_VIEW_FILE(char *fpic_name);
extern  void LOGICAL_INT_VIEW(int x_ul, int y_ul, int x_lr, int y_lr);
extern  void LOGICAL_FLOAT_VIEW(double x_ul, double y_ul, double x_lr, double y_lr);
extern  int  MOVE_VIEW(int, ... );
extern  int  OPEN_VIEW(int, ... );
extern  int  SWITCH_VIEW(int new_view_n, int hide_view);

	/* Now obsolete functions */
extern  int  LINE(int, ... );		/* replaced by DRAW_LINE & BOX */
extern  int  LOCATE(int, int );
extern  void SCALE_INT_VIEW(int x_scale, int y_scale, int x_org, int y_org, int x_dir, int y_dir);
extern  void SCALE_FLOAT_VIEW(double x_scale, double y_scale, double x_org, double y_org, int x_dir, int y_dir);


/* Version 3.0 Functions */

extern int IDENTIFY_SUPER_VGA(void);
extern int SET_SUPER_VGA_MODE(int);

extern int  PUT_PCX_FILE(int arg, ...);
extern int  GET_PCX_FILE(int arg, ... );
extern int  READ_PCX_HEADER(char *file_name, PCX_HDR *pcx_hdr);

extern  int  MAP_VGA_PALETTE(int, int, unsigned char *);
extern  int  READ_VGA_PALETTE(int, int, unsigned char *);
extern  void MAP_VGA_GREY_SCALE(int brightness);

extern  unsigned int ALLOC_GFX_VIDEO_RAM(unsigned int);
extern  unsigned int ALLOC_GFX_WORKSPACE(unsigned int);
extern  unsigned int ALLOC_GFX_SYSTEM_RAM(unsigned int)
extern  unsigned int ALLOC_GFX_DISK_MEM(int, unsigned int);

extern  void FREE_GFX_WORKSPACE(void);
extern  void FREE_GFX_SYSTEM_RAM(void);
extern  void FREE_GFX_DISK_MEM(void);

extern  int  GET_BLOB_NAME_INDEX(char *, int);
extern  int  INIT_BLOB_NAME_LIST(char *, int, int, int *);
extern  int  GET_BLOB_NAME_LIST(char *, int, int, char **, int);
extern  int  OPEN_BLOB_FONT(int, int);
extern  void SORT_BLOB_NAME_LIST(char *blob_names[], int, int);
extern  int  GET_BLOB_ITEM_NAME(char *, int, int);
extern  int  SET_BLOB_ITEM_NAME(char *, int, int);
extern  int  SLICE_FILE_FROM_BLOB(char *, int, int, int);
extern  int  ADD_FILE_TO_BLOB(char *, int, int, int, int);
extern  UTINY_FAR *LOAD_BLOB_ITEM(UTINY_FAR *, long *, int, int);
extern  long READ_PCX_HEADER(int, PCX_HDR *, int *);
extern  int  PUT_IMAGE(int arg, ...);
extern  int  GET_IMAGE(int arg, ...);
extern  int  GET_IMAGE_SIZE(int arg, ...);
extern  int  OPEN_PCX_FILE(char *, PIC *);
extern  PIC *GET_DEFAULT_PIC_STRUCT_PTR(void);
extern  int  INIT_PIC_STRUCT_FOR_BLOB(PIC *, int, int);
extern  void INIT_PIC_STRUCT(PIC *,  int);
extern  int  DELETE_BLOB_ITEM(int, int);
extern  int  GET_BLOB_ITEM_INFO(BLOB_ITEM *, int, int);
extern  int  CLOSE_BLOB_FILE(int);
extern  int  OPEN_BLOB_FILE(char *, int);
extern  int  CREATE_BLOB_FILE(char *, int, int);


#endif


			/*******************************************
			 *	GFX FONTS & MENUS LIBRARY FUNCTIONS   *
			 *******************************************/

#if defined (USE_GFX_FONTS_MENUS)

int CLOSE_FONT(int fh);
int CURR_FONT(int fh);
int FONT_ALIGN(int fh, int alignment);
int FONT_COLOR(int fh, int fgnd, int bkgnd);
int FONT_MAGNIFY(int fh, int width, int height);
int FONT_ROTATE(int fh, int rotation_angle);
int FONT_SPACING(int fh, int delta_space);
int FONT_UNDERLINE(int fh, int location, int thickness, int color);
void FPRINT_FONT(char *, ... );
struct _fontattr *GET_FONT_ATTR(int fh);
struct _lineattr *GET_LINE_ATTR(void);
int GET_FONT_HEIGHT(int fh);
int GET_LINE_LEN(int fh, char * text, int max_n_char);
int LINE_DIRECTION(int text_dir);
int LINE_JUSTIFY(int justify);
int LINE_UNDERLINE(int on_or_off);
int OPEN_FONT(char * text);
int open_mem_font(FONT *);
void PRINT_FONT(int, ... );
void GET_FONT_CURSOR(COOR_PT *cpt);

#endif
#endif


			/*******************************************
			 *	PROTOTYPES FOR LOWER CASE INTERFACE   *
			 *******************************************/

			/*******************************************
			 *	   GFX GRAPHICS LIBRARY FUNCTIONS	   *
			 *******************************************/

#if defined (USE_LOWER_C)

#if defined (USE_GFX_GRAPHICS)

	/* Setting the video 'playing field' */

extern  void color(int fgnd, int bkgnd, int palette, ...);
extern  int  copy_video_pages(int from_page, int to_page);
extern  void init_GFX_struct(int bios_nmbr);
extern  int  screen(int mode);
extern  void set_video_resolution(int res_x, int res_y);
extern  int  set_extended_mode(int x_res, int y_res, int is_color, int mode_n);
extern  int  set_video_pages(int vpage, int wpage);
extern  unsigned int  get_card_monitor(void);
extern  int map_EGA_palette(int index, int color);

	/* Functions getting various screen coordinates. */

extern  void convert_coor_pt(COOR_PT *cpt, int type);
extern  void get_CAP(COOR_PT *cpt);
extern  void view_origin(COOR_PT *cpt);
extern  void view_lr_corner(COOR_PT *cpt);
extern  void view_ul_corner(COOR_PT *cpt);

	/* Pattern setting functions */

extern  void set_line_pat(unsigned int pat);
extern  void set_fill_pat(unsigned char *pat, int color, int bkgnd_color);
extern  void set_fill_solid(int color);

	/* General purpose */

extern  void cls( void );
extern  void move_to(int, ... );
extern  int  point(int, ... );
extern  void move_to_f(double, ... );
extern  int  point_f(double, ... );
extern  void set_video_aspect_ratio(int, int);

	/* Drawing functions for integer viewports */

extern  void box(int, ... );
extern  void circle(int, ... );
extern  void circle_arc(int, ... );
extern  int  draw(char *fmt, ... );
extern  int  draw_line(int, ... );
extern  void oval(int, ... );
extern  void oval_arc(int, ... );
extern  int  pie_slice(int, ... );
extern  int  oval_pie_slice(int, ... );
extern  void poly_point(int, ... );
extern  int  poly_line(int, ... );
extern  int  pset(int, ... );

	/* Drawing functions for floating-point viewports */

extern  void box_f(double, ... );
extern  void circle_f(double, ... );
extern  void circle_arc_f(double, ... );
extern  int  draw_line_f(double, ... );
extern  void oval_f(double, ... );
extern  void oval_arc_f(double, ... );
extern  int  pie_slice_f(double, ... );
extern  int  oval_pie_slice_f(double, ... );
extern  void poly_point_f(double, ... );
extern  int  poly_line_f(double, ... );
extern  int  pset_f(double, ... );

	/* Seed fill functions */
				/* integer viewports */
extern  int  fastfill(int, ... );
extern  int  paint(int, ... );

				/* floating-point viewports */
extern  int  fastfill_f(double, ... );
extern  int  paint_f(double, ... );


	/* Pic & FPic functions */

extern  int  close_fpic(int pic_handle);
extern  int  compress_fpic_file(char *file_name, char *temp_file_name);
extern  int  create_fpic(char *fname, int max_fpics);
extern  int  delete_fpic(int fpic_index, int fpic_handle);
extern  void free_pic(PIC *);
extern  int  open_fpic(char *fname, int update_file);
extern  PIC *read_fpic(int fpic_index, int fpic_handle);
extern  int  write_fpic(PIC *pic, int fpic_index, int fpic_handle);

				/* functions for integer viewports */
extern  int  get_fpic(int, ... );
extern  PIC *get_pic(int, ... );
extern  int  put_fpic(int, ... );
extern  int  put_pic(int, ... );

				/* functions for floating-point viewports */
extern  int  get_fpic_f(double, ... );
extern  PIC *get_pic_f(double, ... );
extern  int  put_fpic_f(double, ... );
extern  int  put_pic_f(double, ... );

	/* Viewport control functions */

extern  int  clear_view(int view_n, int color);
extern  int  close_view(int view_n, int hide_view);
extern  int  close_view_file(void );
extern  int  delete_view_file(void );
extern  int  create_view_file(char *fpic_name);
extern  void logical_int_view(int x_ul, int y_ul, int x_lr, int y_lr);
extern  void logical_float_view(double x_ul, double y_ul, double x_lr, double y_lr);
extern  int  switch_view(int new_view_n, int hide_view);

				/* integer viewports */
extern  int  move_view(int, ... );
extern  int  open_view(int, ... );

				/* floating-point viewports */
extern  int  move_view_f(double, ... );
extern  int  open_view_f(double, ... );

	/* Now obsolete functions */
extern  int  line(int, ... );		/* replaced by DRAW_LINE & BOX */
extern  int  locate(int, int );
extern  void scale_int_view(int x_scale, int y_scale, int x_org, int y_org, int x_dir, int y_dir);
extern  void scale_float_view(double x_scale, double y_scale, double x_org, double y_org, int x_dir, int y_dir);

	/* NEW 3.0 FUNCTIONS */

extern  int  identify_super_VGA(void);
extern  int  set_super_VGA_mode(int);
extern  int  get_super_VGA_info(int **svga_modes, int vram);

extern  int  put_PCX_file(int arg, ...);
extern  int  get_PCX_file(int arg, ... );

extern  int  map_VGA_palette(int, int, unsigned char *);
extern  int  read_VGA_palette(int, int, unsigned char *);
extern  void map_VGA_grey_scale(int brightness);

extern  unsigned int alloc_GFX_video_ram(unsigned int);
extern  unsigned int alloc_GFX_workspace(unsigned int);
extern  unsigned int alloc_GFX_system_ram(unsigned int);
extern  unsigned int alloc_GFX_disk_mem(int, unsigned int);

extern  void free_GFX_workspace(void);
extern  void free_GFX_system_ram(void);
extern  void free_GFX_disk_mem(void);

extern  int  get_blob_name_index(char *, int);
extern  int  init_blob_name_list(char *, int, int, int *);
extern  int  get_blob_name_list(char *, int, int, char **, int);
extern  int  open_blob_font(int, int);
extern  void sort_blob_name_list(char *blob_names[], int, int);
extern  int  get_blob_item_name(char *, int, int);
extern  int  set_blob_item_name(char *, int, int);
extern  int  slice_file_from_blob(char *, int, int, int);
extern  int  add_file_to_blob(char *, int, int, int, int);
extern  UTINY_FAR *load_blob_item(UTINY_FAR *, long *, int, int);
extern  int  read_PCX_header(char *, PCX_HDR *);
extern  int  open_PCX_file(char *, PIC *);
extern  PIC *get_default_pic_struct_ptr(void);
extern  int  init_pic_struct_for_blob(PIC *, int, int);
extern  void init_pic_struct(PIC *,  int);
extern  int  delete_blob_item(int, int);
extern  int  get_blob_item_info(BLOB_ITEM *, int, int);
extern  int  close_blob_file(int);
extern  int  open_blob_file(char *, int);
extern  int  create_blob_file(char *, int, int);
extern  long get_image_size(int, ...);
extern  int  get_image_size_f(int, ...);
extern  int  put_image(int, ...);
extern  int  put_image_f(double, ...);
extern  int  get_image(int, ...);
extern  int  get_image_f(double, ...);



#endif
			/*******************************************
			 *	GFX FONTS & MENUS LIBRARY FUNCTIONS   *
			 *******************************************/

#if defined (USE_GFX_FONTS_MENUS)

	/* font functions */

extern int close_font(int);
extern int curr_font(int);
extern int font_align(int, int);
extern int font_color(int, int, int);
extern int font_magnify(int, int, int);
extern int font_rotate(int, int);
extern int font_spacing(int, int);
extern int font_underline(int, int, int, int);
extern void fprint_font(char *, ...);
extern struct _fontattr *get_font_attr(int);
struct _lineattr *get_line_attr(void);
extern int get_font_height(int);
extern int get_line_len(int , char *,int);
extern int line_direction(int);
extern int line_justify(int);
extern int line_underline(int);
extern int open_font(char *);
extern int open_mem_font(FONT *);
extern void print_font(int, ...);
extern void get_font_cursor(COOR_PT *cpt);
#endif

#endif

#if defined (USE_UPPER_C) || (USE_LOWER_C)

#if defined (USE_GFX_FONTS_MENUS)


	/* menu functions */

extern void  set_double_click(int n_ticks);
extern void  set_mouse_event(int mouse_event_mask);
extern void  init_exit_hook(EVENT *(*exit_hook_fcn)(), unsigned int mask_for_exit_hook);
extern void  init_loop_hook(void  (*event_vec)());
extern void  init_event_timer(int n_ticks, int auto_repeat);
extern EVENT *peek_event(void);
extern int   push_event(EVENT *event);
extern void  flush_event_q(void);
extern void  halt_q(void);
extern void  set_cursor_regions(int n_regions, CURSOR_REGION *crsr_regions);
extern EVENT *get_event(int event_mask);
extern int   qprint(int x, int y, int fh, int color, int justify, char *_tp, int max_n_char);
extern int   qstring_len(int	fh, char	*tp);
extern int   xlat_keystroke_to_hot_spot(EVENT *ev);
extern int   push_hot_spot(HOT_SPOT *hs);
extern int   pop_hot_spots(int n_hot_spots);
extern void  sleep_all_hot_spots(void);
extern void  wake_hot_spots(void);
extern void  mod_hot_spots_status(int first_hs_n, int n_hot_spots, int type, int status);
extern void  set_root_menu(ROOT_MENU *new_root);
extern void  position_root_menu(int root_top);
extern void  menu_title(char *title, int color, int fh, int r_pad);
extern void  clear_menus(void);
extern void  init_GFX_menu(int n_box_layers, int dflt_fh, int orig_x_res, int orig_y_res);
extern void  reset_auto_cs_layout(int save_orig_settings);
extern void  reset_color_scheme(int n_cs, COLOR_SCHEME **csa, int n_items, COLOR_MAP *color_map);
extern void  reset_layout(int n_layouts, LAYOUT **layouts);
extern void  init_auto_GFX_menu(int used_video_pages, int n_sys_para, int n_box_layers, int dflt_fh, int x_res, int y_res);
extern void  init_auto_gfx_menu(int used_video_pages, int n_sys_para, int n_box_layers, int dflt_fh, int x_res, int y_res);
extern EVENT *set_box_wrt_menu(int justify, int delta_x, int delta_y, GFX_BOX *gbox);
extern EVENT *set_box_wrt_center(int dx, int dy, GFX_BOX *gbox);
extern void  clear_box(void);
extern int   curr_box(int box_n);
extern EVENT *set_box(int args, ...);
extern EVENT *_set_box(int x, int y, GFX_BOX *gbox);
extern EVENT *revisit_box(void);
extern int   init_auto_box(int type, GFX_BOX *dflt_gbox);
extern void  init_auto_buttons(int n_button_sets, BUTTON_SET **buttons);
extern int   init_mouse(void);
extern GFX_BOX *build_auto_box(int type, char *box_title, int button_n, int n_items, char **items);
extern int   get_directory(char *asciiz, int flags, unsigned char ***items);
extern void  shade_box(int x, int y, int x_width, int y_height, int pattern, int color);
extern int   scale_dim(int val, int x_or_y);
extern void  init_video_storage(unsigned int used_video_mem, UTINY_FAR *buf_mem, unsigned int n_para_ram_mem);
extern void  _gfx_color_box(int x1, int y1, int x2, int y2, int color);
extern void  _gfx_solid_box(int x1, int y1, int x2, int y2, int color);

/* Low level mouse functions */

extern int  AFdecl mouse_reset(void);
extern void AFdecl mouse_show_cursor(void);
extern void AFdecl mouse_show_crsr(void);
extern void AFdecl mouse_hide_cursor(void);
extern void AFdecl mouse_hide_crsr(void);
extern void AFdecl mouse_get_cursor(int crsr[3]);
extern void AFdecl mouse_set_cursor(int x, int y);
extern int  AFdecl mouse_get_button_press_info(int button_n, int crsr[3]);
extern int  AFdecl mouse_get_button_release_info(int button_n, int crsr[3]);
extern void AFdecl mouse_set_bounds(int min_x, int min_y, int max_x, int max_y);
extern void AFdecl mouse_define_cursor_shape(CURSOR_SHAPE *crsr_shape);
extern void AFdecl mouse_read_motion_counters(int *h_count, int *v_count);
extern void AFdecl mouse_set_mickey_to_pixel_ratio(int x_ratio, int y_ratio);
extern void AFdecl mouse_area_to_hide_cursor(int min_x, int min_y, int max_x, int max_y);
extern void AFdecl mouse_set_speed_threshold(int mickeys_per_sec);

#endif


			/*******************************************
			 *	 GFX SCREEN DUMP LIBRARY FUNCTIONS    *
			 *******************************************/

#if defined (USE_GFX_SCREEN_DUMP)

extern int  set_mem_printer_rec(int dev_id, char *port_name);
extern int  set_disk_printer_rec(int dev_id, char *port_name);
extern void set_prx_options(int pic, int clear, int white, int eject, int reset);
extern int  set_prx_monitor_aspect(int x_asp, int y_asp);
extern int  get_printer_status(int n);
extern void set_prx_swath_hook(int (*hook_fcn)(int, int, int, int));
extern int  print_screen(int width, int height, int lmargin, int tmargin);
extern int  print_area(int arg, ...);
extern int  print_area_f(int arg, ...);
extern void set_prx_solid_output(unsigned char solid_pat[16]);
extern void set_prx_shading(unsigned char *usr_pat_data);
extern void set_prx_color_table(unsigned char *usr_color_data);
extern int  set_prx_output_scale(int type);
extern void set_prx_table_name(char *table_name);
extern void link_all_printer_types(void);
extern void link_dot_matrix_printers(void);
extern void link_laser_printers(void);

#endif

#endif

#if defined (USE_GFX_FONTS_MENUS)


#endif

#if defined (__cplusplus)
	}
#endif

#endif




