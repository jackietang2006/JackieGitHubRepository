#ifndef NETFLOWV9_H
#define NETFLOWV9_H
// netflow v9 template 
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdio.h>

#include <pcap.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/udp.h>
#include <utime.h>
#include "parserCommon.h"


//------------------netflow process -------------
#if defined(__GNUC__)
# ifndef __packed
#  define __packed              __attribute__((__packed__))
# endif
#endif
struct NF9_HEADER {
	u_int16_t version, flows;
	u_int32_t uptime_ms, time_sec;
	u_int32_t package_sequence, source_id;
} __packed;

struct NF9_FLOWSET_HEADER_COMMON {
	u_int16_t flowset_id, length;
} __packed;

struct NF9_TEMPLATE_FLOWSET_HEADER {
	struct NF9_FLOWSET_HEADER_COMMON c;
	u_int16_t template_id, count;
} __packed;

struct NF9_OPTION_TEMPLATE_FLOWSET_HEADER {
	struct NF9_FLOWSET_HEADER_COMMON c;
	u_int16_t template_id, scope_length, option_length;
} __packed;

struct NF9_TEMPLATE_FLOWSET_RECORD {
	u_int16_t type, length;
} __packed;

struct NF9_DATA_FLOWSET_HEADER {
	struct NF9_FLOWSET_HEADER_COMMON c;
} __packed;


#define NF9_TEMPLATE_FLOWSET_ID		0
#define NF9_OPTIONS_FLOWSET_ID		1
#define NF9_MIN_RECORD_FLOWSET_ID	256

/* Flowset record types the we care about */

// description of netflow fieldID
static const char* const NFV9Description [] =
{
	 "--------",  //		0
	 "IN_BYTES",  //			1
	 "IN_PACKETS",  //			2
	 "FLOWS",  //			    3
	 "IN_PROTOCOL",  //			4
           "SRC_TOS",  // 			5
           "TCP_FLAGS",  //			6
           "L4_SRC_PORT",  //			7
           "IPV4_SRC_ADDR",  //		8
           "SRC_MASK",  //			9
           "IF_INDEX_IN",  //			10
           "L4_DST_PORT",  //			11
           "IPV4_DST_ADDR",  //		12
           "DST_MASK",  //			13
           "IF_INDEX_OUT",  //		14
           "IPV4_NEXT_HOP",  //		15
           "SRC_AS",  //      		16
           "DST_AS",  //              17
           "BGP_IPV4_NEXT_HOP",  //   18
           "MUL_DST_PKTS",  //        19
	     "MUL_DST_BYTES",  //       20
	     "FIRST_SWITCHED",  //		21
	"LAST_SWITCHED",  //		22
	"OUT_BYTES",  //           23
	"OUT_PKTS",  //            24
	"MIN_PKT_LNGTH",  //       25
	"MAX_PKT_LNGTH",  //       26
	"IPV6_SRC_ADDR",  //		27
	"IPV6_DST_ADDR",  //		28
	"IPV6_SRC_MASK",  //		29
	"IPV6_DST_MASK",  //		30
	"IPV6_FLOW_LABEL",  //		31
	"ICMP_TYPE",  //   		32
	"MUL_ICMP_TYPE",  //   	33
	"SAMPLING_INTERVAL",  //       34
	"SAMPLING_ALGORITHM",  //      35
	"FLOW_ACTIVE_TIMEOUT",  //     36
	"FLOW_INACTIVE_TIMEOUT",  //   37
	"ENGINE_TYPE",  //             38
	"ENGINE_ID",  //               39
	"TOTAL_BYTES_EXP",  //         40
	"TOTAL_PKTS_EXP",  //          41
	"TOTAL_FLOWS_EXP",  //         42
	     "Vendor Proprietary",  //             43
	"IPV4_SRC_PREFIX",  //         44
	"IPV4_DST_PREFIX",  //         45
	"MPLS_TOP_LABEL_TYPE",  //     46
	"MPLS_TOP_LABEL_IP_ADD",  //R  47
	"FLOW_SAMPLER_ID",  //         48
             "FLOW_SAMPLER_MODE",  //       49
             "FLOW_SAMPLER_RANDOM_INTERVAL",  // 50
	     "Vendor Proprietary",  //             51
             "MIN_TTL",  //                 52
             "MAX_TTL",  //                 53
             "IPV4_IDENT",  //              54
             "DST_TOS",  //                 55
             "IN_SRC_MAC",  //              56
             "OUT_DST_MAC",  //             57
             "SRC_VLAN",  //                58
             "DST_VLAN",  //                59
             "IP_PROTOCOL_VERSION",  //		60
             "DIRECTION",  //               61
             "IPV6_NEXT_HOP",  //           62
             "BPG_IPV6_NEXT_HOP",  //       63
             "IPV6_OPTION_HEADERS",  //     64
             "VENDOR_PROPRIETARY1",  //     65
             "VENDOR_PROPRIETARY2",  //     66
             "VENDOR_PROPRIETARY3",  //     67
             "VENDOR_PROPRIETARY4",  //     68
             "VENDOR_PROPRIETARY5",  //     69
             "MPLS_LABEL_1",  //            70
             "MPLS_LABEL_2",  //            71
             "MPLS_LABEL_3",  //            72
             "MPLS_LABEL_4",  //            73
             "MPLS_LABEL_5",  //            74
             "MPLS_LABEL_6",  //            75
             "MPLS_LABEL_7",  //		76
             "MPLS_LABEL_8",  //            77
             "MPLS_LABEL_9",  //            78
             "MPLS_LABEL_10",  //           79
             "IN_DST_MAC",  //              80
             "OUT_SRC_MAC",  //             81
             "IF_NAME",  //                 82
             "IF_DESC",  //                 83
             "SAMPLER_NAME",  //            84
             "IN_PERMANENT_BYTES",  //      85
             "IN_PERMANENT_PKTS ",  //      86
	     "Vendor Proprietary",  //            87
             "FRAGMENT_OFFSET",  //         88
             "FORWARDING STATUS",  //       89
             "MPLS_PAL_RD",  //             90
             "MPLS_PREFIX_LEN",  //         91
             "SRC_TRAFFIC_INDEX",  //       92
             "DST_TRAFFIC_INDEX",  //       93
             "APPLICATION_DESCRIPTION",  // 94
             "APPLICATION_TAG",  //         95
             "APPLICATION_NAME",  //        96
             "postipDiffServCodePoint",  // 98
             "replication_factor",  //      99
             "DEPRECATED",  //              100
             "layer2packetSectionOffset",  //   102
             "layer2packetSectionSize",  //     103
             "layer2packetSectionData",  //     104
};  // end of description


#define NF9_DEFAULT_TEMPLATE_INTERVAL	16

/* ... */
#define NF9_OPTION_SCOPE_SYSTEM    1
#define NF9_OPTION_SCOPE_INTERFACE 2
#define NF9_OPTION_SCOPE_LINECARD  3
#define NF9_OPTION_SCOPE_CACHE     4
#define NF9_OPTION_SCOPE_TEMPLATE  5
/* ... */
#define NF9_SAMPLING_ALGORITHM_DETERMINISTIC 1
#define NF9_SAMPLING_ALGORITHM_RANDOM        2
/* ... */

#define NF9_IN_BYTES			1
#define NF9_IN_PACKETS			2
#define NF9_FLOWS			    3
#define NF9_IN_PROTOCOL			4
#define NF9_SRC_TOS 			5
#define NF9_TCP_FLAGS			6
#define NF9_L4_SRC_PORT			7
#define NF9_IPV4_SRC_ADDR		8
#define NF9_SRC_MASK			9
#define NF9_IF_INDEX_IN			10
#define NF9_L4_DST_PORT			11
#define NF9_IPV4_DST_ADDR		12
#define NF9_DST_MASK			13
#define NF9_IF_INDEX_OUT		14
#define NF9_IPV4_NEXT_HOP		15
#define NF9_SRC_AS      		16
#define NF9_DST_AS              17
#define NF9_BGP_IPV4_NEXT_HOP   18
#define NF9_MUL_DST_PKTS        19
#define NF9_MUL_DST_BYTES       20
#define NF9_LAST_SWITCHED		21
#define NF9_FIRST_SWITCHED		22
#define NF9_OUT_BYTES           23
#define NF9_OUT_PKTS            24
#define NF9_MIN_PKT_LNGTH       25
#define NF9_MAX_PKT_LNGTH       26
#define NF9_IPV6_SRC_ADDR		27
#define NF9_IPV6_DST_ADDR		28
#define NF9_IPV6_SRC_MASK		29
#define NF9_IPV6_DST_MASK		30
#define NF9_IPV6_FLOW_LABEL		31
#define NF9_ICMP_TYPE   		32
#define NF9_MUL_ICMP_TYPE   	33
#define NF9_SAMPLING_INTERVAL       34
#define NF9_SAMPLING_ALGORITHM      35
#define NF9_FLOW_ACTIVE_TIMEOUT     36
#define NF9_FLOW_INACTIVE_TIMEOUT   37
#define NF9_ENGINE_TYPE             38
#define NF9_ENGINE_ID               39
#define NF9_TOTAL_BYTES_EXP         40
#define NF9_TOTAL_PKTS_EXP          41
#define NF9_TOTAL_FLOWS_EXP         42
/* *Vendor Proprietary*             43 */
#define NF9_IPV4_SRC_PREFIX         44
#define NF9_IPV4_DST_PREFIX         45
#define NF9_MPLS_TOP_LABEL_TYPE     46
#define NF9_MPLS_TOP_LABEL_IP_ADDR  47
#define NF9_FLOW_SAMPLER_ID         48
#define NF9_FLOW_SAMPLER_MODE       49
#define NF9_FLOW_SAMPLER_RANDOM_INTERVAL 50
/* *Vendor Proprietary*             51 */
#define NF9_MIN_TTL                 52
#define NF9_MAX_TTL                 53
#define NF9_IPV4_IDENT              54
#define NF9_DST_TOS                 55
#define NF9_IN_SRC_MAC              56
#define NF9_OUT_DST_MAC             57
#define NF9_SRC_VLAN                58
#define NF9_DST_VLAN                59
#define NF9_IP_PROTOCOL_VERSION		60
#define NF9_DIRECTION               61
#define NF9_IPV6_NEXT_HOP           62
#define NF9_BPG_IPV6_NEXT_HOP       63
#define NF9_IPV6_OPTION_HEADERS     64
#define NF9_VENDOR_PROPRIETARY1     65
#define NF9_VENDOR_PROPRIETARY2     66
#define NF9_VENDOR_PROPRIETARY3     67
#define NF9_VENDOR_PROPRIETARY4     68
#define NF9_VENDOR_PROPRIETARY5     69
#define NF9_MPLS_LABEL_1            70
#define NF9_MPLS_LABEL_2            71
#define NF9_MPLS_LABEL_3            72
#define NF9_MPLS_LABEL_4            73
#define NF9_MPLS_LABEL_5            74
#define NF9_MPLS_LABEL_6            75
#define NF9_MPLS_LABEL_7            76
#define NF9_MPLS_LABEL_8            77
#define NF9_MPLS_LABEL_9            78
#define NF9_MPLS_LABEL_10           79
#define NF9_IN_DST_MAC              80
#define NF9_OUT_SRC_MAC             81
#define NF9_IF_NAME                 82
#define NF9_IF_DESC                 83
#define NF9_SAMPLER_NAME            84
#define NF9_IN_PERMANENT_BYTES      85
#define NF9_IN_PERMANENT_PKTS       86	
// Vendor Proprietary			87
#define NF9_FRAGMENT_OFFSET         88
#define NF9_FORWARDING STATUS       89
#define NF9_MPLS_PAL_RD             90
#define NF9_MPLS_PREFIX_LEN         91
#define NF9_SRC_TRAFFIC_INDEX       92
#define NF9_DST_TRAFFIC_INDEX       93
#define NF9_APPLICATION_DESCRIPTION 94
#define NF9_APPLICATION_TAG         95
#define NF9_APPLICATION_NAME        96
#define NF9_postipDiffServCodePoint 98
#define NF9_replication_factor      99
#define NF9_DEPRECATED              100
#define NF9_layer2packetSectionOffset   102
#define NF9_layer2packetSectionSize     103
#define NF9_layer2packetSectionData     104
struct templateField
{
  u_int16_t fieldID;
  u_int16_t length;
};

class v9_template
{
private:
  bool isOption;
  u_int32_t flowSizeInBytes;
  u_int16_t templateID;
  u_int16_t nField;
  templateField* tField;
public:
  v9_template(  const u_int16_t tID, 
		const u_int16_t nf,
		const u_int16_t* fID,
		const u_int16_t* len,
		const bool option );
  virtual ~v9_template();

  bool compare( const u_int16_t tID, 
		const u_int16_t nf,
		const u_int16_t* fID,
		const u_int16_t* len,
		const bool option );
  bool compare( v9_template* temp);

  static v9_template* createDefaultTemplate(u_int32_t tId);
  static v9_template* createDefaultOptionTemplate(u_int32_t tId);
// print content
  void printContent (int* data); // print data with the template format
  void printContent(); 	// print template 
  void printContent( FILE* fb); // print template in CSV to a file
// get & set
  u_int16_t getTId() { return templateID;}
  u_int16_t getNField() { return nField;}
  bool	    isOptionTemplate() { return isOption;}
  templateField* getField (int n1) { return &tField[n1];}
  u_int32_t getFlowSizeInBytes() { return flowSizeInBytes;}
};

#endif
