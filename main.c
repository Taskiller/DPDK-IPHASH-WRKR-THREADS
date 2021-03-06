/*
 *The MIT License (MIT)
 *Copyright (c) 2016 Vipin Varghese
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation 
 *files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, 
 *modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
 *Software is furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
 *WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
 *COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "config.h"
#include "node.h"
#include "stats.h"

#include <getopt.h>

#include <rte_jhash.h>

#define TEST_TX 0

/* custom pkt template */
const char pktarpreq[64] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x0c, 0x29, 0x34, 0x0b, 0xde, 0x80, 0x35, 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x03, 0x00, 0x0c, 0x29, 0x34, 0x0b, 0xde, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x29, 0x34, 0x0b, 0xde, 0x00, 0x00, 0x00, 0x00};

const char pktarprep[64] = {0x00, 0x0c, 0x29, 0x34, 0x0b, 0xde, 0x00, 0x0c, 0x29, 0xc5, 0xf6, 0x9b, 0x80, 0x35, 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x04, 0x00, 0x0c, 0x29, 0xc5, 0xf6, 0x9b, 0x0a, 0x01, 0x01, 0x0a, 0x00, 0x0c, 0x29, 0x34, 0x0b, 0xde, 0x0a, 0x01, 0x01, 0x64};

const char pkt[1600] = {0x00, 0x0c, 0x29, 0xda, 0xd1, 0xde, 0x00, 0x0c, 0x29, 0xe3, 0xc6, 0x4d, 0x08, 0x00, 0x45, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x40, 0x00, 0x40, 0x11, 0x67, 0xbb, 0xc0, 0xa8, 0x28, 0xb3, 0xc0, 0xa8, 0x28, 0xb2, 0x08, 0x68, 0x08, 0x68, 0x00, 0x68, 0xbf, 0x64, 0x32, 0xff, 0x00, 0x58, 0x00, 0x00, 0x00, 0x01, 0x28, 0xdb, 0x00, 0x00, 0x45, 0x00, 0x00, 0x54, 0x00, 0x00, 0x40, 0x00, 0x40, 0x01, 0x5e, 0xa5, 0xca, 0x0b, 0x28, 0x9e, 0xc0, 0xa8, 0x28, 0xb2, 0x08, 0x00, 0xbe, 0xe7, 0x00, 0x00, 0x28, 0x7b, 0x04, 0x11, 0x20, 0x4b, 0xf4, 0x3d, 0x0d, 0x00, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};

const char pktTx[MAX_INTFCOUNT][14] = {
                                        {0x00, 0x0c, 0x29, 0xda, 0xd1, 0xde, 0x00, 0x0c, 0x29, 0xe3, 0xc6, 0x4d, 0x08, 0x00},
                                        {0x01, 0x0c, 0x29, 0xda, 0xd1, 0xde, 0x00, 0x0c, 0x29, 0xe3, 0xc6, 0x4d, 0x08, 0x00},
                                        {0x02, 0x0c, 0x29, 0xda, 0xd1, 0xde, 0x00, 0x0c, 0x29, 0xe3, 0xc6, 0x4d, 0x08, 0x00},
                                        {0x03, 0x0c, 0x29, 0xda, 0xd1, 0xde, 0x00, 0x0c, 0x29, 0xe3, 0xc6, 0x4d, 0x08, 0x00},
                                        {0x04, 0x0c, 0x29, 0xda, 0xd1, 0xde, 0x00, 0x0c, 0x29, 0xe3, 0xc6, 0x4d, 0x08, 0x00},
                                        {0x05, 0x0c, 0x29, 0xda, 0xd1, 0xde, 0x00, 0x0c, 0x29, 0xe3, 0xc6, 0x4d, 0x08, 0x00},
                                        {0x06, 0x0c, 0x29, 0xda, 0xd1, 0xde, 0x00, 0x0c, 0x29, 0xe3, 0xc6, 0x4d, 0x08, 0x00},
                                      };

/*default port configuration*/
static const struct rte_eth_conf port_conf_default = {
    .rxmode = {
        .split_hdr_size = 0,
        .header_split   = 0,    /**< Header Split disabled. */
        .hw_ip_checksum = 0,    /**< IP checksum offload disabled. */
        .hw_vlan_filter = 0,    /**< VLAN filtering enabled. */
        .hw_vlan_strip  = 0,    /**< VLAN strip enabled. */
        .hw_vlan_extend = 0,    /**< Extended VLAN disabled. */
        .jumbo_frame    = 0,    /**< Jumbo Frame Support disabled. */
        .hw_strip_crc   = 0,    /**< CRC stripping by hardware disabled. */
    },
    .txmode = {
        .mq_mode = ETH_MQ_TX_NONE,
    },
};

const struct rte_hash_parameters ipv4_hash_params = {
    .socket_id = 0,
    .name = "ipv4-hash",
    .entries = 1024,
    .key_len = 8,
    .hash_func = rte_jhash,
    .hash_func_init_val = 0,
};

uint16_t ipv4_hash_result [1024] = {0};

/*config parameters*/
uint8_t rxPorts = 1; /*recv interface worker threads with hashing*/
uint8_t fifoWrk = 2; /*fifo worker threads pkt processing*/
uint8_t prsOpt  = 0; /* 0 - all, 1 - v4, 2 - v6*/
uint8_t totalPorts = 0;
/*SW ring which acts as FIFO for worker threads*/
struct rte_ring *srb [MAX_FIFO];

struct rte_hash* ipv4_hash_table = NULL;
struct rte_hash* ipv6_hash_table = NULL;

/* E X T E R N */
extern pkt_stats_t prtPktStats [MAX_INTFCOUNT];
	
/* 
 * Initialize a given ring port
 */
static inline int
fifo_init(uint8_t fifoId)
{
    char srbName [25];

    if (fifoId >= MAX_FIFO) {
      printf("\n ERROR: Fifo ID: %d invalid!!!\n", fifoId);
      return -1;
    }

    printf("\n DEBUG: fifo index:%d\n", fifoId);
    sprintf(srbName, "%s%d", "SW-FIFO-", fifoId);
    srb [fifoId] = rte_ring_create(srbName, RTE_RING_SIZE, 
                                  0/*SOCKET_ID_ANY*/, 0);
    if (NULL == srb [fifoId]) {
      printf("ERROR: Cannot create output ring %s\n", srbName);
      return -2;
    }

    return 0;
}

/*
 * Initializes a given port using global settings and with the RX buffers
 * coming from the mbuf_pool passed as a parameter.
 */
static inline int
port_init(uint8_t port)
{
    char mempoolName[32] = {0};
    struct rte_mempool *mbuff_pool = NULL;

    struct rte_eth_conf port_conf = port_conf_default;
    int retval;
    
    struct rte_eth_dev_info devInfo;
    
    printf("\n\n INFO: setting up port %d\n", port);
    
    if (port >= rte_eth_dev_count())
      return -1;
    
    rte_eth_dev_info_get(port, &devInfo);

    /* allocate mempool TX and RX seperate */
    sprintf(mempoolName, "mbufpool-%d-%d-0-rx", devInfo.pci_dev->addr.function, port);
    mbuff_pool = rte_mempool_create(mempoolName, NUM_MBUFS,
                                    MBUF_CACHE_SIZE, 0,
                                    RTE_MBUF_DEFAULT_BUF_SIZE, 
                                    rte_pktmbuf_pool_init, NULL,
                                    rte_pktmbuf_init, NULL,
                                    0/*SOCKET_ID_ANY*/,
                                    MEMPOOL_F_SP_PUT | MEMPOOL_F_SC_GET);
    if (unlikely(mbuff_pool == NULL)) {
        rte_panic("\n ERROR: failed to get mem-pool for rx on node 0 intf %d\n", port);
        return -1;
    }
    //printf("\n INFO: created rx & tx mempool!!");

    /* Configure the Ethernet device. */
    retval = rte_eth_dev_configure(port, 1, 1, &port_conf);
    if (retval != 0)
      return retval;
    //printf("\n INFO: configured port with 1 rx and 1 tx queues!!");

    /* Allocate and set up 1 RX queue per Ethernet port. */
    retval = rte_eth_rx_queue_setup(port, 0, RX_RING_SIZE, 
                                    rte_eth_dev_socket_id(port), 
                                    NULL, mbuff_pool);
    if (retval < 0) {
      printf("\n DEBUG: RX queue ret %d!!", retval);
      return retval;
    }
    //printf("\n INFO: rx queue 0 setup!!");
    
    /* Allocate and set up 1 TX queue per Ethernet port. */
    retval = rte_eth_tx_queue_setup(port, 0, TX_RING_SIZE,
    			rte_eth_dev_socket_id(port), NULL);
    if (retval < 0) {
      printf("\n DEBUG: TX queue ret %d!!", retval);
      return retval;
    }
    
    //printf("\n INFO: tx queue 0 setup!!");
    
    /* Enable RX in promiscuous mode for the Ethernet device. */
    rte_eth_promiscuous_enable(port);
    
    /* Start the Ethernet port. */
    retval = rte_eth_dev_start(port);
    if (retval < 0)
    	return retval;
    
  return 0;
}

/*
 * Initializes a given port using global settings and with the RX buffers
 * coming from the mbuf_pool passed as a parameter.
 */
static inline int port_status ()
{
    int8_t port, ports = rte_eth_dev_count();
    struct ether_addr addr;
    struct rte_eth_link link;

    for (port = 0; port < ports; port++)
    {
      rte_eth_link_get(port, &link);
      {
        printf(" port:%u; duplex:%s, status:%s",
                (unsigned) port,
                (link.link_duplex == ETH_LINK_FULL_DUPLEX)?"Full":"half",
                (link.link_status == 1)?"up":"down");
      }
      
      /* Display the port MAC address. */
      rte_eth_macaddr_get(port, &addr);
      printf("MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8
    	   " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
    		addr.addr_bytes[0], addr.addr_bytes[1],
    		addr.addr_bytes[2], addr.addr_bytes[3],
    		addr.addr_bytes[4], addr.addr_bytes[5]);
    }

  return 0;
}

/*
 * This is the main therad that does the work, reading from
 * the FIFO queue and process the pkts for transmit ports.
 */
static int
lcore_fifoTransmit (void *arg)
{
    uint8_t fifoIndex = *((uint8_t *) arg);
    uint16_t nb_tx = 0;
    uint8_t txport = 0;
    int32_t ret = 0, i = 0, j = 0;
    uint32_t socketId = rte_lcore_to_socket_id(rte_lcore_id());

    char mempoolName[32] = {0};

    struct rte_mbuf *ptr[8] = {NULL}, *m = NULL;
    struct rte_mbuf *hdr[MAX_INTFCOUNT] = {NULL};
    struct rte_mempool *mbuff_Hdrpool = NULL;

    struct ether_hdr *ethHdr = NULL;
    struct ipv4_hdr  *ipHdr  = NULL;

    printf("\n INFO: %s FIFO: %d on socket %d lcore %d", \
            __func__, fifoIndex, socketId, rte_lcore_id());

    /*
     * approach 1: creates a clone of the input packet, walk though all segments of the input packet and for each of segment, 
                   create a new buffer and attach that new buffer to the segment. A new buffer is then allocated for the packet 
                   header and is prepended to the cloned buffer.
     * approach 2: does not make a clone, it just increments the reference counter for all input packet segment, allocates a 
                   new buffer for the packet header and prepends it to the input packet.
     */

    /* create the mbuff pool for new pkt header*/
    sprintf(mempoolName, "mbufhdrpool-%d", fifoIndex);
    mbuff_Hdrpool = rte_mempool_create(mempoolName, NUM_MBUFS,
                                    MBUF_CACHE_SIZE, 0,
                                    RTE_MBUF_DEFAULT_BUF_SIZE, 
                                    rte_pktmbuf_pool_init, NULL,
                                    rte_pktmbuf_init, NULL,
                                    0/*SOCKET_ID_ANY*/,
                                    0/*MEMPOOL_F_SP_PUT | MEMPOOL_F_SC_GET*/);
    if (unlikely(mbuff_Hdrpool == NULL)) {
        rte_panic("\n ERROR: failed to get mem-pool for fifo %d\n", fifoIndex);
        return -1;
    }

    /*
     * The first approach reuses only the input packet’s data, but creates its own copy of packet’s metadata. 
     * The second approach reuses both input packet’s data and metadata : zero-copy buffer cloning
     */

    /*fetch tx port*/
    uint8_t port = 0;

    while (1) {
      //ret = rte_ring_sc_dequeue_bulk(srb[fifoIndex], (void *)&ptr, 8);
      ret = rte_ring_dequeue_burst(srb[fifoIndex], (void *)&ptr, 8);
      if (likely(ret)) {
 
        /*fetch tx port from userdata64 of mbuf*/
        for (i =0 ; i < ret; i ++)
        {
          m = ptr[i];
          txport = (ptr[i]->udata64 & 0xff00)>>8;

#if 1
        /* Remove the Ethernet header from the input packet */
        ipHdr = (struct ipv4_hdr *)rte_pktmbuf_adj(m, sizeof(struct ether_hdr));
        RTE_MBUF_ASSERT(ipHdr != NULL);

        /*alloc new mbuf for the header*/
        for (j = 0; j < (totalPorts - rxPorts); j++) 
        {
          hdr[j] = rte_pktmbuf_alloc(mbuff_Hdrpool);
	  if (unlikely(hdr[j] == NULL)) {
            prtPktStats[(rxPorts + j)].dropped += ((totalPorts - rxPorts) - j);
	    break;
	  }
          prtPktStats[(rxPorts + j)].queue_fet[fifoIndex] += 1;
          
          /* prepend new header */
	  hdr[j]->next = m;
	
	  /* update header's fields */
	  hdr[j]->pkt_len = (uint16_t)(hdr[j]->data_len + m->pkt_len);
	  hdr[j]->nb_segs = (uint8_t)(m->nb_segs + 1);
	
	  /* copy metadata from source packet */
	  hdr[j]->port           = m->port;
          hdr[j]->vlan_tci       = m->vlan_tci;
          hdr[j]->vlan_tci_outer = m->vlan_tci_outer;
          hdr[j]->hash           = m->hash;
          hdr[j]->ol_flags       = m->ol_flags;

          rte_mbuf_sanity_check(hdr[j], 1);

          /*add custom header ethernet + ip in hdr[j]*/
          ethHdr = rte_pktmbuf_mtod(hdr[j], struct ether_hdr *);
          rte_memcpy((void *)ethHdr, (void *)pktTx[j + rxPorts], 14);

          /*transmit pkts to tx ports*/ 
          nb_tx = rte_eth_tx_burst((rxPorts + j), 0, &hdr[j], 1);
          if (nb_tx != 1) {
            prtPktStats[rxPorts + j].queue_drp[fifoIndex] += 1;
            rte_pktmbuf_free(hdr[j]);
          }
        }

        /*since we use approach 2, I think we should free the original packet*/
        rte_pktmbuf_free(m);
#else

          if (unlikely(txport < rxPorts)) {
            rte_exit(EXIT_FAILURE, "txport %d is invalid!!", txport);
          }

          nb_tx = rte_eth_tx_burst(txport, 0, &m, 1);
          if (nb_tx != 1) {
            prtPktStats[port + rxPorts].queue_drp[fifoIndex] += 1;
            rte_pktmbuf_free(m);
          }
#endif
        }
      }
    }  

    return 0;
}

/*
 * This is the main thread that does the work, reading from
 * an input port and hashing to FIFO queues.
 */
static int
lcore_testCuckoohash(void *arg)
{
    uint8_t rxport = *((uint8_t *) arg);
    uint8_t txport = rxport + rxPorts; /* use offset for mentioning tx port*/
    uint8_t fifoIndex = 0;

    uint8_t lkpPort = 0, lkpFifo = 0;

    totalPorts = rte_eth_dev_count();
    int32_t ret = 0, i = 0, j = 0;
    uint32_t socketId = rte_lcore_to_socket_id(rte_lcore_id());

    uint64_t key = 0, data = 0;

    struct rte_eth_link rxLink, txLink;

    struct rte_mempool *mbuff_pool = NULL;

    struct rte_mbuf *m = NULL, *ptr[8] = {NULL};

    struct ether_hdr *ethHdr = NULL;
    struct ipv4_hdr  *ipHdr  = NULL;

    printf("\n INFO: %s port RX: %d TX: %d on socket %d lcore %d", \
            __func__, rxport, txport, socketId, rte_lcore_id());

    char mempoolName[25];
    struct rte_mbuf *bufs[BURST_SIZE];

    sprintf(mempoolName, "mbufpool-%d-0-tx", (int)rxport);
    mbuff_pool = rte_mempool_create(
                    mempoolName, NUM_MBUFS,
                    MBUF_CACHE_SIZE, 0,
                    RTE_MBUF_DEFAULT_BUF_SIZE,
                    rte_pktmbuf_pool_init, NULL,
                    rte_pktmbuf_init, NULL,
                    0/*SOCKET_ID_ANY*/,
                    MEMPOOL_F_SP_PUT | MEMPOOL_F_SC_GET);
    if (unlikely(mbuff_pool == NULL)) {
        rte_panic("\n ERROR: failed to get mem-pool for rx on node 0 intf %d\n", rxport);
        return -1;
    }
    //printf("\n INFO: created tx mempool!!");

    if (rte_eth_dev_socket_id(rxport) > 0 &&
            rte_eth_dev_socket_id(rxport) !=
            (int)rte_socket_id())
            printf("WARNING,RX port %u is on remote NUMA node to "
                        "polling thread.\n\tPerformance will "
                        "not be optimal.\n", rxport);

    if (rte_eth_dev_socket_id(txport) > 0 &&
            rte_eth_dev_socket_id(txport) !=
            (int)rte_socket_id())
            printf("WARNING, TX port %u is on remote NUMA node to "
                        "polling thread.\n\tPerformance will "
                        "not be optimal.\n", txport);

    printf("\nCore %u Recieving packets on %d Transmit on %d. [Ctrl+C to quit]\n",
                        rte_lcore_id(), rxport, txport);

    /* check RX and TX port are functional */
    if ((rxport >= totalPorts) || 
        ((txport) >= totalPorts)) {
        rte_panic("\n ERROR: Total ports %d, port RX: %d TX: %d\n", totalPorts, rxport, txport);
        return -1;
    } 
    
    rte_eth_link_get(rxport, &rxLink);
    rte_eth_link_get(txport, &txLink);

    if ((rxLink.link_status == ETH_LINK_DOWN) || 
        (txLink.link_status == ETH_LINK_DOWN)) {
        rte_panic("\n ERROR: ports RX %u - %s TX: %u - %s\n", 
                   rxport, (rxLink.link_status == ETH_LINK_DOWN)?"Down":"Up",
                   txport, (txLink.link_status == ETH_LINK_DOWN)?"Down":"Up");
        return -1;
    }

    fflush(stdout); /*flsuh all details before we start the thread*/

    while (1) {
      const int16_t nb_rx = rte_eth_rx_burst(rxport, 0, bufs, BURST_SIZE);

      if (unlikely(nb_rx == 0)) {
        //printf("\n pkts rcv %d", nb_rx);
#if TEST_TX
      for(i = 0; i < 8; i++ )
      {
          if (unlikely((ptr[i] = rte_pktmbuf_alloc(mbuff_pool)) == NULL)) {
              printf("\n -------------- ");
              continue;
          }

          /* memcpy pkt content */
          ethHdr = rte_pktmbuf_mtod(ptr[i], struct ether_hdr *);

#if 0
          /*dummy pkt*/
          ptr[i]->data_len = ptr[i]->pkt_len = 120;
          rte_memcpy(((char *)ethHdr), pkt,ptr[i]->pkt_len);
          /*arp-req pkt*/
          rte_memcpy(((char *)ethHdr), pktarpreq,ptr[i]->pkt_len);
#endif
          ptr[i]->data_len = ptr[i]->pkt_len = 64;
          /*arp-rep pkt*/
          rte_memcpy(((char *)ethHdr), pktarpreq,ptr[i]->pkt_len);
      }
      //rte_pktmbuf_dump (stdout, ptr[0], 64);

      ethHdr = rte_pktmbuf_mtod(ptr[0], struct ether_hdr *);
      struct arp_hdr *arpHdr = (struct arp_hdr *) ((char *) ethHdr + 14);

      printf("\n ETH-ARP DETAILS \n");
      printf("ETHER");
      printf(" - dst: %2x:%2x:%2x:%2x:%2x:%2x", 
                                               ethHdr->d_addr.addr_bytes[0], ethHdr->d_addr.addr_bytes[1], 
                                               ethHdr->d_addr.addr_bytes[2], ethHdr->d_addr.addr_bytes[3], 
                                               ethHdr->d_addr.addr_bytes[4], ethHdr->d_addr.addr_bytes[5]);
      printf(" - src: %2x:%2x:%2x:%2x:%2x:%2x",
                                               ethHdr->s_addr.addr_bytes[0], ethHdr->s_addr.addr_bytes[1], 
                                               ethHdr->s_addr.addr_bytes[2], ethHdr->s_addr.addr_bytes[3], 
                                               ethHdr->s_addr.addr_bytes[4], ethHdr->s_addr.addr_bytes[5]);
      printf(" - type: %x", ethHdr->ether_type);
      printf("ARP");
      printf(" - arp_hrd: %u", arpHdr->arp_hrd);
      printf(" - arp_pro: %u", arpHdr->arp_pro);
      printf(" - arp_hln: %u", arpHdr->arp_hln);
      printf(" - arp_pln: %u", arpHdr->arp_pln);
      printf(" - arp_op: %u", arpHdr->arp_op);
      printf(" - arp_data.arp_sha: %2x:%2x:%2x:%2x:%2x:%2x", 
                                                           arpHdr->arp_data.arp_sha.addr_bytes[0], arpHdr->arp_data.arp_sha.addr_bytes[1],
                                                           arpHdr->arp_data.arp_sha.addr_bytes[2], arpHdr->arp_data.arp_sha.addr_bytes[3],
                                                           arpHdr->arp_data.arp_sha.addr_bytes[4], arpHdr->arp_data.arp_sha.addr_bytes[5]);
      printf(" - arp_data.arp_sip: %u", arpHdr->arp_data.arp_sip);
      printf(" - arp_data.arp_tha: %2x:%2x:%2x:%2x:%2x:%2x", 
                                                           arpHdr->arp_data.arp_tha.addr_bytes[0], arpHdr->arp_data.arp_tha.addr_bytes[1], 
                                                           arpHdr->arp_data.arp_tha.addr_bytes[2], arpHdr->arp_data.arp_tha.addr_bytes[3], 
                                                           arpHdr->arp_data.arp_tha.addr_bytes[4], arpHdr->arp_data.arp_tha.addr_bytes[5]);
      printf(" - arp_data.arp_tip: %u", arpHdr->arp_data.arp_tip);


      ret = rte_eth_tx_burst(txport, 0, (struct rte_mbuf **)&ptr, 8);
      if (unlikely(8 != ret )) {
          for (i = ret; i < 8; i++)
          {
            m = ptr[i];
            rte_pktmbuf_free(m);
          }
      }
#endif
        continue;
      }

      //printf("\n INFO: recv pkt count: %d", nb_rx);
      //rte_pktmbuf_dump (stdout, ptr[0], 64); /*dump the first packet content*/

      /* prefetch packets for pipeline */
      for (j = 0; ((j < PREFETCH_OFFSET) &&
                   (j < nb_rx)); j++)
      {
        rte_prefetch0(rte_pktmbuf_mtod(bufs[j], void *));
      } /*for loop till PREFETCH_OFFSET*/

      /* 
       * check packet ether type 
       *  - IPv4: fetch src and dst
       *  - IPv6: to do
       *  - others: drop
       */

      for (j = 0; j < (nb_rx - PREFETCH_OFFSET); j++) /* Prefetch others packets */
      {
        m = bufs[j];
        rte_prefetch0(rte_pktmbuf_mtod(bufs[j + PREFETCH_OFFSET], void *));

        ethHdr = rte_pktmbuf_mtod(m, struct ether_hdr*);
        //printf("\n ether type : %x\n", ethHdr->ether_type);

/*
        printf("\n dst MAC: %x:%x:%x:%x:%x:%x port %u ",
            ethHdr->d_addr.addr_bytes[0], ethHdr->d_addr.addr_bytes[1],
            ethHdr->d_addr.addr_bytes[2], ethHdr->d_addr.addr_bytes[3],
            ethHdr->d_addr.addr_bytes[4], ethHdr->d_addr.addr_bytes[5],
            m->port);
*/

        if (likely(ethHdr->ether_type == 0x0008)) {
            prtPktStats[rxport].rx_ipv4 += 1;

            ipHdr = (struct ipv4_hdr *) ((char *)(ethHdr + 1));
            //printf("\n IP src:%x dst: %x", ipHdr->src_addr, ipHdr->dst_addr);


#if 0
            const uint16_t nb_tx = rte_eth_tx_burst(txport, 0, &m, 1);
            if (likely(nb_tx == 1)) {
                continue;
            }
#else
            /* lkp builk for ip src-dst pair*/
            key = ipHdr->src_addr | (ipHdr->dst_addr << 32);
            ret = rte_hash_lookup(ipv4_hash_table, (const void *)&key);
            if (likely(ret >= 0)) {
              //printf("\n ret %d data: %x\n", ret, ipv4_hash_result[ret]);
              fifoIndex = ipv4_hash_result[ret] & 0xff;
              m->udata64 = ipv4_hash_result[ret];
            }
            else if (-ENOENT == ret) {
              /*add entry with new fifoindex and tx port*/
              fifoIndex = (lkpFifo++)%fifoWrk;
              txport = ((lkpPort++ % (rte_eth_dev_count() - rxPorts)) + rxPorts);
             
              if (txport < rxPorts)
                rte_exit(EXIT_FAILURE, "\n computed tx port %d is invalid", txport);

              ret = rte_hash_add_key(ipv4_hash_table, &key); 
              if (0 < ret) { 
                printf("\n ERROR: failed to add entries to hash table");
                rte_pktmbuf_free(m);
                continue;
              }

              ipv4_hash_result[ret] = fifoIndex | txport << 8;
              m->udata64 = ipv4_hash_result[ret];
            }
            else {
              printf("\n ERROR: invalid params");
              rte_pktmbuf_free(m);
              continue;
            }

            if (m->udata64 == 0)
              rte_exit(EXIT_FAILURE, "\n usrdata %d is invalid", m->udata64);

            //printf("\n mbuf userdata : %x ret %d\n", m->udata64, ret);

            /* queue the pkts to fifo based on hash result*/
            ret = rte_ring_mp_enqueue(srb[fifoIndex], m);
            //ret = rte_ring_enqueue_burst(srb[fifoIndex], (void *)m, 1);
            if (likely(ret == 0)) {
              prtPktStats[rxport].queue_add[fifoIndex] += 1;
              continue;
            }
#endif
            else 
              prtPktStats[rxport].txQueueErr += 1;
        }
        else
          prtPktStats[rxport].non_ip += 1;

         prtPktStats[rxport].dropped += 1;
         rte_pktmbuf_free(m); /*Free the mbuff*/
      }

      for (; j < nb_rx; j++)
      {
        m = bufs[j];

        ethHdr = rte_pktmbuf_mtod(m, struct ether_hdr*);
        //printf("\n ether type : %x\n", ethHdr->ether_type);

/*
        printf("\n dst MAC: %x:%x:%x:%x:%x:%x port %u ",
            ethHdr->d_addr.addr_bytes[0], ethHdr->d_addr.addr_bytes[1],
            ethHdr->d_addr.addr_bytes[2], ethHdr->d_addr.addr_bytes[3],
            ethHdr->d_addr.addr_bytes[4], ethHdr->d_addr.addr_bytes[5],
            m->port);
*/

        if (likely(ethHdr->ether_type == 0x0008)) {
            prtPktStats[rxport].rx_ipv4 += 1;

            ipHdr = (struct ipv4_hdr *) ((char *)(ethHdr + 1));
            //printf("\n IP src:%x dst: %x", ipHdr->src_addr, ipHdr->dst_addr);

            /*ToDo: fetch details - process on IP to form HASH lkp*/

#if 0
            const uint16_t nb_tx = rte_eth_tx_burst(txport, 0, &m, 1);
            if (likely(nb_tx == 1)) {
                continue;
            }
#else
            /* lkp builk for ip src-dst pair*/
            key = ipHdr->src_addr | (ipHdr->dst_addr << 32);
            ret = rte_hash_lookup(ipv4_hash_table, (const void *)&key);
            if (likely(ret >= 0)) {
              //printf("\n ret %d data: %x\n", ret, ipv4_hash_result[ret]);
              fifoIndex = ipv4_hash_result[ret] & 0xff;
              m->udata64 = ipv4_hash_result[ret];
            }
            else if (-ENOENT == ret) {
              /*add entry with new fifoindex and tx port*/
              fifoIndex = (lkpFifo++)%fifoWrk;
              txport = ((lkpPort++ % (rte_eth_dev_count() - rxPorts)) + rxPorts);
              
              if (txport < rxPorts)
                rte_exit(EXIT_FAILURE, "\n computed tx port %d is invalid", txport);

              ret = rte_hash_add_key(ipv4_hash_table, &key);
              if (0 < ret) {
                printf("\n ERROR: failed to add entries to hash table");
                rte_pktmbuf_free(m);
                continue;
              }

              ipv4_hash_result[ret] = fifoIndex | txport << 8;
              m->udata64 = ipv4_hash_result[ret];
            }
            else {
              printf("\n ERROR: invalid params");
              rte_pktmbuf_free(m);
              continue;
            }

            if (m->udata64 == 0)
              rte_exit(EXIT_FAILURE, "\n usrdata %d is invalid", m->udata64);

            //printf("\n mbuf userdata : %x ret %d\n", m->udata64, ret);

            /* queue the pkts to fifo based on hash result*/
            ret = rte_ring_mp_enqueue(srb[fifoIndex], m);
            if (likely(ret == 0)) {
              prtPktStats[rxport].queue_add[fifoIndex] += 1;
              continue;
            }
#endif
            else
              prtPktStats[rxport].txQueueErr += 1;
        }
        else
          prtPktStats[rxport].non_ip += 1;

        prtPktStats[rxport].dropped += 1;
        rte_pktmbuf_free(m); /*Free the mbuff*/
      }
#if 0
      /* Send burst of TX packets, to second port of pair. */
      const uint16_t nb_tx = rte_eth_tx_burst((port + 1), 0, bufs, nb_rx);

      /* Free any unsent packets. */
      if (unlikely(nb_tx < nb_rx)) {
        uint16_t buf;
        for (buf = nb_tx; buf < nb_rx; buf++)
          rte_pktmbuf_free(bufs[buf]);
      }
#endif
    }  

    return 0;
}

static int display (void)
{
    do {
      rte_delay_ms(1000);
      rte_timer_manage();
    }while(1);

    return 0;
}


/* display usage */
static void
print_usage(const char *prgname)
{
	printf ("%s [EAL options] -- -r Recieve Ports <integer>"\
		"  -f FIFO: number of worker cores <2 ^ n>\n"\
		"  -P : enable promiscuous mode\n"\
		"  -4 : hash only ipv4\n"\
		"  -6 : hash only ipv6\n"\
		"  -a : hash both ipv4 and ipv6\n",
		prgname);
}

/* Parse the argument given in the command line of the application */
static int
parse_args(int argc, char **argv)
{
    int opt, ret = 0;
    char **argvopt;
    int option_index;
    char *prgname = argv[0];
    char *end = NULL;
    long pm;

    static struct option lgopts[] = {
                                     {CMD_LINE_OPT_RX, required_argument, 0, 'r'},
                                     {CMD_LINE_OPT_FIFO, required_argument, 0, 'f'},
                                     {CMD_LINE_OPT_PARSE_IPV4, no_argument, 0, '4'},
                                     {CMD_LINE_OPT_PARSE_IPV6, no_argument, 0, '6'},
                                     {CMD_LINE_OPT_PARSE_IP, no_argument, 0, 'a'},
                                     {NULL, 0, 0, 0}
                                    };

    argvopt = argv;

    while ((opt = getopt_long(argc, argvopt, "r:f:a:4:6",
		              lgopts, &option_index)) != EOF) {
      switch (opt) {
        case 'r':
          pm = strtol(optarg, &end, 10);
          if ((optarg[0] == '\0') || (end == NULL) || (*end != '\0') || (pm < 1)) {
                printf("\n ERR: RX DPDK intf invalid!!!\n");
                print_usage(prgname);
		return -1;
          }

          rxPorts = (uint8_t)pm;
          break;

        case 'f':
          pm = strtol(optarg, &end, 10);
          if ((optarg[0] == '\0') || (end == NULL) || (*end != '\0') || (pm < 1)) {
                printf("\n ERR: FIFO DPDK workers count Invalid!!!\n");
                print_usage(prgname);
		return -1;
          }

          fifoWrk = (uint8_t)((pm & 0xfe) == 0)?fifoWrk:(pm & 0xfe);
          break;

        case 'a':
          prsOpt = 0;
          break;

        case '4':
          prsOpt = 1;
          break;

        case '6':
          prsOpt = 2;
          break;

        default:
	  print_usage(prgname);
	  return -1;
      }
    }

    return ret;
}

/*
 * The main function, which does initialization and calls the per-lcore
 * functions.
 */
int
main(int argc, char *argv[])
{
    unsigned nb_ports, nb_workers, workers, wlcore;
    uint8_t portid = 0, argPort = 0;

    /*check if dpdk version is greater than 2.1 to support cuckoo hash*/
    if ((RTE_VER_YEAR < 16) && (RTE_VER_MONTH < 4))
    	rte_exit(EXIT_FAILURE, "DPDK version may not support Cuckoo hash Collision!\n");
    printf("\n INFO: DPDK Ver: %s\n", rte_version());

    /* Initialize the Environment Abstraction Layer (EAL). */
    int ret = rte_eal_init(argc, argv);
    if (ret < 0)
    	rte_exit(EXIT_FAILURE, "with EAL initialization\n");
    
    argc -= ret;
    argv += ret;
   
    /* Register Signal */
    signal(SIGUSR1, sigExtraStats);
    signal(SIGUSR2, sigDetails);

    ret = parse_args(argc, argv);
    if (ret < 0)
	rte_exit(EXIT_FAILURE, "Invalid parameters\n");
    printf ("\n DEBUG: RX:%d, FIFO:%d, Parse Option: %d", rxPorts, fifoWrk, prsOpt);

    /* Check that there is an even number of ports to send/receive on. */
    nb_ports = rte_eth_dev_count();
    if (nb_ports <= 1)
    	rte_exit(EXIT_FAILURE, "Error: In sufficent ports available\n");

    /*check recv intf count and available dpdk are in sync*/
    if (rxPorts >= (nb_ports))
	rte_exit(EXIT_FAILURE, "Insufficent TX ports, please reduce RX %d or increase DPDK bind ports!!!\n", rxPorts);

    /*
     * check sufficent lcore are present for running the application
     *  - Master core: stats are collected for dipaly
     *  - RX threads: reads from RX ports
     *  - FIFO threads: reads from SW ring
     * **Condition ==> lcoreCount >= (master+RX+FIFO)
     */
    nb_workers = rte_lcore_count();
    if (nb_workers <= (unsigned) (rxPorts + fifoWrk))
	rte_exit(EXIT_FAILURE, "Insufficent LCORE, avail: %d, RX: %d, FIFO:%d Master: 1!!!\n", nb_workers, rxPorts, fifoWrk);

    printf ("\n INFO: Total NIC on which function starts %d", rxPorts);
    printf ("\n INFO: Total FIFO on which function starts %d", fifoWrk);

    /*
     * Initalizae the ports under DPDK
     */
    for (portid = 0; portid < nb_ports; portid++) {
      argPort = portid;
      if (port_init(argPort) != 0)
        rte_exit(EXIT_FAILURE, "Cannot init port %"PRIu8 "\n", argPort);
    }
    printf("\nINFO: Ports: total %d Initialized %d\n", nb_ports, workers);

    /*
     * create ring for tx count = nb_ports - workers
     * RING Enqueue
     *  - single producer if RX is 1
     *  - multi producer if  RX is greater than 1
     * RING Dequeue - is always single consumer
     */
    for (portid = 0; portid < fifoWrk; portid++) {
      argPort = portid;
      if (fifo_init(argPort) != 0)
        rte_exit(EXIT_FAILURE, "Cannot init SW RING %"PRIu8 "\n", argPort);
    }
    printf("\nINFO: FIFO: Initialized %d\n", fifoWrk);

    /*
     * create hash function for ipv4
     */
     ipv4_hash_table = rte_hash_create(&ipv4_hash_params); 	
     if (NULL == ipv4_hash_table)
        rte_exit(EXIT_FAILURE, " Failed to create hash table for ipv4 %s\n", rte_strerror(rte_errno));

    /*
     * create hash function for ipv6
     */

    /*configuration info*/
    printf("\n#########################################################\n");
    rte_mempool_list_dump(stdout);
    printf("\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
    rte_ring_list_dump(stdout);
    printf("\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
    port_status();
    printf("\n#########################################################\n");

    /*
     * launch RECV threads
     */
    wlcore = rte_get_next_lcore(-1, 1, 0);

    for (portid = 0; portid < rxPorts; portid++) {
        argPort = portid;
        ret = rte_eal_remote_launch(lcore_testCuckoohash, (void *) &argPort, wlcore);
        if (ret != 0)
          rte_exit(EXIT_FAILURE, "\n unable to launch on core id %d!!", wlcore);
        printf("\n Initialized lcore %d with port %d", wlcore, argPort);

        wlcore = rte_get_next_lcore(wlcore, 1, 0);
    }
    printf("\nINFO: started RECV worker threads\n");

    /*
     * launch FIFO worker threads
     */
    for (portid = 0; portid < fifoWrk; portid++) {
        argPort = portid;
        ret = rte_eal_remote_launch(lcore_fifoTransmit, (void *) &argPort, wlcore);
        if (ret != 0)
          rte_exit(EXIT_FAILURE, "\n unable to launch on core id %d!!", wlcore);
        printf("\n Initialized lcore %d with fifo Index %d", wlcore, argPort);

        wlcore = rte_get_next_lcore(wlcore, 1, 0);
    }
    printf("\nINFO: started FIFO worker threads\n");

    if (ret == 0) {
      rte_delay_ms(1500);
      STATS_CLR_SCREEN;

      set_stats_timer();
      show_static_display();
     
      while (1) display();
    }

    return 0;
}
