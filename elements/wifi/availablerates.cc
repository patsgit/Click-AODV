/*
 * availablerates.{cc,hh} -- Poor man's arp table
 * John Bicket
 *
 * Copyright (c) 2003 Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the conditions
 * listed in the Click LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the Click LICENSE file; the license in that file is
 * legally binding.
 */

#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/glue.hh>
#include <click/straccum.hh>
#include <clicknet/ether.h>
#include "availablerates.hh"
CLICK_DECLS

AvailableRates::AvailableRates()
  : Element(0, 0)
{
  MOD_INC_USE_COUNT;

  /* bleh */
  static unsigned char bcast_addr[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  _bcast = EtherAddress(bcast_addr);

}

AvailableRates::~AvailableRates()
{
  MOD_DEC_USE_COUNT;
}

void *
AvailableRates::cast(const char *n)
{
  if (strcmp(n, "AvailableRates") == 0)
    return (AvailableRates *) this;
  else
    return 0;
}

int
AvailableRates::parse_and_insert(String s, ErrorHandler *errh)
{
  EtherAddress e;
  Vector<int> rates;
  Vector<String> args;
  cp_spacevec(s, args);
  if (args.size() < 2) {
    return errh->error("error param %s must have > 1 arg", s.cc());
  }
  if (!cp_ethernet_address(args[0], &e)) 
    return errh->error("error param %s: must start with ethernet address", s.cc());
  for (int x = 1; x< args.size(); x++) {
    int r;
    cp_integer(args[x], &r);
    rates.push_back(r);
  }
  
  DstInfo d = DstInfo(e);
  d._rates = rates;
  d._eth = e;
  _rtable.insert(e, d);
  return 0;
}
int
AvailableRates::configure(Vector<String> &conf, ErrorHandler *errh)
{
  int res = 0;
  for (int x = 0; x < conf.size(); x++) {
    res = parse_and_insert(conf[x], errh);
    if (res != 0) {
      return res;
    }
  }

  return res;
}

void 
AvailableRates::take_state(Element *e, ErrorHandler *)
{
  AvailableRates *q = (AvailableRates *)e->cast("AvailableRates");
  if (!q) return;
  _rtable = q->_rtable;

}
Vector<int>
AvailableRates::lookup(EtherAddress eth)
{
  if (!eth) {
    click_chatter("%s: lookup called with NULL eth!\n", id().cc());
    return Vector<int>();
  }
  DstInfo *dst = _rtable.findp(eth);
  if (dst) {
    return dst->_rates;
  }
  return Vector<int>();
}

int
AvailableRates::insert(EtherAddress eth, Vector<int> rates) 
{
  if (!(eth)) {
    click_chatter("AvailableRates %s: You fool, you tried to insert %s\n",
		  id().cc(),
		  eth.s().cc());
    return -1;
  }
  DstInfo *dst = _rtable.findp(eth);
  if (!dst) {
    _rtable.insert(eth, DstInfo(eth));
    dst = _rtable.findp(eth);
  }
  dst->_eth = eth;
  dst->_rates = rates;
  return 0;
}



enum {H_DEBUG, H_INSERT, H_REMOVE, H_RATES};


static String
AvailableRates_read_param(Element *e, void *thunk)
{
  AvailableRates *td = (AvailableRates *)e;
  switch ((uintptr_t) thunk) {
  case H_DEBUG:
    return String(td->_debug) + "\n";
  case H_RATES: {
    StringAccum sa;
    for (AvailableRates::RIter iter = td->_rtable.begin(); iter; iter++) {
      AvailableRates::DstInfo n = iter.value();
      sa << n._eth.s().cc() << " ";
      for (int x = 0; x < n._rates.size(); x++) {
	sa << " " << n._rates[x];
      }
      sa << "\n";
    }
    return sa.take_string();
  }
  default:
    return String();
  }
}
static int 
AvailableRates_write_param(const String &in_s, Element *e, void *vparam,
		      ErrorHandler *errh)
{
  AvailableRates *f = (AvailableRates *)e;
  String s = cp_uncomment(in_s);
  switch((int)vparam) {
  case H_DEBUG: {
    bool debug;
    if (!cp_bool(s, &debug)) 
      return errh->error("debug parameter must be boolean");
    f->_debug = debug;
    break;
  }
  case H_INSERT: 
    return f->parse_and_insert(in_s, errh);
  case H_REMOVE: {
    EtherAddress e;
    if (!cp_ethernet_address(s, &e)) 
      return errh->error("remove parameter must be ethernet address");
    f->_rtable.remove(e);
    break;
  }

  }
  return 0;
}

void
AvailableRates::add_handlers()
{
  add_default_handlers(true);


  add_read_handler("debug", AvailableRates_read_param, (void *) H_DEBUG);
  add_read_handler("rates", AvailableRates_read_param, (void *) H_RATES);


  add_write_handler("debug", AvailableRates_write_param, (void *) H_DEBUG);
  add_write_handler("insert", AvailableRates_write_param, (void *) H_INSERT);
  add_write_handler("remove", AvailableRates_write_param, (void *) H_REMOVE);

  
}




// generate Vector template instance
#include <click/bighashmap.cc>
#if EXPLICIT_TEMPLATE_INSTANCES
template class HashMap<IPAddress, AvailableRates::DstInfo>;
#endif
CLICK_ENDDECLS
EXPORT_ELEMENT(AvailableRates)
