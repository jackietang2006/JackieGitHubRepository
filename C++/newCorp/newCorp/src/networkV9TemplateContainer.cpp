#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "networkV9TemplateContainer.h"


bool sortByIPAddr(routerV9TemplateContainer* a, routerV9TemplateContainer* b)
{
  return (a->getRouterAddr() < b->getRouterAddr());
}

bool decIpAddr(routerV9TemplateContainer* a, uint32_t b)
{
  return (a->getRouterAddr() < b);
}
networkV9TemplateContainer::~networkV9TemplateContainer()
{
  int n1;
  cout << "network template destructor activated; size=" << ntContainer.size() << endl;
  for (n1=0; n1<ntContainer.size(); n1++)
  {
     routerV9TemplateContainer* iObj = ntContainer[n1];
     if (iObj !=NULL) delete iObj;
  }
}


int networkV9TemplateContainer::initTemplateCSV(char* idir)
{
  // read in allInterfaceInTheNetwork file
  char fullName[MAX_CHARS_PER_LINE];
  ifstream fin;
  char routerToken[MAX_NAME_LEN] = {"//RouterName="};
  char buf[MAX_CHARS_PER_LINE];
  u_int16_t tID, tLen, fId[30], fValue[30];
  u_int32_t sourceID;
  u_int32_t routerAddress;
  bool isOption;
  int totalInterface = 0;
  int n1, iPosInc, cPos, n2;
  strcpy (fullName, idir);
  strcat (fullName, "/allV9TemplatesInTheNetwork.csv");
  fin.open(fullName); // open a file
  if (!fin.good())
  {
	//cout << "No Template "<< fullName << endl;
	return -1;
  }

  // start reading the csv file
  while (!fin.eof())
  {
    fin.getline(buf, MAX_CHARS_PER_LINE);
    if (buf == NULL) continue;
    //cout << buf<<"size of buffer= "<<sizeof(buf) << endl;
    //iPosInc = 0;
    //cPos = getTokenizedChar(&buf[0], token[0],',');
    char* token = strtok(buf,",");
    //cout <<"token= " << token << endl;
    if (token == NULL) continue;
    char* routerName = strtok (NULL, ",");
    //printf("%s ; %s\n", token, routerName);
    //if (compareString(token[0], "{") == 0) continue;
	char *c1, *c2, *c3, *c4;
	c1 = strtok (NULL,",");
	c2 = strtok (NULL,",");
	c3 = strtok (NULL,",");
	c4 = strtok (NULL,",");
    if (c4 == NULL || c3 == NULL) // new format
    {
	totalInterface = atoi (c1);
	routerAddress = ipChar2Int(routerName);
    }
    else
    {
	totalInterface = atoi (c4);
	routerAddress = ipChar2Int(c3);
    }

    // create new router template then insert to ntContainer

    routerV9TemplateContainer * rtCont = new routerV9TemplateContainer();
    memset (rtCont, 0, sizeof(routerV9TemplateContainer));
    rtCont->setRouterAddr(routerAddress);
    //ntContainer.insert (elow, rtCont);
    insertRtContainer(rtCont);

	//printf("router name=%s total template=%d\n", routerName, totalInterface);
	// next line is "{"
	fin.getline(buf, MAX_CHARS_PER_LINE);
	if (compareString (buf, "{") !=0) continue;  // should never happen; must be ill formed data

	// get each template
	for (n1=0; n1<totalInterface; n1++)
	{
	   fin.getline(buf, MAX_CHARS_PER_LINE);
	   //cout << buf << endl;
	   c1 = strtok (buf, ",");
	   sourceID = atoi (c1);
	   c2 = strtok(NULL, ",");
	   tID = atoi (c2);
	   c3 = strtok (NULL, ",");
	   tLen = atoi (c3);
	   c4 = strtok(NULL, ",");
	   isOption = FALSE;
	   if (atoi(c4) == 1) isOption = TRUE;
	   //printf("    %u,%u,%u,%u\n",sourceID, tID, tLen, isOption);
	   for (n2=0; n2<tLen; n2++)
	   {
		c1 = strtok (NULL, ",");
		fId[n2] = atoi(c1);
		c1 = strtok (NULL, ",");
		fValue[n2] = atoi(c1);
	   }
	   //v9_template* v9Temp = gTempCont.addTemplate (tID, tLen, fId, fValue, isOption);
	   rtCont->addTemplate (sourceID, tID, tLen, fId, fValue, isOption);
	}// for totalInterface
      // add rtCont to the networkTemplateContainer
	fin.getline(buf, MAX_CHARS_PER_LINE);  // should be "}"
      
  }
  cout << "import template --- completed"<<endl;
  fin.close();
}

routerV9TemplateContainer * networkV9TemplateContainer::getRtContainer (uint32_t routerAddress)
{
    // create new router template then insert to ntContainer
    std::vector<routerV9TemplateContainer*>::iterator elow = std::lower_bound(
	ntContainer.begin(), ntContainer.end(),
	routerAddress, decIpAddr);
    if (elow  >= ntContainer.end()||
	routerAddress != ntContainer[elow-ntContainer.begin()]->getRouterAddr()) return NULL;
    return ntContainer[elow-ntContainer.begin()];
}


void networkV9TemplateContainer::insertRtContainer (routerV9TemplateContainer * rtCont)
{
    // create new router template then insert to ntContainer
    //cout << "Enter networkV9TemplateContainer::insertRtContainer" << endl;
    std::vector<routerV9TemplateContainer*>::iterator elow = std::lower_bound(
	ntContainer.begin(), ntContainer.end(),
	rtCont->getRouterAddr(), decIpAddr);
    ntContainer.insert (elow, rtCont);
}
void networkV9TemplateContainer::printTemplateAll(FILE* fb)
{
  int n1;
  for (n1=0; n1<ntContainer.size(); n1++)
  {
     routerV9TemplateContainer* iObj = ntContainer[n1];
     iObj->printContent(fb);
  }
}


