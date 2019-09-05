#ifndef Structures_h__
#define Structures_h__

typedef struct  
{
	//TMC 3A
	BYTE LTN;		// Location Table Number: 0 encrypted, other clear
	BYTE AFI;		// Alternative Frequency Indicator
	BYTE M;			// Mode of transmission 
	BYTE I;			// International
	BYTE N;			// National
	BYTE R;			// Regional
	BYTE U;			// Urban

	//TMC 3A continue
	BYTE G;			// Gap parameter
	BYTE sID;		// Service ID

	//TMC 3A mode of transmission 1
	BYTE Ta;		// Activity Time
	BYTE Tw;		// Window Time
	BYTE Td;		// Delay Time

	//TMC 8A (clear)
	BYTE T;			// 0 User Message, 1 Tuning Information
	BYTE F;			// 0 Multi group Message, 1 Single Message
	BYTE DP_CI;		// Duration and Persistence in SingleGroup, Continuity Index in Multi Group

	//TMC 8A single group
	BYTE D;			// Diversion
	BYTE Y14_SG;	// Single Group: 0 positive direction, 1 negative direction / Multi group: 0 for third, fourth, fifth Message, 1 for second
	BYTE _extent;	// extent of event
	WORD _event;	// event in TMDD
	WORD location;	// location in TMC database

	//TMC 8a multi group
	BYTE GSI;		// Group Sequence Values
	BYTE label;		// 4 bit label of subsequent multi groups
	WORD labelData;	// x bit defined by label for additional information
	BYTE lastLabel; // last label if not enough label data 

	//TMC 8A (encrypted)
	BYTE encrypted;				// 5 less significant bits of block RDSB = 0
	BYTE encryptedVariantCode;	// variant 0 defined, other rfu
	BYTE encryptedTest;			// test bits
	BYTE encID;					// encryption Identifier
	BYTE LTNBE;					// Location Table Number Before Encryption

}SKYRADIO_TMCStruct;

typedef struct 
{
	BYTE contentType;
	BYTE startMarker;
	BYTE lengthMarker;
}SKYRADIO_RTContent;

#endif // Structures_h__