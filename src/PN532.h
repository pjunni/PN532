/**************************************************************************/
/*!
    @file     PN532/PN532/PN532.h
    @author   Adafruit Industries & Seeed Studio
    @license  BSD
*/
/**************************************************************************/

#ifndef __PN532_H__
#define __PN532_H__

#include <stdint.h>
#include "PN532Interface.h"
#include "Buffer.h"
#include "PN532.h"
#include "DES.h"
#include "AES128.h"

// DESFIRE CONTENT STARTS HERE

//#include "DesFireKey.h"
// Just an invalid key number
#define NOT_AUTHENTICATED      255

#define MAX_FRAME_SIZE         60 // The maximum total length of a packet that is transfered to / from the card

// ------- Desfire legacy instructions --------

#define DF_INS_AUTHENTICATE_LEGACY        0x0A
#define DF_INS_CHANGE_KEY_SETTINGS        0x54
#define DF_INS_GET_KEY_SETTINGS           0x45
#define DF_INS_CHANGE_KEY                 0xC4
#define DF_INS_GET_KEY_VERSION            0x64

#define DF_INS_CREATE_APPLICATION         0xCA
#define DF_INS_DELETE_APPLICATION         0xDA
#define DF_INS_GET_APPLICATION_IDS        0x6A
#define DF_INS_SELECT_APPLICATION         0x5A

#define DF_INS_FORMAT_PICC                0xFC
#define DF_INS_GET_VERSION                0x60

#define DF_INS_GET_FILE_IDS               0x6F
#define DF_INS_GET_FILE_SETTINGS          0xF5
#define DF_INS_CHANGE_FILE_SETTINGS       0x5F
#define DF_INS_CREATE_STD_DATA_FILE       0xCD
#define DF_INS_CREATE_BACKUP_DATA_FILE    0xCB
#define DF_INS_CREATE_VALUE_FILE          0xCC
#define DF_INS_CREATE_LINEAR_RECORD_FILE  0xC1
#define DF_INS_CREATE_CYCLIC_RECORD_FILE  0xC0
#define DF_INS_DELETE_FILE                0xDF

#define DF_INS_READ_DATA                  0xBD
#define DF_INS_WRITE_DATA                 0x3D
#define DF_INS_GET_VALUE                  0x6C
#define DF_INS_CREDIT                     0x0C
#define DF_INS_DEBIT                      0xDC
#define DF_INS_LIMITED_CREDIT             0x1C
#define DF_INS_WRITE_RECORD               0x3B
#define DF_INS_READ_RECORDS               0xBB
#define DF_INS_CLEAR_RECORD_FILE          0xEB
#define DF_COMMIT_TRANSACTION             0xC7
#define DF_INS_ABORT_TRANSACTION          0xA7

#define DF_INS_ADDITIONAL_FRAME           0xAF // data did not fit into a frame, another frame will follow

// -------- Desfire EV1 instructions ----------

#define DFEV1_INS_AUTHENTICATE_ISO        0x1A
#define DFEV1_INS_AUTHENTICATE_AES        0xAA
#define DFEV1_INS_FREE_MEM                0x6E
#define DFEV1_INS_GET_DF_NAMES            0x6D
#define DFEV1_INS_GET_CARD_UID            0x51
#define DFEV1_INS_GET_ISO_FILE_IDS        0x61
#define DFEV1_INS_SET_CONFIGURATION       0x5C

// ---------- ISO7816 instructions ------------

#define ISO7816_INS_EXTERNAL_AUTHENTICATE 0x82
#define ISO7816_INS_INTERNAL_AUTHENTICATE 0x88
#define ISO7816_INS_APPEND_RECORD         0xE2
#define ISO7816_INS_GET_CHALLENGE         0x84
#define ISO7816_INS_READ_RECORDS          0xB2
#define ISO7816_INS_SELECT_FILE           0xA4
#define ISO7816_INS_READ_BINARY           0xB0
#define ISO7816_INS_UPDATE_BINARY         0xD6


// Status codes (errors) returned from Desfire card
enum DESFireStatus
{
    ST_Success               = 0x00,
    ST_NoChanges             = 0x0C,
    ST_OutOfMemory           = 0x0E,
    ST_IllegalCommand        = 0x1C,
    ST_IntegrityError        = 0x1E,
    ST_KeyDoesNotExist       = 0x40,
    ST_WrongCommandLen       = 0x7E,
    ST_PermissionDenied      = 0x9D,
    ST_IncorrectParam        = 0x9E,
    ST_AppNotFound           = 0xA0,
    ST_AppIntegrityError     = 0xA1,
    ST_AuthentError          = 0xAE,
    ST_MoreFrames            = 0xAF, // data did not fit into a frame, another frame will follow
    ST_LimitExceeded         = 0xBE,
    ST_CardIntegrityError    = 0xC1,
    ST_CommandAborted        = 0xCA,
    ST_CardDisabled          = 0xCD,
    ST_InvalidApp            = 0xCE,
    ST_DuplicateAidFiles     = 0xDE,
    ST_EepromError           = 0xEE,
    ST_FileNotFound          = 0xF0,
    ST_FileIntegrityError    = 0xF1,
};

enum PICC_Type : byte {
    PICC_TYPE_UNKNOWN		,
    PICC_TYPE_ISO_14443_4	,	// PICC compliant with ISO/IEC 14443-4
    PICC_TYPE_ISO_18092		, 	// PICC compliant with ISO/IEC 18092 (NFC)
    PICC_TYPE_MIFARE_MINI	,	// MIFARE Classic protocol, 320 bytes
    PICC_TYPE_MIFARE_1K		,	// MIFARE Classic protocol, 1KB
    PICC_TYPE_MIFARE_4K		,	// MIFARE Classic protocol, 4KB
    PICC_TYPE_MIFARE_UL		,	// MIFARE Ultralight or Ultralight C
    PICC_TYPE_MIFARE_PLUS	,	// MIFARE Plus
    PICC_TYPE_MIFARE_DESFIRE,	// MIFARE DESFire
    PICC_TYPE_TNP3XXX		,	// Only mentioned in NXP AN 10833 MIFARE Type Identification Procedure
    PICC_TYPE_NOT_COMPLETE	= 0xff	// SAK indicates UID is not complete.
};

// Card information about software and hardware version.
struct DESFireCardVersion
{
    byte hardwareVendorId;    // The hardware vendor
    byte hardwareType;        // The hardware type
    byte hardwareSubType;     // The hardware subtype
    byte hardwareMajVersion;  // The hardware major version
    byte hardwareMinVersion;  // The hardware minor version
    byte hardwareStorageSize; // The hardware storage size
    byte hardwareProtocol;    // The hardware protocol

    byte softwareVendorId;    // The software vendor
    byte softwareType;        // The software type
    byte softwareSubType;     // The software subtype
    byte softwareMajVersion;  // The software major version
    byte softwareMinVersion;  // The software minor version
    byte softwareStorageSize; // The software storage size
    byte softwareProtocol;    // The software protocol

    byte uid[7];              // The serial card number
    byte batchNo[5];          // The batch number
    byte cwProd;              // The production week (BCD)
    byte yearProd;            // The production year (BCD)
};

// MK = Application Master Key or PICC Master Key
enum DESFireKeySettings
{
    // ------------ BITS 0-3 ---------------
    KS_ALLOW_CHANGE_MK                = 0x01, // If this bit is set, the MK can be changed, otherwise it is frozen.
    KS_LISTING_WITHOUT_MK             = 0x02, // Picc key: If this bit is set, GetApplicationIDs, GetKeySettings do not require MK authentication.
    // App  key: If this bit is set, GetFileIDs, GetFileSettings, GetKeySettings do not require MK authentication.
    KS_CREATE_DELETE_WITHOUT_MK       = 0x04, // Picc key: If this bit is set, CreateApplication does not require MK authentication.
    // App  key: If this bit is set, CreateFile, DeleteFile do not require MK authentication.
    KS_CONFIGURATION_CHANGEABLE       = 0x08, // If this bit is set, the configuration settings of the MK can be changed, otherwise they are frozen.

    // ------------ BITS 4-7 (not used for the PICC master key) -------------
    KS_CHANGE_KEY_WITH_MK             = 0x00, // A key change requires MK authentication
    KS_CHANGE_KEY_WITH_KEY_1          = 0x10, // A key change requires authentication with key 1
    KS_CHANGE_KEY_WITH_KEY_2          = 0x20, // A key change requires authentication with key 2
    KS_CHANGE_KEY_WITH_KEY_3          = 0x30, // A key change requires authentication with key 3
    KS_CHANGE_KEY_WITH_KEY_4          = 0x40, // A key change requires authentication with key 4
    KS_CHANGE_KEY_WITH_KEY_5          = 0x50, // A key change requires authentication with key 5
    KS_CHANGE_KEY_WITH_KEY_6          = 0x60, // A key change requires authentication with key 6
    KS_CHANGE_KEY_WITH_KEY_7          = 0x70, // A key change requires authentication with key 7
    KS_CHANGE_KEY_WITH_KEY_8          = 0x80, // A key change requires authentication with key 8
    KS_CHANGE_KEY_WITH_KEY_9          = 0x90, // A key change requires authentication with key 9
    KS_CHANGE_KEY_WITH_KEY_A          = 0xA0, // A key change requires authentication with key 10
    KS_CHANGE_KEY_WITH_KEY_B          = 0xB0, // A key change requires authentication with key 11
    KS_CHANGE_KEY_WITH_KEY_C          = 0xC0, // A key change requires authentication with key 12
    KS_CHANGE_KEY_WITH_KEY_D          = 0xD0, // A key change requires authentication with key 13
    KS_CHANGE_KEY_WITH_TARGETED_KEY   = 0xE0, // A key change requires authentication with the same key that is to be changed
    KS_CHANGE_KEY_FROZEN              = 0xF0, // All keys are frozen

    // -------------------------------------
    KS_FACTORY_DEFAULT                = 0x0F,
};

enum DESFireAccessRights
{
    AR_KEY0  = 0x00, // Authentication with application key 0 required (master key)
    AR_KEY1  = 0x01, // Authentication with application key 1 required
    AR_KEY2  = 0x02, // ...
    AR_KEY3  = 0x03,
    AR_KEY4  = 0x04,
    AR_KEY5  = 0x05,
    AR_KEY6  = 0x06,
    AR_KEY7  = 0x07,
    AR_KEY8  = 0x08,
    AR_KEY9  = 0x09,
    AR_KEY10 = 0x0A,
    AR_KEY11 = 0x0B,
    AR_KEY12 = 0x0C,
    AR_KEY13 = 0x0D,
    AR_FREE  = 0x0E, // Always allowed even without authentication
    AR_NEVER = 0x0F  // Always forbidden even with authentication
};

struct DESFireFilePermissions
{
    DESFireAccessRights  e_ReadAccess;
    DESFireAccessRights  e_WriteAccess;
    DESFireAccessRights  e_ReadAndWriteAccess;
    DESFireAccessRights  e_ChangeAccess;

    uint16_t Pack()
    {
        return (e_ReadAccess << 12) | (e_WriteAccess <<  8) | (e_ReadAndWriteAccess <<  4) | e_ChangeAccess;
    }
    void Unpack(uint16_t u16_Data)
    {
        e_ReadAccess         = (DESFireAccessRights)((u16_Data >> 12) & 0x0F);
        e_WriteAccess        = (DESFireAccessRights)((u16_Data >>  8) & 0x0F);
        e_ReadAndWriteAccess = (DESFireAccessRights)((u16_Data >>  4) & 0x0F);
        e_ChangeAccess       = (DESFireAccessRights)((u16_Data      ) & 0x0F);
    }
};

// Defines if data transmitted to files is encrypted (with the session key) or secured with a MAC
enum DESFireFileEncryption
{
    CM_PLAIN   = 0x00,
    CM_MAC     = 0x01,   // not implemented (Plain data transfer with additional MAC)
    CM_ENCRYPT = 0x03,   // not implemented (Does not make data stored on the card more secure. Only encrypts the transfer between Teensy and the card)
};

enum DESFireFileType
{
    MDFT_STANDARD_DATA_FILE             = 0x00,
    MDFT_BACKUP_DATA_FILE               = 0x01, // not implemented
    MDFT_VALUE_FILE_WITH_BACKUP         = 0x02, // not implemented
    MDFT_LINEAR_RECORD_FILE_WITH_BACKUP = 0x03, // not implemented
    MDFT_CYCLIC_RECORD_FILE_WITH_BACKUP = 0x04, // not implemented
};

struct DESFireFileSettings
{
    DESFireFileType        e_FileType;
    DESFireFileEncryption  e_Encrypt;
    DESFireFilePermissions k_Permis;
    // -----------------------------
    // used only for MDFT_STANDARD_DATA_FILE and MDFT_BACKUP_DATA_FILE
    uint32_t u32_FileSize;
    // -----------------------------
    // used only for MDFT_VALUE_FILE_WITH_BACKUP
    uint32_t  u32_LowerLimit;
    uint32_t  u32_UpperLimit;
    uint32_t  u32_LimitedCreditValue;
    bool      b_LimitedCreditEnabled;
    // -----------------------------
    // used only for MDFT_LINEAR_RECORD_FILE_WITH_BACKUP and MDFT_CYCLIC_RECORD_FILE_WITH_BACKUP
    uint32_t  u32_RecordSize;
    uint32_t  u32_MaxNumberRecords;
    uint32_t  u32_CurrentNumberRecords;
};

enum DESFireCmac
{
    MAC_None   = 0,
    // Transmit data:
    MAC_Tmac   = 1, // The CMAC must be calculated for the TX data sent to the card although this Tx CMAC is not transmitted
    MAC_Tcrypt = 2, // To the parameters sent to the card a CRC32 must be appended and then they must be encrypted with the session key
    // Receive data:
    MAC_Rmac   = 4, // The CMAC must be calculated for the RX data received from the card. If status == ST_Success -> verify the CMAC in the response
    MAC_Rcrypt = 8, // The data received from the card must be decrypted with the session key
    // Combined:
    MAC_TmacRmac   = MAC_Tmac   | MAC_Rmac,
    MAC_TmacRcrypt = MAC_Tmac   | MAC_Rcrypt,
    MAC_TcryptRmac = MAC_Tcrypt | MAC_Rmac,
};

// DESFIRE CONTENT ENDS HERE

// PN532 Commands
#define PN532_COMMAND_DIAGNOSE              (0x00)
#define PN532_COMMAND_GETFIRMWAREVERSION    (0x02)
#define PN532_COMMAND_GETGENERALSTATUS      (0x04)
#define PN532_COMMAND_READREGISTER          (0x06)
#define PN532_COMMAND_WRITEREGISTER         (0x08)
#define PN532_COMMAND_READGPIO              (0x0C)
#define PN532_COMMAND_WRITEGPIO             (0x0E)
#define PN532_COMMAND_SETSERIALBAUDRATE     (0x10)
#define PN532_COMMAND_SETPARAMETERS         (0x12)
#define PN532_COMMAND_SAMCONFIGURATION      (0x14)
#define PN532_COMMAND_POWERDOWN             (0x16)
#define PN532_COMMAND_RFCONFIGURATION       (0x32)
#define PN532_COMMAND_RFREGULATIONTEST      (0x58)
#define PN532_COMMAND_INJUMPFORDEP          (0x56)
#define PN532_COMMAND_INJUMPFORPSL          (0x46)
#define PN532_COMMAND_INLISTPASSIVETARGET   (0x4A)
#define PN532_COMMAND_INATR                 (0x50)
#define PN532_COMMAND_INPSL                 (0x4E)
#define PN532_COMMAND_INDATAEXCHANGE        (0x40)
#define PN532_COMMAND_INCOMMUNICATETHRU     (0x42)
#define PN532_COMMAND_INDESELECT            (0x44)
#define PN532_COMMAND_INRELEASE             (0x52)
#define PN532_COMMAND_INSELECT              (0x54)
#define PN532_COMMAND_INAUTOPOLL            (0x60)
#define PN532_COMMAND_TGINITASTARGET        (0x8C)
#define PN532_COMMAND_TGSETGENERALBYTES     (0x92)
#define PN532_COMMAND_TGGETDATA             (0x86)
#define PN532_COMMAND_TGSETDATA             (0x8E)
#define PN532_COMMAND_TGSETMETADATA         (0x94)
#define PN532_COMMAND_TGGETINITIATORCOMMAND (0x88)
#define PN532_COMMAND_TGRESPONSETOINITIATOR (0x90)
#define PN532_COMMAND_TGGETTARGETSTATUS     (0x8A)

#define PN532_RESPONSE_INDATAEXCHANGE       (0x41)
#define PN532_RESPONSE_INLISTPASSIVETARGET  (0x4B)


#define PN532_MIFARE_ISO14443A              (0x00)

// Mifare Commands
#define MIFARE_CMD_AUTH_A                   (0x60)
#define MIFARE_CMD_AUTH_B                   (0x61)
#define MIFARE_CMD_READ                     (0x30)
#define MIFARE_CMD_WRITE                    (0xA0)
#define MIFARE_CMD_WRITE_ULTRALIGHT         (0xA2)
#define MIFARE_CMD_TRANSFER                 (0xB0)
#define MIFARE_CMD_DECREMENT                (0xC0)
#define MIFARE_CMD_INCREMENT                (0xC1)
#define MIFARE_CMD_STORE                    (0xC2)

// FeliCa Commands
#define FELICA_CMD_POLLING                  (0x00)
#define FELICA_CMD_REQUEST_SERVICE          (0x02)
#define FELICA_CMD_REQUEST_RESPONSE         (0x04)
#define FELICA_CMD_READ_WITHOUT_ENCRYPTION  (0x06)
#define FELICA_CMD_WRITE_WITHOUT_ENCRYPTION (0x08)
#define FELICA_CMD_REQUEST_SYSTEM_CODE      (0x0C)

// Prefixes for NDEF Records (to identify record type)
#define NDEF_URIPREFIX_NONE                 (0x00)
#define NDEF_URIPREFIX_HTTP_WWWDOT          (0x01)
#define NDEF_URIPREFIX_HTTPS_WWWDOT         (0x02)
#define NDEF_URIPREFIX_HTTP                 (0x03)
#define NDEF_URIPREFIX_HTTPS                (0x04)
#define NDEF_URIPREFIX_TEL                  (0x05)
#define NDEF_URIPREFIX_MAILTO               (0x06)
#define NDEF_URIPREFIX_FTP_ANONAT           (0x07)
#define NDEF_URIPREFIX_FTP_FTPDOT           (0x08)
#define NDEF_URIPREFIX_FTPS                 (0x09)
#define NDEF_URIPREFIX_SFTP                 (0x0A)
#define NDEF_URIPREFIX_SMB                  (0x0B)
#define NDEF_URIPREFIX_NFS                  (0x0C)
#define NDEF_URIPREFIX_FTP                  (0x0D)
#define NDEF_URIPREFIX_DAV                  (0x0E)
#define NDEF_URIPREFIX_NEWS                 (0x0F)
#define NDEF_URIPREFIX_TELNET               (0x10)
#define NDEF_URIPREFIX_IMAP                 (0x11)
#define NDEF_URIPREFIX_RTSP                 (0x12)
#define NDEF_URIPREFIX_URN                  (0x13)
#define NDEF_URIPREFIX_POP                  (0x14)
#define NDEF_URIPREFIX_SIP                  (0x15)
#define NDEF_URIPREFIX_SIPS                 (0x16)
#define NDEF_URIPREFIX_TFTP                 (0x17)
#define NDEF_URIPREFIX_BTSPP                (0x18)
#define NDEF_URIPREFIX_BTL2CAP              (0x19)
#define NDEF_URIPREFIX_BTGOEP               (0x1A)
#define NDEF_URIPREFIX_TCPOBEX              (0x1B)
#define NDEF_URIPREFIX_IRDAOBEX             (0x1C)
#define NDEF_URIPREFIX_FILE                 (0x1D)
#define NDEF_URIPREFIX_URN_EPC_ID           (0x1E)
#define NDEF_URIPREFIX_URN_EPC_TAG          (0x1F)
#define NDEF_URIPREFIX_URN_EPC_PAT          (0x20)
#define NDEF_URIPREFIX_URN_EPC_RAW          (0x21)
#define NDEF_URIPREFIX_URN_EPC              (0x22)
#define NDEF_URIPREFIX_URN_NFC              (0x23)

#define PN532_GPIO_VALIDATIONBIT            (0x80)
#define PN532_GPIO_P30                      (0)
#define PN532_GPIO_P31                      (1)
#define PN532_GPIO_P32                      (2)
#define PN532_GPIO_P33                      (3)
#define PN532_GPIO_P34                      (4)
#define PN532_GPIO_P35                      (5)

#define CARD_TYPE_106KB_ISO14443A           (0x00) // card baudrate 106 kB
#define CARD_TYPE_212KB_FELICA              (0x01) // card baudrate 212 kB
#define CARD_TYPE_424KB_FELICA              (0x02) // card baudrate 424 kB
#define CARD_TYPE_106KB_ISO14443B           (0x03) // card baudrate 106 kB
#define CARD_TYPE_106KB_JEWEL               (0x04) // card baudrate 106 kB

// FeliCa consts
#define FELICA_READ_MAX_SERVICE_NUM         16
#define FELICA_READ_MAX_BLOCK_NUM           12 // for typical FeliCa card
#define FELICA_WRITE_MAX_SERVICE_NUM        16
#define FELICA_WRITE_MAX_BLOCK_NUM          10 // for typical FeliCa card
#define FELICA_REQ_SERVICE_MAX_NODE_NUM     32

#define PN532_PACKBUFFSIZE                  80

enum eCardType
{
    CARD_Unknown   = 0, // Mifare Classic or other card
    CARD_Desfire   = 1, // A Desfire card with normal 7 byte UID  (bit 0)
    CARD_DesRandom = 3, // A Desfire card with 4 byte random UID  (bit 0 + 1)
};

class PN532
{
public:
    PN532(PN532Interface &interface);

    void begin(void);

    // Generic PN532 functions
    bool SAMConfig(void);
    uint32_t getFirmwareVersion(void);
    uint32_t readRegister(uint16_t reg);
    uint32_t writeRegister(uint16_t reg, uint8_t val);
    bool writeGPIO(uint8_t pinstate);
    uint8_t readGPIO(void);
    bool setPassiveActivationRetries(uint8_t maxRetries);
    bool setRFField(uint8_t autoRFCA, uint8_t rFOnOff);
    bool powerDownMode();

    /**
    * @brief    Init PN532 as a target
    * @param    timeout max time to wait, 0 means no timeout
    * @return   > 0     success
    *           = 0     timeout
    *           < 0     failed
    */
    int8_t tgInitAsTarget(uint16_t timeout = 0);
    int8_t tgInitAsTarget(const uint8_t* command, const uint8_t len, const uint16_t timeout = 0);

    int16_t tgGetData(uint8_t *buf, uint8_t len);
    bool tgSetData(const uint8_t *header, uint8_t hlen, const uint8_t *body = 0, uint8_t blen = 0);

    int16_t inRelease(const uint8_t relevantTarget = 0);
    int16_t inSelectCard(const uint8_t relevantTarget = 0);
    int16_t inDeselectCard(const uint8_t relevantTarget = 0);

    // ISO14443A functions
    bool inListPassiveTarget();
    bool readPassiveTargetID(uint8_t cardbaudrate, uint8_t *uid, uint8_t *uidLength, uint16_t timeout = 1000, bool inlist = false);
    bool ReadPassiveTargetID(byte* uidBuffer, byte* uidLength, eCardType* pe_CardType);
    bool inDataExchange(uint8_t *send, uint8_t sendLength, uint8_t *response, uint8_t *responseLength);

    // NEW functions
    bool SendCommandCheckAck(byte *cmd, byte cmdlen);
    void WriteCommand(byte* cmd, byte cmdlen);
    void SendPacket(byte* buff, byte len);
    bool ReadAck();
    byte ReadData(byte* buff, byte len);
    bool ReadPacket(byte* buff, byte len);
    bool WaitReady();
    bool IsReady();

    // DesFire functions
    bool GetCardVersion(DESFireCardVersion* pk_Version);
    bool FormatCard();
    bool EnableRandomIDForever();
    bool GetRealCardID(uint8_t u8_UID[7]);
    bool GetFreeMemory(uint32_t* pu32_Memory);
    // ---------------------
    bool Authenticate (byte u8_KeyNo, DESFireKey* pi_Key);
    bool ChangeKey    (byte u8_KeyNo, DESFireKey* pi_NewKey, DESFireKey* pi_CurKey);
    bool GetKeyVersion(byte u8_KeyNo, byte* pu8_Version);
    bool GetKeySettings   (DESFireKeySettings* pe_Settg, byte* pu8_KeyCount, DESFireKeyType* pe_KeyType);
    bool ChangeKeySettings(DESFireKeySettings e_NewSettg);
    // ---------------------
    bool GetApplicationIDs(uint32_t u32_IDlist[28], byte* pu8_AppCount);
    bool CreateApplication(uint32_t u32_AppID, DESFireKeySettings e_Settg, byte u8_KeyCount, DESFireKeyType e_KeyType);
    bool SelectApplication(uint32_t u32_AppID);
    bool DeleteApplication(uint32_t u32_AppID);
    bool DeleteApplicationIfExists(uint32_t u32_AppID);
    // ---------------------
    bool GetFileIDs       (byte* u8_FileIDs, byte* pu8_FileCount);
    bool GetFileSettings  (byte u8_FileID, DESFireFileSettings* pk_Settings);
    bool DeleteFile       (byte u8_FileID);
    bool CreateStdDataFile(byte u8_FileID, DESFireFilePermissions* pk_Permis, int s32_FileSize);
    bool ReadFileData     (byte u8_FileID, int s32_Offset, int s32_Length, byte* u8_DataBuffer);
    bool WriteFileData    (byte u8_FileID, int s32_Offset, int s32_Length, const byte* u8_DataBuffer);
    bool ReadFileValue    (byte u8_FileID, uint32_t* pu32_Value);
    // ---------------------
    bool SwitchOffRfField();  // overrides PN532::SwitchOffRfField()
    bool SAFE_TEST();
    bool Selftest();
    byte GetLastPN532Error(); // See comment for this function in CPP file

    // Mifare Classic functions
    bool mifareclassic_IsFirstBlock (uint32_t uiBlock);
    bool mifareclassic_IsTrailerBlock (uint32_t uiBlock);
    uint8_t mifareclassic_AuthenticateBlock (uint8_t *uid, uint8_t uidLen, uint32_t blockNumber, uint8_t keyNumber, uint8_t *keyData);
    uint8_t mifareclassic_ReadDataBlock (uint8_t blockNumber, uint8_t *data);
    uint8_t mifareclassic_WriteDataBlock (uint8_t blockNumber, uint8_t *data);
    uint8_t mifareclassic_FormatNDEF (void);
    uint8_t mifareclassic_WriteNDEFURI (uint8_t sectorNumber, uint8_t uriIdentifier, const char *url);

    // Mifare Ultralight functions
    uint8_t mifareultralight_ReadPage (uint8_t page, uint8_t *buffer);
    uint8_t mifareultralight_WritePage (uint8_t page, uint8_t *buffer);

    // FeliCa Functions
    int8_t felica_Polling(uint16_t systemCode, uint8_t requestCode, uint8_t *idm, uint8_t *pmm, uint16_t *systemCodeResponse, uint16_t timeout=1000);
    int8_t felica_SendCommand (const uint8_t * command, uint8_t commandlength, uint8_t * response, uint8_t * responseLength);
    int8_t felica_RequestService(uint8_t numNode, uint16_t *nodeCodeList, uint16_t *keyVersions) ;
    int8_t felica_RequestResponse(uint8_t *mode);
    int8_t felica_ReadWithoutEncryption (uint8_t numService, const uint16_t *serviceCodeList, uint8_t numBlock, const uint16_t *blockList, uint8_t blockData[][16]);
    int8_t felica_WriteWithoutEncryption (uint8_t numService, const uint16_t *serviceCodeList, uint8_t numBlock, const uint16_t *blockList, uint8_t blockData[][16]);
    int8_t felica_RequestSystemCode(uint8_t *numSystemCode, uint16_t *systemCodeList);
    int8_t felica_Release();

    // Help functions to display formatted text
    static void PrintHex(const uint8_t *data, const uint32_t numBytes);
    static void PrintHexChar(const uint8_t *pbtData, const uint32_t numBytes);

    uint8_t *getBuffer(uint8_t *len) {
        *len = sizeof(pn532_packetbuffer) - 4;
        return pn532_packetbuffer;
    };

    DES  DES2_DEFAULT_KEY; // 2K3DES key with  8 zeroes {00,00,00,00,00,00,00,00}
    DES  DES3_DEFAULT_KEY; // 3K3DES key with 24 zeroes
    AES   AES_DEFAULT_KEY; // AES    key with 16 zeroes

private:
    int  DataExchange(uint8_t      u8_Command, TxBuffer* pi_Params, uint8_t* u8_RecvBuf, int s32_RecvSize, DESFireStatus* pe_Status, DESFireCmac e_Mac);
    int  DataExchange(TxBuffer* pi_Command, TxBuffer* pi_Params, uint8_t* u8_RecvBuf, int s32_RecvSize, DESFireStatus* pe_Status, DESFireCmac e_Mac);
    int  DataExchangeOLD(TxBuffer* pi_Command, TxBuffer* pi_Params, uint8_t* u8_RecvBuf, int s32_RecvSize, DESFireStatus* pe_Status, DESFireCmac e_Mac);
    int  DataExchangeReadFile(uint8_t      u8_Command, TxBuffer* pi_Params, uint8_t* u8_RecvBuf, int s32_RecvSize, DESFireStatus* pe_Status, DESFireCmac e_Mac);
    int  DataExchangeReadFile(TxBuffer* pi_Command, TxBuffer* pi_Params, uint8_t* u8_RecvBuf, int s32_RecvSize, DESFireStatus* pe_Status, DESFireCmac e_Mac);
    bool CheckCardStatus(DESFireStatus e_Status);
    bool CheckPN532Status(byte u8_Status);
    bool SelftestKeyChange(uint32_t u32_Application, DESFireKey* pi_DefaultKey, DESFireKey* pi_NewKeyA, DESFireKey* pi_NewKeyB);

    uint8_t       mu8_LastAuthKeyNo; // The last key which did a successful authetication (0xFF if not yet authenticated)
    uint32_t      mu32_LastApplication;
    uint8_t       mu8_LastPN532Error;
    DESFireKey*   mpi_SessionKey;
    AES           mi_AesSessionKey;
    DES           mi_DesSessionKey;

    // Must have enough space to hold the entire response from DF_INS_GET_APPLICATION_IDS (84 byte) + CMAC padding
    uint8_t       mu8_CmacBuffer_Data[120];
    TxBuffer      mi_CmacBuffer;

    uint8_t _uid[7];  // ISO14443A uid
    uint8_t _uidLen;  // uid len
    uint8_t _key[6];  // Mifare Classic key
    uint8_t inListedTag; // Tg number of inlisted tag.
    uint8_t _felicaIDm[8]; // FeliCa IDm (NFCID2)
    uint8_t _felicaPMm[8]; // FeliCa PMm (PAD)

    byte mu8_DebugLevel = 2;   // 0, 1, or 2
    uint8_t pn532_packetbuffer[PN532_PACKBUFFSIZE];

    PN532Interface *_interface;
};

#endif
