#ifndef CLICK_IPGWOPTIONS_HH
#define CLICK_IPGWOPTIONS_HH
#include <click/element.hh>
#include <click/glue.hh>
#include <click/atomic.hh>
#include <click/ipaddresslist.hh>
CLICK_DECLS

/*
 * =c
 * IPGWOptions(MYADDR [, OTHERADDRS])
 * =s IP
 * processes router IP options
 * =d
 * Process the IP options that should be processed by every router,
 * not just when ip_dst refers to the current router. At the moment
 * that amounts to Record Route and Timestamp (in particular,
 * not the source route options). MYADDR is the router's
 * IP address on the interface downstream from the element.
 *
 * Probably needs to be placed on the output path, since MYADDR
 * must be the outgoing interface's IP address (rfc1812 4.2.2.2).
 *
 * Recomputes the IP header checksum if it modifies the packet.
 *
 * The optional OTHERADDRS argument should be a space-separated list of IP
 * addresses containing the router's other interface addresses. It is used to
 * implement the Timestamp option.
 *
 * The second output may be connected to an ICMPError to produce
 * a parameter problem (type=12,code=0) message. IPGWOptions sets
 * the param_off packet annotation so that ICMPError can set
 * the Parameter Problem pointer to point to the erroneous byte.
 *
 * =a IPDstOptions, ICMPError */

class IPGWOptions : public Element { public:
  
  IPGWOptions();
  ~IPGWOptions();
  
  const char *class_name() const		{ return "IPGWOptions"; }
  const char *processing() const		{ return "a/ah"; }
  int configure(Vector<String> &, ErrorHandler *);
  void notify_noutputs(int);
  IPGWOptions *clone() const;
  void add_handlers();

  uint32_t drops() const			{ return _drops; }
  
  Packet *handle_options(Packet *);
  Packet *simple_action(Packet *);
  
 private:
  
  uatomic32_t _drops;
  struct in_addr _preferred_addr;
  IPAddressList _my_addrs;

};

CLICK_ENDDECLS
#endif
