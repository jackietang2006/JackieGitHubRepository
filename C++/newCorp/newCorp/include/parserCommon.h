#ifndef PARSERCOMMON_H
#define PARSERCOMMON_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <stdint.h>
#include <stdlib.h>   
#include <string.h>
using namespace std;
using std::cout;
using std::endl;
using std::ifstream;

#define FALSE 0
#define TRUE !(FALSE)
const uint32_t NETMASK[] = 
   {0x80000000, 0xc0000000, 0xe0000000, 0xf0000000,
    0xf8000000, 0xfc000000, 0xfe000000, 0xff000000,
    0xff800000, 0xffc00000, 0xffe00000, 0xfff00000,
    0xfff80000, 0xfffc0000, 0xfffe0000, 0xffff0000,
    0xffff8000, 0xffffc000, 0xffffe000, 0xfffff000,
    0xfffff800, 0xfffffc00, 0xfffffe00, 0xffffff00,
    0xffffff80, 0xffffffc0, 0xffffffe0, 0xfffffff0,
    0xfffffff8, 0xfffffffc, 0xfffffffe, 0xffffffff};
const int32_t MAX_CHARS_PER_LINE = 1024;
const int32_t MAX_NAME_LEN = 48;
const int32_t MAX_TOKENS_PER_LINE = 20;
const char* const DELIMITER = " ";
const char* const nameDELIMITER = "_";
const char* const asDELIMITER = " @";
const char* const jDELIMITER = " :,";

enum routerVendor {CISCO, CISCO_XR, JUNIPER, HUAWEI, UNKNOWN};
enum routerType {ALL, CORE, EDGE, CUSTOMER_EDGE};

enum ciscoParseState {
	CISCO_INITIALIZED,
	INTERFACE_INDEX_PARSING,
	RUNNING_CONFIG_PARSING,
	MPLS_FORWARDING_TABLE_PARSING,
	OSPF_DATABASE_PARSING,
	MPLS_L2TRANSPORT_PARSING,
	L2VPN_XCONNECT_PARSING};

enum ipIntParseState{
	IPINT_NULL,
	CISCO_PARSE_INTERFACE,
	CISCO_PARSE_LOOPBACK,
	CISCO_PARSE_SERIAL,
	CISCO_PARSE_MULTILINK,
	JUNIPER_PARSE_PHYSICAL,
	JUNIPER_PARSE_LOGICAL,
	JUNIPER_PARSE_CONFIG_GROUP,
	JUNIPER_PARSE_CONFIG_INTERFACE,
	JUNIPER_PARSE_CONFIG_SYSTEM,
	JUNIPER_PARSE_CONFIG_CHASSIS,
	JUNIPER_PARSE_CONFIG_FIREWALL,
	JUNIPER_PARSE_CONFIG_CLASS_OF_SERVICE,
	JUNIPER_PARSE_CONFIG_POLICY_OPTIONS,
	JUNIPER_PARSE_CONFIG_PROTOCOLS,
	JUNIPER_PARSE_CONFIG_ROUTING_OPTIONS,
	JUNIPER_PARSE_CONFIG_SNMP,
	JUNIPER_PARSE_MPLS0
	};

enum juniperParseState{
	JUNIPER_INITIALIZED,
	JUNIPER_PARSE_INTERFACE, 	//>show interfaces
	JUNIPER_PARSE_LDP,		//>show route protocol ldp detail
	JUNIPER_PARSE_CONFIGURATION,	//>show configuration
	JUNIPER_PARSE_L2CIRCUIT_CONNECTION,	//>show l2circuit connections
	JUNIPER_PARSE_ROUTE_PROTOCOL_LDP,	//>show route protocol ldp detail
	JUNIPER_PARSE_OSPF_DATABASE	//>show ospf database detail
	};

static bool	isDebug = FALSE;

static void ipInt2Char (const uint32_t addr, char* buf)
{
    uint8_t a1, a2, a3, a4;
    a1 = addr >> 24;
    a2 = addr >> 16;
    a3 = addr >> 8;
    a4 = addr;
    sprintf(buf, "%d.%d.%d.%d", a1, a2, a3, a4);
}

static int getTokenizedChar (char* inData, char* outData, char delimeter)
{
    int nlen = strlen (inData);
    int n1;
    for (n1 = 0; n1<nlen; n1++)
    {
	if (inData[n1] == delimeter) break;
	outData[n1] = inData[n1];
    }
    outData[n1] = '\0';
    return n1+1;
}

static uint32_t ipChar2Int (const char* buf )
{
    int a1, a2, a3, a4;
    int sr = sscanf(buf, "%d.%d.%d.%d", &a1, &a2, &a3, &a4);
    if(sr == 4) return ((a1 << 24) + (a2 << 16) + (a3 << 8) + a4);
    else return 0;
}

static bool ipChar2Int (const char* buf, uint32_t* a_low, uint32_t* a_high )
{
    int a1, a2, a3, a4, r1;
    uint32_t adr;
    int sr = sscanf(buf, "%d.%d.%d.%d/%d", &a1, &a2, &a3, &a4, &r1);
    if(sr != 5) return FALSE;
    if (r1 <1 || r1 > 32) return FALSE;
    adr =((a1 << 24) + (a2 << 16) + (a3 << 8) + a4);
    *a_low = adr & NETMASK[r1-1];
    *a_high = adr | (NETMASK[31]-NETMASK[r1-1]);
    return TRUE;
}

static void charToLower( char* buf)
{
    int len = strlen (buf);
    for (int n1=0; n1<len; n1++)
    {
	buf[n1] = tolower(buf[n1]);
    }
}

static bool isInIpRange (const char* buf, uint32_t aIP)
{
    int a1, a2, a3, a4, r1;
    uint32_t adr, a_low, a_high;
    int sr = sscanf(buf, "%d.%d.%d.%d/%d", &a1, &a2, &a3, &a4, &r1);
    if(sr != 5) return FALSE;
    if (r1 <1 || r1 > 32) return FALSE;
    adr =((a1 << 24) + (a2 << 16) + (a3 << 8) + a4);
    if ((aIP >= adr & NETMASK[r1-1]) &&
        (aIP <= adr | (NETMASK[31]-NETMASK[r1-1]))) return TRUE;
    return FALSE;
}
static int char2Int(u_char* buf, int nbyte)
{
    if (nbyte == 4) return ((buf[0]<<24) + (buf[1]<<16) + (buf[2]<<8) + buf[3]);
    if (nbyte == 3) return ((buf[0]<<16) + (buf[1]<<8) + buf[2]);
    if (nbyte == 2) return ((buf[0]<<8) + buf[1]);
    return buf[0];
}

static int compareString (const char* s1, const char* s2)
{
    // case insensitive comparision
    if (s1==NULL || s2==NULL) return -1;
    if (strlen(s1) != strlen(s2)) return -1;
    for (int n1=0; n1<strlen(s1); n1++)
    {
	if (tolower(s1[n1]) != tolower(s2[n1])) return -1;
    }
    return 0;
}
static int compareString (const char* s1, const char* s2, int nc)
{
    // case insensitive comparision

    if (s1==NULL || s2==NULL) return -1;
    
    if (strlen(s1)<nc || strlen(s2) < nc) return -1;
    for (int n1=0; n1<nc; n1++)
    {
	if (toupper(s1[n1]) != toupper(s2[n1]))
	{
	  return -1;
	}
    }
    return 0;
}
static bool isNumeric (char* s1)
{
    if (s1 == NULL || strlen (s1) == 0) return FALSE;
    for (int n1=0; n1 < strlen(s1); n1++)
    {
	if (s1[0] <'0' || s1[0] >'9') return FALSE;
    }
    return TRUE;
}
static bool isDecending (const char* aName, const char* bName)
{
    if (aName == NULL) return TRUE;
    if (bName == NULL) return FALSE;
	int len = strlen(aName);
	if (len > strlen(bName)) len = strlen(bName);
	for (int n1=0; n1< len; n1++)
	{
	    if (aName[n1] == bName[n1]) continue;
	    if (aName[n1] < bName[n1]) return TRUE;
	    else return FALSE;
	}
	if (strlen(aName) < strlen(bName)) return TRUE;
	else return FALSE;
}
static void removeEndingSpace (char* buf, int nsize)
{
  int n;
  //printf("nsize=%d, ", nsize);
  for (n=nsize-1; n>1; n--)
  {
	//printf("n=%d, %02x", n, buf[n]);
	if (buf[n]!= ' ') break;
  }
  buf[n+1] = '\0';
}

static uint32_t findFirstNonSpace (char* buf)
{
  for (int n1=0; n1<strlen(buf); n1++)
  {
	if (buf[n1] > 32) return n1;
  }
  return 0;
}

struct xTime
{
    unsigned int year, mon, day, hour, min, sec;
};

#define YEAR_TO_DAYS(y) ((y)*365 + (y)/4 - (y)/100 + (y)/400)

static void uxTime(unsigned long unixtime, struct xTime *tm)
{
    /* First take out the hour/minutes/seconds - this part is easy. */

    tm->sec = unixtime % 60;
    unixtime /= 60;

    tm->min = unixtime % 60;
    unixtime /= 60;

    tm->hour = unixtime % 24;
    unixtime /= 24;

    /* unixtime is now days since 01/01/1970 UTC
     * Rebaseline to the Common Era */

    unixtime += 719499;

    /* Roll forward looking for the year.  This could be done more efficiently
     * but this will do.  We have to start at 1969 because the year we calculate here
     * runs from March - so January and February 1970 will come out as 1969 here.
     */
    for (tm->year = 1969; unixtime > YEAR_TO_DAYS(tm->year + 1) + 30; tm->year++)
        ;

    /* OK we have our "year", so subtract off the days accounted for by full years. */
    unixtime -= YEAR_TO_DAYS(tm->year);

    /* unixtime is now number of days we are into the year (remembering that March 1
     * is the first day of the "year" still). */

    /* Roll forward looking for the month.  1 = March through to 12 = February. */
    for (tm->mon = 1; tm->mon < 12 && unixtime > 367*(tm->mon+1)/12; tm->mon++)
        ;

    /* Subtract off the days accounted for by full months */
    unixtime -= 367*tm->mon/12;

    /* unixtime is now number of days we are into the month */

    /* Adjust the month/year so that 1 = January, and years start where we
     * usually expect them to. */
    tm->mon += 2;
    if (tm->mon > 12)
    {
        tm->mon -= 12;
        tm->year++;
    }

    tm->day = unixtime;
}

static void uxTime(unsigned long unixtime, char* cTime)
{
    struct xTime dt;
    uxTime (unixtime, &dt);
    sprintf(cTime,"%04d-%02d-%02d %02d:%02d:%02d", dt.year, dt.mon, dt.day, dt.hour, dt.min, dt.sec);
}

static void uxTime2(unsigned long unixtime, char* cTime)
{
    struct xTime dt;
    uxTime (unixtime, &dt);
    sprintf(cTime,"%04d/%02d/%02d-%02d:%02d:%02d", dt.year, dt.mon, dt.day, dt.hour, dt.min, dt.sec);
}

static time_t uxTime(char* cTime)
{
  struct tm tm;
  if (strptime(cTime, "%Y-%m-%d %H:%M:%S", &tm) != NULL) return mktime(&tm);
  return 0;
}
  
#endif //PARSERCOMMON_H
