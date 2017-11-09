#include "../stdafx.h"
#include <conio.h>
#include <vector>
#include <dbghelp.h>
#include <iomanip>
#include <sstream>

static char *szCompiledFile=__FILE__;

DWORD ERR_ATTRIBUTE_IS_SENSITIVE = 0x40000008;
DWORD ERR_OBJECT_HASNT_ATTRIBUTE = 0x40000009;

class initRand {
public:
	initRand() {
		SYSTEMTIME tm;
		GetSystemTime(&tm);
		RAND_seed(&tm, sizeof(SYSTEMTIME));
	}
} _initRand;

BYTE hex2byte(char h) {
	if (h>='0' && h<='9') return(h-'0');
	if (h>='A' && h<='F') return(h+10-'A');
	if (h>='a' && h<='f') return(h+10-'a');
	return(0);
}

bool ishexdigit(char c) {
	if (c>='0' && c<='9') return true;
	if (c>='a' && c<='f') return true;
	if (c>='A' && c<='F') return true;
	return false;
}

DWORD countHexData(const char *data)
{
	DWORD cnt=0;
	size_t slen=strlen(data);
	for (size_t i=0;i<slen;i++) {
		if (isspace(data[i]) || data[i]==',') continue;
		if (!isxdigit(data[i])) {
			throw  CStringException("%s","Carattere non valido");
		}

		if ((i<slen-3) && data[i]=='0' && data[i+3]=='h')
			continue;

		if ((i<slen-2) && data[i]=='0' && data[i+1]=='x') {
			i+=1;
			continue;
		}
		int v=hex2byte(data[i]);
		i++;
		if (i<slen) {
			if (isxdigit(data[i])) {
				v<<=4;
				v|=hex2byte(data[i]);
			}
			else if (!isspace(data[i]))
				throw  CStringException("%s","richiesto spazio");
		}
		cnt++;

		if (i<(slen-1) && data[i+1]=='h')
			i++;
	}
	return cnt;
}

DWORD setHexData(const char *data,BYTE *buf)
{
	DWORD cnt=0;
	size_t slen=strlen(data);
	for (size_t i=0;i<slen;i++) {
		if (isspace(data[i]) || data[i]==',') continue;
		if (!isxdigit(data[i])) {
			throw  CStringException("%s","Carattere non valido");
		}

		if ((i<slen-3) && data[i]=='0' && data[i+3]=='h')
			continue;

		if ((i<slen-2) && data[i]=='0' && data[i+1]=='x') {
			i+=1;
			continue;
		}
		int v=hex2byte(data[i]);
		i++;
		if (i<slen) {
			if (isxdigit(data[i])) {
				v<<=4;
				v|=hex2byte(data[i]);
			}
			else if (!isspace(data[i]))
				throw  CStringException("%s","richiesto spazio");
		}
		buf[0]=v;
		buf++;
		cnt++;

		if (i<(slen-1) && data[i+1]=='h')
			i++;
	}
	return cnt;
}

void readHexData(const char *data,ByteDynArray &ba)
{
	std::vector<BYTE> dt;


	size_t slen=strlen(data);
	for (size_t i=0;i<slen;i++) {
		if (isspace(data[i]) || data[i]==',') continue;
		if (!isxdigit(data[i])) {
			throw  CStringException("%s","Carattere non valido");
		}

		if ((i<slen-3) && data[i]=='0' && data[i+3]=='h')
			continue;

		if ((i<slen-2) && data[i]=='0' && data[i+1]=='x') {
			i+=1;
			continue;
		}
		int v=hex2byte(data[i]);
		i++;
		if (i<slen) {
			if (isxdigit(data[i])) {
				v<<=4;
				v|=hex2byte(data[i]);
			}
			else if (!isspace(data[i]))
				throw  CStringException("%s","richiesto spazio");
		}
		dt.push_back(v);

		if (i<(slen-1) && data[i+1]=='h')
			i++;
	}

	if (dt.size())
		ba.alloc_copy(&dt[0],(DWORD)dt.size());
	else 
		ba.clear();
}

std::string HexByte(BYTE data, bool uppercase) {
	std::stringstream dmp;
	dmp << std::hex << std::setfill('0');
	if (uppercase)
		dmp << std::uppercase;
	dmp << std::setw(2) << static_cast<unsigned>(data);
	return dmp.str();
}

std::string &dumpHexData(ByteArray &data, std::string &dump)
{
	dumpHexData(data,dump,true,true);
	return dump;
}

std::string &dumpHexData(ByteArray &data, std::string &dump, bool withSpace, bool uppercase)
{
	std::stringstream dmp;
	dmp << std::hex << std::setfill('0');
	if (uppercase)
		dmp << std::uppercase;
	for (DWORD i = 0; i<data.size(); i++) {
		dmp << std::setw(2) << static_cast<unsigned>(data[i]);
		if (withSpace)
			dmp << " ";
	}
	dump = dmp.str();
	return dump;
}
void Debug(ByteArray ba) {
	std::string out;
	dumpHexData(ba,out);
	OutputDebugString(out.c_str());
	OutputDebugString("\n");
}

std::string &dumpHexDataLowerCase(ByteArray &data, std::string &dump)
{
	return dumpHexData(data, dump, false, false);
}

void PutPaddingBT0(ByteArray &ba,DWORD dwLen)
{
	init_func

	if (dwLen>ba.size())
		throw CStringException("Lunghezza del padding errata");

	ba.left(ba.size()-dwLen).fill(0);
	exit_func
}


void PutPaddingBT1(ByteArray &ba,DWORD dwLen)
{
	init_func
	if (dwLen>ba.size()-3)
		throw CStringException("Lunghezza del padding errata");

	ba[0]=0;
	ba[1]=1;
	ba.mid(2,ba.size()-dwLen-3).fill(0xff);
	ba[ba.size()-dwLen-1]=0;
	exit_func
}

void PutPaddingBT2(ByteArray &ba,DWORD dwLen)
{
	init_func
	if (dwLen>ba.size()-3)
		throw CStringException("Lunghezza del padding errata");

	ba[0]=0;
	ba[1]=2;
	for (DWORD i=2;i<ba.size()-dwLen;i++) {
		BYTE b=0;
		while (b==0)
			b=rand()%0x100;
		ba[i]=b;
	}
	ba[ba.size()-dwLen-1]=0;
	exit_func
}

DWORD RemoveSha1(ByteArray &paddedData) {
	static BYTE SHA1Algo[] = { 0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a, 0x05, 0x00, 0x04, 0x14 };
	if (paddedData.left(sizeof(SHA1Algo)) == VarToByteArray(SHA1Algo))
		return sizeof(SHA1Algo);
	throw CStringException("OID Algoritmo SHA1 non presente");
}

DWORD RemoveSha256(ByteArray &paddedData) {
	static BYTE SHA256Algo[] = { 0x30, 0x31, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20 };
	if (paddedData.left(sizeof(SHA256Algo)) == VarToByteArray(SHA256Algo))
		return sizeof(SHA256Algo);
	throw CStringException("OID Algoritmo SHA256 non presente");
}

DWORD RemovePaddingBT1(ByteArray &paddedData)
{
	init_func
	if (paddedData[0]!=0)
		throw CStringException("Errore nel padding");
	if (paddedData[1]!=1)
		throw CStringException("Errore nel padding");
	for (DWORD i=2;i<paddedData.size();i++) {
		if (paddedData[i]==0) {
			return i+1;
		}
		if (paddedData[i]!=0xff)
			throw CStringException("Errore nel padding");
	}
	throw CStringException("Errore nel padding");
	exit_func
}

DWORD RemovePaddingBT2(ByteArray &paddedData)
{
	init_func
	if (paddedData[0]!=0)
		throw CStringException("Errore nel padding");
	if (paddedData[1]!=2)
		throw CStringException("Errore nel padding");
	for (DWORD i=2;i<paddedData.size();i++)
		if (paddedData[i]==0) {
			return i+1;
		}
	throw CStringException("Errore nel padding");
	exit_func
}

DWORD RemoveISOPad(ByteArray &paddedData)
{
	init_func
	for (int i=paddedData.size()-1;i>=0;i--) {
		if (paddedData[i]!=0) {
			if (paddedData[i]!=0x80) {
				throw CStringException("Errore nel padding");
			}
			else {
				return i;
			}
		}
	}
	throw CStringException("Errore nel padding");
	exit_func
}

DWORD ANSIPadLen(DWORD Len)
{
	if ((Len & 0x7) == 0)
		return(Len);
	else
		return(Len - (Len & 0x7) + 0x08);
}

void ANSIPad(ByteArray &Data,DWORD DataLen)
{
	init_func
	Data.mid(DataLen).fill(0);
	exit_func

}

DWORD ISOPadLen16(DWORD Len)
{
	if ((Len & 0x10f) == 0)
		return(Len + 16);
	else
		return(Len - (Len & 0x0f) + 0x10);
}

DWORD ISOPadLen(DWORD Len)
{
	if ((Len & 0x7) == 0)
		return(Len+8);
	else
		return(Len - (Len & 0x7) + 0x08);
}

void ISOPad(const ByteArray &Data,DWORD DataLen)
{
	init_func
	Data.mid(DataLen).fill(0);
	Data[DataLen]=0x80;
	exit_func
}

const ByteDynArray& ISOPad16(const ByteArray &data, ByteDynArray &resp) {
	init_func
		resp.resize(ISOPadLen16(data.size()));
	resp.copy(data);
	ISOPad(resp, data.size());
	_return(resp);
	exit_func
}

const ByteDynArray& ISOPad(const ByteArray &data,ByteDynArray &resp) {
	init_func
	resp.resize(ISOPadLen(data.size()));
	resp.copy(data);
	ISOPad(resp,data.size());
	_return (resp);
	exit_func
}

void randomize(ByteArray &ba) 
{
	for (DWORD i=0;i<ba.dwSize;i++)
		ba.pbtData[i]=rand()%0x100;
}

DWORD ByteArrayToInt(ByteArray &ba)
{
	init_func
	long val=0;
	for (DWORD i=0;i<ba.size();i++) {
		val<<=8;
		val|=ba[i];
	}
	return val;
	exit_func
}


static char szWinErrBuffer[300];
char * WinErr(HRESULT err) {
	FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_SYSTEM,NULL,err,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),szWinErrBuffer,300,NULL);
	return(szWinErrBuffer);
}

char _strErrorString[1000];
char _strAppoErrorString[1000];

char *err_string(const char *format,...) {
	va_list params;
	va_start (params, format);
	vsprintf_s(_strAppoErrorString,1000, format, params);
	va_end(params);
	strcpy_s(_strErrorString,_strAppoErrorString);
	return(_strErrorString);
}

char *  CardErr(DWORD dwSW) {
	char *msg;

	switch (dwSW) {
		case ERR_CARD_FILE_DEACTIVATED		: msg="File is deactivated"; break;
		case ERR_CARD_FILE_TERMINATED		: msg="File is terminated"; break;
		case ERR_CARD_AUTENTICATION_FAILED	: msg="Authentication failed"; break;
		case ERR_CARD_EEPROM_ERROR			: msg="EEPROM error; command aborted"; break;
		case ERR_CARD_LC_INVALID			: msg="LC invalid"; break;
		case ERR_CARD_LOGICAL_CHANNEL		: msg="Logical channel not supported"; break;
		case ERR_CARD_SM_MODE				: msg="SM mode not supported"; break;
		case ERR_CARD_CHAINING				: msg="Chaining error"; break;
		case ERR_CARD_FILE_STRUCTURE		: msg="Command cannot be used for file structure"; break;
		case ERR_CARD_ACCESS_RIGHT			: msg="Required access right not granted"; break;
		case ERR_CARD_BSOBJ_BLOCKED			: msg="BS object blocked"; break;
		case ERR_CARD_BSOBJ_FORMAT			: msg="BS object has invalid format"; break;
		case ERR_CARD_NO_RANDOM				: msg="Conditions of use not satisfied; no random number available"; break;
		case ERR_CARD_NO_CURRENT_EF			: msg="No current EF selected"; break;
		case ERR_CARD_NO_SM_KEY				: msg="Key object for SM not found"; break;
		case ERR_CARD_SMKEY_FORMAT			: msg="Key object used for SM has invalid format"; break;
		case ERR_CARD_INVALID_DATA_FIELD	: msg="Invalid parameters in data field"; break;
		case ERR_CARD_FUNCTION_NOT_SUPPORTED: msg="Function / mode not supported"; break;
		case ERR_CARD_FILE_NOT_FOUND		: msg="File not found"; break;
		case ERR_CARD_RECORD_NOT_FOUND		: msg="Record / object not found"; break;
		case ERR_CARD_NOT_ENOUGH_MEMORY		: msg="Not enough memory in file / in file system"; break;
		case ERR_CARD_TLV_LENGTH			: msg="LC does not fit the TLV structure of the data field"; break;
		case ERR_CARD_P1_P2_INVALID			: msg="P1/P2 invalid"; break;
		case ERR_CARD_LC_P1_P2_INVALID		: msg="LC does not fit P1/P2"; break;
		case ERR_CARD_OBJECT_NOT_FOUND		: msg="Object not found (GET DATA)"; break;
		case ERR_CARD_EF_ALREADY_EXISTS		: msg="File already exists"; break;
		case ERR_CARD_DF_ALREADY_EXISTS		: msg="DF name already exists"; break;
		case ERR_CARD_LE_TOO_SMALL			: msg="LE does not fit the data to be sent"; break;
		case ERR_CARD_INS_INVALID			: msg="INS invalid"; break;
		case ERR_CARD_CLA_INVALID			: msg="CLA invalid (Hi nibble)"; break;
		case ERR_CARD_TECHNICAL_ERROR		: msg="Technical error"; break;
		case ERR_CARD_LCYCLE_DEATH			: msg="Card life cycle was set to death"; break;
		case ERR_CARD_FAIL_CORRUPT			: msg="Code file corrupted and terminated"; break;
		case ERR_CARD_CHECKSUM_ERROR		: msg="File is invalid because of checksum error"; break;
		case ERR_CARD_NOT_ENOUGH_XXRAM		: msg="Not enough memory available in XRAM"; break;
		case ERR_CARD_TRANSACTION_ERROR		: msg="Transaction error"; break;
		case ERR_CARD_GENERAL_PROTECTION_FAULT	: msg="General protection fault"; break;
		case ERR_CARD_INTERNAL_FAULT		: msg="Internal failure of PK-API (wrong CCMS format?)"; break;
		case ERR_CARD_KEY_NOT_FOUND			: msg="Key object not found"; break;
		case ERR_CARD_HARDWARE_ATTACK		: msg="Internal hardware attack detected, change to life cycle death"; break;
		case ERR_CARD_TRANS_BUFFER_TOO_SMALL: msg="Transaction buffer too small"; break;
		case ERR_CARD_INTERNAL_ERROR		: msg="Internal assertion"; break;
		case CARD_OK						: msg="OK"; break;
		case CARD_OK_SECOND_TRIAL			: msg="OK, EEPROM written in second trial"; break;
		case ERR_CARD_OVERFLOW_UNDERFLOW	: msg="Overflow through INCREASE / Underflow through DECREASE"; break;
		default: msg="Unknown status code"; break;
	}

	return(msg);
}


char *SystemErr(DWORD dwExcept){
	switch (dwExcept){
		case EXCEPTION_ACCESS_VIOLATION: return "Access Violation"; break;
 		case EXCEPTION_DATATYPE_MISALIGNMENT: return "Datatype Misalignment"; break;
 		case EXCEPTION_BREAKPOINT: return "Breakpoint"; break;
 		case EXCEPTION_SINGLE_STEP: return "Single Step"; break;
 		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "Array Bounds Exceeded"; break;
 		case EXCEPTION_FLT_DENORMAL_OPERAND: return "Flt Denormal Operand"; break;
 		case EXCEPTION_FLT_DIVIDE_BY_ZERO: return "Flt Divide By ZERO"; break;
 		case EXCEPTION_FLT_INEXACT_RESULT: return "Flt Inexact Result"; break;
 		case EXCEPTION_FLT_INVALID_OPERATION: return "Flt Invalid Operation"; break;
 		case EXCEPTION_FLT_OVERFLOW: return "Flt Overflow"; break;
 		case EXCEPTION_FLT_STACK_CHECK: return "Flt Stack Check"; break;
 		case EXCEPTION_FLT_UNDERFLOW: return "Flt Underflow"; break;
 		case EXCEPTION_INT_DIVIDE_BY_ZERO: return "Int Divide By Zero"; break;
 		case EXCEPTION_INT_OVERFLOW: return "Int Overflow"; break;
 		case EXCEPTION_PRIV_INSTRUCTION: return "Priv Instruction"; break;
 		case EXCEPTION_IN_PAGE_ERROR: return "In Page Error"; break;
 		case EXCEPTION_ILLEGAL_INSTRUCTION: return "Illegal Instruction"; break;
 		case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "Noncontinuable Exception"; break;
		case EXCEPTION_STACK_OVERFLOW: return "Stack Overflow"; break;
 		case EXCEPTION_INVALID_DISPOSITION: return "Invalid Disposition"; break;
 		case EXCEPTION_GUARD_PAGE: return "Guard Page"; break;
 		case EXCEPTION_INVALID_HANDLE: return "Invalid Handle"; break;
		default: return ""; break;
	}
}

ByteDynArray& setASN1Tag(ByteDynArray& result,DWORD tag,ByteArray &content) {
	content.setASN1Tag(tag,result);
	return result;
}

int ASN1TLength(DWORD tag) {
	int tLen=0;
	while (tag!=0) {
		tLen++;
		tag>>=8;
	}
	return tLen;
}
void putASN1Tag(DWORD tag,ByteArray &data) {
	int tPos=0;
	while (tag!=0) {
		BYTE b=tag >> 24;
		if (b!=0) {
			data[tPos]=b;
			tPos++;
		}
		tag<<=8;
	}
}

int ASN1LLength(LONG len) {
	if (len<0x80)
		return 1;
	else {
		// vediamo quanti byte mi servono...
		if (len<=0xff)
			return 2;
		else if (len<=0xffff)
			return 3;
		else if (len<=0xffffff)
			return 4;
		else if (len<=0xffffffff)
			return 5;
	}
	throw CStringException("Lunghezza ASN1 non valida");
}

void putASN1Length(LONG len,ByteArray &data) {
	if (len<0x80) {
		data[0]=(BYTE)len;
	}
	else {
		if (len<=0xff) {
			data[0]=(0x81);
			data[1] = (BYTE)(len);
		}
		else if (len<=0xffff) {
			data[0]=(0x82);
			data[1] = (BYTE)(len >> 8);
			data[2]=(len & 0xff);
		}
		else if (len<=0xffffff) {
			data[0]=(0x83);
			data[1] = (BYTE)(len >> 16);
			data[2]=((len >> 8) & 0xff);
			data[3]=(len & 0xff);
		}
		else if (len<=0xffffffff) {
			data[0]=(0x84);
			data[1]=(len >> 24);
			data[2]=((len >> 16) & 0xff);
			data[3]=((len >> 8) & 0xff);
			data[4]=(len & 0xff);
		}
	}
}

BYTE checkdigit(ByteArray &data) {
	int weight[3] = {7,3,1};
	int tot=0;
	int curval=0;
	for(DWORD i = 0;i<data.size();i++) {
		char ch=toupper(data[i]);
		if (ch >= 'A' && ch <= 'Z')
            curval = ch - 'A' + 10;
		else {
			if (ch >= '0' && ch <= '9')
                curval = ch - '0';
			else {
				if (ch == '<')
                    curval = 0;
                else
                    throw CStringException("errore nel calcolo della check digit");
			}
		}
        tot += curval * weight[i % 3];
	}
    return '0' + (tot % 10);
}

void ICAODate(SYSTEMTIME &time,ByteDynArray &result) {
	result.resize(6);
	result[0]=(time.wYear % 100) /10;
	result[1]=time.wYear % 10;

	result[2]=(time.wMonth % 100) /10;
	result[3]=time.wMonth % 10;

	result[4]=(time.wDay % 100) /10;
	result[5]=time.wDay % 10;
}
