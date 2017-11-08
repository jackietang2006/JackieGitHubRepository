#include<RawUdp.h>

RawUdp::RawUdp(u_int32_t destIp, u_int16_t destPort)
{
  dest_ip = destIp;
  dest_port = destPort;

  //zero out the packet buffer
  memset (datagram, 0, BUFFERSIZE);
  memset (pseudogram, 0, BUFFERSIZE);
}

RawUdp::~RawUdp() {}

void RawUdp::buildRawUdpDataGram(u_int32_t sourceIp, u_int16_t sourcePort, const u_char* payload, int payload_len, u_char* builtDG, int& buildDG_len, CSumFuncPtr fp)

{
  //zero out the packet buffer
  memset (datagram, 0, BUFFERSIZE);
  memset (pseudogram, 0, BUFFERSIZE);

  //spoofying source end
  source_ip = sourceIp;
  source_port = sourcePort;

  //IP header
  struct iphdr *iph = (struct iphdr *) datagram;

  //UDP header
  struct udphdr *udph = (struct udphdr *) (datagram + sizeof (struct ip));

  struct sockaddr_in sin;
  struct pseudo_header psh;
     
  //Data part
  char* data = datagram + sizeof(struct iphdr) + sizeof(struct udphdr);
  memcpy(data, payload, payload_len);

  //some address resolution
  sin.sin_family = AF_INET;
  sin.sin_port = dest_port;
  sin.sin_addr.s_addr = dest_ip;
     
  //Fill in the IP Header
  iph->ihl = 5;
  iph->version = 4;
  iph->tos = 0;
  iph->tot_len = htons ((short)(sizeof (struct iphdr) + sizeof (struct udphdr) + payload_len));
  iph->id = htons (0x8795); //Id of this packet: does not matter here!
  iph->frag_off = 0;
  iph->ttl = 255;
  iph->protocol = IPPROTO_UDP;
  iph->check = 0;      //Set to 0 before calculating checksum
  iph->saddr = source_ip;    //Spoof the source ip address
  iph->daddr = sin.sin_addr.s_addr;

  //Ip checksum: does not matter here. Kernel will calculate!
  iph->check = (*fp) ((unsigned short *) datagram, sizeof(struct iphdr));

  //UDP header
  udph->source = source_port;
  udph->dest = dest_port;
  udph->len = htons(8 + payload_len); //udp header size
  udph->check = 0; //leave checksum 0 now, filled later by pseudo header

  //Now the UDP checksum using the pseudo header
  psh.source_address = source_ip;
  psh.dest_address = sin.sin_addr.s_addr;
  psh.placeholder = 0;
  psh.protocol = IPPROTO_UDP;
  psh.udp_length = htons(sizeof(struct udphdr) + payload_len);

  int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + payload_len;

  memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
  memcpy(pseudogram + sizeof(struct pseudo_header) , udph , sizeof(struct udphdr) + payload_len);

  //Calculating checksum in network mode
  udph->check = (*fp) ((unsigned short*) pseudogram , psize);

  memcpy(builtDG, datagram, ntohs(iph->tot_len));
  buildDG_len = iph->tot_len;
  return;
}
