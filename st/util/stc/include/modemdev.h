#ifndef __MODEMDEV_H
#define __MODEMDEV_H

#include <ui_win.hpp>

#if !defined(__CFG_H)
#include <cfg.h>
#endif

#if !defined(COMDEF_H)
#include <comdef.hpp>
#endif

#if !defined(COMPPLIB_H)
#include <compplib.hpp>
#endif

#define MESSAGE_LEN 128

class MODEM_DEVICE: public UI_DEVICE
{
    friend TransferMonitor;
public:
    enum ROLE {
        NO_ROLE,
        SERVER_ROLE,
        CLIENT_ROLE
    };
    enum OPERATION {
        IDLE_OPERATION,
        CONSOLE_OPERATION,
        COMMAND_OPERATION,
        SEND_OPERATION,
        RECEIVE_OPERATION
    };
    enum FILE_PROCESS_STAT {
        IDLE_FILE_PROCESS_STAT,
        START_STAT,
        COMMAND_STAT,
        ACK_AFTER_COMMAND_STAT,
        RECEIVE_CMD_STAT,
        ACK_AFTER_RECEIVE_STAT,
        SEND_CMD_STAT,
        ACK_AFTER_SEND_CMD_STAT,
        PATH_STAT,
        ACK_AFTER_PATH_STAT,
        FILENAME_STAT,
        ACK_AFTER_FILENAME_STAT
    };

    MODEM_DEVICE(DEVICE_STATE state = D_OFF);
    ~MODEM_DEVICE();
    //
    BOOL isActive(void)
    {
        return active;
    }
    BOOL isConnected(void)
    {
        return connected;
    }
    void setRole     (WORD role)
    {
        this->role = role;
    }
    WORD getRole     (void)
    {
        return role;
    }
    void setOperation(WORD operation)
    {
        this->operation = operation;
    }
    WORD getOperation(void)
    {
        return operation;
    }
    BOOL activate    (void);
    BOOL deactivate  (void);
    BOOL connect     (PHONE phone);
    BOOL disconnect  (void);
    char *getViewMessage(void)
    {
        return viewMessage;
    }
    BOOL sendMessage (const char *msg);
    int  receiveMessage(void);
    BOOL setFile     (const char *clientPath, const char *serverPath, const char *name);
    // source window is the window user of this device
    // this device assumes that source window process messages
    //   UE_VIEW: to show messages.
    //   UE_CANCEL: the process was canceled (aborted).
    static void setSourceWindow  (UIW_WINDOW *win)
    {
        window = win;
    }
    static void resetSourceWindow(void)
    {
        window = NULL;
    }
private:
    // modem classes
    GFSerial     *serial;
    GFModem      *modem;
    GFDataFormat *serialDataFmt;
    GFI8250      *sio;
    GFLineStatus *lineStatus;
    //
    BOOL         active;
    BOOL         connected;
    char         viewMessage  [MESSAGE_LEN];
    char         inputMessage [MESSAGE_LEN];
    WORD         role;
    WORD         operation;
    static       UIW_WINDOW *window;
    FILE_NAME    serverPath;
    FILE_NAME    clientPath;
    FILE_NAME    filename;
    WORD         sendStatus;
    int          sendResult;
    WORD         receiveStatus;
    int          receiveResult;
    //
    EVENT_TYPE  Event(const UI_EVENT &event);
    void        Poll(void);
    //
    void processNoRole(void);
    //
    void processClient                (void);
    void processClientSendOperation   (void);
    void processClientReceiveOperation(void);
    void processClientCommandOperation(void);
    void processClientConsoleOperation(void);
    void processClientIdleOperation   (void);
    //
    void processServer                (void);
    void processServerSendOperation   (void);
    void processServerReceiveOperation(void);
    void processServerCommandOperation(void);
    void processServerConsoleOperation(void);
    void processServerIdleOperation   (void);
    //
    void sendFile   (void);
    void receiveFile(void);
    //
    BOOL isOnLine  (void);
    BOOL waitFor   (const char *match);
    char *getInput (char *input, int len);
    //
    void notifyEvent(USER_EVENT event);
};

// modem input
#define MODEM_INPUT_OK       "OK"
#define MODEM_INPUT_CONNECT  "CONNECT"
#define MODEM_INPUT_RING     "RING"

// modem messge
#define MODEM_MESSAGE_ACKNOWLEDGE "ST_ACKNOWLEDGE"
#define MODEM_MESSAGE_CLOSE       "ST_CLOSE"
#define MODEM_MESSAGE_COMMAND     "ST_COMMAND"
#define MODEM_MESSAGE_CONSOLE     "ST_CONSOLE"
#define MODEM_MESSAGE_RECEIVE     "ST_RECEIVE"
#define MODEM_MESSAGE_SEND        "ST_SEND"
#define MODEM_MESSAGE_UNKNOWN     "ST_UNKNOWN"

#endif //__MODEMDEV_H
