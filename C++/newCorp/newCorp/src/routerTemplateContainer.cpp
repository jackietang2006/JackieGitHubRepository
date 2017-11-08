#include "routerTemplateContainer.h"

// destructor
routerV9TemplateContainer::~routerV9TemplateContainer()
{
  //cout << "routerV9TemplateContainer::~routerV9Template; size="<<srcTempContainer.size()<<endl;
  for (int n1=srcTempContainer.size()-1; n1>=0 ; n1--)
  {
    if (srcTempContainer[n1] != NULL) delete srcTempContainer[n1];
    //srcTempContainer.erase (srcTempContainer.begin()+n1);
  }
}


// add new template
// if sID and tID are existed, replace the old template
// other create new entry
routerV9Template* routerV9TemplateContainer::addTemplate 
	(u_int32_t sID, u_int16_t tID, 
		const u_int16_t nf,
		const u_int16_t* fID,
		const u_int16_t* len,
		const bool option)
{
  routerV9Template* st = getrouterV9Template (sID, tID);
  if (st == NULL) 
  {
    v9_template* vt = new v9_template( tID, nf, fID, len, option);
    st = new routerV9Template;
    st-> sourceId = sID;
    st-> templateID = tID;
    st-> v9TemplateObj = vt;
    srcTempContainer.push_back(st);
    char lo0Addr [MAX_NAME_LEN];
    ipInt2Char(routerAddr, lo0Addr );

    if(isDebug)cout << "routerV9TemplateContainer::createTemplate @ Router=" << lo0Addr << " SID=" << sID << " TID="<<tID<<endl;
    if(isDebug)st-> v9TemplateObj->printContent();
  }
  else
  {
    if (!st-> v9TemplateObj->compare(tID, nf, fID, len, option))
    {
	char lo0Addr [MAX_NAME_LEN];
  	ipInt2Char(routerAddr, lo0Addr );
	
	if(isDebug)cout << "routerV9TemplateContainer::replaceTemplate @ Router=" << lo0Addr << " SID=" << sID << " TID="<<tID<<endl;
	if(isDebug)
	{
	  st-> v9TemplateObj->printContent();
	  for (int n1=0; n1< nf; n1++)
	  {
	    printf(",(%u,%u)", fID[n1], len[n1]);
	  }
	  printf("\n");
	}
	v9_template* vt = st-> v9TemplateObj;
	delete vt;
	vt = new v9_template( tID, nf, fID, len, option);
    	st-> v9TemplateObj = vt;
    }
  }
  return st;
}

routerV9Template* routerV9TemplateContainer::getrouterV9Template 
	(u_int32_t sID, u_int16_t tID)
{
  for (int n1=0; n1<srcTempContainer.size(); n1++)
  {
    if (srcTempContainer[n1]->sourceId == sID &&
	srcTempContainer[n1]->templateID == tID)
	return srcTempContainer[n1];
  }
  return NULL;
}

void routerV9TemplateContainer::printContent()
{
  char lo0Addr [MAX_NAME_LEN];
  ipInt2Char(routerAddr, lo0Addr );
  printf ("//RouterName=,%s,%d\n{\n", 
		lo0Addr, srcTempContainer.size());
  for (int n1=0; n1<srcTempContainer.size(); n1++)
  {
    srcTempContainer[n1]->v9TemplateObj->printContent();
  }
  printf ("}\n");
}

void routerV9TemplateContainer::printContent(FILE* fb)
{
  char lo0Addr [MAX_NAME_LEN];
  ipInt2Char(routerAddr, lo0Addr );
  fprintf (fb,"//RouterName=,%s,%d\n{\n", 
		lo0Addr, srcTempContainer.size());

  for (int n1=0; n1<srcTempContainer.size(); n1++)
  {
    fprintf(fb,"%u,", srcTempContainer[n1]->sourceId, srcTempContainer[n1]->templateID);
    srcTempContainer[n1]->v9TemplateObj->printContent(fb);
    fprintf(fb,"\n");  }

  fprintf (fb,"}\n");
}
