#ifndef NETFLOWV9CONTAINER_H
#define NETFLOWV9CONTAINER_H
// netflow v9 container
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include "parserCommon.h"
#include <stdio.h>
#include "netflow_v9.h"

class netflowV9Container
{
private:
  vector <v9_template*> templateContainer;
public:
  virtual ~netflowV9Container();

  // get template by index
  v9_template* getTemplate (int n1)
    { if (n1>=templateContainer.size() or n1<0) return NULL;
	else return (templateContainer[n1]);}

  v9_template* addTemplate (const u_int16_t tID, 
		const u_int16_t nf,
		const u_int16_t* fID,
		const u_int16_t* len,
		const bool option);

  void printContent();
};
#endif //NETFLOWV9CONTAINER_H
