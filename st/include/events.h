#ifndef __EVENTS_H
#define __EVENTS_H

#if !defined(UI_EVT_HPP)
#include <ui_evt.hpp>
#endif

// -------------------------------------------------------------------------
// General events
//
// last USER_EVENT
// 10306
// please update !!! GCC/gcc
//
// -------------------------------------------------------------------------
const USER_EVENT UE_ACCEPT             = 10100;
const USER_EVENT UE_PRINT              = 10101;
const USER_EVENT UE_CANCEL             = 10102;
const USER_EVENT UE_CONTINUE           = 10103;
const USER_EVENT UE_PAY                = 10104;
const USER_EVENT UE_OPTIONS            = 10105;
const USER_EVENT UE_VIEW         			 = 10106;
const USER_EVENT UE_PREVIOUS     			 = 10107;
const USER_EVENT UE_NEXT         			 = 10108;
const USER_EVENT UE_ANOTHER            = 10109;
const USER_EVENT UE_SELECT_ALL         = 10110;
const USER_EVENT UE_PRINT_ALL          = 10111;
const USER_EVENT UE_UPDATE_HLP_BAR     = 10112;
const USER_EVENT UE_REFRESH            = 10113;
const USER_EVENT UE_REFRESH_STBAR      = 10114;
const USER_EVENT UE_RECEIVE            = 10115;
const USER_EVENT UE_SEND               = 10116;
const USER_EVENT UE_CONFIG             = 10117;
const USER_EVENT UE_SERVER             = 10118;
const USER_EVENT UE_CONNECT            = 10119;
const USER_EVENT UE_DISCONNECT         = 10120;
const USER_EVENT UE_CONSOLE            = 10121;
const USER_EVENT UE_CLOSE              = 10122;
const USER_EVENT UE_CONNECTCLIENT      = 10123;
const USER_EVENT UE_CONNECTSERVER      = 10124;
const USER_EVENT UE_ACTIVATESERVER     = 10125;
const USER_EVENT UE_ACTIVATECLIENT     = 10126;
const USER_EVENT UE_PASTE              = 10127;
const USER_EVENT UE_CUT                = 10128;
const USER_EVENT UE_COPY               = 10129;
const USER_EVENT UE_PARTIALSEND        = 10130;

// -------------------------------------------------------------------------
//  Menu events
// -------------------------------------------------------------------------
// --- File events
const USER_EVENT UE_FLUSH       = 10091;
const USER_EVENT UE_FLUSH_ALL   = 10092;
const USER_EVENT UE_REBUILD_ALL = 10096;
// ---- config events
const USER_EVENT UE_TIME_DATE = 10052;
const USER_EVENT UE_SDAYS = 10067;
const USER_EVENT UE_ADD_SDAY = 10083;
const USER_EVENT UE_DEL_SDAY = 10084;
const USER_EVENT UE_SIGNAL = 10054;
const USER_EVENT UE_S_INVERT = 10060;
const USER_EVENT UE_S_TONE   = 10061;
const USER_EVENT UE_S_TIME   = 10062;
const USER_EVENT UE_S_THREAD = 10066;
const USER_EVENT UE_TIMES    = 10053;
const USER_EVENT UE_LOCK_NUM = 10068;
const USER_EVENT UE_NAL_TAR  = 10069;
const USER_EVENT UE_NAL_RTAR = 10075;
const USER_EVENT UE_INTER_TAR = 10070;
const USER_EVENT UE_INTER_RTAR = 10076;
const USER_EVENT UE_REDUCED    = 10078;
const USER_EVENT UE_NEW_CITY           = 10071;
const USER_EVENT UE_NEW_COUNTRY        = 10072;
const USER_EVENT UE_ROUND              = 10056;
const USER_EVENT UE_ALIAS              = 10086;
const USER_EVENT UE_CASH               = 10088;
const USER_EVENT UE_DISPLAY            = 10099;
const USER_EVENT UE_PPORT = 10057;
const USER_EVENT UE_P_PORT = 10063;
const USER_EVENT UE_S_PORT = 10064;
const USER_EVENT UE_CHANGE_PASSWD      = 10073;
const USER_EVENT UE_DEACTIVATE_CONFIG  = 10059;
const USER_EVENT UE_SIMULA             = 10083;
// ---- information events
const USER_EVENT UE_SYS_INFO = 10074;
const USER_EVENT UE_ALARM    = 10089;
const USER_EVENT UE_OP_ID    = 10090;

const USER_EVENT UE_I_ALIAS         = 10087;
const USER_EVENT UE_I_NAL_TAR       = 10079;
const USER_EVENT UE_I_INTER_TAR     = 10080;
const USER_EVENT UE_ACTIVATE_CONFIG = 10058;
const USER_EVENT UE_CONFIG_ON     = 10065;
// ---- printing events
const USER_EVENT UE_SACCUM             = 10077;
const USER_EVENT UE_ZPRINT             = 10055;
const USER_EVENT UE_IPRINT             = 10081;
const USER_EVENT UE_VIEW_TURN          = 10097;
const USER_EVENT UE_VIEW_OTHER_TURN    = 10098;
const USER_EVENT UE_FOOTER             = 10084;
const USER_EVENT UE_ADM_REC            = 10093;
// ---- help events ...
const USER_EVENT UE_PRG_MNG            = 10082;
const USER_EVENT UE_ABOUT              = 10050;
const USER_EVENT UE_AUTHORS            = 10051;

// ---- extension events ...
const USER_EVENT UE_E_ACCOUNT         = 10300;
const USER_EVENT UE_E_RECEIPTS        = 10301;
const USER_EVENT UE_E_ACCUM           = 10302;
const USER_EVENT UE_E_PARAMETERS      = 10303;
const USER_EVENT UE_E_ACTIVATE        = 10304;
const USER_EVENT UE_E_DEACTIVATE      = 10305;
const USER_EVENT UE_EXTENSION_ON      = 10306;
// -------------------------------------------------------------------------
// ToolBar events
// -------------------------------------------------------------------------
const USER_EVENT UE_SP_SERV   = 10000;
const USER_EVENT UE_SPY       = 10001;
const USER_EVENT UE_INTER     = 10002;
const USER_EVENT UE_LOCK      = 10003;
const USER_EVENT UE_UNLOCK    = 10004;
const USER_EVENT UE_CALC      = 10005;
const USER_EVENT UE_RECEIPT   = 10006;
const USER_EVENT UE_ACCUM     = 10007;
const USER_EVENT UE_OPERATION = 10008;
const USER_EVENT UE_AUTO_ON   = 10085;
const USER_EVENT UE_FORMS     = 10009;
const USER_EVENT UE_MANUAL    = 10010;

// toolbar Special services
const USER_EVENT UE_S_N_TEL   = 10011;
const USER_EVENT UE_S_I_TEL   = 10012;
const USER_EVENT UE_S_N_FAX   = 10013;
const USER_EVENT UE_S_I_FAX   = 10014;
const USER_EVENT UE_S_N_TELEX = 10015;
const USER_EVENT UE_S_I_TELEX = 10016;
const USER_EVENT UE_SMCARD    = 10017;
const USER_EVENT UE_SOTHERS   = 10018;

// Toolbar Receipt
const USER_EVENT UE_RNPNP     = 10020;
const USER_EVENT UE_RNPP      = 10021;
const USER_EVENT UE_RP        = 10022;

const USER_EVENT UE_MD        = 10030;

const USER_EVENT UE_SAVER_ON = 10094;
const USER_EVENT UE_DO_SAVER = 10095;
// -------------------------------------------------------------------------
// User Device Events
// -------------------------------------------------------------------------

const DEVICE_TYPE E_CONTROLLER   = 80;
const DEVICE_TYPE E_SPOOLER      = 81;
const DEVICE_TYPE E_SAVER        = 82;
const DEVICE_TYPE E_MODEM_DEVICE = 83;


// -------------------------------------------------------------------------
// Message from clients to UID_CONTROLLER
// -------------------------------------------------------------------------

const USER_EVENT UE_PRINT_FROM_UIW_MANUAL   = 10200;
//
const USER_EVENT UE_CANCEL_FROM_UIW_SIMULA  = 10201;
const USER_EVENT UE_HANG_FROM_UIW_SIMULA    = 10202;
const USER_EVENT UE_COM_FROM_UIW_SIMULA     = 10203;
//
const USER_EVENT UE_PAY_FROM_UIW_RECEIPT    = 10204;
const USER_EVENT UE_PRINT_FROM_UIW_RECEIPT  = 10205;
const USER_EVENT UE_NUMBER_FROM_UIW_RECEIPT = 10206;
const USER_EVENT UE_BOOTH_FROM_UIW_RECEIPT  = 10207;
const USER_EVENT UE_CANCEL_FROM_UIW_RECEIPT = 10208;
const USER_EVENT UE_PRINT_FROM_UIW_SP_SERV  = 10209;
const USER_EVENT UE_VIEW_FROM_UIW_ACCUM     = 10210;
const USER_EVENT UE_PRINT_FROM_UIW_ACCUM    = 10211;
const USER_EVENT UE_VIEW_FROM_UIW_SACCUM    = 10212;
const USER_EVENT UE_PRINT_FROM_UIW_SACCUM   = 10213;
const USER_EVENT UE_PRINT_FROM_DB_VIEW      = 10214;
const USER_EVENT UE_LOADPR_DLLS             = 10215;
const USER_EVENT UE_UPDATE_TOTAL_DISPLAY    = 10216;
const USER_EVENT UE_PHQUERY					= 10217;
const USER_EVENT UE_DEMO_TOGGLE             = 10218; // 2.50 -- pause/resume DEMO_ENGINE

#endif // __EVENTS_H
