/* eoe.h
 *
 * Copyright 2012, Synapticon GmbH. All rights reserved.
 * Author: Frank Jeschke <jeschke@fjes.de>
 */

/* What to do here:
 * - get EoE header and check package
 * - forward ethernet resp. tcp/ip packet to application resp. module to handle
 *   these.
 */

/* FIXME check ETG for packet type numbers */
#define EOE_INIT_REQ             0x02
#define EOE_INIT_RSP             0x03 /* what for? asumption: optional */
#define EOE_FRAGMENT_REQ         0x00

#define EOE_IP_PARAM_REQ         0x02
#define EOE_IP_PARAM_RSP         0x03
#define EOE_ADDR_FILTER_REQ      0x04
#define EOE_ADDR_FILTER_RSP      0x05

/* EoE Error codes */
#define EOE_RESULT_SUCCESS           0x0000
#define EOE_RESULT_UNSPECIFIED       0x0001
#define EOE_RESULT_UNSUPPORTED       0x0000
#define EOE_RESULT_NOIPSUPPORT       0x0000
#define EOE_RESULT_NOFILTERSUPPORT   0x0000

#define MAX_EOE_DATA             512 /* FIXME put correct value here */

#define EOE_STATE_IDLE           0
#define EOE_STATE_REC_FRAGMENT   1
#define EOE_STATE_LAST_FRAGMENT  2

/* another try/test with bit field sizes: */

struct _eoe_packet {
	unsigned type            :4;
	unsigned port            :4;  /* 0x00 (no spec. port) - 0x0f */
	unsigned lastFragment    :1;  /* 0 - at least one frame follows, 1 - last packet */
	unsigned timeAppended    :1;  /* 0 no time appended 1 - timestamp appended after data section */
	unsigned timeRequest     :1;  /* 0 no timestamp request, 1 timestamp request */
	unsigned reserved        :5;  /* reseverved 0x00 */
	unsigned fragmentNumber  :6;  /* 0 if type = INIT_REQ, 0x01 - 0x2f  for type FRAGMET_REQ */
	union {
		unsigned complete:6;
		unsigned offset  :6; /* 6-bit */
	} a;
	unsigned frameNumber     :4; /* 4-bit - number of the ethernet frame */
	union {
		unsigned char data[MAX_EOE_DATA];
		struct _eoe_data sdata;
		struct _eoe_parameter param; /* check for size */
	} b;                         /* ommited if, type is EOE_INIT_RSP */
	uint32_t timestamp;          /* optional */
};

/* sub structures */

struct _eoe_data {
	unsigned char dst_mac[6];
	unsigned char src_mac[6];
	unsigned char vlan_tag[4];               /* optional could be omitted! */
	uint16_t ether_type;
	unsigned char data[MAX_EOE_DATA-18];     /* 18 byte data head */
	unsigned char padding[MAX_EOE_DATA-18];
};

union {
	struct _eoe_parameter {
		unsigned mac_included         :1;
		unsigned ip_included          :1;
		unsigned subnetmasq_included  :1;
		unsigned gateway_included     :1;
		unsigned dns_server_included  :1;
		unsigned dns_name_included    :1;
		unsigned reserved             :26;
		unsigned char mac[6];
		unsigned char ip[4];
		unsigned char subnet[4];
		unsigned char gateway[4];
		unsigned char dnsip[4];
		char dns_name[32];
	};
	struct _eoeParameter {
		unsigned mac_filter_count     :4;
		unsigned eac_filter_mask      :2;
		unsigned reserved             :1;
		unsigned inhibit_bcast        :1;
		unsigned reserved             :8;
		unsigned char maclist[6*16];       /* each entry is 6 bytes long, and max 2^4=16 entries */
		unsigned char listfilter[6*16];
	};
} param;


#if 0
/* first blood: */
struct _eoe_head {
	unsigned char type;
	unsigned char port; /* 0x00 (no spec. port) - 0x0f */
	unsigned char lastFragment;  /* 0 - at least one frame follows, 1 - last packet */
	unsigned char timeAppended;  /* 0 no time appended 1 - timestamp appended after data section */
	unsigned char timeRequest;  /* 0 no timestamp request, 1 timestamp request */
	unsigned char fragmentNumber; /* 0 - 0x2f */
	unsigned char offset; /* 6-bit */
	unsigned char frameNumber; /* 4-bit - number of the ethernet frame */
	struct _eoe_data;
	uint32_t timestamp; /* optional if timeAppended==1 */
};


struct _eoe_setup_parameter {
	int mac_included    :1;
	int ip_included     :1;
	int subnet_mask     :1;
	int default_gateway :1;
	int dns_server_ip   :1;
	int dns_name        :1;
	int rserved        :26;
};

struct _eoe_setip_req {
	unsigned char type;
	unsigned char port; /* 0x00 (no spec. port) - 0x0f */
	unsigned char lastFragment;  /* 0 - at least one frame follows, 1 - last packet */
	unsigned char timeAppended;  /* 0 no time appended 1 - timestamp appended after data section */
	unsigned char timeRequest;  /* 0 no timestamp request, 1 timestamp request */
	unsigned char fragmentNumber; /* 0 - 0x2f */
	unsigned char offset; /* 6-bit */
	unsigned char frameNumber; /* 4-bit - number of the ethernet frame */
	struct _eoe_setip_parameter params;
};

struct _eoe_addrfilter_rsp {
	unsigned char type;
	unsigned char port; /* 0x00 (no spec. port) - 0x0f */
	unsigned char lastFragment;  /* 0 - at least one frame follows, 1 - last packet */
	unsigned char timeAppended;  /* 0 no time appended 1 - timestamp appended after data section */
	unsigned char timeRequest;  /* 0 no timestamp request, 1 timestamp request */
	uint16_t response;
};


struct _eoe_addrfilter_req {
	unsigned char type;
	unsigned char port; /* 0x00 (no spec. port) - 0x0f */
	unsigned char lastFragment;  /* 0 - at least one frame follows, 1 - last packet */
	unsigned char timeAppended;  /* 0 no time appended 1 - timestamp appended after data section */
	unsigned char timeRequest;  /* 0 no timestamp request, 1 timestamp request */
	unsigned char fragmentNumber; /* 0 - 0x2f */
	unsigned char offset; /* 6-bit */
	unsigned char frameNumber; /* 4-bit - number of the ethernet frame */
	struct _eoe_parameter parameter;
};

struct _eoe_addrfilter_rsp {
	unsigned char type;
	unsigned char port; /* 0x00 (no spec. port) - 0x0f */
	unsigned char lastFragment;  /* 0 - at least one frame follows, 1 - last packet */
	unsigned char timeAppended;  /* 0 no time appended 1 - timestamp appended after data section */
	unsigned char timeRequest;  /* 0 no timestamp request, 1 timestamp request */
	uint16_t response;
};

typedef struct
{
	unsigned FrameType:       4;
	unsigned Port:            4;
	unsigned LastFragment:    1;
	unsigned TimeAppended:    1;
	unsigned TimeRequested:   1;
	unsigned Reserved:        5;
	unsigned FragmentNumber:  6;
	unsigned CompleteSize:    6;
	unsigned FrameNumber:     4;
} TEOEHEADER;
#endif

void eoe_init(void);
int eoe_handler(chanend eoe_rx, chanend eoe_tx);
