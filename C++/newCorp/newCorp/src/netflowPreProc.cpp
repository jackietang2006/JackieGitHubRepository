/*
 * Usage: netflowPreProc -DEBUG -HOME ../data/
 *
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
uint32_t   fstWin, fedWin;

FILE* errorFB;
FILE* scanFB;

char taddr[MAX_NAME_LEN];
u_int16_t  fId[40], fValue[40];

//------------file sorting function--------------
static bool fileSort (char* aName, char* bName)
{
    return isDecending (aName, bName);
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
    if (fstWin == 0) fstWin = timestamp;
    else if (fstWin > timestamp) fstWin = timestamp;
    if (fedWin < timestamp) fedWin = timestamp;
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
  long nextReport = 1000;
  long totalFlow = 0;
  long  nflow;
  totalNfRecord = 1;
    while (0 != (packet = pcap_next(pcapFB, &header)))
    {
        ether_header *eh = (ether_header*)packet;
	packetNo++; 
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

        /*
         * print UDP datagram in hex format
         */
        printf("UDP datagram length: %d\n", ntohs(udp_hdr->len));
        #define BYTETOHEX "%02X "
        for(int i = 0; i < ntohs(udp_hdr->len) - 8; i++)
        {
          printf(BYTETOHEX, pkt_ptr[i]);
        }
        printf("\n");

	// process netflow v9 record here
	nflow = processNF9Packet(pkt_ptr, ntohs(udp_hdr->len), ip_hdr);
	totalFlow = totalFlow + nflow;
	if(isDebug) printf ("	total Flows= %d\n", totalFlow);
	if (packetNo > nextReport)
	{
	    printf("processed %d packets %d flows\n", packetNo, totalFlow);
	    nextReport = nextReport +20000;
	}
	if (packetNo > nline && nline > 0) break;
     } // end pcap file reading
    pcap_close(pcapFB);
    fprintf(scanFB,"%s,%u,%u,%u\n", inFileName, fstWin, fedWin, totalFlow);
    pcapFB=NULL;
}


//-----------------main program-------------------
int main(int argc, char** argv)
{
  // check arguments
  char nfPcap[MAX_CHARS_PER_LINE] ;
  char nfCtrl[MAX_CHARS_PER_LINE] ;


  int nDirectory = 0;
  int nFile = 0;
  int n1 = 0;
  int nHome = 0;
  
  for (n1=1; n1<argc; n1++)
  {
	// start export time stamp in Unix time format
	if (compareString (argv[n1], "-HOME") == 0) nHome = n1+1;
	else if (compareString (argv[n1], "-DEBUG") == 0) isDebug = TRUE;
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
  strcat (errName,"/log/netflowDecoderError.log");
  errorFB=fopen(errName, "w");
  if (errorFB == NULL)
  {
    printf("fail to open log/netflowDecoderError.log, check the file priviledge\n");
    return 0;
  }
  strcpy (errName, argv[nHome]);
  strcat (errName,"/control/scanResult.log");
  scanFB=fopen(errName, "w");
  if (scanFB == NULL)
  {
    printf("fail to open control/scanResult.log, check the file priviledge\n");
    return 0;
  }
  // netflow pcap directory
  strcpy (nfPcap, argv[nHome]);
  strcat (nfPcap,"/pcap");
  // netflow template
  strcpy (nfCtrl, argv[nHome]);
  strcat (nfCtrl,"/control");
  //-------------start process here--------------
  string in;
  char input[MAX_CHARS_PER_LINE] = "";
  int ninput;
  ntContainer = new networkV9TemplateContainer();

// --------- start Netflow Process ---------------------
  		struct dirent *dirp;
 		DIR *dp = opendir(nfPcap);
  		if (dp == NULL) return 0;

		vector <char*> fileContainer;
  		int totalFile = 0;
  		while ((dirp = readdir( dp )))
  		{
		    if (compareString(dirp->d_name,".")==0 || 
			compareString(dirp->d_name,"..")==0) continue;
		    char * filename = new char[48];
    		    strcpy (filename, dirp->d_name);
		    fileContainer.push_back (filename);
		    totalFile++;
		}

  		// sort files to decending order
  		std::stable_sort (fileContainer.begin(),fileContainer.end(),fileSort);
		for (int f1=0; f1< fileContainer.size(); f1++)
		{
		    cout << "Scan Netflow file: "<<fileContainer[f1]<< endl;
		    netFlowPcap(nfPcap, fileContainer[f1], -1);
		}

		// export template
                char tempName[MAX_CHARS_PER_LINE];
                strcpy (tempName, nfCtrl);
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
  fclose (scanFB);
  //delete ntContainer;
}
