/*
 * Usage: netflowDecoder -HOME ../data -ST 1371162300 -NRP 2
 */
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
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

long	   totalNfRecord;

uint32_t	startTrafficTime = 1371099600;
uint32_t	nReportPeriods = 300;
uint32_t	nSecondPerPeriod = 300;
int32_t		reportWindow0 = 0; 
int32_t		reportWindow1 = -1; 
uint32_t		fstWin, fedWin;

FILE* trafficFB[300];
FILE* errorFB;

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
			NF9_HEADER* hdr, struct iphdr * ip_hdr)
{
    if(isDebug)cout<< "enter parseNetflowSet" << endl;

    u_int32_t sourceID= ntohl(hdr->source_id);
    u_int32_t timestamp = ntohl(hdr->time_sec);
    u_int32_t sysUpTime = ntohl(hdr->uptime_ms);
    u_int32_t routerIpAddr = ntohl(ip_hdr->saddr);
    int tdiff;
    //----------get template based on IP address---------------
    routerV9TemplateContainer* rtCont =ntContainer->getRtContainer(routerIpAddr);
    routerV9Template* routerTemp = NULL;
    if (rtCont != NULL) routerTemp = rtCont->getrouterV9Template (sourceID, fsid);
    if (rtCont == NULL || routerTemp ==NULL) 
    {
        char routerAddr[MAX_NAME_LEN];
	u_int32_t saddr = ntohl(ip_hdr->saddr);
	ipInt2Char (saddr, routerAddr);
	fprintf(errorFB,"Router=%s sourceID=%d no template with ID = %d found\n", routerAddr, sourceID, fsid);
	return;
    }
    v9_template* currentTemp= routerTemp->v9TemplateObj;
    if (currentTemp ==NULL) return;

    // parsing according to the template definition
    u_int32_t fieldValue;
    int len, n1, n2, n3, n4, t1,t2, nFlow;


    //n1 = 0;
    len = sh-4;
    n1 = 0;
    nFlow = 0;
    int expectedFlow = len / currentTemp->getFlowSizeInBytes();
    if(isDebug)printf("flow size =%d, expected number of flows =%d\n", currentTemp->getFlowSizeInBytes(), expectedFlow);


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
	    if(isDebug)printf (" 	( %u, %u,  %u)", tFD->fieldID, etKeyBuf[tFD->fieldID], fieldValue);
	    if (tFD->fieldID <0 || tFD->fieldID >=MAXKEYFIELDS) continue;
	    if (etKeyBuf[tFD->fieldID] <0) continue;
	    exportBuf[etKeyBuf[tFD->fieldID]] = fieldValue;
	}
	if(isDebug)printf("\n");
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
	if(isDebug)printf( "	Router=%s lastTime=%u uptime=%u timestamp=%u timeperiod=%u starttime=%u nf=%d\n",rIP, exportBuf[1], sysUpTime, timestamp, timePeriod, startTrafficTime, nf);
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

void decodeOptionTemplate (u_char* fset, short sh, NF9_HEADER* hdr, struct iphdr *ip_hdr)
{

    u_int32_t sourceID= ntohl(hdr->source_id);
    u_int32_t timestamp = ntohl(hdr->time_sec);
    u_int32_t sysUpTime = ntohl(hdr->uptime_ms);
    if(isDebug)printf("start decodeOptionTemplate;  length = %d\n", sh);
    u_int16_t tID, tLen, sLen;
    int len, n1, n2;
    len=sh-4;
    n1 = 0;
    //printf("\n");
    while(n1<len)
    {
	tID = char2Int(&fset[n1],2);
	n1+=2;
	if(isDebug)printf ("Template ID = %d,", tID);
	sLen = fset[n1++]<<8;
	sLen = sLen + fset[n1++];
	tLen = fset[n1++]<<8;
	tLen = tLen + fset[n1++];
	tLen = (tLen +sLen) /4;  //defined in bytes
	if(isDebug)printf (" scope length = %d option length=%d\n",sLen, tLen);
	n2 = n1 + tLen*4;
	if (n2 > len)
	{
	    // this should not happen.  but did see some ill formed data
	    // skip this record
    	    //char taddr[MAX_NAME_LEN];
    	    u_int32_t saddr = ntohl(ip_hdr->saddr);
    	    ipInt2Char (saddr, taddr);
	    fprintf (errorFB, "router %s illed form tLen=%u > len=%u\n", taddr, tLen, len);
	    return;
	}
	for (n2=0; n2 < tLen; n2++)
	{
	  fId[n2]= fset[n1++]<<8;
	  fId[n2]= fId[n2]+fset[n1++];
	  if(isDebug)printf("      Field ID = %d, ",fId[n2]);
	  fValue[n2]= fset[n1++]<<8;
	  fValue[n2]= fValue[n2]+fset[n1++];
	  if(isDebug)printf("      Field length = %d\n, ",fValue[n2]);
	}
	
	//v9_template* v9Temp = new v9_template(tID, tLen, fId, fValue, TRUE);
	routerV9TemplateContainer * rtCont = ntContainer->getRtContainer(ntohl(ip_hdr->saddr));
	if (rtCont ==NULL) 
	{
	    rtCont = new routerV9TemplateContainer();
	    memset (rtCont, 0, sizeof(routerV9TemplateContainer));
	    rtCont->setRouterAddr(ntohl(ip_hdr->saddr));
	    ntContainer->insertRtContainer(rtCont);
	}
	
	  if(isDebug)printf("router=%s sourceID=%d add template=%d\n", taddr, sourceID, tID);
          rtCont->addTemplate (sourceID, tID, tLen, fId, fValue, TRUE);
	//cout<<"O";
	if ((n2=n1%4)!=0) n1 = n1+4-n2;  // padding to round up to 4
	if(isDebug)printf("end of the template; n1=%d\n", n1);
    }
    if(isDebug)cout << "end of decodeOptionTemplate" << endl;
}

//-----------------------------------------
//process netflow template data
//----------------------------------------
void decodeTemplate (u_char* fset, short sh, NF9_HEADER* hdr, struct iphdr *ip_hdr)
{
    if(isDebug) cout<<"decodeTemplate ----"<<endl;
    u_int32_t sourceID= ntohl(hdr->source_id);
    u_int32_t timestamp = ntohl(hdr->time_sec);
    u_int32_t sysUpTime = ntohl(hdr->uptime_ms);
    if(isDebug)printf("	start decodeTemplate;  length = %d source ID = %d\n", sh, sourceID);
    u_int16_t tID, tLen;
    int len, n1, n2;
    len=sh-4;
    n1 = 0;
    if(isDebug)printf("\n");
    while(n1<len)
    {
	tID = fset[n1++]<<8;
	tID = tID + fset[n1++];
	if(isDebug)printf ("Template ID = %d,", tID);
	tLen = fset[n1++]<<8;
	tLen = tLen + fset[n1++];
	if(isDebug)printf ("	length = %d\n", tLen);
	n2 = n1 + tLen*4;
	if (n2 > len)
	{
	    // this should not happen.  but for the illed formed data
	    // skip this record
    	    //char taddr[MAX_NAME_LEN];
    	    u_int32_t saddr = ntohl(ip_hdr->saddr);
    	    ipInt2Char (saddr, taddr);
	    fprintf (errorFB, "router %s illed form tLen=%u > len=%u\n", taddr, tLen, len);
	    return;
	}
	for (n2=0; n2 < tLen; n2++)
	{
	  fId[n2]= fset[n1++]<<8;
	  fId[n2]= fId[n2]+fset[n1++];
	  if(isDebug)printf("      Field ID = %d, ",fId[n2]);
	  fValue[n2]= fset[n1++]<<8;
	  fValue[n2]= fValue[n2]+fset[n1++];
	  if(isDebug)printf("      Field length = %d\n, ",fValue[n2]);
	}
	
	routerV9TemplateContainer * rtCont = ntContainer->getRtContainer(ntohl(ip_hdr->saddr));
	if (rtCont ==NULL) 
	{
	    rtCont = new routerV9TemplateContainer();
	    memset (rtCont, 0, sizeof(routerV9TemplateContainer));
	    rtCont->setRouterAddr(ntohl(ip_hdr->saddr));
	    ntContainer->insertRtContainer(rtCont);
	}
	
	  if(isDebug)printf("	router=%s sourceID=%d add template=%d\n", taddr, sourceID, tID);
          rtCont->addTemplate (sourceID, tID, tLen, fId, fValue, FALSE);
    }
    if(isDebug)cout << "end of decodeTemplate" << endl;
}

//-------------------------------------
//process netflow v9 records
//-------------------------------------
long processNF9Packet(u_char *buf, int len, struct iphdr *ip_hdr)
{
    //char taddr[MAX_NAME_LEN];
    if(isDebug) cout << "enter processNF9Packet" << endl;
    struct NF9_HEADER *hdr = (struct NF9_HEADER*)buf;

    // get router who reports netflow
    u_int32_t saddr = ntohl(ip_hdr->saddr);

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
  while (offset < len-8) 
  {
    //cout <<"X";
    NF9_FLOWSET_HEADER_COMMON *fs_hdr = (NF9_FLOWSET_HEADER_COMMON *)&buf[offset];
    short fsid = htons(fs_hdr->flowset_id);
    short fsLen = htons(fs_hdr->length);
    if(fsLen <= 0) break;
    int oData = offset + sizeof(NF9_FLOWSET_HEADER_COMMON);
    short sh = fsLen;
    offset += fsLen;

    if(isDebug)cout << "	processNF9FlowSet. fs_hdr->flowset_id is " << fsid << "; fs_hdr->length is " << fsLen << " offset= "<< offset << "len = " << len << endl;

    // process flowset
    switch (fsid)
    {
      case NF9_TEMPLATE_FLOWSET_ID:
	// template definition
	decodeTemplate (&buf[oData], sh, hdr, ip_hdr);
	break;
      case NF9_OPTIONS_FLOWSET_ID:
	// optional template
	decodeOptionTemplate (&buf[oData], sh, hdr, ip_hdr);
	break;
      default:
        // process netflow flowset data
	parseNetflowSet(&buf[oData], fsid, sh, hdr, ip_hdr);
	break;

    } // end process flowset
  } // end while
    return ntohs(hdr->flows);
}

void netFlowPcap(char* inDir, char* inFileName, int nline)
{
  char fullName[MAX_CHARS_PER_LINE];
  char errbuf[PCAP_ERRBUF_SIZE];
  const u_char *packet;
  struct pcap_pkthdr header;
  int n1;

  if(isDebug) cout <<"netFlowPcap----------"<<endl;
  strcpy (fullName, inDir);
  strcat (fullName, "/");
  strcat (fullName, inFileName);
  if(isDebug) cout << "	process PCAP file="<<fullName<<endl;

  pcap_t* pcapFB = pcap_open_offline(fullName, errbuf);
  if (pcapFB == NULL)
  {
      cout << "failed to open pcap file" << endl;
      return;
  }

  fstWin = 0;
  fedWin = 0;

  // start reading pcap packets
  long packetNo = 0;
  long numOfPacketsPerReport = 20000;
  long nextReport = packetNo + numOfPacketsPerReport;
  long totalFlow = 0;
  long  nflow;
  totalNfRecord = 1;
  while (0 != (packet = pcap_next(pcapFB, &header)))
  {
        ether_header *eh = (ether_header*)packet;
        uint16_t ether_type = ntohs(eh->ether_type);
	if(isDebug) cout <<"	ether type= "<<ether_type << endl;

        u_char *pkt_ptr = (u_char *)packet;
        int ether_offset = 0;
        if (ether_type != ETHERTYPE_IP)
            continue;

        ether_offset = 14;

        pkt_ptr += ether_offset;  //skip past the Ethernet II header
        struct iphdr *ip_hdr = (struct iphdr *)pkt_ptr; //point to an IP header structure

        if(ip_hdr->protocol != IPPROTO_UDP)  continue;// CFLOW transfered via UDP

        pkt_ptr += sizeof(iphdr);
        udphdr *udp_hdr = (struct udphdr*)pkt_ptr;

        pkt_ptr += sizeof(udphdr);

	// process netflow v9 record here
	packetNo++;
	nflow = processNF9Packet(pkt_ptr, ntohs(udp_hdr->len), ip_hdr);
	totalFlow = totalFlow + nflow;
	if(isDebug) printf ("	total Flows= %d\n", totalFlow);
	if (packetNo == nextReport)
	{
	    printf("processed %d packets %d flows\n", packetNo, totalFlow);
	    nextReport = packetNo + numOfPacketsPerReport;
	}
	if (packetNo > nline && nline > 0) break;
  } // end pcap file reading
  printf("processed %d packets %d flows\n", packetNo, totalFlow);
  pcap_close(pcapFB);
  printf("file %s:  start time: %u  end time: %u\n", inFileName, fstWin, fedWin);
  pcapFB=NULL;
}


//-----------------main program-------------------
int main(int argc, char** argv)
{
  // check arguments
  char netflowDir[MAX_CHARS_PER_LINE] ;
  char netflowHome[MAX_CHARS_PER_LINE] ;
  char netflowTemp[MAX_CHARS_PER_LINE] ;
  char netflowOutput[MAX_CHARS_PER_LINE] ;

  int nDirectory = 0;
  int nFile = 0;
  int n1 = 0;
  int nHome = 0;
  bool isPcap = FALSE;
  bool isCsv = FALSE;
  routerType rtype = ALL;
  isDebug = FALSE;

  int ntst = 0;
  int nted = 99999999;
  for (n1=1; n1<argc; n1++)
  {
	// start export time stamp in Unix time format
	if (compareString (argv[n1], "-ST") == 0)
	{
	    //cout << startTrafficTime << endl;
	    startTrafficTime = atol (argv[n1+1]);
	    //cout << argv[n1+1]<<"  -->  "<< startTrafficTime << endl;
	}
	// number of export report period, default 300
	else if (compareString (argv[n1], "-NRP") == 0) 
	{
	    if (argv[n1+1] != NULL) nReportPeriods = atol (argv[n1+1]);
	}
	// number of seconds per report period, default 300 seconds
	else if (compareString (argv[n1], "-NSPP") == 0)
	{
	    if (argv[n1+1] != NULL) nSecondPerPeriod = atol (argv[n1+1]);
	}
	else if (compareString (argv[n1], "-HOME") == 0) nHome = n1+1;
	else if (compareString (argv[n1], "-DEBUG") == 0) isDebug = TRUE;
	else if (compareString (argv[n1], "-PSTR") == 0)
	{
	    ntst = atoi (argv[n1+1]);
	}
	else if (compareString (argv[n1], "-PEND") == 0)
	{
	    nted = atoi (argv[n1+1]);
	}
   }

  if ( nHome == 0 || argv[nHome] == NULL) 
  {
		cout << "Home directory is not defined, exit now" << endl;
		return 0;
  }

  //-----------------------------------------
  // open file descripters for error and reports

  char errName[MAX_CHARS_PER_LINE];
  strcpy (errName, argv[nHome]);
  strcat (errName,"/log/errorNetflowDecoder.log");
  errorFB=fopen(errName, "w");
  if (errorFB == NULL)
  {
    printf("fail to open log/errorNetflowDecoder.log, check the file priviledge\n");
    return 0;
  }

  if (argc < 2)
  {
	cout << " cParser -D Directory -F filename -P(or PE or ALL)" << endl;
	return 0;
  }

  // netflow pcap directory
  strcpy (netflowDir, argv[nHome]);
  strcat (netflowDir,"/pcap");
  // netflow template
  strcpy (netflowTemp, argv[nHome]);
  strcat (netflowTemp,"/control");
  // netflow template
  strcpy (netflowOutput, argv[nHome]);
  strcat (netflowOutput,"/netflow_csv");

  //------------ define export template---------
/*
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
*/

  //-------------define export template here--------
  //  The first two buffers are used to collect time stamp
  //  
  for (int n1=0; n1<MAXKEYFIELDS; n1++)
  {
	etKeyBuf[n1] = -1;
  }
  strcpy (exportTemp[0].description, "FIRST_SWITCHED");
  exportTemp[0].key = NF9_FIRST_SWITCHED;
  etKeyBuf[NF9_FIRST_SWITCHED] = 0;
  strcpy (exportTemp[1].description, "LAST_SWITCHED");
  exportTemp[1].key = NF9_LAST_SWITCHED;
  char etf[MAX_CHARS_PER_LINE] ;
  etKeyBuf[NF9_LAST_SWITCHED] = 1;
  nExportKey = 2;
  strcpy (etf, argv[nHome]);
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

// --------- start Netflow Process ---------------------
  		struct dirent *dirp;
 		DIR *dp = opendir (netflowDir);
  		if (dp == NULL) return 0;
		long tt = startTrafficTime;
		// open output files
		for (int f1=0; f1<nReportPeriods; f1++)
		{
	    	    char outName[MAX_CHARS_PER_LINE];
		    sprintf(outName,"%s/%u.csv", netflowOutput,tt);
  		    trafficFB[f1]=fopen(outName, "w");
  		    if (trafficFB [f1]== NULL) return -1;
		    tt = tt+nSecondPerPeriod;
  		    // print netflow traffic report header 
		}
		vector <char*> fileContainer;
  		int totalFile = 0;
  		while ((dirp = readdir( dp )))
  		{
		    if (compareString (dirp->d_name,"test",4)!=0) continue;
		    int nf = atoi(&dirp->d_name[4]);
		    if (nf < ntst || nf > nted) continue;
		    char * filename = new char[48];
    		    strcpy (filename, dirp->d_name);
		    fileContainer.push_back (filename);
		    totalFile++;
		}

  		// sort files to decending order
  		std::stable_sort (fileContainer.begin(),fileContainer.end(),fileSort);
		for (int f1=0; f1< fileContainer.size(); f1++)
		{
		    cout << "Process Netflow file: "<<fileContainer[f1]<< endl;
		    netFlowPcap(netflowDir, fileContainer[f1], -1);
		}

		// export template
  char tempName[MAX_CHARS_PER_LINE];
  strcpy (tempName, netflowTemp);
  strcat (tempName, "/allV9TemplatesInTheNetwork.csv");
  FILE* tempFB=fopen(tempName, "w");
  ntContainer->printTemplateAll(tempFB);
  fclose (tempFB);
  cout << "export template -- completed" << endl;
		// clean up buffer
		for (int f1=0; f1< fileContainer.size(); f1++)
		{
		    delete (fileContainer[f1]);
		}
		for (int f1=0; f1<nReportPeriods; f1++)
		{
		    fclose (trafficFB[f1]);
		    trafficFB[f1]=NULL;
		}


  //delete ntContainer;

}

