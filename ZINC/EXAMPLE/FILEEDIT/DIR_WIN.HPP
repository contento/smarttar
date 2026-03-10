#ifdef USE_DRIVE_ERROR
const USHORT ERROR_STRING                     = 0x0001;
const USHORT ERROR_RETRY_FIELD                = 0x0002;
const USHORT ICON_FIELD                       = 0x0004;
const USHORT ERROR_CANCEL_FIELD               = 0x0005;
#endif // USE_DRIVE_ERROR

#ifdef USE_FILE_STATISTICS
const USHORT NAME_PROMPT                      = 0x0001;
const USHORT NAME_FIELD                       = 0x0002;
const USHORT SIZE_PROMPT                      = 0x0003;
const USHORT SIZE_FIELD                       = 0x0004;
const USHORT DATE_PROMPT                      = 0x0005;
const USHORT DATE_FIELD                       = 0x0006;
const USHORT TIME_PROMPT                      = 0x0007;
const USHORT TIME_FIELD                       = 0x0008;
const USHORT FIELD_14                         = 0x000E;
const USHORT FIELD_15                         = 0x000F;
#endif // USE_FILE_STATISTICS

#ifdef USE_HELP_CONTEXTS
const UI_HELP_CONTEXT HELP_FILE_EDITOR                 = 0x0001; // Help File Editor
const UI_HELP_CONTEXT HELP_FILE_ABOUT                  = 0x0007; // Help, About File Editor...
const UI_HELP_CONTEXT HELP_FILE_COMMANDS               = 0x0006; // Help, Commands...
const UI_HELP_CONTEXT HELP_FILE_STATISTICS             = 0x0002; // Help File Statistics
const UI_HELP_CONTEXT HELP_DIRECTORY_SERVICE           = 0x0003; // Help File Selection
const UI_HELP_CONTEXT HELP_DIRECTORY_SERVICE_MTF       = 0x0004; // Help File Selection
const UI_HELP_CONTEXT HELP_DIRECTORY_SERVICE_OS2       = 0x0005; // Help File Selection
#endif // USE_HELP_CONTEXTS

#ifdef USE_FILE_ERROR
const USHORT ERROR_OK_BUTTON                  = 0x0002;
const USHORT FILE_ICON_FIELD                  = 0x0004;
const USHORT ERROR_TEXT                       = 0x0010;
#endif // USE_FILE_ERROR

#ifdef USE_FILE_EDITOR
const USHORT MENU_FIELD                       = 0x0016;
const USHORT OPTION_FILE                      = 0x0017;
const USHORT FIELD_237                        = 0x00ED;
const USHORT OPTION_FILE_NEW                  = 0x002D;
const USHORT OPTION_FILE_OPEN                 = 0x002E;
const USHORT OPTION_FILE_SAVE                 = 0x0044;
const USHORT OPTION_FILE_SAVE_AS              = 0x0045;
const USHORT OPTION_FILE_SEPARATOR1           = 0x0046;
const USHORT OPTION_FILE_DELETE               = 0x0047;
const USHORT OPTION_FILE_SEPARATOR2           = 0x0048;
const USHORT OPTION_FILE_STATISTICS           = 0x00F7;
const USHORT OPTION_FILE_SEPARATOR3           = 0x00F8;
const USHORT OPTION_FILE_EXIT                 = 0x0049;
const USHORT OPTION_HELP                      = 0x00EE;
const USHORT FIELD_240                        = 0x00F0;
const USHORT OPTION_HELP_COMMANDS             = 0x0147;
const USHORT OPTION_HELP_ABOUT                = 0x00EF;
const USHORT TEXT_FIELD                       = 0x0001;
#endif // USE_FILE_EDITOR

#ifdef USE_DIRECTORY_SERVICE
const USHORT FILE_PROMPT                      = 0x0001;
const USHORT FILE_STRING                      = 0x0002;
const USHORT DIRECTORY_PROMPT                 = 0x0003;
const USHORT DIRECTORY_STRING                 = 0x0004;
const USHORT FILE_LIST                        = 0x0005;
const USHORT DIRECTORY_LIST                   = 0x0007;
const USHORT FILTER_PROMPT                    = 0x000D;
const USHORT FILTER_LIST                      = 0x000B;
const USHORT FIELD_12                         = 0x000C;
const USHORT FILTER_ALL_FILES                 = 0x00CD;
const USHORT FILTER_CPP_FILES                 = 0x00CE;
const USHORT FILTER_C_FILES                   = 0x0264;
const USHORT FILTER_HPP_FILES                 = 0x00CF;
const USHORT FILTER_H_FILES                   = 0x0265;
const USHORT FILTER_TXT_FILES                 = 0x00D0;
const USHORT DRIVE_PROMPT                     = 0x000E;
const USHORT DRIVE_LIST                       = 0x000F;
const USHORT FIELD_16                         = 0x0010;
const USHORT SYSTEM_OK_BUTTON                 = 0x0012;
const USHORT SYSTEM_CANCEL_BUTTON             = 0x0013;
const USHORT SYSTEM_HELP_BUTTON               = 0x0014;
#endif // USE_DIRECTORY_SERVICE

#ifdef USE_DIRECTORY_SERVICE_MTF
const USHORT FILE_PROMPT                      = 0x0001;
const USHORT FILE_STRING                      = 0x0002;
const USHORT DIRECTORY_PROMPT                 = 0x0003;
const USHORT FILE_LIST                        = 0x0005;
const USHORT DIRECTORY_LIST                   = 0x0007;
const USHORT FILTER_PROMPT                    = 0x000D;
const USHORT FILTER_LIST                      = 0x000B;
const USHORT FIELD_12                         = 0x000C;
const USHORT FILTER_ALL_FILES                 = 0x00CD;
const USHORT FILTER_CPP_FILES                 = 0x00CE;
const USHORT FILTER_C_FILES                   = 0x01B9;
const USHORT FILTER_HPP_FILES                 = 0x00CF;
const USHORT FILTER_H_FILES                   = 0x01BA;
const USHORT FILTER_TXT_FILES                 = 0x00D0;
const USHORT SYSTEM_OK_BUTTON                 = 0x0012;
const USHORT SYSTEM_CANCEL_BUTTON             = 0x0013;
const USHORT SYSTEM_HELP_BUTTON               = 0x0014;
const USHORT FIELD_330                        = 0x014A;
#endif // USE_DIRECTORY_SERVICE_MTF

#ifdef USE_DIRECTORY_SERVICE_OS2
const USHORT FILE_PROMPT                      = 0x0001;
const USHORT FILE_STRING                      = 0x0002;
const USHORT DIRECTORY_PROMPT                 = 0x0003;
const USHORT FILE_LIST                        = 0x0005;
const USHORT DIRECTORY_LIST                   = 0x0007;
const USHORT FILTER_PROMPT                    = 0x000D;
const USHORT FILTER_LIST                      = 0x000B;
const USHORT FIELD_12                         = 0x000C;
const USHORT FILTER_ALL_FILES                 = 0x00CD;
const USHORT FILTER_CPP_FILES                 = 0x00CE;
const USHORT FILTER_C_FILES                   = 0x0215;
const USHORT FILTER_HPP_FILES                 = 0x00CF;
const USHORT FILTER_H_FILES                   = 0x0216;
const USHORT FILTER_TXT_FILES                 = 0x00D0;
const USHORT DRIVE_PROMPT                     = 0x000E;
const USHORT DRIVE_LIST                       = 0x000F;
const USHORT FIELD_16                         = 0x0010;
const USHORT SYSTEM_OK_BUTTON                 = 0x0012;
const USHORT SYSTEM_CANCEL_BUTTON             = 0x0013;
const USHORT SYSTEM_HELP_BUTTON               = 0x0014;
const USHORT FILENAME_PROMPT                  = 0x01BF;
#endif // USE_DIRECTORY_SERVICE_OS2

#ifdef USE_DIRECTORY_SERVICE_TXT
const USHORT FILE_PROMPT                      = 0x0001;
const USHORT FILE_STRING                      = 0x0002;
const USHORT DIRECTORY_PROMPT                 = 0x0003;
const USHORT DIRECTORY_STRING                 = 0x0004;
const USHORT FILE_LIST                        = 0x0005;
const USHORT DIRECTORY_LIST                   = 0x0007;
const USHORT FILTER_PROMPT                    = 0x000D;
const USHORT FILTER_LIST                      = 0x000B;
const USHORT FIELD_12                         = 0x000C;
const USHORT FILTER_ALL_FILES                 = 0x00CD;
const USHORT FILTER_CPP_FILES                 = 0x00CE;
const USHORT FILTER_C_FILES                   = 0x028E;
const USHORT FILTER_HPP_FILES                 = 0x00CF;
const USHORT FILTER_H_FILES                   = 0x028F;
const USHORT FILTER_TXT_FILES                 = 0x00D0;
const USHORT DRIVE_PROMPT                     = 0x000E;
const USHORT DRIVE_LIST                       = 0x000F;
const USHORT FIELD_16                         = 0x0010;
const USHORT SYSTEM_OK_BUTTON                 = 0x0012;
const USHORT SYSTEM_CANCEL_BUTTON             = 0x0013;
const USHORT SYSTEM_HELP_BUTTON               = 0x0014;
#endif // USE_DIRECTORY_SERVICE_TXT

