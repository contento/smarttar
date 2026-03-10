//  Derived Classes...
const OBJECTID	ID_UIW_PHONE                       	 = 3503;


//  Help Contexts...


#ifdef USE_W_FOOTER
const USHORT FIELD_13                         = 0x000D;
const USHORT W_MSG1                           = 0x000E;
const USHORT W_MSG2                           = 0x000F;
const USHORT FIELD_3                          = 0x0003;
const USHORT FIELD_4                          = 0x0004;
#endif // USE_W_FOOTER

#ifdef USE_W_CALC
const USHORT FIELD_8                          = 0x0008;
const USHORT FIELD_9                          = 0x0009;
const USHORT FIELD_10                         = 0x000A;
const USHORT W_OP                             = 0x000B;
const USHORT W_RES                            = 0x000C;
const USHORT FIELD_5                          = 0x0005;
#endif // USE_W_CALC

#ifdef USE_W_IPRINT
const USHORT FIELD_23                         = 0x0017;
const USHORT FIELD_25                         = 0x0019;
const USHORT W_AREA                           = 0x001A;
const USHORT FIELD_3                          = 0x0003;
const USHORT W_ALPHA                          = 0x0004;
const USHORT FIELD_5                          = 0x0005;
const USHORT FIELD_6                          = 0x0006;
const USHORT FIELD_7                          = 0x0007;
const USHORT W_LIST                           = 0x0035;
const USHORT FIELD_60                         = 0x003C;
#endif // USE_W_IPRINT

#ifdef USE_W_LOCK
const USHORT FIELD_6                          = 0x0006;
const USHORT B_LOCK                           = 0x0007;
const USHORT B_UNLOCK                         = 0x0008;
const USHORT FIELD_1                          = 0x0001;
const USHORT W_BOOTH                          = 0x0002;
const USHORT FIELD_3                          = 0x0003;
const USHORT FIELD_4                          = 0x0004;
#endif // USE_W_LOCK

#ifdef USE_W_CASH
const USHORT G_DEV                            = 0x0001;
const USHORT W_PRN                            = 0x0006;
const USHORT W_COM1                           = 0x0004;
const USHORT W_COM2                           = 0x0005;
const USHORT W_ST                             = 0x0007;
const USHORT FIELD_2                          = 0x0002;
const USHORT FIELD_3                          = 0x0003;
#endif // USE_W_CASH

#ifdef USE_W_LEVEL
const USHORT W_MSG                            = 0x000C;
const USHORT W_REF                            = 0x0006;
const USHORT FIELD_2                          = 0x0002;
#endif // USE_W_LEVEL

#ifdef USE_W_P_PORT
const USHORT W_PORT                           = 0x0001;
const USHORT W_LPT1                           = 0x0002;
const USHORT W_LPT2                           = 0x0003;
const USHORT FIELD_4                          = 0x0004;
const USHORT FIELD_5                          = 0x0005;
#endif // USE_W_P_PORT

#ifdef USE_W_OP_ID
const USHORT FIELD_1                          = 0x0001;
const USHORT W_MR                             = 0x0006;
const USHORT W_MRS                            = 0x0007;
const USHORT W_MISS                           = 0x0008;
const USHORT FIELD_9                          = 0x0009;
const USHORT W_NAME                           = 0x000A;
const USHORT FIELD_2                          = 0x0002;
const USHORT FIELD_3                          = 0x0003;
#endif // USE_W_OP_ID

#ifdef USE_W_OPERATION
const USHORT FIELD_1                          = 0x0001;
const USHORT W_AUTO                           = 0x0002;
const USHORT W_MANUAL                         = 0x0003;
const USHORT FIELD_5                          = 0x0005;
const USHORT FIELD_6                          = 0x0006;
#endif // USE_W_OPERATION

#ifdef USE_W_ZPRINT
const USHORT FIELD_23                         = 0x0017;
const USHORT FIELD_25                         = 0x0019;
const USHORT W_AREA                           = 0x001A;
const USHORT W_LOCAL                          = 0x0018;
const USHORT FIELD_3                          = 0x0003;
const USHORT W_ALPHA                          = 0x0004;
const USHORT FIELD_5                          = 0x0005;
const USHORT FIELD_6                          = 0x0006;
const USHORT FIELD_7                          = 0x0007;
const USHORT W_LIST                           = 0x0035;
const USHORT FIELD_60                         = 0x003C;
#endif // USE_W_ZPRINT

#ifdef USE_W_TIMES
const USHORT FIELD_18                         = 0x0012;
const USHORT FIELD_19                         = 0x0013;
const USHORT FIELD_20                         = 0x0014;
const USHORT FIELD_21                         = 0x0015;
const USHORT FIELD_22                         = 0x0016;
const USHORT FIELD_23                         = 0x0017;
const USHORT W_T_TALK                         = 0x0018;
const USHORT W_T_DIAL                         = 0x0019;
const USHORT W_T_LOCK                         = 0x001A;
const USHORT W_T_ANSWER                       = 0x001B;
const USHORT W_T_COM                          = 0x001C;
const USHORT FIELD_29                         = 0x001D;
const USHORT FIELD_30                         = 0x001E;
const USHORT FIELD_31                         = 0x001F;
const USHORT FIELD_32                         = 0x0020;
const USHORT FIELD_33                         = 0x0021;
const USHORT FIELD_11                         = 0x000B;
const USHORT FIELD_12                         = 0x000C;
#endif // USE_W_TIMES

#ifdef USE_W_S_PORT
const USHORT W_PORT                           = 0x0001;
const USHORT W_COM1                           = 0x0007;
const USHORT W_COM2                           = 0x0008;
const USHORT W_SPEED                          = 0x0002;
const USHORT W_1200                           = 0x000B;
const USHORT W_2400                           = 0x000C;
const USHORT W_4800                           = 0x000D;
const USHORT W_9600                           = 0x000E;
const USHORT W_19200                          = 0x000F;
const USHORT W_BITS                           = 0x0004;
const USHORT W_7                              = 0x0012;
const USHORT W_8                              = 0x0013;
const USHORT W_PARITY                         = 0x0003;
const USHORT W_EVEN                           = 0x0010;
const USHORT W_ODD                            = 0x0011;
const USHORT W_NONE                           = 0x0014;
const USHORT W_STOP                           = 0x0015;
const USHORT W_1                              = 0x0017;
const USHORT W_2                              = 0x0018;
const USHORT FIELD_5                          = 0x0005;
const USHORT FIELD_6                          = 0x0006;
#endif // USE_W_S_PORT

#ifdef USE_W_SP_SERV
const USHORT FIELD_57                         = 0x0039;
const USHORT FIELD_58                         = 0x003A;
const USHORT FIELD_69                         = 0x0045;
const USHORT FIELD_61                         = 0x003D;
const USHORT FIELD_70                         = 0x0046;
const USHORT FIELD_59                         = 0x003B;
const USHORT FIELD_71                         = 0x0047;
const USHORT FIELD_60                         = 0x003C;
const USHORT FIELD_62                         = 0x003E;
const USHORT W_SERVICE                        = 0x0044;
const USHORT W_PRINT                          = 0x0034;
const USHORT FIELD_51                         = 0x0033;
#endif // USE_W_SP_SERV

#ifdef USE_W_SPY
const USHORT FIELD_11                         = 0x000B;
const USHORT B_IN_LINE                        = 0x000D;
const USHORT B_EX_LINE                        = 0x000C;
const USHORT FIELD_1                          = 0x0001;
const USHORT W_BOOTH                          = 0x0002;
const USHORT FIELD_3                          = 0x0003;
const USHORT FIELD_4                          = 0x0004;
#endif // USE_W_SPY

#ifdef USE_W_ALARM
const USHORT FIELD_1                          = 0x0001;
const USHORT FIELD_2                          = 0x0002;
const USHORT W_CERR                           = 0x0004;
const USHORT FIELD_3                          = 0x0003;
const USHORT W_DERR                           = 0x0005;
const USHORT FIELD_6                          = 0x0006;
const USHORT FIELD_7                          = 0x0007;
#endif // USE_W_ALARM

#ifdef USE_W_CHANGE_PASSWD
const USHORT FIELD_8                          = 0x0008;
const USHORT FIELD_1                          = 0x0009;
const USHORT S_PASSWD                         = 0x000B;
const USHORT FIELD_6                          = 0x000A;
const USHORT S_NEW_PASSWD                     = 0x000C;
const USHORT FIELD_4                          = 0x0004;
const USHORT FIELD_5                          = 0x0005;
#endif // USE_W_CHANGE_PASSWD

#ifdef USE_W_RECEIPT
const USHORT FIELD_178                        = 0x00B2;
const USHORT FIELD_179                        = 0x00B3;
const USHORT FIELD_180                        = 0x00B4;
const USHORT FIELD_181                        = 0x00B5;
const USHORT FIELD_17                         = 0x0011;
const USHORT W_BOOTH                          = 0x0012;
const USHORT FIELD_189                        = 0x00BD;
const USHORT W_NUMBER                         = 0x00D6;
const USHORT W_LIST                           = 0x00B6;
const USHORT W_PAY                            = 0x00B9;
const USHORT FIELD_123                        = 0x007B;
const USHORT FIELD_124                        = 0x007C;
const USHORT FIELD_229                        = 0x00E5;
const USHORT FIELD_230                        = 0x00E6;
const USHORT FIELD_231                        = 0x00E7;
const USHORT FIELD_232                        = 0x00E8;
const USHORT FIELD_233                        = 0x00E9;
const USHORT FIELD_242                        = 0x00F2;
const USHORT FIELD_243                        = 0x00F3;
const USHORT FIELD_250                        = 0x00FA;
#endif // USE_W_RECEIPT

#ifdef USE_W_ALIAS
const USHORT G_BOOTHS                         = 0x0001;
const USHORT FIELD_5                          = 0x0005;
const USHORT W_CANCEL                         = 0x0006;
#endif // USE_W_ALIAS

#ifdef USE_W_MANUAL
const USHORT FIELD_1                          = 0x0001;
const USHORT G_TOTAL                          = 0x0004;
const USHORT FIELD_58                         = 0x003A;
const USHORT FIELD_59                         = 0x003B;
const USHORT FIELD_60                         = 0x003C;
const USHORT FIELD_66                         = 0x0042;
const USHORT FIELD_67                         = 0x0043;
const USHORT W_NO_PAY                         = 0x0044;
const USHORT W_TOLL_FREE                      = 0x0045;
const USHORT W_SUB_TOTAL                      = 0x0046;
const USHORT W_PAID                           = 0x0047;
const USHORT W_BACK                           = 0x0048;
const USHORT FIELD_5                          = 0x0005;
const USHORT FIELD_6                          = 0x0006;
const USHORT FIELD_7                          = 0x0019;
const USHORT FIELD_8                          = 0x001A;
const USHORT FIELD_9                          = 0x001B;
const USHORT FIELD_10                         = 0x001C;
const USHORT FIELD_11                         = 0x001D;
const USHORT FIELD_12                         = 0x001E;
const USHORT FIELD_61                         = 0x003D;
const USHORT W_CALLS                          = 0x003F;
const USHORT FIELD_64                         = 0x0040;
const USHORT W_TOTAL                          = 0x0041;
const USHORT FIELD_139                        = 0x008B;
const USHORT FIELD_140                        = 0x008C;
const USHORT W_BOOTH                          = 0x00B5;
#endif // USE_W_MANUAL

#ifdef USE_W_NAL_TAR
const USHORT G_FULL                           = 0x007E;
const USHORT G_REDUCED                        = 0x007F;
const USHORT FIELD_130                        = 0x0082;
const USHORT FIELD_131                        = 0x0083;
const USHORT FIELD_132                        = 0x0084;
const USHORT FIELD_133                        = 0x0085;
const USHORT FIELD_146                        = 0x0092;
const USHORT FIELD_147                        = 0x0093;
const USHORT W_B_APPLY                        = 0x0096;
const USHORT W_E_APPLY                        = 0x0097;
const USHORT FIELD_125                        = 0x007D;
const USHORT FIELD_79                         = 0x004F;
const USHORT W_CANCEL                         = 0x0091;
const USHORT W_P_TAX                          = 0x0094;
const USHORT W_TAX                            = 0x0095;
#endif // USE_W_NAL_TAR

#ifdef USE_W_INTER_TAR
const USHORT G_FULL                           = 0x0001;
const USHORT G_USA                            = 0x00B6;
const USHORT G_OTHERS                         = 0x00BA;
const USHORT G_BORDER                         = 0x00C9;
const USHORT W_B_APPLY                        = 0x00D2;
const USHORT W_E_APPLY                        = 0x00D3;
const USHORT FIELD_176                        = 0x00B0;
const USHORT FIELD_3                          = 0x0003;
const USHORT W_CANCEL                         = 0x00B1;
const USHORT FIELD_187                        = 0x00BB;
const USHORT FIELD_130                        = 0x00C6;
const USHORT FIELD_131                        = 0x00C7;
const USHORT FIELD_132                        = 0x00C8;
const USHORT FIELD_205                        = 0x00CD;
const USHORT FIELD_206                        = 0x00CE;
const USHORT W_P_TAX                          = 0x00CF;
const USHORT W_TAX                            = 0x00D0;
#endif // USE_W_INTER_TAR

#ifdef USE_DERIVE_TABLE
#endif // USE_DERIVE_TABLE

#ifdef USE_W_PASSWORD
const USHORT FIELD_1                          = 0x0001;
const USHORT S_PASSWD                         = 0x0002;
const USHORT FIELD_4                          = 0x0004;
const USHORT FIELD_5                          = 0x0005;
#endif // USE_W_PASSWORD

#ifdef USE_E_PARAMETERS
const USHORT RANGE                            = 0x000B;
const USHORT FIELD_13                         = 0x000D;
const USHORT BOOTH                            = 0x000E;
const USHORT FIRST                            = 0x000F;
const USHORT LAST                             = 0x0010;
const USHORT FIELD_17                         = 0x0011;
const USHORT FIELD_18                         = 0x0012;
const USHORT FIELD_19                         = 0x0013;
const USHORT FIELD_20                         = 0x0014;
const USHORT CHARGES                          = 0x000C;
const USHORT FIELD_21                         = 0x0015;
const USHORT FIELD_22                         = 0x0016;
const USHORT INSTALL                          = 0x0017;
const USHORT LINE                             = 0x0018;
const USHORT OTHERS                           = 0x0022;
const USHORT FIELD_35                         = 0x0023;
const USHORT DISCOUNT                         = 0x0024;
const USHORT FIELD_37                         = 0x0025;
const USHORT FIELD_40                         = 0x0028;
const USHORT MIN                              = 0x0029;
const USHORT SHOW                             = 0x0026;
const USHORT OK                               = 0x0007;
const USHORT CANCEL                           = 0x0008;
#endif // USE_E_PARAMETERS

#ifdef USE_E_ACCOUNT
const USHORT FIELD_3                          = 0x0003;
const USHORT BOOTH                            = 0x0005;
const USHORT NAME                             = 0x004C;
const USHORT ACTIVE                           = 0x0030;
const USHORT OK                               = 0x0025;
const USHORT CANCEL                           = 0x0049;
const USHORT CREDITS                          = 0x0001;
const USHORT FIELD_29                         = 0x001D;
const USHORT FIELD_30                         = 0x001E;
const USHORT FIELD_31                         = 0x001F;
const USHORT FIELD_32                         = 0x0020;
const USHORT FIELD_6                          = 0x0006;
const USHORT FIELD_14                         = 0x000E;
const USHORT FIELD_19                         = 0x0013;
const USHORT DEBITS                           = 0x0021;
const USHORT FIELD_29                         = 0x001D;
const USHORT FIELD_30                         = 0x001E;
const USHORT FIELD_31                         = 0x001F;
const USHORT FIELD_32                         = 0x0020;
const USHORT FIELD_6                          = 0x0006;
const USHORT FIELD_14                         = 0x000E;
const USHORT OTHERS                           = 0x0046;
const USHORT FIELD_29                         = 0x001D;
const USHORT FIELD_30                         = 0x001E;
const USHORT FIELD_31                         = 0x001F;
const USHORT FIELD_32                         = 0x0020;
const USHORT FIELD_6                          = 0x0006;
const USHORT FIELD_14                         = 0x000E;
const USHORT RANGE                            = 0x003A;
const USHORT PR_RECS                          = 0x0048;
const USHORT FIELD_59                         = 0x003B;
const USHORT FROM                             = 0x003D;
const USHORT TO                               = 0x003E;
const USHORT FIELD_63                         = 0x003F;
const USHORT PRINT                            = 0x0022;
const USHORT FIELD_44                         = 0x002C;
const USHORT CHARGES                          = 0x002D;
const USHORT FIELD_46                         = 0x002E;
const USHORT TOTAL                            = 0x002F;
const USHORT FIELD_49                         = 0x0031;
const USHORT AVAIL                            = 0x0032;
const USHORT FIELD_52                         = 0x0034;
const USHORT CALLS                            = 0x0035;
const USHORT DISCOUNT                         = 0x004A;
const USHORT FIELD_75                         = 0x004B;
#endif // USE_E_ACCOUNT

#ifdef USE_CONFIG
const USHORT TEXT                             = 0x0004;
const USHORT FIELD_20                         = 0x0014;
const USHORT 10102                            = 0x0015;
#endif // USE_CONFIG

#ifdef USE_E_ACCUM
const USHORT FIELD_3                          = 0x0003;
const USHORT RANGE                            = 0x0004;
const USHORT G_CALLS                          = 0x000B;
const USHORT FIELD_16                         = 0x0010;
const USHORT FIELD_17                         = 0x0011;
const USHORT FIELD_12                         = 0x000C;
const USHORT N_AMOUNT                         = 0x000D;
const USHORT N_COST                           = 0x000E;
const USHORT N_TAX                            = 0x000F;
const USHORT FIELD_19                         = 0x0013;
const USHORT I_AMOUNT                         = 0x0014;
const USHORT I_COST                           = 0x0015;
const USHORT I_TAX                            = 0x0016;
const USHORT FIELD_23                         = 0x0017;
const USHORT AMOUNT                           = 0x0018;
const USHORT COST                             = 0x0019;
const USHORT TAX                              = 0x001A;
const USHORT TAX_NAME                         = 0x002F;
const USHORT BALANCE                          = 0x001B;
const USHORT FIELD_28                         = 0x001C;
const USHORT FIELD_29                         = 0x001D;
const USHORT CREDITS                          = 0x001E;
const USHORT DEBITS                           = 0x001F;
const USHORT INSTALL                          = 0x0020;
const USHORT FIELD_33                         = 0x0021;
const USHORT FIELD_37                         = 0x0025;
const USHORT LINE                             = 0x0026;
const USHORT FIELD_39                         = 0x0027;
const USHORT FIELD_40                         = 0x0028;
const USHORT OTHERS                           = 0x0029;
const USHORT DISCOUNT                         = 0x002A;
const USHORT REMAINDER                        = 0x002B;
const USHORT FIELD_44                         = 0x002C;
const USHORT FIELD_45                         = 0x002D;
const USHORT CALLS                            = 0x002E;
const USHORT CLOSE                            = 0x0024;
const USHORT PRINT                            = 0x0023;
const USHORT ARCHIVE                          = 0x0022;
#endif // USE_E_ACCUM

#ifdef USE_W_MD_02
const USHORT W_MSG                            = 0x0001;
const USHORT FIELD_2                          = 0x0002;
#endif // USE_W_MD_02

#ifdef USE_W_SETUP
const USHORT FIELD_1                          = 0x0001;
const USHORT INI2CFG                          = 0x0007;
const USHORT CFG2INI                          = 0x0008;
const USHORT INF2DAT                          = 0x0009;
const USHORT DAT2INF                          = 0x000A;
const USHORT RECEIVESEND                      = 0x0058;
const USHORT SERVER                           = 0x0059;
const USHORT W_QUIT                           = 0x0053;
const USHORT W_MSG                            = 0x0054;
#endif // USE_W_SETUP

#ifdef USE_W_PPORT
const USHORT W_CONECTION                      = 0x0001;
const USHORT W_P_PORT                         = 0x0002;
const USHORT W_S_PORT                         = 0x0003;
const USHORT FIELD_4                          = 0x0004;
const USHORT FIELD_5                          = 0x0005;
const USHORT FIELD_6                          = 0x0006;
#endif // USE_W_PPORT

#ifdef USE_W_NEW_COUNTRY
const USHORT FIELD_14                         = 0x000E;
const USHORT FIELD_15                         = 0x000F;
const USHORT W_COUNTRY                        = 0x0014;
const USHORT FIELD_16                         = 0x0010;
const USHORT W_TAR                            = 0x0015;
const USHORT FIELD_19                         = 0x0013;
const USHORT W_NUM                            = 0x0018;
const USHORT FIELD_11                         = 0x000B;
const USHORT FIELD_12                         = 0x000C;
const USHORT FIELD_27                         = 0x001B;
#endif // USE_W_NEW_COUNTRY

#ifdef USE_W_MD_01
const USHORT W_MSG                            = 0x0001;
const USHORT FIELD_2                          = 0x0002;
#endif // USE_W_MD_01

#ifdef USE_W_INTER
const USHORT FIELD_9                          = 0x0009;
const USHORT FIELD_10                         = 0x000A;
const USHORT W_BOOTH                          = 0x000C;
const USHORT FIELD_11                         = 0x000B;
const USHORT W_VALUE                          = 0x000E;
const USHORT FIELD_3                          = 0x0003;
const USHORT FIELD_7                          = 0x0007;
#endif // USE_W_INTER

#ifdef USE_W_SIMULA
const USHORT FIELD_4                          = 0x0004;
const USHORT W_BOOTH                          = 0x0005;
const USHORT FIELD_6                          = 0x0006;
const USHORT W_PHONE                          = 0x0018;
const USHORT FIELD_8                          = 0x0016;
const USHORT FIELD_9                          = 0x0017;
const USHORT FIELD_15                         = 0x000F;
const USHORT W_RECEIPT                        = 0x0012;
const USHORT W_ACCUM                          = 0x0013;
const USHORT FIELD_3                          = 0x0003;
#endif // USE_W_SIMULA

#ifdef USE_DUMP
const USHORT FIELD_3                          = 0x0003;
const USHORT FIELD_4                          = 0x0004;
const USHORT DATA                             = 0x0005;
const USHORT FIELD_6                          = 0x0006;
const USHORT FIELD_7                          = 0x0007;
const USHORT FIELD_8                          = 0x0008;
const USHORT FIELD_9                          = 0x0009;
const USHORT FIELD_10                         = 0x000A;
const USHORT FIELD_11                         = 0x000B;
const USHORT FIELD_12                         = 0x000C;
const USHORT FIELD_13                         = 0x000D;
const USHORT FIELD_14                         = 0x000E;
const USHORT FIELD_15                         = 0x000F;
const USHORT F1                               = 0x0010;
const USHORT F2                               = 0x0011;
const USHORT F3                               = 0x0012;
const USHORT F4                               = 0x0013;
const USHORT F5                               = 0x0014;
const USHORT F6                               = 0x0015;
const USHORT F7                               = 0x0016;
const USHORT F8                               = 0x0017;
const USHORT FIELD_24                         = 0x0018;
const USHORT V1                               = 0x0019;
const USHORT V2                               = 0x001A;
const USHORT V3                               = 0x001B;
const USHORT V4                               = 0x001C;
const USHORT V5                               = 0x001D;
const USHORT V6                               = 0x001E;
const USHORT V7                               = 0x001F;
const USHORT V8                               = 0x0020;
const USHORT FIELD_35                         = 0x0023;
const USHORT T1                               = 0x0024;
const USHORT T2                               = 0x0025;
const USHORT T3                               = 0x0026;
const USHORT T4                               = 0x0027;
const USHORT T5                               = 0x0028;
const USHORT T6                               = 0x0029;
const USHORT T7                               = 0x002A;
const USHORT T8                               = 0x002B;
#endif // USE_DUMP

#ifdef USE_W_LOCK_NUM
const USHORT BY_NUMBER                        = 0x0001;
const USHORT FIELD_6                          = 0x0006;
const USHORT FIELD_7                          = 0x0007;
const USHORT FIELD_8                          = 0x0008;
const USHORT FIELD_9                          = 0x0009;
const USHORT FIELD_32                         = 0x0020;
const USHORT FIELD_33                         = 0x0021;
const USHORT INTER                            = 0x000A;
const USHORT BORDER                           = 0x000B;
const USHORT CELLULAR                         = 0x000C;
const USHORT NAL                              = 0x000D;
const USHORT LOCAL                            = 0x0022;
const USHORT SPECIAL                          = 0x0023;
const USHORT FIELD_41                         = 0x0029;
const USHORT LOCK_LIST                        = 0x002B;
const USHORT W_PHONE                          = 0x0057;
const USHORT FIELD_88                         = 0x0058;
const USHORT FIELD_89                         = 0x0059;
const USHORT FIELD_90                         = 0x005A;
const USHORT FIELD_91                         = 0x005B;
const USHORT FIELD_92                         = 0x005C;
const USHORT FIELD_93                         = 0x005D;
const USHORT FIELD_94                         = 0x005E;
const USHORT FIELD_95                         = 0x005F;
const USHORT FIELD_96                         = 0x0060;
const USHORT LOCK_RANGES                      = 0x006B;
const USHORT W_PHONE                          = 0x0057;
const USHORT FIELD_88                         = 0x0058;
const USHORT FIELD_89                         = 0x0059;
const USHORT FIELD_90                         = 0x005A;
const USHORT FIELD_91                         = 0x005B;
const USHORT FIELD_92                         = 0x005C;
const USHORT FIELD_93                         = 0x005D;
const USHORT FIELD_94                         = 0x005E;
const USHORT FIELD_95                         = 0x005F;
const USHORT FIELD_96                         = 0x0060;
const USHORT FIELD_108                        = 0x006C;
const USHORT FIELD_109                        = 0x006D;
const USHORT FIELD_110                        = 0x006E;
const USHORT FIELD_111                        = 0x006F;
const USHORT FIELD_112                        = 0x0070;
const USHORT FIELD_4                          = 0x0004;
const USHORT FIELD_5                          = 0x0005;
const USHORT FIELD_85                         = 0x0055;
#endif // USE_W_LOCK_NUM

#ifdef USE_W_TIME_DATE
const USHORT FIELD_7                          = 0x0007;
const USHORT FIELD_8                          = 0x0008;
const USHORT W_TIME                           = 0x000A;
const USHORT FIELD_9                          = 0x0009;
const USHORT W_DATE                           = 0x000B;
const USHORT FIELD_5                          = 0x0005;
const USHORT FIELD_6                          = 0x0006;
#endif // USE_W_TIME_DATE

#ifdef USE_W_NEW_CITY
const USHORT FIELD_2                          = 0x0002;
const USHORT FIELD_8                          = 0x0008;
const USHORT W_CITY                           = 0x000C;
const USHORT FIELD_10                         = 0x000A;
const USHORT W_TAR                            = 0x000E;
const USHORT FIELD_11                         = 0x000B;
const USHORT W_NUM                            = 0x0010;
const USHORT LOCAL                            = 0x0014;
const USHORT FIELD_3                          = 0x0003;
const USHORT FIELD_4                          = 0x0004;
const USHORT FIELD_21                         = 0x0015;
#endif // USE_W_NEW_CITY

#ifdef USE_MESSAGE
const USHORT MSG                              = 0x0001;
#endif // USE_MESSAGE

#ifdef USE_SPLASH
const USHORT MESSAGE                          = 0x0001;
#endif // USE_SPLASH

#ifdef USE_FTRANSFER
const USHORT MESSAGE                          = 0x0003;
const USHORT CANCEL                           = 0x0001;
const USHORT FIELD_4                          = 0x0004;
#endif // USE_FTRANSFER

#ifdef USE_DISPLAY
const USHORT FIELD_58                         = 0x003F;
const USHORT MESSAGE                          = 0x003E;
const USHORT PORT                             = 0x0001;
const USHORT COM1                             = 0x0003;
const USHORT COM2                             = 0x0002;
const USHORT BAUDS                            = 0x0013;
const USHORT 9600                             = 0x0017;
const USHORT 19200                            = 0x0018;
const USHORT OK                               = 0x002E;
const USHORT CANCEL                           = 0x003D;
const USHORT FIELD_65                         = 0x0041;
const USHORT ENABLE                           = 0x0042;
#endif // USE_DISPLAY

#ifdef USE_CONSOLE
const USHORT LINE                             = 0x0002;
const USHORT SEND                             = 0x0007;
const USHORT CLOSE                            = 0x0004;
const USHORT MESSAGE                          = 0x0003;
const USHORT FIELD_5                          = 0x0005;
const USHORT FIELD_6                          = 0x0006;
#endif // USE_CONSOLE

#ifdef USE_MODEMCFG
const USHORT PORT                             = 0x0001;
const USHORT COM1                             = 0x0003;
const USHORT COM2                             = 0x0002;
const USHORT COM3                             = 0x0005;
const USHORT COM4                             = 0x0004;
const USHORT IRQ                              = 0x0008;
const USHORT IRQ1                             = 0x000A;
const USHORT IRQ2                             = 0x000B;
const USHORT IRQ3                             = 0x0009;
const USHORT IRQ4                             = 0x000C;
const USHORT BASE                             = 0x000E;
const USHORT 2E8                              = 0x000F;
const USHORT 2F8                              = 0x0010;
const USHORT 3E8                              = 0x0011;
const USHORT 3F8                              = 0x0012;
const USHORT BAUDS                            = 0x0013;
const USHORT 1200                             = 0x0015;
const USHORT 2400                             = 0x0014;
const USHORT 9600                             = 0x0017;
const USHORT 19200                            = 0x0018;
const USHORT DIAL                             = 0x0024;
const USHORT PULSE                            = 0x0025;
const USHORT TONE                             = 0x0026;
const USHORT OK                               = 0x002E;
const USHORT CANCEL                           = 0x002F;
#endif // USE_MODEMCFG

#ifdef USE_STC
const USHORT FIELD_1                          = 0x0001;
const USHORT CONNECTSERVER                    = 0x0058;
const USHORT ACTIVATECLIENT                   = 0x005B;
const USHORT CONNECTCLIENT                    = 0x005A;
const USHORT ACTIVATESERVER                   = 0x0059;
const USHORT CONFIG                           = 0x0007;
const USHORT W_QUIT                           = 0x0053;
const USHORT W_MSG                            = 0x0054;
#endif // USE_STC

#ifdef USE_SERVER
const USHORT MESSAGE                          = 0x0003;
const USHORT CLOSE                            = 0x0001;
#endif // USE_SERVER

#ifdef USE_ACTIVATION
const USHORT MESSAGE                          = 0x0003;
#endif // USE_ACTIVATION

#ifdef USE_CONNECTION
const USHORT FIELD_2                          = 0x0002;
const USHORT PHONE                            = 0x0003;
const USHORT FIELD_4                          = 0x0004;
const USHORT FIELD_5                          = 0x0005;
const USHORT MESSAGE                          = 0x0006;
#endif // USE_CONNECTION

#ifdef USE_W_FORMS
const USHORT FIELD_5                          = 0x0005;
const USHORT FIELD_6                          = 0x0006;
const USHORT W_P_FORMS                        = 0x001D;
const USHORT FIELD_32                         = 0x0020;
const USHORT W_80DR                           = 0x0022;
const USHORT W_80TP                           = 0x0047;
const USHORT W_80L                            = 0x0024;
const USHORT W_80SR                           = 0x0050;
const USHORT W_40DR                           = 0x0025;
const USHORT W_40RS                           = 0x0026;
const USHORT W_18DR                           = 0x0028;
const USHORT W_28RS                           = 0x0029;
const USHORT W_EME                            = 0x003D;
const USHORT W_HALF                           = 0x003E;
#endif // USE_W_FORMS

#ifdef USE_W_ADM_REC
const USHORT FIELD_3                          = 0x0003;
const USHORT W_REC                            = 0x0002;
const USHORT FIELD_1                          = 0x0001;
const USHORT W_PAY                            = 0x0006;
const USHORT W_NO_PAY                         = 0x0007;
const USHORT W_FREE                           = 0x0008;
const USHORT FIELD_4                          = 0x0004;
const USHORT FIELD_5                          = 0x0005;
#endif // USE_W_ADM_REC

#ifdef USE_W_INTER2
const USHORT FIELD_9                          = 0x0009;
const USHORT FIELD_10                         = 0x000A;
const USHORT W_BOOTH                          = 0x000C;
const USHORT FIELD_11                         = 0x000B;
const USHORT W_VALUE                          = 0x000E;
const USHORT FIELD_82                         = 0x0053;
const USHORT FIELD_3                          = 0x0003;
const USHORT FIELD_7                          = 0x0007;
const USHORT FIELD_16                         = 0x0010;
const USHORT FIELD_17                         = 0x0011;
const USHORT FIELD_19                         = 0x0013;
const USHORT FIELD_20                         = 0x0014;
const USHORT FIELD_21                         = 0x0015;
const USHORT FIELD_22                         = 0x0016;
const USHORT FIELD_23                         = 0x0017;
const USHORT FIELD_24                         = 0x0018;
const USHORT FIELD_25                         = 0x0019;
const USHORT 1                                = 0x001A;
const USHORT FIELD_27                         = 0x001B;
const USHORT 2                                = 0x001C;
const USHORT 3                                = 0x001D;
const USHORT 4                                = 0x001E;
const USHORT FIELD_31                         = 0x001F;
const USHORT FIELD_32                         = 0x0020;
const USHORT 5                                = 0x0021;
const USHORT 6                                = 0x0022;
const USHORT 7                                = 0x0023;
const USHORT 8                                = 0x0024;
const USHORT 9                                = 0x0025;
const USHORT 10                               = 0x0026;
const USHORT 11                               = 0x0027;
const USHORT 12                               = 0x0028;
const USHORT 13                               = 0x0029;
const USHORT 14                               = 0x002A;
const USHORT 15                               = 0x002B;
const USHORT 16                               = 0x002C;
const USHORT 17                               = 0x002D;
const USHORT 18                               = 0x002E;
const USHORT 19                               = 0x002F;
const USHORT 20                               = 0x0030;
const USHORT 21                               = 0x0031;
const USHORT 22                               = 0x0032;
const USHORT 23                               = 0x0033;
const USHORT 24                               = 0x0034;
const USHORT 25                               = 0x0035;
const USHORT 26                               = 0x0036;
const USHORT 27                               = 0x0037;
const USHORT 28                               = 0x0038;
const USHORT 29                               = 0x0039;
const USHORT 30                               = 0x003A;
const USHORT 31                               = 0x003B;
const USHORT 32                               = 0x003C;
const USHORT FIELD_61                         = 0x003D;
const USHORT FIELD_62                         = 0x003E;
const USHORT FIELD_63                         = 0x003F;
const USHORT FIELD_64                         = 0x0040;
const USHORT FIELD_65                         = 0x0041;
const USHORT FIELD_66                         = 0x0042;
const USHORT FIELD_67                         = 0x0043;
const USHORT FIELD_68                         = 0x0044;
const USHORT FIELD_69                         = 0x0045;
const USHORT FIELD_70                         = 0x0046;
const USHORT FIELD_71                         = 0x0047;
const USHORT FIELD_72                         = 0x0048;
const USHORT FIELD_73                         = 0x0049;
const USHORT FIELD_74                         = 0x004A;
const USHORT FIELD_75                         = 0x004B;
const USHORT FIELD_76                         = 0x004C;
const USHORT FIELD_77                         = 0x004D;
const USHORT FIELD_78                         = 0x004E;
const USHORT FIELD_79                         = 0x004F;
const USHORT FIELD_80                         = 0x0050;
const USHORT FIELD_81                         = 0x0051;
#endif // USE_W_INTER2

#ifdef USE_W_SIMULA2
const USHORT FIELD_25                         = 0x0019;
const USHORT C1                               = 0x0043;
const USHORT C2                               = 0x004A;
const USHORT C3                               = 0x004B;
const USHORT C4                               = 0x004E;
const USHORT C5                               = 0x004C;
const USHORT C6                               = 0x004F;
const USHORT C7                               = 0x0050;
const USHORT C8                               = 0x0051;
const USHORT T1                               = 0x0020;
const USHORT T2                               = 0x002C;
const USHORT T3                               = 0x002D;
const USHORT T4                               = 0x002E;
const USHORT T5                               = 0x002F;
const USHORT T6                               = 0x0030;
const USHORT T7                               = 0x0031;
const USHORT T8                               = 0x0032;
const USHORT P1                               = 0x0023;
const USHORT P2                               = 0x0033;
const USHORT P3                               = 0x0034;
const USHORT P4                               = 0x0035;
const USHORT P5                               = 0x0036;
const USHORT P6                               = 0x0037;
const USHORT P7                               = 0x0038;
const USHORT P8                               = 0x0039;
const USHORT L1                               = 0x0021;
const USHORT L2                               = 0x0052;
const USHORT L3                               = 0x0053;
const USHORT L4                               = 0x0054;
const USHORT L5                               = 0x0055;
const USHORT L6                               = 0x0056;
const USHORT L7                               = 0x0057;
const USHORT L8                               = 0x0058;
const USHORT ALL                              = 0x005E;
const USHORT FIELD_92                         = 0x005C;
const USHORT FIELD_93                         = 0x005D;
const USHORT FIELD_95                         = 0x005F;
const USHORT FIELD_15                         = 0x000F;
const USHORT RECEIPT                          = 0x0012;
const USHORT STATISTICS                       = 0x0013;
const USHORT FIELD_3                          = 0x0003;
#endif // USE_W_SIMULA2

#ifdef USE_CLIENT
const USHORT PATHS                            = 0x0010;
const USHORT LOCAL                            = 0x0011;
const USHORT FIELD_18                         = 0x0012;
const USHORT FIELD_19                         = 0x0013;
const USHORT REMOTE                           = 0x0014;
const USHORT FILES                            = 0x0004;
const USHORT DDN.INF                          = 0x000B;
const USHORT DDI.INF                          = 0x000C;
const USHORT LOCAL.INF                        = 0x002A;
const USHORT PH_INFO.DAT                      = 0x000A;
const USHORT ST.INI                           = 0x0009;
const USHORT ST.CFG                           = 0x0008;
const USHORT ST.EXE                           = 0x0005;
const USHORT SETUP.EXE                        = 0x001E;
const USHORT STC.EXE                          = 0x003F;
const USHORT RES.DAT                          = 0x0006;
const USHORT HELP.DAT                         = 0x0007;
const USHORT RX.DAT                           = 0x0018;
const USHORT RX.IDX                           = 0x0019;
const USHORT RX.STA                           = 0x001A;
const USHORT SEND                             = 0x0002;
const USHORT RECEIVE                          = 0x0003;
const USHORT CONSOLE                          = 0x0041;
const USHORT CLOSE                            = 0x000E;
const USHORT FIELD_57                         = 0x0039;
const USHORT ANOTHER                          = 0x003A;
const USHORT MESSAGE                          = 0x0020;
#endif // USE_CLIENT

#ifdef USE_W_SYS_INFO
const USHORT FIELD_1                          = 0x0001;
const USHORT FIELD_3                          = 0x0003;
const USHORT W_S_SIGNAL                       = 0x0004;
const USHORT W_TIME                           = 0x0005;
const USHORT FIELD_6                          = 0x0006;
const USHORT FIELD_7                          = 0x0007;
const USHORT FIELD_8                          = 0x0008;
const USHORT FIELD_9                          = 0x0009;
const USHORT FIELD_10                         = 0x000A;
const USHORT W_T_TALK                         = 0x000B;
const USHORT W_T_ANSWER                       = 0x000C;
const USHORT W_T_DIAL                         = 0x000D;
const USHORT W_T_LOCK                         = 0x000E;
const USHORT FIELD_41                         = 0x0029;
const USHORT FIELD_42                         = 0x002A;
const USHORT FIELD_43                         = 0x002B;
const USHORT FIELD_44                         = 0x002C;
const USHORT FIELD_27                         = 0x001B;
const USHORT FIELD_28                         = 0x001C;
const USHORT FIELD_29                         = 0x001D;
const USHORT FIELD_30                         = 0x001E;
const USHORT W_FORM                           = 0x001F;
const USHORT W_OPERATION                      = 0x0020;
const USHORT W_PORT                           = 0x0021;
const USHORT FIELD_24                         = 0x0018;
const USHORT FIELD_26                         = 0x001A;
const USHORT FIELD_35                         = 0x0023;
const USHORT FIELD_58                         = 0x003A;
const USHORT FIELD_59                         = 0x003B;
const USHORT FIELD_60                         = 0x003C;
const USHORT FIELD_61                         = 0x003D;
const USHORT FIELD_62                         = 0x003E;
const USHORT FIELD_63                         = 0x003F;
const USHORT INTER                            = 0x0040;
const USHORT BORDER                           = 0x0041;
const USHORT CELLULAR                         = 0x0042;
const USHORT NAL                              = 0x0043;
const USHORT LOCAL                            = 0x0044;
const USHORT SPECIAL                          = 0x0045;
const USHORT FIELD_53                         = 0x0035;
const USHORT W_CASH                           = 0x0036;
const USHORT W_BOOTHS                         = 0x0037;
const USHORT W_ROUND                          = 0x0046;
#endif // USE_W_SYS_INFO

#ifdef USE_W_SIGNAL
const USHORT FIELD_1                          = 0x0001;
const USHORT W_INV                            = 0x0005;
const USHORT W_TIME                           = 0x0007;
const USHORT W_THREAD                         = 0x0008;
const USHORT W_TONE                           = 0x0006;
const USHORT W_SIGNAL_GROUP                   = 0x0002;
const USHORT FIELD_3                          = 0x0003;
const USHORT FIELD_4                          = 0x0004;
#endif // USE_W_SIGNAL

#ifdef USE_W_INSTALL
const USHORT FIELD_29                         = 0x001D;
const USHORT W_COUNTRY                        = 0x001E;
const USHORT FIELD_33                         = 0x0021;
const USHORT FIELD_64                         = 0x0040;
const USHORT FIELD_65                         = 0x0041;
const USHORT FIELD_36                         = 0x0024;
const USHORT W_CURRENCY                       = 0x0025;
const USHORT FIELD_40                         = 0x0028;
const USHORT FIELD_67                         = 0x0043;
const USHORT FIELD_68                         = 0x0044;
const USHORT FIELD_370                        = 0x0172;
const USHORT FIELD_371                        = 0x0173;
const USHORT FIELD_372                        = 0x0174;
const USHORT FIELD_41                         = 0x0029;
const USHORT W_TAX_NAME                       = 0x002A;
const USHORT FIELD_45                         = 0x002D;
const USHORT FIELD_71                         = 0x0047;
const USHORT FIELD_72                         = 0x0048;
const USHORT FIELD_47                         = 0x002F;
const USHORT W_TAX_PERCENT                    = 0x0030;
const USHORT FIELD_49                         = 0x0031;
const USHORT W_DEALER                         = 0x0032;
const USHORT FIELD_53                         = 0x0035;
const USHORT FIELD_73                         = 0x0049;
const USHORT FIELD_74                         = 0x004A;
const USHORT FIELD_282                        = 0x011A;
const USHORT FIELD_54                         = 0x0036;
const USHORT W_ACCESS                         = 0x0037;
const USHORT FIELD_58                         = 0x003A;
const USHORT FIELD_62                         = 0x003E;
const USHORT FIELD_63                         = 0x003F;
const USHORT FIELD_88                         = 0x0058;
const USHORT FIELD_34                         = 0x0022;
const USHORT W_CITY                           = 0x0023;
const USHORT FIELD_106                        = 0x006A;
const USHORT W_COMPANY                        = 0x006B;
const USHORT FIELD_108                        = 0x006C;
const USHORT W_ID                             = 0x006D;
const USHORT FIELD_75                         = 0x004B;
const USHORT W_P_FORM                         = 0x005B;
const USHORT FIELD_32                         = 0x0020;
const USHORT W_80DR                           = 0x0022;
const USHORT W_80T                            = 0x0023;
const USHORT W_80TP                           = 0x0127;
const USHORT W_80L                            = 0x0024;
const USHORT W_40DR                           = 0x0025;
const USHORT W_40RS                           = 0x0026;
const USHORT W_18DR                           = 0x0028;
const USHORT W_28RS                           = 0x0029;
const USHORT W_EME                            = 0x003D;
const USHORT W_HALF                           = 0x003E;
const USHORT W_80SR                           = 0x014D;
const USHORT W_INFO                           = 0x0093;
const USHORT FIELD_80                         = 0x0050;
const USHORT FIELD_122                        = 0x007A;
const USHORT W_MSG                            = 0x00DC;
const USHORT W_PATCH                          = 0x0134;
#endif // USE_W_INSTALL

#ifdef USE_SETUP
const USHORT FIELD_1                          = 0x0001;
const USHORT INI2CFG                          = 0x0007;
const USHORT CFG2INI                          = 0x0008;
const USHORT INF2DAT                          = 0x0009;
const USHORT DAT2INF                          = 0x000A;
const USHORT W_QUIT                           = 0x0053;
const USHORT W_MSG                            = 0x0054;
#endif // USE_SETUP

#ifdef USE_W_VIEWER
const USHORT FIELD_1                          = 0x0001;
const USHORT W_DATE                           = 0x0004;
const USHORT FIELD_2                          = 0x0002;
const USHORT W_TURN                           = 0x0005;
const USHORT FIELD_3                          = 0x0003;
const USHORT W_NUMBER                         = 0x0006;
const USHORT FIELD_8                          = 0x0008;
const USHORT W_SERV                           = 0x0017;
const USHORT W_TIME                           = 0x0018;
const USHORT W_BOOTH                          = 0x0019;
const USHORT W_PHONE                          = 0x001B;
const USHORT W_AMOUNT                         = 0x001D;
const USHORT W_TOTAL                          = 0x001E;
const USHORT W_NC                             = 0x0021;
const USHORT W_PR                             = 0x0022;
const USHORT W_P_SERV                         = 0x002A;
const USHORT W_P_TIME                         = 0x002B;
const USHORT W_P_BOOTH                        = 0x002C;
const USHORT W_P_PHONE                        = 0x002D;
const USHORT W_P_AMOUNT                       = 0x002E;
const USHORT W_P_TOTAL                        = 0x002F;
#endif // USE_W_VIEWER

#ifdef USE_W_ROUND
const USHORT FIELD_1                          = 0x0001;
const USHORT W_001                            = 0x0017;
const USHORT W_005                            = 0x0010;
const USHORT W_010                            = 0x0011;
const USHORT W_020                            = 0x0012;
const USHORT W_025                            = 0x0013;
const USHORT W_050                            = 0x0014;
const USHORT W_1                              = 0x0015;
const USHORT W_2                              = 0x0016;
const USHORT W_5                              = 0x0002;
const USHORT W_10                             = 0x0003;
const USHORT W_20                             = 0x0004;
const USHORT W_50                             = 0x0005;
const USHORT W_100                            = 0x0006;
const USHORT W_200                            = 0x000D;
const USHORT W_500                            = 0x000E;
const USHORT W_1000                           = 0x000F;
const USHORT FIELD_8                          = 0x0008;
const USHORT FIELD_9                          = 0x0009;
#endif // USE_W_ROUND

#ifdef USE_W_ABOUT
const USHORT FIELD_4                          = 0x0004;
const USHORT FIELD_31                         = 0x001F;
const USHORT VERSION                          = 0x01FB;
const USHORT AUTHORS                          = 0x02C7;
const USHORT BUILD                            = 0x0389;
#endif // USE_W_ABOUT

#ifdef USE_DBVIEW
const USHORT FIELD_1                          = 0x0001;
const USHORT DATE                             = 0x0004;
const USHORT FIELD_2                          = 0x0002;
const USHORT TURN                             = 0x0005;
const USHORT FIELD_3                          = 0x0003;
const USHORT NUMBER                           = 0x0006;
const USHORT NUMBERS                          = 0x0031;
const USHORT RECEIPT                          = 0x0008;
const USHORT SERV                             = 0x0017;
const USHORT TIME                             = 0x0018;
const USHORT BOOTH                            = 0x0019;
const USHORT PHONE                            = 0x001B;
const USHORT AMOUNT                           = 0x001D;
const USHORT TOTAL                            = 0x001E;
const USHORT NC                               = 0x0021;
const USHORT PR                               = 0x0022;
const USHORT PSERV                            = 0x002A;
const USHORT PTIME                            = 0x002B;
const USHORT PBOOTH                           = 0x002C;
const USHORT PPHONE                           = 0x002D;
const USHORT PAMOUNT                          = 0x002E;
const USHORT PTOTAL                           = 0x002F;
const USHORT PRINT                            = 0x0036;
const USHORT CLOSE                            = 0x0033;
const USHORT STTURN                           = 0x0034;
const USHORT STSPECIAL                        = 0x0035;
#endif // USE_DBVIEW

#ifdef USE_PHONE_QUERY
const USHORT TYPE                             = 0x004C;
const USHORT LOCAL                            = 0x004D;
const USHORT NAL                              = 0x004E;
const USHORT INTER                            = 0x004F;
const USHORT FIELD_51                         = 0x0033;
const USHORT PLACE                            = 0x0034;
const USHORT PLACES                           = 0x00D4;
const USHORT FIELD_55                         = 0x0037;
const USHORT TIME                             = 0x0038;
const USHORT FIELD_67                         = 0x0045;
const USHORT TOTAL                            = 0x0046;
const USHORT CLOSE                            = 0x0047;
const USHORT FIELD_66                         = 0x008F;
const USHORT COST                             = 0x0090;
#endif // USE_PHONE_QUERY

#ifdef USE_SPECIAL_SERVICES
const USHORT FIELD_57                         = 0x0039;
const USHORT FIELD_58                         = 0x003A;
const USHORT FIELD_69                         = 0x0045;
const USHORT FIELD_61                         = 0x003D;
const USHORT FIELD_70                         = 0x0046;
const USHORT FIELD_59                         = 0x003B;
const USHORT FIELD_60                         = 0x003C;
const USHORT FIELD_62                         = 0x003E;
const USHORT W_SERVICE                        = 0x0044;
const USHORT W_PRINT                          = 0x0034;
const USHORT FIELD_51                         = 0x0033;
#endif // USE_SPECIAL_SERVICES

#ifdef USE_W_SACCUM
const USHORT FIELD_82                         = 0x0052;
const USHORT W_FT                             = 0x0054;
const USHORT W_FD                             = 0x0055;
const USHORT FIELD_86                         = 0x0056;
const USHORT W_FR                             = 0x0057;
const USHORT FIELD_144                        = 0x0090;
const USHORT FIELD_145                        = 0x0091;
const USHORT W_TT                             = 0x0099;
const USHORT W_TD                             = 0x009A;
const USHORT W_TR                             = 0x009B;
const USHORT FIELD_156                        = 0x009C;
const USHORT FIELD_99                         = 0x0063;
const USHORT FIELD_101                        = 0x0065;
const USHORT FIELD_102                        = 0x0066;
const USHORT FIELD_103                        = 0x0067;
const USHORT FIELD_105                        = 0x0069;
const USHORT W_NPR                            = 0x006A;
const USHORT W_TFR                            = 0x006B;
const USHORT W_NPV                            = 0x006F;
const USHORT W_TFV                            = 0x0070;
const USHORT FIELD_117                        = 0x0075;
const USHORT W_NP_TOTAL                       = 0x0076;
const USHORT FIELD_39                         = 0x0027;
const USHORT FIELD_40                         = 0x0028;
const USHORT FIELD_41                         = 0x0029;
const USHORT FIELD_42                         = 0x002A;
const USHORT FIELD_43                         = 0x002B;
const USHORT W_NR                             = 0x002C;
const USHORT W_NV                             = 0x003E;
const USHORT FIELD_74                         = 0x004A;
const USHORT FIELD_75                         = 0x004B;
const USHORT W_IR                             = 0x004C;
const USHORT W_IV                             = 0x004F;
const USHORT W_N_TOTAL                        = 0x0050;
const USHORT FIELD_81                         = 0x0051;
const USHORT W_NMT                            = 0x0071;
const USHORT W_NMP                            = 0x0072;
const USHORT W_IMT                            = 0x0073;
const USHORT W_IMP                            = 0x0074;
const USHORT FIELD_134                        = 0x0086;
const USHORT W_N_SUB                          = 0x0088;
const USHORT W_N_TAX                          = 0x0089;
const USHORT W_P_TAX1                         = 0x00AD;
const USHORT FIELD_20                         = 0x0014;
const USHORT FIELD_48                         = 0x0030;
const USHORT FIELD_49                         = 0x0031;
const USHORT FIELD_50                         = 0x0032;
const USHORT FIELD_51                         = 0x0033;
const USHORT FIELD_52                         = 0x0034;
const USHORT FIELD_53                         = 0x0035;
const USHORT W_SNTR                           = 0x0036;
const USHORT W_SNXR                           = 0x0037;
const USHORT W_SNFR                           = 0x0038;
const USHORT W_SNMR                           = 0x0039;
const USHORT W_SNOR                           = 0x003A;
const USHORT FIELD_59                         = 0x003B;
const USHORT FIELD_60                         = 0x003C;
const USHORT W_SNTV                           = 0x003F;
const USHORT W_SNXV                           = 0x0040;
const USHORT W_SNFV                           = 0x0041;
const USHORT W_SNMV                           = 0x0042;
const USHORT W_SNOV                           = 0x0043;
const USHORT W_S_TOTAL                        = 0x0044;
const USHORT W_SITR                           = 0x0059;
const USHORT W_SIXR                           = 0x005A;
const USHORT W_SIFR                           = 0x005B;
const USHORT W_SITV                           = 0x005E;
const USHORT W_SIXV                           = 0x005F;
const USHORT W_SIFV                           = 0x0060;
const USHORT W_S_TAX                          = 0x008A;
const USHORT W_S_SUB                          = 0x008B;
const USHORT FIELD_140                        = 0x008C;
const USHORT W_P_TAX2                         = 0x00AC;
const USHORT FIELD_142                        = 0x008E;
const USHORT W_AD                             = 0x00A6;
const USHORT W_AW                             = 0x00A7;
const USHORT W_AM                             = 0x00A8;
const USHORT W_AY                             = 0x00A9;
const USHORT FIELD_22                         = 0x0016;
const USHORT FIELD_23                         = 0x0017;
const USHORT FIELD_146                        = 0x0092;
const USHORT FIELD_147                        = 0x0093;
const USHORT FIELD_149                        = 0x0095;
const USHORT W_SUB                            = 0x0096;
const USHORT W_TAX                            = 0x0097;
const USHORT W_TOTAL                          = 0x0098;
const USHORT W_P_TAX3                         = 0x00AE;
const USHORT FIELD_159                        = 0x009F;
const USHORT FIELD_160                        = 0x00A0;
const USHORT FIELD_161                        = 0x00A1;
const USHORT W_DERR                           = 0x00A2;
const USHORT W_CERR                           = 0x00A3;
#endif // USE_W_SACCUM

#ifdef USE_W_ACCUM
const USHORT FIELD_82                         = 0x0052;
const USHORT W_FT                             = 0x0054;
const USHORT W_FD                             = 0x0055;
const USHORT FIELD_86                         = 0x0056;
const USHORT W_FR                             = 0x0057;
const USHORT FIELD_144                        = 0x0090;
const USHORT FIELD_145                        = 0x0091;
const USHORT W_TT                             = 0x0099;
const USHORT W_TD                             = 0x009A;
const USHORT W_TR                             = 0x009B;
const USHORT FIELD_156                        = 0x009C;
const USHORT FIELD_99                         = 0x0063;
const USHORT FIELD_101                        = 0x0065;
const USHORT FIELD_102                        = 0x0066;
const USHORT FIELD_103                        = 0x0067;
const USHORT FIELD_105                        = 0x0069;
const USHORT W_NPR                            = 0x006A;
const USHORT W_TFR                            = 0x006B;
const USHORT W_NPV                            = 0x006F;
const USHORT W_TFV                            = 0x0070;
const USHORT FIELD_117                        = 0x0075;
const USHORT W_NP_TOTAL                       = 0x0076;
const USHORT W_ARC                            = 0x0015;
const USHORT W_NO_DEL                         = 0x0045;
const USHORT FIELD_70                         = 0x0046;
const USHORT FIELD_39                         = 0x0027;
const USHORT FIELD_40                         = 0x0028;
const USHORT FIELD_41                         = 0x0029;
const USHORT FIELD_42                         = 0x002A;
const USHORT FIELD_43                         = 0x002B;
const USHORT W_NR                             = 0x002C;
const USHORT W_NV                             = 0x003E;
const USHORT FIELD_74                         = 0x004A;
const USHORT FIELD_75                         = 0x004B;
const USHORT W_IR                             = 0x004C;
const USHORT W_IV                             = 0x004F;
const USHORT W_N_TOTAL                        = 0x0050;
const USHORT FIELD_81                         = 0x0051;
const USHORT W_NMT                            = 0x0071;
const USHORT W_NMP                            = 0x0072;
const USHORT W_IMT                            = 0x0073;
const USHORT W_IMP                            = 0x0074;
const USHORT FIELD_134                        = 0x0086;
const USHORT W_N_SUB                          = 0x0088;
const USHORT W_N_TAX                          = 0x0089;
const USHORT W_P_TAX1                         = 0x00A8;
const USHORT FIELD_20                         = 0x0014;
const USHORT FIELD_48                         = 0x0030;
const USHORT FIELD_49                         = 0x0031;
const USHORT FIELD_50                         = 0x0032;
const USHORT FIELD_51                         = 0x0033;
const USHORT FIELD_52                         = 0x0034;
const USHORT FIELD_53                         = 0x0035;
const USHORT W_SNTR                           = 0x0036;
const USHORT W_SNXR                           = 0x0037;
const USHORT W_SNFR                           = 0x0038;
const USHORT W_SNMR                           = 0x0039;
const USHORT W_SNOR                           = 0x003A;
const USHORT FIELD_59                         = 0x003B;
const USHORT FIELD_60                         = 0x003C;
const USHORT W_SNTV                           = 0x003F;
const USHORT W_SNXV                           = 0x0040;
const USHORT W_SNFV                           = 0x0041;
const USHORT W_SNMV                           = 0x0042;
const USHORT W_SNOV                           = 0x0043;
const USHORT W_S_TOTAL                        = 0x0044;
const USHORT W_SITR                           = 0x0059;
const USHORT W_SIFR                           = 0x005B;
const USHORT W_SITV                           = 0x005E;
const USHORT W_SIFV                           = 0x0060;
const USHORT W_S_TAX                          = 0x008A;
const USHORT W_S_SUB                          = 0x008B;
const USHORT FIELD_140                        = 0x008C;
const USHORT W_P_TAX2                         = 0x00A9;
const USHORT FIELD_22                         = 0x0016;
const USHORT FIELD_23                         = 0x0017;
const USHORT W_OP                             = 0x008E;
const USHORT W_NAME                           = 0x008F;
const USHORT FIELD_146                        = 0x0092;
const USHORT FIELD_147                        = 0x0093;
const USHORT FIELD_149                        = 0x0095;
const USHORT W_SUB                            = 0x0096;
const USHORT W_TAX                            = 0x0097;
const USHORT W_TOTAL                          = 0x0098;
const USHORT W_P_TAX3                         = 0x00AA;
const USHORT FIELD_159                        = 0x009F;
const USHORT FIELD_160                        = 0x00A0;
const USHORT FIELD_161                        = 0x00A1;
const USHORT W_DERR                           = 0x00A2;
const USHORT W_CERR                           = 0x00A3;
#endif // USE_W_ACCUM

#ifdef USE_W_SIMULA3
const USHORT FIELD_25                         = 0x0019;
const USHORT C1                               = 0x0043;
const USHORT C2                               = 0x004A;
const USHORT C3                               = 0x004B;
const USHORT C4                               = 0x004E;
const USHORT C5                               = 0x004C;
const USHORT C6                               = 0x004F;
const USHORT C7                               = 0x0050;
const USHORT C8                               = 0x0051;
const USHORT T1                               = 0x0020;
const USHORT T2                               = 0x002C;
const USHORT T3                               = 0x002D;
const USHORT T4                               = 0x002E;
const USHORT T5                               = 0x002F;
const USHORT T6                               = 0x0030;
const USHORT T7                               = 0x0031;
const USHORT T8                               = 0x0032;
const USHORT P1                               = 0x0023;
const USHORT P2                               = 0x0033;
const USHORT P3                               = 0x0034;
const USHORT P4                               = 0x0035;
const USHORT P5                               = 0x0036;
const USHORT P6                               = 0x0037;
const USHORT P7                               = 0x0038;
const USHORT P8                               = 0x0039;
const USHORT L1                               = 0x0021;
const USHORT L2                               = 0x0052;
const USHORT L3                               = 0x0053;
const USHORT L4                               = 0x0054;
const USHORT L5                               = 0x0055;
const USHORT L6                               = 0x0056;
const USHORT L7                               = 0x0057;
const USHORT L8                               = 0x0058;
const USHORT ALL                              = 0x005E;
const USHORT FIELD_92                         = 0x005C;
const USHORT FIELD_93                         = 0x005D;
const USHORT FIELD_95                         = 0x005F;
const USHORT FIELD_15                         = 0x000F;
const USHORT RECEIPT                          = 0x0012;
const USHORT STATISTICS                       = 0x0013;
const USHORT FIELD_3                          = 0x0003;
#endif // USE_W_SIMULA3

#ifdef USE_W_MD_00
const USHORT W_MSG                            = 0x0001;
const USHORT FIELD_2                          = 0x0002;
const USHORT FIELD_373                        = 0x0175;
#endif // USE_W_MD_00

