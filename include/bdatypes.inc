ifndef _BDATYPES_
define _BDATYPES_

include winapifamily.inc

if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)

include exposeenums2managed.inc

MIN_DIMENSION		equ 1

ifndef MANAGED_ENUMS

BDA_TEMPLATE_CONNECTION STRUC
FromNodeType		ULONG ?
FromNodePinType		ULONG ?
ToNodeType		ULONG ?
ToNodePinType		ULONG ?
BDA_TEMPLATE_CONNECTION ENDS
PBDA_TEMPLATE_CONNECTION typedef ptr BDA_TEMPLATE_CONNECTION


BDA_TEMPLATE_PIN_JOINT	STRUC
uliTemplateConnection	ULONG ?
ulcInstancesMax		ULONG ?
BDA_TEMPLATE_PIN_JOINT	ENDS
PBDA_TEMPLATE_PIN_JOINT typedef ptr BDA_TEMPLATE_PIN_JOINT
endif


.enum BDA_EVENT_ID {
    BDA_EVENT_SIGNAL_LOSS = 0,
    BDA_EVENT_SIGNAL_LOCK,
    BDA_EVENT_DATA_START,
    BDA_EVENT_DATA_STOP,
    BDA_EVENT_CHANNEL_ACQUIRED,
    BDA_EVENT_CHANNEL_LOST,
    BDA_EVENT_CHANNEL_SOURCE_CHANGED,
    BDA_EVENT_CHANNEL_ACTIVATED,
    BDA_EVENT_CHANNEL_DEACTIVATED,
    BDA_EVENT_SUBCHANNEL_ACQUIRED,
    BDA_EVENT_SUBCHANNEL_LOST,
    BDA_EVENT_SUBCHANNEL_SOURCE_CHANGED,
    BDA_EVENT_SUBCHANNEL_ACTIVATED,
    BDA_EVENT_SUBCHANNEL_DEACTIVATED,
    BDA_EVENT_ACCESS_GRANTED,
    BDA_EVENT_ACCESS_DENIED,
    BDA_EVENT_OFFER_EXTENDED,
    BDA_EVENT_PURCHASE_COMPLETED,
    BDA_EVENT_SMART_CARD_INSERTED,
    BDA_EVENT_SMART_CARD_REMOVED
    }


ifndef MANAGED_ENUMS
KS_BDA_FRAME_INFO	STRUC
ExtendedHeaderSize	ULONG ? ;; Size of this extended header
dwFrameFlags		dd ?	;;
ulEvent			ULONG ? ;;
ulChannelNumber		ULONG ? ;;
ulSubchannelNumber	ULONG ? ;;
ulReason		ULONG ? ;;
KS_BDA_FRAME_INFO	ENDS
PKS_BDA_FRAME_INFO	typedef ptr KS_BDA_FRAME_INFO


BDA_ETHERNET_ADDRESS	STRUC
rgbAddress		db 6 dup(?)
BDA_ETHERNET_ADDRESS	ENDS
PBDA_ETHERNET_ADDRESS	typedef ptr BDA_ETHERNET_ADDRESS

BDA_ETHERNET_ADDRESS_LIST  STRUC
ulcAddresses		ULONG ?
rgAddressl		BDA_ETHERNET_ADDRESS MIN_DIMENSION dup(<>)
BDA_ETHERNET_ADDRESS_LIST ENDS
 PBDA_ETHERNET_ADDRESS_LIST typedef ptr BDA_ETHERNET_ADDRESS_LIST

endif

.enum BDA_MULTICAST_MODE {
    BDA_PROMISCUOUS_MULTICAST = 0,
    BDA_FILTERED_MULTICAST,
    BDA_NO_MULTICAST
    }


ifndef MANAGED_ENUMS
BDA_IPv4_ADDRESS	STRUC
rgbAddress		db 4 dup(?)
BDA_IPv4_ADDRESS	ENDS
PBDA_IPv4_ADDRESS	typedef ptr BDA_IPv4_ADDRESS

BDA_IPv4_ADDRESS_LIST	STRUC
ulcAddresses		ULONG ?
rgAddressl		BDA_IPv4_ADDRESS MIN_DIMENSION dup(<>)
BDA_IPv4_ADDRESS_LIST	ENDS
 PBDA_IPv4_ADDRESS_LIST typedef ptr BDA_IPv4_ADDRESS_LIST

BDA_IPv6_ADDRESS	STRUC
rgbAddress		db 6 dup(?)
BDA_IPv6_ADDRESS	ENDS
PBDA_IPv6_ADDRESS	typedef ptr BDA_IPv6_ADDRESS

BDA_IPv6_ADDRESS_LIST	STRUC
ulcAddresses		ULONG ?
rgAddressl		BDA_IPv6_ADDRESS MIN_DIMENSION dup(<>)
BDA_IPv6_ADDRESS_LIST	ENDS
 PBDA_IPv6_ADDRESS_LIST typedef ptr BDA_IPv6_ADDRESS_LIST

endif


.enum BDA_SIGNAL_STATE {
    BDA_SIGNAL_UNAVAILABLE = 0,
    BDA_SIGNAL_INACTIVE,
    BDA_SIGNAL_ACTIVE
    }


.enum BDA_CHANGE_STATE {
    BDA_CHANGES_COMPLETE = 0,
    BDA_CHANGES_PENDING
    }


ifndef MANAGED_ENUMS
BDANODE_DESCRIPTOR	STRUC
ulBdaNodeType		ULONG ?
guidFunction		GUID <>
guidName		GUID <>
BDANODE_DESCRIPTOR	ENDS
PBDANODE_DESCRIPTOR	typedef ptr BDANODE_DESCRIPTOR

BDA_TABLE_SECTION	STRUC
ulPrimarySectionId	ULONG ?
ulSecondarySectionId	ULONG ?
ulcbSectionLength	ULONG ?
argbSectionData		ULONG MIN_DIMENSION dup(?)
BDA_TABLE_SECTION	ENDS
PBDA_TABLE_SECTION	typedef ptr BDA_TABLE_SECTION

endif

ifndef MANAGED_ENUMS

BDA_DISEQC_SEND		STRUC
ulRequestId		ULONG ?
ulPacketLength		ULONG ?
argbPacketData		db 8 dup(?)
BDA_DISEQC_SEND		ENDS
PBDA_DISEQC_SEND	typedef ptr BDA_DISEQC_SEND

BDA_DISEQC_RESPONSE	STRUC
ulRequestId		ULONG ?
ulPacketLength		ULONG ?
argbPacketData		db 8 dup(?)
BDA_DISEQC_RESPONSE	ENDS
PBDA_DISEQC_RESPONSE	typedef ptr BDA_DISEQC_RESPONSE


endif

.enum MEDIA_SAMPLE_CONTENT {
    MEDIA_TRANSPORT_PACKET,
    MEDIA_ELEMENTARY_STREAM,
    MEDIA_MPEG2_PSI,
    MEDIA_TRANSPORT_PAYLOAD
    }

ifndef MANAGED_ENUMS

PID_MAP			STRUC
ulPID			ULONG ?
MediaSampleContent	MEDIA_SAMPLE_CONTENT ?
PID_MAP			ENDS

BDA_PID_MAP		STRUC
MediaSampleContent	MEDIA_SAMPLE_CONTENT ?
ulcPIDs			ULONG ?
aulPIDs			ULONG MIN_DIMENSION dup(?)
BDA_PID_MAP		ENDS
PBDA_PID_MAP		typedef ptr BDA_PID_MAP

BDA_PID_UNMAP		STRUC
ulcPIDs			ULONG ?
aulPIDs			ULONG MIN_DIMENSION dup(?)
BDA_PID_UNMAP		ENDS
PBDA_PID_UNMAP		typedef ptr BDA_PID_UNMAP


BDA_CA_MODULE_UI	STRUC
ulFormat		ULONG ?
ulbcDesc		ULONG ?
ulDesc			ULONG MIN_DIMENSION dup(?)
BDA_CA_MODULE_UI	ENDS
PBDA_CA_MODULE_UI	typedef ptr BDA_CA_MODULE_UI

BDA_PROGRAM_PID_LIST	STRUC
ulProgramNumber		ULONG ?
ulcPIDs			ULONG ?
ulPID			ULONG MIN_DIMENSION dup(?)
BDA_PROGRAM_PID_LIST	ENDS
PBDA_PROGRAM_PID_LIST	typedef ptr BDA_PROGRAM_PID_LIST

endif

ifndef MANAGED_ENUMS

PBDARESULT		typedef LONG

BDA_DRM_DRMSTATUS	STRUC
lResult			PBDARESULT ?
DRMuuid			GUID <>
ulDrmUuidListStringSize ULONG ?
argbDrmUuidListString	GUID MIN_DIMENSION dup(<>)
BDA_DRM_DRMSTATUS	ENDS
PBDA_DRM_DRMSTATUS	typedef ptr BDA_DRM_DRMSTATUS

BDA_WMDRM_STATUS	STRUC
lResult			PBDARESULT ?
ulMaxCaptureTokenSize	ULONG ?
uMaxStreamingPid	ULONG ?
ulMaxLicense		ULONG ?
ulMinSecurityLevel	ULONG ?
ulRevInfoSequenceNumber ULONG ?
ulRevInfoIssuedTime	ULONGLONG ?
ulRevListVersion	ULONG ?
ulRevInfoTTL		ULONG ?
ulState			ULONG ?
BDA_WMDRM_STATUS	ENDS
PBDA_WMDRM_STATUS	typedef ptr BDA_WMDRM_STATUS

BDA_WMDRM_KEYINFOLIST	STRUC
lResult			PBDARESULT ?
ulKeyuuidBufferLen	ULONG ?
argKeyuuidBuffer	GUID MIN_DIMENSION dup(<>)
BDA_WMDRM_KEYINFOLIST	ENDS
PBDA_WMDRM_KEYINFOLIST	typedef ptr BDA_WMDRM_KEYINFOLIST

BDA_BUFFER		STRUC
lResult			PBDARESULT ?
ulBufferSize		ULONG ?
argbBuffer		db MIN_DIMENSION dup(?)
BDA_BUFFER		ENDS
PBDA_BUFFER		typedef ptr BDA_BUFFER

BDA_WMDRM_RENEWLICENSE	STRUC
lResult			PBDARESULT ?
ulDescrambleStatus	ULONG ?
ulXmrLicenseOutputLength ULONG ?
argbXmrLicenceOutputBuffer db MIN_DIMENSION dup(?)	;;License and Entitlement Token Buffer
BDA_WMDRM_RENEWLICENSE	ENDS
PBDA_WMDRM_RENEWLICENSE typedef ptr BDA_WMDRM_RENEWLICENSE

BDA_WMDRMTUNER_PIDPROTECTION STRUC
lResult			PBDARESULT ?
uuidKeyID		GUID <>
BDA_WMDRMTUNER_PIDPROTECTION ENDS
PBDA_WMDRMTUNER_PIDPROTECTION typedef ptr BDA_WMDRMTUNER_PIDPROTECTION

BDA_WMDRMTUNER_PURCHASEENTITLEMENT STRUC
lResult			PBDARESULT ?
ulDescrambleStatus	ULONG ?
ulCaptureTokenLength	ULONG ?
argbCaptureTokenBuffer	db MIN_DIMENSION dup(?)
BDA_WMDRMTUNER_PURCHASEENTITLEMENT ENDS
PBDA_WMDRMTUNER_PURCHASEENTITLEMENT typedef ptr BDA_WMDRMTUNER_PURCHASEENTITLEMENT

BDA_TUNER_TUNERSTATE	STRUC
lResult			PBDARESULT ?
ulTuneLength		ULONG ?
argbTuneData		db MIN_DIMENSION dup(?)
BDA_TUNER_TUNERSTATE	ENDS
PBDA_TUNER_TUNERSTATE	typedef ptr BDA_TUNER_TUNERSTATE

BDA_TUNER_DIAGNOSTICS	STRUC
lResult			PBDARESULT ?
ulSignalLevel		ULONG ?
ulSignalLevelQuality	ULONG ?
ulSignalNoiseRatio	ULONG ?
BDA_TUNER_DIAGNOSTICS	ENDS
PBDA_TUNER_DIAGNOSTICS	typedef ptr BDA_TUNER_DIAGNOSTICS

BDA_STRING		STRUC
lResult			PBDARESULT ?
ulStringSize		ULONG ?
argbString		db MIN_DIMENSION dup(?)
BDA_STRING		ENDS
PBDA_STRING		typedef ptr BDA_STRING

BDA_SCAN_CAPABILTIES	STRUC
lResult			PBDARESULT ?
ul64AnalogStandardsSupported UINT64 ?
BDA_SCAN_CAPABILTIES	ENDS
PBDA_SCAN_CAPABILTIES	typedef ptr BDA_SCAN_CAPABILTIES

BDA_SCAN_STATE		STRUC
lResult			PBDARESULT ?
ulSignalLock		ULONG ?
ulSecondsLeft		ULONG ?
ulCurrentFrequency	ULONG ?
BDA_SCAN_STATE		ENDS
PBDA_SCAN_STATE		typedef ptr BDA_SCAN_STATE

BDA_SCAN_START		STRUC
lResult			PBDARESULT ?
LowerFrequency		ULONG ?
HigerFrequency		ULONG ?
BDA_SCAN_START		ENDS
PBDA_SCAN_START		typedef ptr BDA_SCAN_START


BDA_GDDS_DATATYPE	STRUC
lResult			PBDARESULT ?
uuidDataType		GUID <>
BDA_GDDS_DATATYPE	ENDS
P_BDA_GDDS_DATATYPE	typedef ptr BDA_GDDS_DATATYPE

BDA_GDDS_DATA		STRUC
lResult			PBDARESULT ?
ulDataLength		ULONG ?
ulPercentageProgress	ULONG ?
argbData		db MIN_DIMENSION dup(?)
BDA_GDDS_DATA		ENDS
P_BDA_GDDS_DATA		typedef ptr BDA_GDDS_DATA


BDA_USERACTIVITY_INTERVAL STRUC
lResult			PBDARESULT ?
ulActivityInterval	ULONG ?
BDA_USERACTIVITY_INTERVAL ENDS
P_BDA_USERACTIVITY_INTERVAL typedef ptr BDA_USERACTIVITY_INTERVAL

BDA_CAS_CHECK_ENTITLEMENTTOKEN STRUC
lResult			PBDARESULT ?
ulDescrambleStatus	ULONG ?
BDA_CAS_CHECK_ENTITLEMENTTOKEN ENDS
PBDA_CAS_CHECK_ENTITLEMENTTOKEN typedef ptr BDA_CAS_CHECK_ENTITLEMENTTOKEN

BDA_CAS_CLOSE_MMIDIALOG STRUC
lResult			PBDARESULT ?
SessionResult		ULONG ?
BDA_CAS_CLOSE_MMIDIALOG ENDS
PBDA_CAS_CLOSE_MMIDIALOG typedef ptr BDA_CAS_CLOSE_MMIDIALOG

BDA_CAS_REQUESTTUNERDATA STRUC
ucRequestPriority	UCHAR ?
ucRequestReason		UCHAR ?
ucRequestConsequences	UCHAR ?
ulEstimatedTime		ULONG ?
BDA_CAS_REQUESTTUNERDATA ENDS
PBDA_CAS_REQUESTTUNERDATA typedef ptr BDA_CAS_REQUESTTUNERDATA

BDA_CAS_OPENMMIDATA	STRUC
ulDialogNumber		ULONG ?
ulDialogRequest		ULONG ?
uuidDialogType		GUID <>
usDialogDataLength	USHORT ?
argbDialogData		db MIN_DIMENSION dup(?)
BDA_CAS_OPENMMIDATA	ENDS
PBDA_CAS_OPENMMIDATA	typedef ptr BDA_CAS_OPENMMIDATA

BDA_CAS_CLOSEMMIDATA	STRUC
ulDialogNumber		ULONG ?
BDA_CAS_CLOSEMMIDATA	ENDS
PBDA_CAS_CLOSEMMIDATA	typedef ptr BDA_CAS_CLOSEMMIDATA

.pragma pack(push, 1)

.enum ISDBCAS_REQUEST_ID {
    ISDBCAS_REQUEST_ID_EMG = 0x38,
    ISDBCAS_REQUEST_ID_EMD = 0x3A,
    }

BDA_ISDBCAS_REQUESTHEADER STRUC
bInstruction		db ?	;; EMD/EMG
bReserved		db 3 dup(?)	;; future use
ulDataLength		ULONG ? ;; byte size of argbIsdbCommand
argbIsdbCommand		db MIN_DIMENSION dup(?)
BDA_ISDBCAS_REQUESTHEADER ENDS
PBDA_ISDBCAS_REQUESTHEADER typedef ptr BDA_ISDBCAS_REQUESTHEADER

BDA_ISDBCAS_RESPONSEDATA STRUC
lResult			PBDARESULT ?
ulRequestID		ULONG ?
ulIsdbStatus		ULONG ?
ulIsdbDataSize		ULONG ?
argbIsdbCommandData	db MIN_DIMENSION dup(?)
BDA_ISDBCAS_RESPONSEDATA ENDS
PBDA_ISDBCAS_RESPONSEDATA typedef ptr BDA_ISDBCAS_RESPONSEDATA

BDA_ISDBCAS_EMG_REQ	STRUC
bCLA			db ?	;; 0x90
bINS			db ?	;; 0x38
bP1			db ?	;; 0x00
bP2			db ?	;; 0x00
bLC			db ?	;; Following bytes - 1(LE)
bCardId			db 6 dup(?)	;; from EMM message packet
bProtocol		db ?	;; from EMM message packet
bCABroadcasterGroupId	db ?	;; from EMM message packet
bMessageControl		db ?	;; from EMM message packet
bMessageCode		db MIN_DIMENSION dup(?) ;; Last byte is reserved as 'LE'
BDA_ISDBCAS_EMG_REQ	ENDS
PBDA_ISDBCAS_EMG_REQ	typedef ptr BDA_ISDBCAS_EMG_REQ

.pragma pack(pop)

.enum MUX_PID_TYPE {
    PID_OTHER = -1,
    PID_ELEMENTARY_STREAM,
    PID_MPEG2_SECTION_PSI_SI,
    }

.pragma pack(push, 2)
BDA_MUX_PIDLISTITEM	STRUC
usPIDNumber		USHORT ?
usProgramNumber		USHORT ?
ePIDType		MUX_PID_TYPE ?
BDA_MUX_PIDLISTITEM	ENDS
PBDA_MUX_PIDLISTITEM	typedef ptr BDA_MUX_PIDLISTITEM
.pragma pack(pop)

endif

.pragma pack(push, 1)
BDA_TS_SELECTORINFO	STRUC
bTSInfolength		db ?	;; buffer length including extension
bReserved		db 2 dup(?)
guidNetworkType		GUID <> ;; current type of tuning
bTSIDCount		db ?	;; number of usTSID
usTSID			USHORT MIN_DIMENSION dup(?)
BDA_TS_SELECTORINFO	ENDS
PBDA_TS_SELECTORINFO	typedef ptr BDA_TS_SELECTORINFO

BDA_TS_SELECTORINFO_ISDBS_EXT STRUC
bTMCC			db 48 dup(?)
BDA_TS_SELECTORINFO_ISDBS_EXT ENDS
PBDA_TS_SELECTORINFO_ISDBS_EXT typedef ptr BDA_TS_SELECTORINFO_ISDBS_EXT
.pragma pack(pop)

BDA_DVBT2_L1_SIGNALLING_DATA STRUC
L1Pre_TYPE		db ?
L1Pre_BWT_S1_S2		db ?
L1Pre_REPETITION_GUARD_PAPR db ?
L1Pre_MOD_COD_FEC	db ?
L1Pre_POSTSIZE_INFO_PILOT db 5 dup(?)
L1Pre_TX_ID_AVAIL	db ?
L1Pre_CELL_ID		db 2 dup(?)
L1Pre_NETWORK_ID	db 2 dup(?)
L1Pre_T2SYSTEM_ID	db 2 dup(?)
L1Pre_NUM_T2_FRAMES	db ?
L1Pre_NUM_DATA_REGENFLAG_L1POSTEXT db 2 dup(?)
L1Pre_NUMRF_CURRENTRF_RESERVED db 2 dup(?)
L1Pre_CRC32		db 4 dup(?)
L1PostData		db MIN_DIMENSION dup(?)
BDA_DVBT2_L1_SIGNALLING_DATA ENDS
PBDA_DVBT2_L1_SIGNALLING_DATA typedef ptr BDA_DVBT2_L1_SIGNALLING_DATA


BDA_RATING_PINRESET	STRUC
bPinLength		db ?	;;Buffer size including a null termination
argbNewPin		db MIN_DIMENSION dup(?) ;;Null terminated UTF8. Use empty string if disable pin
BDA_RATING_PINRESET	ENDS
PBDA_RATING_PINRESET	typedef ptr BDA_RATING_PINRESET

.enum DVBSystemType {
    DVB_Cable,
    DVB_Terrestrial,
    DVB_Satellite,
    ISDB_Terrestrial,
    ISDB_Satellite,
    }

.enum  BDA_Channel {
    BDA_UNDEFINED_CHANNEL = -1,
    }


.enum ComponentCategory {
    CategoryNotSet = -1,
    CategoryOther = 0,
    CategoryVideo,
    CategoryAudio,
    CategoryText,
    CategorySubtitles,
    CategoryCaptions,
    CategorySuperimpose,
    CategoryData,
    CATEGORY_COUNT,
    }

.enum ComponentStatus {
    StatusActive,
    StatusInactive,
    StatusUnavailable,
    }


.enum MPEG2StreamType {
    BDA_UNITIALIZED_MPEG2STREAMTYPE = -1,
    Reserved1			    = 0x00,
    ISO_IEC_11172_2_VIDEO	    = 0x01,
    ISO_IEC_13818_2_VIDEO	    = 0x02,
    ISO_IEC_11172_3_AUDIO	    = 0x03,
    ISO_IEC_13818_3_AUDIO	    = 0x04,
    ISO_IEC_13818_1_PRIVATE_SECTION = 0x05,
    ISO_IEC_13818_1_PES		    = 0x06,
    ISO_IEC_13522_MHEG		    = 0x07,
    ANNEX_A_DSM_CC		    = 0x08,
    ITU_T_REC_H_222_1		    = 0x09,
    ISO_IEC_13818_6_TYPE_A	    = 0x0A,
    ISO_IEC_13818_6_TYPE_B	    = 0x0B,
    ISO_IEC_13818_6_TYPE_C	    = 0x0C,
    ISO_IEC_13818_6_TYPE_D	    = 0x0D,
    ISO_IEC_13818_1_AUXILIARY	    = 0x0E,
    ISO_IEC_13818_7_AUDIO	    = 0x0F,
    ISO_IEC_14496_2_VISUAL	    = 0x10,
    ISO_IEC_14496_3_AUDIO	    = 0x11,
    ISO_IEC_14496_1_IN_PES	    = 0x12,
    ISO_IEC_14496_1_IN_SECTION	    = 0x13,
    ISO_IEC_13818_6_DOWNLOAD	    = 0x14,
    METADATA_IN_PES		    = 0x15,
    METADATA_IN_SECTION		    = 0x16,
    METADATA_IN_DATA_CAROUSEL	    = 0x17,
    METADATA_IN_OBJECT_CAROUSEL	    = 0x18,
    METADATA_IN_DOWNLOAD_PROTOCOL   = 0x19,
    IRPM_STREAMM		    = 0x1A,
    ITU_T_H264			    = 0x1B,
    ISO_IEC_13818_1_RESERVED	    = 0x1C,
    USER_PRIVATE		    = 0x10,
    ISO_IEC_USER_PRIVATE	    = 0x80,
    DOLBY_AC3_AUDIO		    = 0x81,
    DOLBY_DIGITAL_PLUS_AUDIO_ATSC   = 0X87
    }

ifndef MANAGED_ENUMS
MPEG2_TRANSPORT_STRIDE	STRUC
dwOffset		dd ?
dwPacketLength		dd ?
dwStride		dd ?
MPEG2_TRANSPORT_STRIDE	ENDS
PMPEG2_TRANSPORT_STRIDE	 typedef ptr MPEG2_TRANSPORT_STRIDE
endif

.enum ATSCComponentTypeFlags {
    ATSCCT_AC3 = 0x00000001,
    }


.enum BinaryConvolutionCodeRate {
    BDA_BCC_RATE_NOT_SET = -1,
    BDA_BCC_RATE_NOT_DEFINED = 0,
    BDA_BCC_RATE_1_2 = 1,
    BDA_BCC_RATE_2_3,
    BDA_BCC_RATE_3_4,
    BDA_BCC_RATE_3_5,
    BDA_BCC_RATE_4_5,
    BDA_BCC_RATE_5_6,
    BDA_BCC_RATE_5_11,
    BDA_BCC_RATE_7_8,
    BDA_BCC_RATE_1_4,
    BDA_BCC_RATE_1_3,
    BDA_BCC_RATE_2_5,
    BDA_BCC_RATE_6_7,
    BDA_BCC_RATE_8_9,
    BDA_BCC_RATE_9_10,
    BDA_BCC_RATE_MAX,
    }

.enum FECMethod {
    BDA_FEC_METHOD_NOT_SET = -1,
    BDA_FEC_METHOD_NOT_DEFINED = 0,
    BDA_FEC_VITERBI = 1,
    BDA_FEC_RS_204_188,
    BDA_FEC_LDPC,
    BDA_FEC_BCH,
    BDA_FEC_RS_147_130,
    BDA_FEC_MAX,
    }

.enum ModulationType {
    BDA_MOD_NOT_SET = -1,
    BDA_MOD_NOT_DEFINED = 0,
    BDA_MOD_16QAM = 1,
    BDA_MOD_32QAM,
    BDA_MOD_64QAM,
    BDA_MOD_80QAM,
    BDA_MOD_96QAM,
    BDA_MOD_112QAM,
    BDA_MOD_128QAM,
    BDA_MOD_160QAM,
    BDA_MOD_192QAM,
    BDA_MOD_224QAM,
    BDA_MOD_256QAM,
    BDA_MOD_320QAM,
    BDA_MOD_384QAM,
    BDA_MOD_448QAM,
    BDA_MOD_512QAM,
    BDA_MOD_640QAM,
    BDA_MOD_768QAM,
    BDA_MOD_896QAM,
    BDA_MOD_1024QAM,
    BDA_MOD_QPSK,
    BDA_MOD_BPSK,
    BDA_MOD_OQPSK,
    BDA_MOD_8VSB,
    BDA_MOD_16VSB,
    BDA_MOD_ANALOG_AMPLITUDE,
    BDA_MOD_ANALOG_FREQUENCY,
    BDA_MOD_8PSK,
    BDA_MOD_RF,
    BDA_MOD_16APSK,
    BDA_MOD_32APSK,
    BDA_MOD_NBC_QPSK,
    BDA_MOD_NBC_8PSK,
    BDA_MOD_DIRECTV,
    BDA_MOD_ISDB_T_TMCC,
    BDA_MOD_ISDB_S_TMCC,
    BDA_MOD_MAX,
    }

ifdef _MANAGED
.enum ScanModulationTypes : uint_t {
else
.enum ScanModulationTypes {
endif
    BDA_SCAN_MOD_16QAM	  = 0x00000001,
    BDA_SCAN_MOD_32QAM	  = 0x00000002,
    BDA_SCAN_MOD_64QAM	  = 0x00000004,
    BDA_SCAN_MOD_80QAM	  = 0x00000008,
    BDA_SCAN_MOD_96QAM	  = 0x00000010,
    BDA_SCAN_MOD_112QAM	  = 0x00000020,
    BDA_SCAN_MOD_128QAM	  = 0x00000040,
    BDA_SCAN_MOD_160QAM	  = 0x00000080,
    BDA_SCAN_MOD_192QAM	  = 0x00000100,
    BDA_SCAN_MOD_224QAM	  = 0x00000200,
    BDA_SCAN_MOD_256QAM	  = 0x00000400,
    BDA_SCAN_MOD_320QAM	  = 0x00000800,
    BDA_SCAN_MOD_384QAM	  = 0x00001000,
    BDA_SCAN_MOD_448QAM	  = 0x00002000,
    BDA_SCAN_MOD_512QAM	  = 0x00004000,
    BDA_SCAN_MOD_640QAM	  = 0x00008000,
    BDA_SCAN_MOD_768QAM	  = 0x00010000,
    BDA_SCAN_MOD_896QAM	  = 0x00020000,
    BDA_SCAN_MOD_1024QAM  = 0x00040000,
    BDA_SCAN_MOD_QPSK	  = 0x00080000,
    BDA_SCAN_MOD_BPSK	  = 0x00100000,
    BDA_SCAN_MOD_OQPSK	  = 0x00200000,
    BDA_SCAN_MOD_8VSB	  = 0x00400000,
    BDA_SCAN_MOD_16VSB	  = 0x00800000,
    BDA_SCAN_MOD_AM_RADIO = 0x01000000,
    BDA_SCAN_MOD_FM_RADIO = 0x02000000,
    BDA_SCAN_MOD_8PSK	  = 0x04000000,
    BDA_SCAN_MOD_RF	  = 0x08000000,
    ScanModulationTypesMask_MCE_DigitalCable = BDA_MOD_64QAM or BDA_MOD_256QAM,
    ScanModulationTypesMask_MCE_TerrestrialATSC = BDA_MOD_8VSB,
    ScanModulationTypesMask_MCE_AnalogTv = BDA_MOD_RF,
    ScanModulationTypesMask_MCE_All_TV = 0xffffffff,
    ScanModulationTypesMask_DVBC = BDA_MOD_64QAM or BDA_SCAN_MOD_128QAM or BDA_MOD_256QAM,
    BDA_SCAN_MOD_16APSK	    = 0x10000000,
    BDA_SCAN_MOD_32APSK	    = 0x20000000
    }


.enum SpectralInversion {
    BDA_SPECTRAL_INVERSION_NOT_SET = -1,
    BDA_SPECTRAL_INVERSION_NOT_DEFINED = 0,
    BDA_SPECTRAL_INVERSION_AUTOMATIC = 1,
    BDA_SPECTRAL_INVERSION_NORMAL,
    BDA_SPECTRAL_INVERSION_INVERTED,
    BDA_SPECTRAL_INVERSION_MAX
    }

.enum Polarisation {
    BDA_POLARISATION_NOT_SET = -1,
    BDA_POLARISATION_NOT_DEFINED = 0,
    BDA_POLARISATION_LINEAR_H = 1,
    BDA_POLARISATION_LINEAR_V,
    BDA_POLARISATION_CIRCULAR_L,
    BDA_POLARISATION_CIRCULAR_R,
    BDA_POLARISATION_MAX,
    }

.enum LNB_Source {
    BDA_LNB_SOURCE_NOT_SET = -1,
    BDA_LNB_SOURCE_NOT_DEFINED = 0,
    BDA_LNB_SOURCE_A = 1,
    BDA_LNB_SOURCE_B = 2,
    BDA_LNB_SOURCE_C = 3,
    BDA_LNB_SOURCE_D = 4,
    BDA_LNB_SOURCE_MAX,
    }


.enum GuardInterval {
    BDA_GUARD_NOT_SET = -1,
    BDA_GUARD_NOT_DEFINED = 0,
    BDA_GUARD_1_32 = 1,
    BDA_GUARD_1_16,
    BDA_GUARD_1_8,
    BDA_GUARD_1_4,
    BDA_GUARD_1_128,
    BDA_GUARD_19_128,
    BDA_GUARD_19_256,
    BDA_GUARD_MAX,
    }

.enum HierarchyAlpha {
    BDA_HALPHA_NOT_SET = -1,
    BDA_HALPHA_NOT_DEFINED = 0,
    BDA_HALPHA_1 = 1,
    BDA_HALPHA_2,
    BDA_HALPHA_4,
    BDA_HALPHA_MAX,
    }

.enum TransmissionMode {
    BDA_XMIT_MODE_NOT_SET = -1,
    BDA_XMIT_MODE_NOT_DEFINED = 0,
    BDA_XMIT_MODE_2K = 1,
    BDA_XMIT_MODE_8K,
    BDA_XMIT_MODE_4K,
    BDA_XMIT_MODE_2K_INTERLEAVED,
    BDA_XMIT_MODE_4K_INTERLEAVED,
    BDA_XMIT_MODE_1K,
    BDA_XMIT_MODE_16K,
    BDA_XMIT_MODE_32K,
    BDA_XMIT_MODE_MAX,
    }

.enum RollOff {
    BDA_ROLL_OFF_NOT_SET = -1,
    BDA_ROLL_OFF_NOT_DEFINED = 0,
    BDA_ROLL_OFF_20 = 1,
    BDA_ROLL_OFF_25,
    BDA_ROLL_OFF_35,
    BDA_ROLL_OFF_MAX,
    }

.enum Pilot {
    BDA_PILOT_NOT_SET = -1,
    BDA_PILOT_NOT_DEFINED = 0,
    BDA_PILOT_OFF = 1,
    BDA_PILOT_ON,
    BDA_PILOT_MAX,
    }

BDA_SIGNAL_TIMEOUTS	STRUC
ulCarrierTimeoutMs	ULONG ?
ulScanningTimeoutMs	ULONG ?
ulTuningTimeoutMs	ULONG ?
BDA_SIGNAL_TIMEOUTS	ENDS
PBDA_SIGNAL_TIMEOUTS	typedef ptr BDA_SIGNAL_TIMEOUTS

.enum BDA_Frequency {
    BDA_FREQUENCY_NOT_SET = -1,
    BDA_FREQUENCY_NOT_DEFINED = 0
    }

.enum BDA_Range {
    BDA_RANGE_NOT_SET = -1,
    BDA_RANGE_NOT_DEFINED = 0
    }

.enum BDA_Channel_Bandwidth {
    BDA_CHAN_BANDWITH_NOT_SET	   = -1,
    BDA_CHAN_BANDWITH_NOT_DEFINED  = 0
    }

.enum BDA_Frequency_Multiplier {
    BDA_FREQUENCY_MULTIPLIER_NOT_SET	   = -1,
    BDA_FREQUENCY_MULTIPLIER_NOT_DEFINED   = 0
    }

.enum BDA_Comp_Flags {
    BDACOMP_NOT_DEFINED		     = 0x00000000,
    BDACOMP_EXCLUDE_TS_FROM_TR	     = 0x00000001,
    BDACOMP_INCLUDE_LOCATOR_IN_TR    = 0x00000002,
    BDACOMP_INCLUDE_COMPONENTS_IN_TR = 0x00000004
    }

.enum ApplicationTypeType {
    SCTE28_ConditionalAccess = 0,
    SCTE28_POD_Host_Binding_Information,
    SCTE28_IPService,
    SCTE28_NetworkInterface_SCTE55_2,
    SCTE28_NetworkInterface_SCTE55_1,
    SCTE28_CopyProtection,
    SCTE28_Diagnostic,
    SCTE28_Undesignated,
    SCTE28_Reserved,
    }


.enum BDA_CONDITIONALACCESS_REQUESTTYPE {
    CONDITIONALACCESS_ACCESS_UNSPECIFIED = 0,
    CONDITIONALACCESS_ACCESS_NOT_POSSIBLE,
    CONDITIONALACCESS_ACCESS_POSSIBLE,
    CONDITIONALACCESS_ACCESS_POSSIBLE_NO_STREAMING_DISRUPTION
    }

.enum BDA_CONDITIONALACCESS_MMICLOSEREASON {
    CONDITIONALACCESS_UNSPECIFIED = 0,
    CONDITIONALACCESS_CLOSED_ITSELF,
    CONDITIONALACCESS_TUNER_REQUESTED_CLOSE,
    CONDITIONALACCESS_DIALOG_TIMEOUT,
    CONDITIONALACCESS_DIALOG_FOCUS_CHANGE,
    CONDITIONALACCESS_DIALOG_USER_DISMISSED,
    CONDITIONALACCESS_DIALOG_USER_NOT_AVAILABLE
    }

.enum BDA_CONDITIONALACCESS_SESSION_RESULT {
    CONDITIONALACCESS_SUCCESSFULL = 0,
    CONDITIONALACCESS_ENDED_NOCHANGE,
    CONDITIONALACCESS_ABORTED
    }

.enum BDA_DISCOVERY_STATE {
    BDA_DISCOVERY_UNSPECIFIED = 0,
    BDA_DISCOVERY_REQUIRED,
    BDA_DISCOVERY_COMPLETE
    }

;; Digital Demodulator for DVBT2 Physical Layer Pipe
BDA_PLP_ID_NOT_SET	equ -1

include unexposeenums2managed.inc


endif ;; WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)

endif ;; not defined _BDATYPES_
