#include "netflowV9Container.h"
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>

using namespace std;

// destructor
netflowV9Container::~netflowV9Container()
{
  //cout << "netflowV9Container::~netflowV9Container; size="<<templateContainer.size()<<endl;
  for (int n1=templateContainer.size()-1; n1>=0; n1--)
  {
    delete templateContainer[n1];
    templateContainer.erase(templateContainer.begin()+n1);
  }
}
    
// add netflow elements
// check if the identical template has been created
// if not, add the new template
// return the template pointer
v9_template* netflowV9Container::addTemplate (const u_int16_t tID, 
		const u_int16_t nf,
		const u_int16_t* fID,
		const u_int16_t* len,
		const bool option)
{
  for (int n1=templateContainer.size()-1; n1>=0; n1--)
  {
    if( templateContainer[n1]->compare(tID, nf, fID, len, option)) 
	return templateContainer[n1];
  }
  v9_template* vt = new v9_template( tID, nf, fID, len, option);
  templateContainer.push_back(vt);
  return vt;
}


void netflowV9Container::printContent()
{
  for (int n1=templateContainer.size()-1; n1>=0; n1--)
  {
    templateContainer[n1]->printContent();
  }
}

