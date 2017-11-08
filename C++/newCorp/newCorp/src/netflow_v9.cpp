#include "netflow_v9.h"

v9_template::v9_template(  const u_int16_t tID, 
		const u_int16_t nf,
		const u_int16_t* fID,
		const u_int16_t* len,
		const bool option)
{
  templateID = tID;
  isOption = option;
  nField = nf;
  tField = new templateField[nf];
  flowSizeInBytes = 0;
  for (int n1=0; n1< nf; n1++)
  {
    tField[n1].fieldID = fID[n1];
    tField[n1].length  = len[n1];
    flowSizeInBytes = flowSizeInBytes + len[n1];
  }
}
// predefined cisco template
v9_template* v9_template::createDefaultOptionTemplate(u_int32_t tId)
{
  v9_template* vt = NULL;
  	u_int16_t fID257[3] = {1,35,34};
  	u_int16_t len257[3] = {0,1,4};
  switch (tId)
  {
    case 257:
  	vt= new v9_template(257,3, fID257, len257, TRUE);
	break;
  }
  return vt;
}
v9_template* v9_template::createDefaultTemplate(u_int32_t tId)
{
  v9_template* vt = NULL;
  	u_int16_t fID256[17] = {21,22,1,2,10,14,8,12,4,5,7,11,6,70,71,46,47};
  	u_int16_t len256[17] = {4,4,4,4,2,2,4,4,1,1,2,2,1,3,3,1,4};
 	u_int16_t fID257[17] = {21,22,1,2,10,14,8,12,4,5,7,11,6,70,71,46,47};
  	u_int16_t len257[17] = {4,4,4,4,2,2,4,4,1,1,2,2,1,3,3,1,4};
  	u_int16_t fID258[20] = {21,22,1,2,10,14,8,12,4,5,7,11,48,15,17,16,13,9,6,61};
  	u_int16_t len258[20] = {4,4,4,4,2,2,4,4,1,1,2,2,1,4,2,2,1,1,1,1};
  	u_int16_t fID259[17] = {21,22,1,2,10,14,8,12,4,5,7,11,6,70,71,46,47};
  	u_int16_t len259[17] = {4,4,4,4,2,2,4,4,1,1,2,2,1,3,3,1,4};
  	u_int16_t fID260[10] = {21,22,1,2,10,14,70,71,46,47};
  	u_int16_t len260[10] = {4,4,4,4,2,2,3,3,1,4};

  switch (tId)
  {
    case 256:
  	vt= new v9_template(256,17, fID256, len256, FALSE);
  	break;
    case 257:
  	vt= new v9_template(257,17, fID257, len257, FALSE);
	break;
    case 258:
  	vt= new v9_template(258,20, fID258, len258, FALSE);
    	break;
    case 259:
  	vt= new v9_template(259,17, fID259, len259, FALSE);
	break;
    case 260:
  	vt= new v9_template(260,10, fID260, len260, FALSE);
	break;
  }
  return vt;
}


v9_template::~v9_template()
{
  if (tField !=NULL) delete tField;
}

void v9_template::printContent()
{
  printf("Template ID = %d, number of Field = %d size of flow=%d", 
	templateID, nField, flowSizeInBytes);
  if (isOption) printf (" option template \n");
  else printf ("\n");

  for (int n1=0; n1< nField; n1++)
  {

    printf("      Field ID = %d  %s;	 Length = %d \n", 
	tField[n1].fieldID, NFV9Description[tField[n1].fieldID],
	tField[n1].length);
  }
}


void v9_template::printContent(FILE* fb)
{
  fprintf(fb,"%u,%u,%u", templateID, nField, (isOption)? 1:0);
  for (int n1=0; n1< nField; n1++)
  {
    fprintf(fb,",%u,%u", tField[n1].fieldID, tField[n1].length);
  }
}
// print netflow data with the template format
void v9_template::printContent(int* data)
{
  //printf("Template ID = %d, number of Field = %d\n", templateID, nField);
  for (int n1=0; n1< nField; n1++)
  {
    if (tField[n1].fieldID == NF9_IPV4_SRC_ADDR ||
        tField[n1].fieldID == NF9_SRC_MASK ||
	tField[n1].fieldID == NF9_IPV4_DST_ADDR ||
	tField[n1].fieldID == NF9_DST_MASK ||
	tField[n1].fieldID == NF9_MPLS_TOP_LABEL_IP_ADDR)
    {
	char outbuf[20];
	ipInt2Char(data[n1], outbuf);
         printf("      %s (%d):  %s \n", 
	NFV9Description[tField[n1].fieldID], tField[n1].fieldID, 
	outbuf);
	continue;
    }
    printf("      %s (%d):  %u \n", 
	NFV9Description[tField[n1].fieldID], tField[n1].fieldID,
	data[n1]);
  }
}
bool  v9_template::compare(  const u_int16_t tID, 
		const u_int16_t nf,
		const u_int16_t* fID,
		const u_int16_t* len,
		const bool option)
{
  if (option != isOption) return FALSE;
  if (templateID != tID) return FALSE;
  if (nField != nf) return FALSE;
  for (int n1=0; n1< nf; n1++)
  {
    if (tField[n1].fieldID != fID[n1]) return FALSE;
    if (tField[n1].length  != len[n1]) return FALSE;
  }
  return TRUE;
}  

bool  v9_template::compare(v9_template* temp)
{
  if (templateID != temp->templateID)  return FALSE;
  if (nField != temp->nField) return FALSE;
  if (isOption != temp->isOption) return FALSE;
  for (int n1=0; n1< nField; n1++)
  {
    if (tField[n1].fieldID != temp->tField[n1].fieldID) return FALSE;
    if (tField[n1].length  != temp->tField[n1].length) return FALSE;
  }
  return TRUE;
}  
