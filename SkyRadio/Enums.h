#ifndef Enums_h__
#define Enums_h__

#include <vector>
#include <string>

//Max number of USB Devices allowed
#define MAX_USB_DEVICES	64

#define FMTUNERDATA_REPORT_SIZE	61

//Device data
#define SILABS_VID	0x10C4
#define SILABS_PID	0x818A
#define PCEAR_VID	0x10C5
#define PCEAR_PID	0x819A
#define ADSTECH_VID	0x06E1
#define ADSTECH_PID	0xA155

#define FMRADIO_SW_VERSION		7
#define FMRADIO_HW_VERSION		1
#define FMRADIO_FMTUNER_VERSION	10

//Return Codes
#define STATUS_OK					0x00
#define STATUS_ERROR				0x01
#define STATUS_OUTPUTAUDIO_ERROR	0x02
#define STATUS_FMRADIOAUDIO_ERROR	0x03
#define STATUS_FMRADIODATA_ERROR	0x04
#define STATUS_BOOTLOAD_ERROR		0x05

//Register definitions
#define FMRADIO_REGISTER_NUM	16
#define RDS_REGISTER_NUM		6

#define FMRADIO_REGISTER	WORD
#define FMRADIO_REGISTER_SIZE	sizeof(FMRADIO_REGISTER)
#define RDS_REGISTER_SIZE	sizeof(FMRADIO_REGISTER)

#define CORRECTED_NONE          0
#define CORRECTED_ONE_TO_TWO    1
#define CORRECTED_THREE_TO_FIVE 2
#define UNCORRECTABLE           3
#define ERRORS_CORRECTED(data,block) ((data>>block) & 0x03)


#define DEVICEID		0
#define CHIPID			1
#define POWERCFG		2
#define CHANNEL			3
#define SYSCONFIG1		4
#define SYSCONFIG2		5
#define SYSCONFIG3		6
#define TEST1			7
#define TEST2			8
#define BOOTCONFIG		9
#define STATUSRSSI		10
#define READCHAN		11
#define RDSA			12
#define RDSB			13
#define RDSC			14
#define RDSD			15

//Bit definitions
#define DEVICEID_PN		0xF000
#define DEVICEID_MFGID	0x0FFF

#define CHIPID_REV		0xFC00
#define CHIPID_DEV		0x0200
#define CHIPID_FIRMWARE	0x01FF

#define POWERCFG_DSMUTE		0x8000
#define POWERCFG_DMUTE		0x4000
#define POWERCFG_MONO		0x2000
#define POWERCFG_RDSM		0x0800
#define POWERCFG_SKMODE		0x0400
#define POWERCFG_SEEKUP		0x0200
#define POWERCFG_SEEK		0x0100
#define POWERCFG_DISABLE	0x0040
#define POWERCFG_ENABLE		0x0001

#define CHANNEL_TUNE	0x8000
#define CHANNEL_CHAN	0x03FF

#define SYSCONFIG1_DE		0x0800
#define SYSCONFIG1_RDS		0x1000

#define SYSCONFIG2_SEEKTH	0xFF00
#define SYSCONFIG2_BAND		0x0080
#define SYSCONFIG2_SPACE	0x0030
#define SYSCONFIG2_VOLUME	0x000F

#define SYSCONFIG2_SPACE_200KHZ	0x0000
#define SYSCONFIG2_SPACE_100KHZ	0x0010
#define SYSCONFIG2_SPACE_50KHZ	0x0020

#define SYSCONFIG3_SMUTER	0xC000
#define SYSCONFIG3_SMUTEA	0x3000
#define SYSCONFIG3_VOLEXT	0x0100
#define SYSCONFIG3_SKSNR	0x00F0
#define SYSCONFIG3_SKCNT	0x000F

#define STATUSRSSI_RDSR		0x8000
#define STATUSRSSI_STC		0x4000
#define STATUSRSSI_SF_BL	0x2000
#define STATUSRSSI_RDSS		0x0800
#define STATUSRSSI_BLERA	0x0600
#define STATUSRSSI_ST		0x0100
#define STATUSRSSI_RSSI		0x00FF

#define READCHAN_BLERB		0xC000
#define READCHAN_BLERC		0x3000
#define READCHAN_BLERD		0x0C00
#define READCHAN_READCHAN	0x03FF

//Scratch page
#define SCRATCH_PAGE_SIZE		63
#define SCRATCH_PAGE_USED_SIZE	22
typedef BYTE	SCRATCH_PAGE[SCRATCH_PAGE_SIZE];

#define SCRATCH_PAGE_SW_VERSION	1
#define SCRATCH_PAGE_HW_VERSION	2

//Report definitions
#define DEVICEID_REPORT		DEVICEID + 1
#define CHIPID_REPORT		CHIPID + 1
#define POWERCFG_REPORT		POWERCFG + 1
#define CHANNEL_REPORT		CHANNEL + 1
#define SYSCONFIG1_REPORT	SYSCONFIG1 + 1
#define SYSCONFIG2_REPORT	SYSCONFIG2 + 1
#define SYSCONFIG3_REPORT	SYSCONFIG3 + 1
#define TEST1_REPORT		TEST1 + 1
#define TEST2_REPORT		TEST2 + 1
#define BOOTCONFIG_REPORT	BOOTCONFIG + 1
#define STATUSRSSI_REPORT	STATUSRSSI + 1
#define READCHAN_REPORT		READCHAN + 1
#define RDSA_REPORT			RDSA + 1
#define RDSB_REPORT			RDSB + 1
#define RDSC_REPORT			RDSC + 1
#define RDSD_REPORT			RDSD + 1
#define LED_REPORT			19
#define STREAM_REPORT		19
#define SCRATCH_REPORT		20

#define ENTIRE_REPORT		FMRADIO_REGISTER_NUM + 1
#define RDS_REPORT			FMRADIO_REGISTER_NUM + 2

#define REGISTER_REPORT_SIZE	(FMRADIO_REGISTER_SIZE + 1)
#define ENTIRE_REPORT_SIZE		((FMRADIO_REGISTER_NUM * FMRADIO_REGISTER_SIZE) + 1)
#define RDS_REPORT_SIZE			((RDS_REGISTER_NUM * FMRADIO_REGISTER_SIZE) + 1)
#define LED_REPORT_SIZE			3
#define STREAM_REPORT_SIZE		3
#define SCRATCH_REPORT_SIZE		(SCRATCH_PAGE_SIZE + 1)

//LED State definitions
#define LED_COMMAND			0x35

#define NO_CHANGE_LED		0x00
#define ALL_COLOR_LED		0x01
#define BLINK_GREEN_LED		0x02
#define BLINK_RED_LED		0x04
#define BLINK_ORANGE_LED	0x10
#define SOLID_GREEN_LED		0x20
#define SOLID_RED_LED		0x40
#define SOLID_ORANGE_LED	0x80

#define CONNECT_STATE		BLINK_GREEN_LED
#define DISCONNECT_STATE	BLINK_ORANGE_LED
#define BOOTLOAD_STATE		SOLID_RED_LED
#define STREAMING_STATE		ALL_COLOR_LED
#define TUNING_STATE		SOLID_GREEN_LED
#define SEEKING_STATE		SOLID_GREEN_LED

//Stream State definitions
#define STREAM_COMMAND	0x36

#define STREAM_VIDPID	0x00
#define STREAM_AUDIO	0xFF

//Function definitions
#define SEEK_UP		true
#define SEEK_DOWN	false

#define TUNE_UP		true
#define TUNE_DOWN	false

//Volume Definitions
#define VOLUME_MIN	0		//0% - 100%
#define VOLUME_MAX	100

//Timeout definitions
#define	POLL_TIMEOUT_SECONDS		3
#define DISCONNECT_TIMEOUT_SECONDS	15

//Seek Threshold Definitions
#define MAX_SEEK_THRESHOLD			63
#define PREFERRED_SEEK_THRESHOLD	25

//Number of presets definition
#define PRESET_NUM	24

//Values used within the RadioData structure to assign values to the band, spacing, deemphasis, and gui options
#define DATA_BAND				0x20
#define DATA_BAND_875_108MHZ	0x00
#define DATA_BAND_76_90MHZ		0x20

#define DATA_SPACING			0x0C
#define DATA_SPACING_200KHZ		0x00
#define DATA_SPACING_100KHZ		0x04
#define DATA_SPACING_50KHZ		0x08

#define DATA_DEEMPHASIS			0x02
#define DATA_DEEMPHASIS_75		0x00
#define DATA_DEEMPHASIS_50		0x02

#define DATA_MONOSTEREO			0x10
#define DATA_MONOSTEREO_STEREO	0x00
#define DATA_MONOSTEREO_MONO	0x10

#define DATA_ALWAYSONTOP		0x80
#define DATA_SHOWINTRAY			0x40
#define DATA_SHOWINTITLEBAR		0x20
#define DATA_MUTEONSTARTUP		0x10
#define DATA_SCANTIME			0x0F

#define BLOCK_A 6
#define BLOCK_B 4
#define BLOCK_C 2
#define BLOCK_D 0


typedef enum
{
	RDS_0A  = ( 0 * 2 + 0), //Basic tuning and switching information only
	RDS_0B  = ( 0 * 2 + 1), //Basic tuning and switching information only
	RDS_1A  = ( 1 * 2 + 0), //Program Item Number and slow labeling codes only
	RDS_1B  = ( 1 * 2 + 1), //Program Item Number
	RDS_2A  = ( 2 * 2 + 0), //RadioText only
	RDS_2B  = ( 2 * 2 + 1), //RadioText only
	RDS_3A  = ( 3 * 2 + 0), //Applications Identification for ODA only
	RDS_3B  = ( 3 * 2 + 1), //Open Data Applications
	RDS_4A  = ( 4 * 2 + 0), //Clock-time and date only
	RDS_4B  = ( 4 * 2 + 1), //Open Data Applications
	RDS_5A  = ( 5 * 2 + 0), //Transparent Data Channels (32 channels) or ODA
	RDS_5B  = ( 5 * 2 + 1), //Transparent Data Channels (32 channels) or ODA
	RDS_6A  = ( 6 * 2 + 0), //In House applications or ODA
	RDS_6B  = ( 6 * 2 + 1), //In House applications or ODA
	RDS_7A  = ( 7 * 2 + 0), //Radio Paging or ODA
	RDS_7B  = ( 7 * 2 + 1), //Open Data Applications
	RDS_8A  = ( 8 * 2 + 0), //Traffic Message Channel or ODA
	RDS_8B  = ( 8 * 2 + 1), //Open Data Applications
	RDS_9A  = ( 9 * 2 + 0), //Emergency Warning System or ODA
	RDS_9B  = ( 9 * 2 + 1), //Open Data Applications
	RDS_10A = (10 * 2 + 0), //Program Type Name
	RDS_10B = (10 * 2 + 1), //Open Data Applications
	RDS_11A = (11 * 2 + 0), //Open Data Applications
	RDS_11B = (11 * 2 + 1), //Open Data Applications
	RDS_12A = (12 * 2 + 0), //Open Data Applications
	RDS_12B = (12 * 2 + 1), //Open Data Applications
	RDS_13A = (13 * 2 + 0), //Enhanced Radio Paging or ODA
	RDS_13B = (13 * 2 + 1), //Open Data Applications
	RDS_14A = (14 * 2 + 0), //Enhanced Other Networks information only
	RDS_14B = (14 * 2 + 1), //Enhanced Other Networks information only
	RDS_15A = (15 * 2 + 0), //Defined in RBDS only
	RDS_15B = (15 * 2 + 1)  //Fast switching information only
}RDS_GROUP_TYPE;

#define TA_VALIDATE_LIMIT	2
#define PI_VALIDATE_LIMIT	2
#define PSSIZE				8
#define PS_VALIDATE_LIMIT	2
#define PTYN_VALIDATE_LIMIT 2
#define PTYNSIZE			8


inline std::wstring str_to_wstr(const char *str, int length, UINT cp = CP_ACP)
{
	int len = MultiByteToWideChar(cp, 0, str, length, 0, 0);
	if (!len)
		return L"";

	std::vector<wchar_t> wbuff(len + 1);
	// NOTE: this does not NULL terminate the string in wbuff, but this is ok
	//       since it was zero-initialized in the vector constructor
	if (!MultiByteToWideChar(cp, 0, str, length, &wbuff[0], len))
		return L"";

	return &wbuff[0];
}//str_to_wstr

#endif // Enums_h__