/*
 * nfCollector.cpp - NF Collector utilizing UDP socket 
 * usage: nfCollector <port> -HOME ../data -ST 1371162300 -NRP 2
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 65535

/*
 * error - wrapper for perror
 */
void error(const char *msg) {
  perror(msg);
  exit(1);
}

/*
 * nfDecoder
 */
#include <iostream>
#include <sstream>
#include <dirent.h>
#include "netflow_v9.h"
#include "netflowV9Container.h"
#include "routerTemplateContainer.h"
#include "networkV9TemplateContainer.h"

using namespace std;
using std::cout;
using std::endl;
using std::ifstream;

// global variables
networkV9TemplateContainer* ntContainer;

uint32_t	startTrafficTime = 1371162300;
uint32_t	nReportPeriods = 2;
uint32_t	nSecondPerPeriod = 300;
int32_t		reportWindow0 = 0; 
int32_t		reportWindow1 = -1; 
uint32_t	fstWin, fedWin;

FILE* trafficFB[300];
FILE* errorFB;
FILE* outputUdpPacketsFB;

char taddr[MAX_NAME_LEN];
u_int16_t  fId[40], fValue[40];

//--------------declaration for export ----------
const uint32_t  MAXKEYFIELDS = 65535;
int32_t		etKeyBuf[MAXKEYFIELDS];
struct eTempKey
{
  char description[40];
  uint32_t key;
};
eTempKey exportTemp[120];
uint32_t nExportKey=0;
uint32_t exportBuf[120];

std::string get_env_var( std::string const & key )
{
  char * val;
  val = getenv( key.c_str() );
  std::string retval = "";
  if (val != NULL) {
    retval = val;
  }
  return retval;
}

//------------file sorting function--------------
static bool fileSort (char* aName, char* bName)
{
    if (aName == NULL) return TRUE;
    if (bName == NULL) return FALSE;
    int a1 = atoi (&aName[4]);
    int b1 = atoi (&bName[4]);

	if (a1 < b1) return TRUE;
	else return FALSE;
}
//-------------------------------------
//process netflow flowset data
//-----------------------------------
void parseNetflowSet (u_char* fset, short fsid, short sh,
		      NF9_HEADER* hdr, u_int32_t cliRouterIpAddr)
{
    u_int32_t sourceID= ntohl(hdr->source_id);
    u_int32_t timestamp = ntohl(hdr->time_sec);
    u_int32_t sysUpTime = ntohl(hdr->uptime_ms);
    u_int32_t routerIpAddr = ntohl(cliRouterIpAddr);
    int tdiff;
    //----------get template based on IP address---------------
    routerV9TemplateContainer* rtCont =ntContainer->getRtContainer(routerIpAddr);
    routerV9Template* routerTemp = NULL;
    if (rtCont != NULL) routerTemp = rtCont->getrouterV9Template (sourceID, fsid);
    if (rtCont == NULL || routerTemp ==NULL) 
    {
        char routerAddr[MAX_NAME_LEN];
	u_int32_t saddr = ntohl(cliRouterIpAddr);
	ipInt2Char (saddr, routerAddr);
	fprintf(errorFB,"Router=%s sourceID=%d no template with ID = %d found\n", routerAddr, sourceID, fsid);
	return;
    }
    v9_template* currentTemp= routerTemp->v9TemplateObj;
    if (currentTemp ==NULL) return;

    // parsing according to the template definition
    u_int32_t fieldValue;
    int len, n1, n2, n3, n4, t1,t2, nFlow;

    len = sh-4;
    n1 = 0;
    nFlow = 0;
    int expectedFlow = len / currentTemp->getFlowSizeInBytes();

    while (nFlow < expectedFlow)
    {
    	memset (exportBuf, 0, sizeof(exportBuf));
        // Initialize
	templateField* tFD = NULL;
        // loop through number of fields
	for (n2=0; n2<currentTemp->getNField(); n2++)
	{
	    tFD = currentTemp->getField(n2);
	    fieldValue = char2Int (&fset[n1],tFD->length);
	    n1 += tFD->length;
	    if (tFD->fieldID <0 || tFD->fieldID >=MAXKEYFIELDS) continue;
	    if (etKeyBuf[tFD->fieldID] <0) continue;
	    exportBuf[etKeyBuf[tFD->fieldID]] = fieldValue;
	}
	nFlow++;
	//printf("  Flow %d  ---------------\n", nFlow);
	//if ((n1%2)!=0) n1++; // pad to even number

	//------------- concise format---------------
	tdiff = exportBuf[1]-sysUpTime;
	uint32_t timePeriod=timestamp+tdiff/1000;
	//char timeText[24];
	//uxTime(timePeriod, timeText);
	if (fstWin == 0) fstWin = timePeriod;
	else if (fstWin > timePeriod) fstWin = timePeriod;
	if (fedWin < timePeriod) fedWin = timePeriod;
	int nf = (timePeriod - startTrafficTime)/nSecondPerPeriod;
	    char rIP[24];
	    ipInt2Char (routerIpAddr, rIP);
        if (nf >=0 && nf<nReportPeriods)
	{
	    char rIP[24];
	    ipInt2Char (routerIpAddr, rIP);
	    if (nf > reportWindow0)
	    {
		if (reportWindow1 >=0)
		{
		    char execHdfs[512];
		    uint32_t tff = reportWindow1 * nSecondPerPeriod + startTrafficTime;
		    //sprintf (execHdfs,"hadoop fs -copyFromLocal ../result/%u.csv network/traffic/%u.csv", tff, tff);
		    //cout << execHdfs << endl;
		    //printf ("store data to HDFS network/traffic/%u.csv\n", tff);
		    //system (execHdfs);
		}
		cout << "move report Window from " << reportWindow0 <<" to " << nf <<endl;
		reportWindow0 = nf;
		reportWindow1 = nf - 6;
	    }

    //------------------------print results---------------------------------------
	     fprintf(trafficFB[nf],"%s,%u",rIP, timePeriod);
	     for (int n1=2; n1<nExportKey; n1++)
	     {
		fprintf(trafficFB[nf],",%u",exportBuf[n1]);  
	     }
	     fprintf(trafficFB[nf],"\n");
	}

    } //while (nFlow < expectedFlow)
        //printf("n1=%d   ", n1);
	//if ((n2=n1%4)!=0 ) n1 = n1+4-n2;  // padding to round up to 4
	//printf("  ; n1=%d; n2=%d\n", n1 , n2);
}

void decodeOptionTemplate (u_char* fset, short sh, NF9_HEADER* hdr, u_int32_t cliRouterIpAddr)
{

    u_int32_t sourceID= ntohl(hdr->source_id);
    u_int32_t timestamp = ntohl(hdr->time_sec);
    u_int32_t sysUpTime = ntohl(hdr->uptime_ms);
    u_int16_t tID, tLen, sLen;
    int len, n1, n2;
    len=sh-4;
    n1 = 0;
    //printf("\n");
    while(n1<len)
    {
	tID = char2Int(&fset[n1],2);
	n1+=2;
	sLen = fset[n1++]<<8;
	sLen = sLen + fset[n1++];
	tLen = fset[n1++]<<8;
	tLen = tLen + fset[n1++];
	tLen = (tLen +sLen) /4;  //defined in bytes
	n2 = n1 + tLen*4;
	if (n2 > len)
	{
	    // this should not happen.  but did see some ill formed data
	    // skip this record
    	    //char taddr[MAX_NAME_LEN];
    	    u_int32_t saddr = ntohl(cliRouterIpAddr);
    	    ipInt2Char (saddr, taddr);
	    fprintf (errorFB, "router %s illed form tLen=%u > len=%u\n", taddr, tLen, len);
	    return;
	}
	for (n2=0; n2 < tLen; n2++)
	{
	  fId[n2]= fset[n1++]<<8;
	  fId[n2]= fId[n2]+fset[n1++];
	  fValue[n2]= fset[n1++]<<8;
	  fValue[n2]= fValue[n2]+fset[n1++];
	}
	
	//v9_template* v9Temp = new v9_template(tID, tLen, fId, fValue, TRUE);
	routerV9TemplateContainer * rtCont = ntContainer->getRtContainer(ntohl(cliRouterIpAddr));
	if (rtCont ==NULL) 
	{
	    rtCont = new routerV9TemplateContainer();
	    memset (rtCont, 0, sizeof(routerV9TemplateContainer));
	    rtCont->setRouterAddr(ntohl(cliRouterIpAddr));
	    ntContainer->insertRtContainer(rtCont);
	}
	
          rtCont->addTemplate (sourceID, tID, tLen, fId, fValue, TRUE);
	//cout<<"O";
	if ((n2=n1%4)!=0) n1 = n1+4-n2;  // padding to round up to 4
    }
}

//-----------------------------------------
//process netflow template data
//----------------------------------------
void decodeTemplate (u_char* fset, short sh, NF9_HEADER* hdr, u_int32_t cliRouterIpAddr)
{
    u_int32_t sourceID= ntohl(hdr->source_id);
    u_int32_t timestamp = ntohl(hdr->time_sec);
    u_int32_t sysUpTime = ntohl(hdr->uptime_ms);
    u_int16_t tID, tLen;
    int len, n1, n2;
    len=sh-4;
    n1 = 0;
    while(n1<len)
    {
	tID = fset[n1++]<<8;
	tID = tID + fset[n1++];
	tLen = fset[n1++]<<8;
	tLen = tLen + fset[n1++];
	n2 = n1 + tLen*4;
	if (n2 > len)
	{
	    // this should not happen.  but for the illed formed data
	    // skip this record
    	    //char taddr[MAX_NAME_LEN];
    	    u_int32_t saddr = ntohl(cliRouterIpAddr);
    	    ipInt2Char (saddr, taddr);
	    fprintf (errorFB, "router %s illed form tLen=%u > len=%u\n", taddr, tLen, len);
	    return;
	}
	for (n2=0; n2 < tLen; n2++)
	{
	  fId[n2]= fset[n1++]<<8;
	  fId[n2]= fId[n2]+fset[n1++];
	  fValue[n2]= fset[n1++]<<8;
	  fValue[n2]= fValue[n2]+fset[n1++];
	}
	
	routerV9TemplateContainer * rtCont = ntContainer->getRtContainer(ntohl(cliRouterIpAddr));
	if (rtCont ==NULL) 
	{
	    rtCont = new routerV9TemplateContainer();
	    memset (rtCont, 0, sizeof(routerV9TemplateContainer));
	    rtCont->setRouterAddr(ntohl(cliRouterIpAddr));
	    ntContainer->insertRtContainer(rtCont);
	}
	
          rtCont->addTemplate (sourceID, tID, tLen, fId, fValue, FALSE);
    }
}

//-------------------------------------
//process netflow v9 records
//-------------------------------------
long processNF9Packet(u_char *buf, int len, u_int32_t cliRouterIpAddr)
{
    //char taddr[MAX_NAME_LEN];
    struct NF9_HEADER *hdr = (struct NF9_HEADER*)buf;

    // get router who reports netflow
    u_int32_t saddr = ntohl(cliRouterIpAddr);

    if(ntohs(hdr->version) != 9)
    {

        ipInt2Char (saddr, taddr);
        fprintf(errorFB, "Router %s unrecognized netflow version = %u\n ", taddr, ntohs(hdr->version));
        return 0;
    }

  // process netflow flowset
  int offset = sizeof(NF9_HEADER);
  // IP header size = 14
  // udp header size = 8
  while (offset < len)
  {
    //cout <<"X";
    NF9_FLOWSET_HEADER_COMMON *fs_hdr = (NF9_FLOWSET_HEADER_COMMON *)&buf[offset];
    short fsid = htons(fs_hdr->flowset_id);
    short fsLen = htons(fs_hdr->length);
    if(fsLen <= 0) break;
    int oData = offset + sizeof(NF9_FLOWSET_HEADER_COMMON);
    short sh = fsLen;
    offset += fsLen;


    // process flowset
    switch (fsid)
    {
      case NF9_TEMPLATE_FLOWSET_ID:
	// template definition
	decodeTemplate (&buf[oData], sh, hdr, cliRouterIpAddr);
	break;
      case NF9_OPTIONS_FLOWSET_ID:
	// optional template
	decodeOptionTemplate (&buf[oData], sh, hdr, cliRouterIpAddr);
	break;
      default:
        // process netflow flowset data
	parseNetflowSet(&buf[oData], fsid, sh, hdr, cliRouterIpAddr);
	break;

    } // end process flowset
  } // end while
    return ntohs(hdr->flows);
}

int main(int argc, char **argv)
{
  int sockfd; /* socket */
  int portno; /* port to listen on */
  socklen_t clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  struct hostent *hostp; /* client host info */
  char buf[BUFSIZE]; /* message buf */
  char *clientaddrp; /* dotted decimal host addr string */
  int optval; /* flag value for setsockopt */
  int udpDataGramLen; /* message byte size */

  /* 
   * check command line arguments 
   */
  if (argc != 2) {
    error("usage: %s <port>");
  }
  portno = atoi(argv[1]);

  // read env variables for DataDir, ST(StartTimeStamp) & NRP(Numer of Reporting Periods)
  char netflowDataDir[MAX_CHARS_PER_LINE] ;
  char netflowTemp[MAX_CHARS_PER_LINE] ;
  char netflowOutput[MAX_CHARS_PER_LINE] ;

  string envVar = get_env_var((char*)"NetFlowDataDir");
  if (envVar.empty())
  {
    error("ENV Variable - NetFlowDataDir: not defined yet!");
  }
  cout << "ENV Variable - NetFlowDataDir: " << envVar << endl;
  // newflow data directory
  strcpy (netflowDataDir, envVar.c_str());
  // netflow template
  strcpy (netflowTemp, envVar.c_str());
  strcat (netflowTemp,"/control");
  // netflow output
  strcpy (netflowOutput, envVar.c_str());
  strcat (netflowOutput,"/netflow_csv");

  envVar = get_env_var((char*)"NetFlowST");
  if (envVar.empty())
  {
    printf("ENV Variable - NetFlowST: not defined, default to 1371162300\n");
  }
  else
  {
    cout << "ENV Variable - NetFlowST: " << envVar << endl;
    startTrafficTime = atol (envVar.c_str());
  }

  envVar = get_env_var((char*)"NetFlowNRP");
  if (envVar.empty())
  {
    printf("ENV Variable - NetFlowNRP: not defined, default to 2\n");
  }
  else
  {
    cout << "ENV Variable - NetFlowNRP: " << envVar << endl;
    nReportPeriods = atol (envVar.c_str());
  }

  //-----------------------------------------
  // open file descripters for error and reports
  char errName[MAX_CHARS_PER_LINE];
  strcpy (errName, netflowDataDir);
  strcat (errName,"/log/errorNetflowDecoder.log");
  errorFB=fopen(errName, "w");
  if (errorFB == NULL)
  {
    printf("fail to open log/errorNetflowDecoder.log, check the file priviledge\n");
    return 0;
  }

  // open file descripters to write UDP packets
  char outputUdpPackets[MAX_CHARS_PER_LINE];
  strcpy (outputUdpPackets, netflowDataDir);
  strcat (outputUdpPackets,"/log/outputUdpPackets.log");
  outputUdpPacketsFB=fopen(outputUdpPackets, "w");
  if (outputUdpPacketsFB == NULL)
  {
    printf("fail to open log/outputUdpPackeets.log, check the file priviledge\n");
    return 0;
  }

  //-------------define export template here--------
  //  The first two buffers are used to collect time stamp
  //  
  for (int i=0; i<MAXKEYFIELDS; i++)
  {
    etKeyBuf[i] = -1;
  }
  strcpy (exportTemp[0].description, "FIRST_SWITCHED");
  exportTemp[0].key = NF9_FIRST_SWITCHED;
  etKeyBuf[NF9_FIRST_SWITCHED] = 0;
  strcpy (exportTemp[1].description, "LAST_SWITCHED");
  exportTemp[1].key = NF9_LAST_SWITCHED;
  char etf[MAX_CHARS_PER_LINE] ;
  etKeyBuf[NF9_LAST_SWITCHED] = 1;
  nExportKey = 2;
  strcpy (etf, netflowDataDir);
  strcat (etf,"/control/exportTemplate.csv");
  //nExportKey =0;
  ifstream fin;
  fin.open(etf); // open a file
  if (!fin.good())
  {
    cout << "failed to open the export template file = "<< etf << endl;
    return -1;
  }
  while (!fin.eof())
  {
    fin.getline(etf, MAX_CHARS_PER_LINE);
    if (etf == NULL) continue;

    char* e0 = strtok(etf,",");
    char* e1 = strtok(NULL,",");
    if (e0 == NULL || e1 == NULL) continue;
    int nk = atoi(e1);
    if (nk < 0 || nk >= MAXKEYFIELDS) continue;
    if (nk == NF9_FIRST_SWITCHED || nk==NF9_LAST_SWITCHED) continue;
    strcpy (exportTemp[nExportKey].description, e0);
    exportTemp[nExportKey].key = nk;
    cout << exportTemp[nExportKey].description<<" ' "<<exportTemp[nExportKey].key<<endl;
    etKeyBuf[nk] = nExportKey;
    nExportKey++;
  }
  fin.close();
  //-------------start process here--------------
  string in;
  char input[MAX_CHARS_PER_LINE] = "";
  int ninput;
  ntContainer = new networkV9TemplateContainer();

  // get template
  ntContainer->initTemplateCSV(netflowTemp);

  // open output files
  long tt = startTrafficTime;
  for (int f1=0; f1<nReportPeriods; f1++)
  {
    char outName[MAX_CHARS_PER_LINE];
    sprintf(outName,"%s/%u.csv", netflowOutput,tt);
    trafficFB[f1]=fopen(outName, "w");
    if (trafficFB [f1]== NULL) return -1;
    tt = tt+nSecondPerPeriod;
  }

  // export template
  char tempName[MAX_CHARS_PER_LINE];
  strcpy (tempName, netflowTemp);
  strcat (tempName, "/allV9TemplatesInTheNetwork.csv");
  FILE* tempFB = NULL;

  /* 
   * socket: create the parent socket 
   */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
    error((char*)"ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   */
  optval = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int)) < 0)
    error((char*)"ERROR setsockopt: SO_REUSEADDR");
  int sndbuf = 13107200;
  int rcvbuf = 13107200;
  int optlen = sizeof optval;
  //if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const void *)&sndbuf, sizeof(int)) < 0)
  //  error((char*)"ERROR setsockopt: SO_SNDBUF");
  //if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (const void *)&rcvbuf, sizeof(int)) < 0)
  //  error((char*)"ERROR setsockopt: SO_RCVBUF");

  /*
   * getsockopt: Query UDP socket's SNDBUF and RCVBUF
   */
  sndbuf = 0;
  rcvbuf = 0;
  if (getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (void *)&sndbuf, (socklen_t*)&optlen) < 0)
    error((char*)"ERROR getsockopt: SO_SNDBUF");
  if (getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (void *)&rcvbuf, (socklen_t*)&optlen) < 0)
    error((char*)"ERROR getsockopt: SO_RCVBUF");
  printf("Querying socket SNDBUF = %d, RCVBUF = %d\n", sndbuf, rcvbuf);

  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)portno);

  /* 
   * bind: associate the parent socket with a port 
   */
  if (bind(sockfd, (struct sockaddr *) &serveraddr, 
	   sizeof(serveraddr)) < 0) 
    error((char*)"ERROR on binding");

  /* 
   * main loop: wait for a datagram, then echo it
   */
  clientlen = sizeof(clientaddr);
  long	   totalNfRecords = 0;

  while (1) {
    /*
     * recvfrom: receive a UDP datagram from a NetFlow Exportor
     */
    bzero(buf, BUFSIZE);
    udpDataGramLen = recvfrom(sockfd, buf, BUFSIZE, 0,
		 (struct sockaddr *) &clientaddr, &clientlen);
    if (udpDataGramLen < 0)
      error((char*)"ERROR in recvfrom");

    /*
     * inet_ntoa: determine where clients come from
     */
    clientaddrp = inet_ntoa(clientaddr.sin_addr);
    if (clientaddrp == NULL)
      error((char*)"ERROR on inet_ntoa\n");

    totalNfRecords ++;

    /* 
     * gethostbyaddr: determine who sent the datagram, which might be expensive due to nslookup
    hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
			  sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    //do Not return on Null of hostp as clients might be using raw socket to spoof source
    //if (hostp == NULL) error((char*)"ERROR on gethostbyaddr");

    if (hostp)
      fprintf(outputUdpPacketsFB, "Received datagram#%d (%d bytes) from %s (%s:%d)\n", totalNfRecords, udpDataGramLen, hostp->h_name, clientaddrp, ntohs(clientaddr.sin_port));
    else
      fprintf(outputUdpPacketsFB, "Received datagram#%d (%d bytes) from Spoofing Sourece (%s:%d)\n", totalNfRecords, udpDataGramLen, clientaddrp, ntohs(clientaddr.sin_port));
     */

    fprintf(outputUdpPacketsFB, "Received datagram#%d (%d bytes) from Sourece (%s:%d)\n", totalNfRecords, udpDataGramLen, clientaddrp, ntohs(clientaddr.sin_port));

    /* 
     * print UDP datagram in hex format
     */
    #define BYTETOHEX "%02X "
    //for(int i = 0; i < udpDataGramLen; i++)
    //{
    //  fprintf(outputUdpPacketsFB, BYTETOHEX, buf[i] & 0xFF);
    //}
    //fprintf(outputUdpPacketsFB, "\n");
    fflush(outputUdpPacketsFB);

    //nfDecoder
    u_char *packet = (u_char*)buf;

    fstWin = 0;
    fedWin = 0;

    long totalFlow = 0;
    long  nflow;

    // process netflow v9 record here
    nflow = processNF9Packet(packet, udpDataGramLen, clientaddr.sin_addr.s_addr);
    totalFlow += nflow;

    tempFB = fopen(tempName, "w");
    // export template
    ntContainer->printTemplateAll(tempFB);
    fclose (tempFB);
  }

  //exit
  for (int f1=0; f1<nReportPeriods; f1++)
  {
    fclose (trafficFB[f1]);
    trafficFB[f1]=NULL;
  }
}
