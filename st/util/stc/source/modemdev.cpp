//
// [ MODEMDEV.CPP ]
//

#include <stdlib.h>
#include <io.h>
#include <modemdev.h>
#include <events.h>
#include <filexfer.hpp>

extern CFG *g_cfg;
extern int g_where;

UIW_WINDOW *MODEM_DEVICE::window = NULL;

class TransferMonitor: public GFMonitor
{
public:
    TransferMonitor(MODEM_DEVICE *modemDevice): GFMonitor()
    {
        this->modemDevice = modemDevice;
    }
    void TransferMessage(char *message) // to test ..
    {
        char sBytes[16];
        if (sending)
        {
            strncpy(modemDevice->viewMessage, "Enviando ", sizeof(modemDevice->viewMessage)-1);
            modemDevice->viewMessage[sizeof(modemDevice->viewMessage)-1] = '\0';
            strncat(modemDevice->viewMessage, modemDevice->filename, sizeof(modemDevice->viewMessage)-1-strlen(modemDevice->viewMessage));
        }
        else
        {
            strncpy(modemDevice->viewMessage, "Recibiendo en ", sizeof(modemDevice->viewMessage)-1);
            modemDevice->viewMessage[sizeof(modemDevice->viewMessage)-1] = '\0';
            strncat(modemDevice->viewMessage, modemDevice->serverPath, sizeof(modemDevice->viewMessage)-1-strlen(modemDevice->viewMessage));
        }
        strncat(modemDevice->viewMessage, ": ", sizeof(modemDevice->viewMessage)-1-strlen(modemDevice->viewMessage));
        strncat(modemDevice->viewMessage, ltoa(nbytes, sBytes, 10), sizeof(modemDevice->viewMessage)-1-strlen(modemDevice->viewMessage));
        strncat(modemDevice->viewMessage, " bytes", sizeof(modemDevice->viewMessage)-1-strlen(modemDevice->viewMessage));
        modemDevice->notifyEvent(UE_VIEW);
    }
private:
    MODEM_DEVICE *modemDevice;
};

MODEM_DEVICE::MODEM_DEVICE(DEVICE_STATE state)
        : UI_DEVICE(E_MODEM_DEVICE, state)
{
    modem         = NULL;
    serialDataFmt = NULL;
    serial        = NULL;
    sio           = NULL;
    lineStatus    = NULL;
    //
    active    = FALSE;
    connected = FALSE;
    role      = NO_ROLE;
    inputMessage[0]  = '\0';
    viewMessage[0]   = '\0';
    window           = NULL;
    //
    sendStatus       = IDLE_FILE_PROCESS_STAT;
    sendResult       = 0;
    receiveStatus    = IDLE_FILE_PROCESS_STAT;
    receiveResult    = 0;
}

MODEM_DEVICE::~MODEM_DEVICE()
{}


void MODEM_DEVICE::Poll(void)
{
    if (state != D_ON)
        return;
    switch (role)
    {
    case NO_ROLE    :
        processNoRole();
        break;
    case CLIENT_ROLE:
        processClient();
        break;
    case SERVER_ROLE:
        processServer();
        break;
    }
}

void MODEM_DEVICE::processNoRole(void)
{}


void MODEM_DEVICE::processClient(void)
{
    if (!connected)
    {
        lineStatus->Status();
        if (lineStatus->RingDetected())
        {
            modem->AnswerPhone();
            strncpy(viewMessage, "Conectando ...", sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
            notifyEvent(UE_VIEW);
            if (waitFor(MODEM_INPUT_CONNECT))
            {
                strncpy(viewMessage, "Conectado", sizeof(viewMessage)-1);
                viewMessage[sizeof(viewMessage)-1] = '\0';
                connected = TRUE;
            }
            else
            {
                strncpy(viewMessage, "No se pudo contestar la llamada", sizeof(viewMessage)-1);
                viewMessage[sizeof(viewMessage)-1] = '\0';
            }
            notifyEvent(UE_VIEW);
        }
    }
    else
    {
        if (isOnLine())
        {
            switch (operation)
            {
            case IDLE_OPERATION   :
                processClientIdleOperation()   ;
                break;
            case CONSOLE_OPERATION:
                processClientConsoleOperation();
                break;
            case COMMAND_OPERATION:
                processClientCommandOperation();
                break;
            case SEND_OPERATION   :
                processClientSendOperation()   ;
                break;
            case RECEIVE_OPERATION:
                processClientReceiveOperation();
                break;
            }
        }
        else
        {
            notifyEvent(UE_CANCEL);
        }
    }
}

void MODEM_DEVICE::processClientIdleOperation(void)
{}


void MODEM_DEVICE::processClientConsoleOperation(void)
{
    if (receiveMessage())
    {
        strncpy(viewMessage, inputMessage, sizeof(viewMessage)-1);
        viewMessage[sizeof(viewMessage)-1] = '\0';
        notifyEvent(UE_VIEW);
    }
}

void MODEM_DEVICE::processClientCommandOperation(void)
{}


void MODEM_DEVICE::processClientSendOperation(void)
{
    if (sendStatus == IDLE_FILE_PROCESS_STAT)
    {
        filename[0] = '\0'; // to check for a valid filename after notify
        notifyEvent(UE_NEXT); // ask for the next filename to the client
        // we suppose a new file name after notifyEvent()
        if (strlen(filename))
        {
            // now: receive the file by using the protocol
            sendStatus = START_STAT;
        }
    }
    else
    {
        sendFile();
    }
}

void MODEM_DEVICE::sendFile(void)
{
    switch (sendStatus)
    {
    case START_STAT:
        {
            // force remote machine to clear port input
            sendMessage(MODEM_MESSAGE_UNKNOWN); // | CLEAR ->
            //
            sendMessage(MODEM_MESSAGE_COMMAND); // | COMMAND ->
            sendStatus = COMMAND_STAT;
            break;
        }
    case COMMAND_STAT:
        {
            if (receiveMessage())
            { //  <- ACKNOWLEDGE |
                if (!strcmp(inputMessage, MODEM_MESSAGE_ACKNOWLEDGE))
                {
                    sendStatus = ACK_AFTER_COMMAND_STAT;
                }
            }
            break;
        }
    case ACK_AFTER_COMMAND_STAT:
        {
            sendMessage(MODEM_MESSAGE_RECEIVE); // | RECEIVE ->
            sendStatus = RECEIVE_CMD_STAT;
            break;
        }
    case RECEIVE_CMD_STAT:
        {
            if (receiveMessage())
            { // <- ACKNOWLEDGE |
                if (!strcmp(inputMessage, MODEM_MESSAGE_ACKNOWLEDGE))
                {
                    strncpy(viewMessage, "Notificando al servidor", sizeof(viewMessage)-1);
                    viewMessage[sizeof(viewMessage)-1] = '\0';
                    notifyEvent(UE_VIEW);
                    sendStatus = ACK_AFTER_RECEIVE_STAT;
                }
            }
            break;
        }
    case ACK_AFTER_RECEIVE_STAT:
        {
            strncpy(viewMessage, "Ruta enviada", sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
            notifyEvent(UE_VIEW);
            sendMessage(serverPath); // | PATH ->
            sendStatus = PATH_STAT;
            break;
        }
    case PATH_STAT:
        {
            if (receiveMessage())
            { //  <- ACKNOWLEDGE |
                if (!strcmp(inputMessage, MODEM_MESSAGE_ACKNOWLEDGE))
                {
                    strncpy(viewMessage, "Servidor preparándose", sizeof(viewMessage)-1);
                    viewMessage[sizeof(viewMessage)-1] = '\0';
                    notifyEvent(UE_VIEW);
                    sendStatus = ACK_AFTER_PATH_STAT;
                }
                else if (!strcmp(inputMessage, MODEM_MESSAGE_UNKNOWN))
                {
                    // impossible to receive file
                    sendStatus = IDLE_FILE_PROCESS_STAT;
                    strncpy(viewMessage, "El servidor no pudo recibir", sizeof(viewMessage)-1);
                    viewMessage[sizeof(viewMessage)-1] = '\0';
                    notifyEvent(UE_VIEW);
                }
            }
            break;
        }
    case ACK_AFTER_PATH_STAT:
        {
            STR128 fn;
            strncpy(fn, clientPath, sizeof(fn)-1);
            fn[sizeof(fn)-1] = '\0';
            strncat(fn, "\\", sizeof(fn)-1-strlen(fn));
            strncat(fn, filename, sizeof(fn)-1-strlen(fn));
            // the remote machine is ready to receive the file
            TransferMonitor monitor(this);
            GFYmodem *fileTransfer = new GFYmodem(serial, &monitor);
            fileTransfer->SetStripPathOption(GCPP_ON);
            sendResult = fileTransfer->SendFile(fn);
            if (sendResult == GCPP_OK)
            {
                strncpy(viewMessage, "Se envió ", sizeof(viewMessage)-1);
                viewMessage[sizeof(viewMessage)-1] = '\0';
                strncat(viewMessage, fn, sizeof(viewMessage)-1-strlen(viewMessage));
                notifyEvent(UE_VIEW);
            }
            else
            {
                strncpy(viewMessage, "Problemas enviando ", sizeof(viewMessage)-1);
                viewMessage[sizeof(viewMessage)-1] = '\0';
                strncat(viewMessage, fn, sizeof(viewMessage)-1-strlen(viewMessage));
                notifyEvent(UE_VIEW);
            }
            delete fileTransfer;
            //
            sendStatus = IDLE_FILE_PROCESS_STAT;
            //
            break;
        }
    }
}

void MODEM_DEVICE::processClientReceiveOperation(void)
{
    if (receiveStatus == IDLE_FILE_PROCESS_STAT)
    {
        filename[0] = '\0'; // to check for a valid filename after notify
        notifyEvent(UE_NEXT); // ask for the next filename to the client
        // we suppose a new file name after notifyEvent()
        if (strlen(filename))
        {
            // now: receive the file by using the protocol
            receiveStatus = START_STAT;
        }
    }
    else
    {
        receiveFile();
    }
}

void MODEM_DEVICE::receiveFile(void)
{
    switch (receiveStatus)
    {
    case START_STAT:
        {
            // force remote machine to clear port input
            sendMessage(MODEM_MESSAGE_UNKNOWN); // | CLEAR ->
            //
            sendMessage(MODEM_MESSAGE_COMMAND); // | COMMAND ->
            receiveStatus = COMMAND_STAT;
            break;
        }
    case COMMAND_STAT:
        {
            if (receiveMessage())
            { //  <- ACKNOWLEDGE |
                if (!strcmp(inputMessage, MODEM_MESSAGE_ACKNOWLEDGE))
                {
                    receiveStatus = ACK_AFTER_COMMAND_STAT;
                }
            }
            break;
        }
    case ACK_AFTER_COMMAND_STAT:
        {
            sendMessage(MODEM_MESSAGE_SEND); // | SEND ->
            receiveStatus = SEND_CMD_STAT;
            break;
        }
    case SEND_CMD_STAT:
        {
            if (receiveMessage())
            { // <- ACKNOWLEDGE |
                if (!strcmp(inputMessage, MODEM_MESSAGE_ACKNOWLEDGE))
                {
                    strncpy(viewMessage, "Notificando al servidor", sizeof(viewMessage)-1);
                    viewMessage[sizeof(viewMessage)-1] = '\0';
                    notifyEvent(UE_VIEW);
                    receiveStatus = ACK_AFTER_SEND_CMD_STAT;
                }
            }
            break;
        }
    case ACK_AFTER_SEND_CMD_STAT:
        {
            STR128 fn;
            strncpy(fn, serverPath, sizeof(fn)-1);
            fn[sizeof(fn)-1] = '\0';
            strncat(fn, "\\", sizeof(fn)-1-strlen(fn));
            strncat(fn, filename, sizeof(fn)-1-strlen(fn));
            strncpy(viewMessage, "Nombre enviado", sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
            notifyEvent(UE_VIEW);
            sendMessage(fn); // | FILENAME ->
            receiveStatus = FILENAME_STAT;
            break;
        }
    case FILENAME_STAT:
        {
            if (receiveMessage())
            { //  <- ACKNOWLEDGE |
                if (!strcmp(inputMessage, MODEM_MESSAGE_ACKNOWLEDGE))
                {
                    strncpy(viewMessage, "Servidor preparándose", sizeof(viewMessage)-1);
                    viewMessage[sizeof(viewMessage)-1] = '\0';
                    notifyEvent(UE_VIEW);
                    receiveStatus = ACK_AFTER_FILENAME_STAT;
                }
                else if (!strcmp(inputMessage, MODEM_MESSAGE_UNKNOWN))
                {
                    // impossible to send file
                    receiveStatus = IDLE_FILE_PROCESS_STAT;
                    strncpy(viewMessage, "El servidor no pudo enviar", sizeof(viewMessage)-1);
                    viewMessage[sizeof(viewMessage)-1] = '\0';
                    notifyEvent(UE_VIEW);
                }
            }
            break;
        }
    case ACK_AFTER_FILENAME_STAT:
        {
            // try to create or change the current dir
            char curDir[MAXPATH]; // preserve
            strncpy(curDir, "X:\\", sizeof(curDir)-1); // fill string with form of response: X:\
            curDir[sizeof(curDir)-1] = '\0';
            curDir[0] = 'A' + getdisk(); // replace X with current drive letter
            getcurdir(0, curDir+3);      // fill rest of string with current directory
            mkdir(clientPath);
            if (chdir(clientPath) != 0)
            {
                strncpy(viewMessage, "Ruta errónea", sizeof(viewMessage)-1);
                viewMessage[sizeof(viewMessage)-1] = '\0';
            }
            else
            {
                // the remote machine is ready to send the file
                TransferMonitor monitor(this);
                GFYmodem *fileTransfer = new GFYmodem(serial, &monitor);
                fileTransfer->SetStripPathOption(GCPP_ON);
                receiveResult = fileTransfer->ReceiveFile();
                if (receiveResult == GCPP_OK)
                {
                    strncpy(viewMessage, "Se recibió en ", sizeof(viewMessage)-1);
                    viewMessage[sizeof(viewMessage)-1] = '\0';
                    strncat(viewMessage, clientPath, sizeof(viewMessage)-1-strlen(viewMessage));
                }
                else
                {
                    strncpy(viewMessage, "Problemas recibiendo en ", sizeof(viewMessage)-1);
                    viewMessage[sizeof(viewMessage)-1] = '\0';
                    strncat(viewMessage, clientPath, sizeof(viewMessage)-1-strlen(viewMessage));
                }
                delete fileTransfer;
            }
            chdir(curDir); // restore
            //
            receiveStatus = IDLE_FILE_PROCESS_STAT;
            //
            notifyEvent(UE_VIEW);
            break;
        }
    }
}

void MODEM_DEVICE::processServer(void)
{
    if (!connected)
    {
        lineStatus->Status();
        if (lineStatus->RingDetected())
        {
            modem->AnswerPhone();
            strncpy(viewMessage, "Conectando ...", sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
            notifyEvent(UE_VIEW);
            if (waitFor(MODEM_INPUT_CONNECT))
            {
                strncpy(viewMessage, "Conectado", sizeof(viewMessage)-1);
                viewMessage[sizeof(viewMessage)-1] = '\0';
                connected = TRUE;
            }
            else
            {
                strncpy(viewMessage, "No se pudo contestar la llamada", sizeof(viewMessage)-1);
                viewMessage[sizeof(viewMessage)-1] = '\0';
            }
            notifyEvent(UE_VIEW);
        }
    }
    else
    {
        if (isOnLine())
        {
            switch (operation)
            {
            case IDLE_OPERATION   :
                processServerIdleOperation()   ;
                break;
            case CONSOLE_OPERATION:
                processServerConsoleOperation();
                break;
            case COMMAND_OPERATION:
                processServerCommandOperation();
                break;
            case SEND_OPERATION   :
                processServerSendOperation()   ;
                break;
            case RECEIVE_OPERATION:
                processServerReceiveOperation();
                break;
            }
        }
        else
        {
            notifyEvent(UE_CANCEL);
        }
    }
}

void MODEM_DEVICE::processServerIdleOperation(void)
{
    if (receiveMessage())
    {
        if (!strcmp(inputMessage, MODEM_MESSAGE_CONSOLE))
        {
            strncpy(viewMessage, "Consola activada", sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
            setOperation(CONSOLE_OPERATION);
            sendMessage(MODEM_MESSAGE_ACKNOWLEDGE);
        }
        else if (!strcmp(inputMessage, MODEM_MESSAGE_COMMAND))
        {
            strncpy(viewMessage, "Comandos activados", sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
            setOperation(COMMAND_OPERATION);
            sendMessage(MODEM_MESSAGE_ACKNOWLEDGE);
        }
        else
        { // it's an unknown message !!!
            //~~~strcpy(viewMessage, "Esperando mensaje del cliente...");
            strncpy(viewMessage, inputMessage, sizeof(viewMessage)-1); // to test
            viewMessage[sizeof(viewMessage)-1] = '\0';
        }

        notifyEvent(UE_VIEW);
    }
}

void MODEM_DEVICE::processServerConsoleOperation(void)
{
    if (receiveMessage())
    {
        if (!strcmp(inputMessage, MODEM_MESSAGE_COMMAND))
        {
            strncpy(viewMessage, "Comandos activados", sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
            setOperation(COMMAND_OPERATION);
            sendMessage(MODEM_MESSAGE_ACKNOWLEDGE);
        }
        else
        {
            strncpy(viewMessage, inputMessage, sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
            sendMessage(inputMessage);
        }
        notifyEvent(UE_VIEW);
    }
}

void MODEM_DEVICE::processServerCommandOperation(void)
{
    if (receiveMessage())
    {
        // wait for command
        if (!strcmp(inputMessage, MODEM_MESSAGE_CONSOLE))
        {
            strncpy(viewMessage, "Consola activada", sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
            setOperation(CONSOLE_OPERATION);
            sendMessage(MODEM_MESSAGE_ACKNOWLEDGE);
        }
        else 	if (!strcmp(inputMessage, MODEM_MESSAGE_SEND))
        {
            strncpy(viewMessage, "Enviar activado", sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
            setOperation(SEND_OPERATION);
            sendMessage(MODEM_MESSAGE_ACKNOWLEDGE);
        }
        else if (!strcmp(inputMessage, MODEM_MESSAGE_RECEIVE))
        {
            strncpy(viewMessage, "Recibir activado", sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
            setOperation(RECEIVE_OPERATION);
            sendMessage(MODEM_MESSAGE_ACKNOWLEDGE);
        }
        else if (!strcmp(inputMessage, MODEM_MESSAGE_CLOSE))
        {
            strncpy(viewMessage, "Cierre remoto", sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
            sendMessage(MODEM_MESSAGE_ACKNOWLEDGE);
            notifyEvent(UE_CLOSE);
        }
        else
        { // it's an unknown command !!!
            strncpy(viewMessage, "Comando desconocido", sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
            sendMessage(MODEM_MESSAGE_UNKNOWN);
        }
        notifyEvent(UE_VIEW);
    }
}

void MODEM_DEVICE::processServerReceiveOperation(void)
{
    if (receiveMessage())
    {
        if (!strcmp(inputMessage, MODEM_MESSAGE_CONSOLE))
        {
            strncpy(viewMessage, "Consola activada", sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
            setOperation(CONSOLE_OPERATION);
            sendMessage(MODEM_MESSAGE_ACKNOWLEDGE);
        }
        else if (!strcmp(inputMessage, MODEM_MESSAGE_COMMAND))
        {
            strncpy(viewMessage, "Comandos activados", sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
            setOperation(COMMAND_OPERATION);
            sendMessage(MODEM_MESSAGE_ACKNOWLEDGE);
        }
        else
        { // it's the path of file
            strncpy(serverPath, inputMessage, sizeof(serverPath)-1);
            serverPath[sizeof(serverPath)-1] = '\0';
            // try to create or change the current dir
            char curDir[MAXPATH]; // preserve
            strncpy(curDir, "X:\\", sizeof(curDir)-1); // fill string with form of response: X:\
            curDir[sizeof(curDir)-1] = '\0';
            curDir[0] = 'A' + getdisk(); // replace X with current drive letter
            getcurdir(0, curDir+3);      // fill rest of string with current directory
            mkdir(serverPath);
            if (chdir(serverPath) == 0)
            {
                sendMessage(MODEM_MESSAGE_ACKNOWLEDGE);
                // transfer
                TransferMonitor monitor(this);
                GFYmodem *fileTransfer = new GFYmodem(serial, &monitor);
                receiveResult = fileTransfer->ReceiveFile();
                if (receiveResult == GCPP_OK)
                {
                    strncpy(viewMessage, "Archivo recibido en ", sizeof(viewMessage)-1);
                    viewMessage[sizeof(viewMessage)-1] = '\0';
                    strncat(viewMessage, serverPath, sizeof(viewMessage)-1-strlen(viewMessage));
                }
                else
                {
                    strncpy(viewMessage, "Problemas recibiendo archivo", sizeof(viewMessage)-1);
                    viewMessage[sizeof(viewMessage)-1] = '\0';
                }
                delete fileTransfer;
            }
            else
            {
                sendMessage(MODEM_MESSAGE_UNKNOWN);
                strncpy(viewMessage, "Ruta errónea", sizeof(viewMessage)-1);
                viewMessage[sizeof(viewMessage)-1] = '\0';
            }
            chdir(curDir); // restore
            //
            setOperation(IDLE_OPERATION);
        }
        notifyEvent(UE_VIEW);
    }
}

void MODEM_DEVICE::processServerSendOperation(void)
{
    if (receiveMessage())
    {
        if (!strcmp(inputMessage, MODEM_MESSAGE_CONSOLE))
        {
            strncpy(viewMessage, "Consola activada", sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
            setOperation(CONSOLE_OPERATION);
            sendMessage(MODEM_MESSAGE_ACKNOWLEDGE);
        }
        else if (!strcmp(inputMessage, MODEM_MESSAGE_COMMAND))
        {
            strncpy(viewMessage, "Comandos activados", sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
            setOperation(COMMAND_OPERATION);
            sendMessage(MODEM_MESSAGE_ACKNOWLEDGE);
        }
        else
        { // it's the name of file
            STR128 fn;
            strncpy(fn, inputMessage, sizeof(fn)-1);
            fn[sizeof(fn)-1] = '\0';
            // split filename
            char drive[MAXDRIVE];
            char ext[MAXEXT];
            int flags;
            flags=fnsplit(fn,drive, serverPath, filename, ext);
            if(flags & EXTENSION)
            {
                strncat(filename, ext, sizeof(filename)-1-strlen(filename));
            }
            if ((access(fn, 0) == 0))
            { // exist ?
                sendMessage(MODEM_MESSAGE_ACKNOWLEDGE);
                // transfer
                TransferMonitor monitor(this);
                GFYmodem *fileTransfer = new GFYmodem(serial, &monitor);
                sendResult = fileTransfer->SendFile(fn);
                if (sendResult == GCPP_OK)
                {
                    strncpy(viewMessage, "enviando archivo ", sizeof(viewMessage)-1);
                    viewMessage[sizeof(viewMessage)-1] = '\0';
                    strncat(viewMessage, fn, sizeof(viewMessage)-1-strlen(viewMessage));
                }
                else
                {
                    strncpy(viewMessage, "Problemas enviando archivo", sizeof(viewMessage)-1);
                    viewMessage[sizeof(viewMessage)-1] = '\0';
                }
                delete fileTransfer;
            }
            else
            {
                sendMessage(MODEM_MESSAGE_UNKNOWN);
                strncpy(viewMessage, "Archivo erróneo", sizeof(viewMessage)-1);
                viewMessage[sizeof(viewMessage)-1] = '\0';
            }
            //
            setOperation(IDLE_OPERATION);
        }
        notifyEvent(UE_VIEW);
    }
}

EVENT_TYPE MODEM_DEVICE::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case E_DEVICE:
    case E_MODEM_DEVICE:
        {
            // Turn the device on or off.
            switch (event.rawCode)
            {
            case D_OFF:
            case D_ON:
                {
                    state = event.rawCode;
                    break;
                }
            }
        }
    }
    // Return the control code.
    return (ccode);
}

BOOL MODEM_DEVICE::activate()
{
    strncpy(viewMessage, "Activando ...", sizeof(viewMessage)-1);
    viewMessage[sizeof(viewMessage)-1] = '\0';
    notifyEvent(UE_VIEW);
    //
    active = FALSE;
    // open SIO
	sio = new GFI8250(g_cfg->MODEM_COM, g_cfg->MODEM_BASE, g_cfg->MODEM_IRQ);
    // open serial;
    serial = new GFSerial(sio);
    unsigned short error;
    if (serial->GetCommError(error) != GCPP_OK)
    {
        strncpy(viewMessage, "No se pudo instalar puerto serial", sizeof(viewMessage)-1);
        viewMessage[sizeof(viewMessage)-1] = '\0';
    }
    else
    {
        // init line status
        lineStatus = new GFLineStatus(serial);
        // delay for putchar
		serial->Delay(g_cfg->MODEM_RECEIVESENDDELAY); // v.220
        // set data format
        serialDataFmt = new GFDataFormat(serial);
		serialDataFmt->SetUp(g_cfg->MODEM_BAUDS, 'N', 8, 1);
        // open modem
        modem = new GFModem;
        //
		modem->FixedDelay(g_cfg->MODEM_DELAY);
        modem->Install(serial);
        if (!waitFor(MODEM_INPUT_OK))
        {
            strncpy(viewMessage, "No se pudo instalar modem", sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
        }
        else
        {
            modem->Reset();
            if (!waitFor(MODEM_INPUT_OK))
            {
                strncpy(viewMessage, "No se pudo iniciar modem", sizeof(viewMessage)-1);
                viewMessage[sizeof(viewMessage)-1] = '\0';
            }
            else
            {
				modem->SetSpeakerLevel(g_cfg->MODEM_SPEAKER);
                strncpy(viewMessage, "Activado", sizeof(viewMessage)-1);
                viewMessage[sizeof(viewMessage)-1] = '\0';
                active = TRUE;
            }
        }
    }
    notifyEvent(UE_VIEW);
    return active;
}

BOOL MODEM_DEVICE::deactivate()
{
    strncpy(viewMessage, "Desactivando ...", sizeof(viewMessage)-1);
    viewMessage[sizeof(viewMessage)-1] = '\0';
    notifyEvent(UE_VIEW);
    if (modem)
    {
        delete modem;
        modem = NULL;
    }
    if (lineStatus)
    {
        delete lineStatus;
        lineStatus = NULL;
    }
    if (serialDataFmt)
    {
        delete serialDataFmt;
        serialDataFmt = NULL;
    }
    if (serial)
    {
        delete serial;
        serial = NULL;
    }
    if (sio)
    {
        delete sio;
        sio = NULL;
    }
    active = FALSE;
    strncpy(viewMessage, "Desactivado", sizeof(viewMessage)-1);
    viewMessage[sizeof(viewMessage)-1] = '\0';
    notifyEvent(UE_VIEW);
    return TRUE;
}

BOOL MODEM_DEVICE::connect(PHONE phone)
{
    strncpy(viewMessage, "Marcando ...", sizeof(viewMessage)-1);
    viewMessage[sizeof(viewMessage)-1] = '\0';
    notifyEvent(UE_VIEW);
    connected = FALSE;
	modem->DialMode(g_cfg->MODEM_DIAL);
    modem->Dial(phone);
    strncpy(viewMessage, "Conectando...", sizeof(viewMessage)-1);
    viewMessage[sizeof(viewMessage)-1] = '\0';
    notifyEvent(UE_VIEW);
    if (!waitFor(MODEM_INPUT_CONNECT))
    {
        strncpy(viewMessage, "No se pudo conectar", sizeof(viewMessage)-1);
        viewMessage[sizeof(viewMessage)-1] = '\0';
    }
    else
    {
        strncpy(viewMessage, "Conectado", sizeof(viewMessage)-1);
        viewMessage[sizeof(viewMessage)-1] = '\0';
        connected = TRUE;
    }
    notifyEvent(UE_VIEW);
    return connected;
}

BOOL MODEM_DEVICE::disconnect()
{
    strncpy(viewMessage, "Desconectando ...", sizeof(viewMessage)-1);
    viewMessage[sizeof(viewMessage)-1] = '\0';
    notifyEvent(UE_VIEW);
    if (modem)
        modem->HangUp();
    connected = FALSE;
    strncpy(viewMessage, "Desconectado", sizeof(viewMessage)-1);
    viewMessage[sizeof(viewMessage)-1] = '\0';
    notifyEvent(UE_VIEW);
    return TRUE;
}

BOOL MODEM_DEVICE::waitFor(const char *match)
{
    char input[81];
    int len = 80;
	long msecs = g_cfg->MODEM_MAXTIME;
    while (msecs)
    {
        msecs = modem->GetModemInput(input, len, msecs);
        if(strstr(input, match) != 0 )
            return TRUE;
    }
    return FALSE;
}

char *MODEM_DEVICE::getInput(char *input, int len)
{
	long msecs = g_cfg->MODEM_MAXTIME;
    while (msecs)
    {
        msecs = modem->GetModemInput(input, len, msecs);
    }
    return input;
}

BOOL MODEM_DEVICE::isOnLine(void)
{
    // check Carrier Detect
    lineStatus->Status();
    if (lineStatus->CarrierChanged())
    {
        if (lineStatus->Carrier() == GCPP_OFF)
        {
            strncpy(viewMessage, "Conectado", sizeof(viewMessage)-1);
            viewMessage[sizeof(viewMessage)-1] = '\0';
            connected = FALSE;
        }
    }
    return connected;
}

BOOL MODEM_DEVICE::sendMessage(const char *msg)
{
    BOOL ok = FALSE;
    if (connected)
    {
        ok = serial->PutString(msg, -2) == (strlen(msg)+1); // add '\n'
    }

    return ok;
}

int MODEM_DEVICE::receiveMessage(void)
{
    int n = 0;
    if (connected)
    {
        if (serial->RXCount())
        {
            static inCount = 0;
            int c = serial->GetChar(g_cfg->MODEM_RECEIVESENDDELAY); // !!!
            if (c != GCPP_TIMEOUT)
            {
                if (inCount < MESSAGE_LEN)
                {
                    inputMessage[inCount] = c;
                    if (c == '\n')
                    {
                        inputMessage[inCount] = '\0';
                        n = inCount;
                        // reset count
                        inCount = 0;
                    }
                    else
                    {
                        inCount++;
                    }
                }
            }
        }
    }
    return n;
}

BOOL MODEM_DEVICE::setFile(const char *clientPath, const char *serverPath, const char *filename)
{
    strncpy(this->clientPath, clientPath, sizeof(this->clientPath)-1);
    this->clientPath[sizeof(this->clientPath)-1] = '\0';
    strncpy(this->serverPath, serverPath, sizeof(this->serverPath)-1);
    this->serverPath[sizeof(this->serverPath)-1] = '\0';
    strncpy(this->filename, filename, sizeof(this->filename)-1);
    this->filename[sizeof(this->filename)-1] = '\0';
    //
    sendStatus = IDLE_FILE_PROCESS_STAT;
    return TRUE;
}

void MODEM_DEVICE::notifyEvent(USER_EVENT event)
{
    if (window)
    {
        window->Event(UI_EVENT(event, 0));
    }
}