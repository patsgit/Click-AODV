#ifndef CLICK_STATIONTABLE_HH
#define CLICK_STATIONTABLE_HH
#include <click/element.hh>
#include <click/ipaddress.hh>
#include <click/etheraddress.hh>
#include <click/bighashmap.hh>
#include <click/glue.hh>
CLICK_DECLS

/*
 * =c
 * 
 * StationTable()
 * 
 *
 */



class Station {
public:
  EtherAddress _eth;
  struct timeval _when; // When we last heard from this node.
  Station() { 
    memset(this, 0, sizeof(*this));
  }
};
  


class StationTable : public Element { public:
  
  StationTable();
  ~StationTable();
  
  const char *class_name() const		{ return "StationTable"; }

  int configure(Vector<String> &, ErrorHandler *);
  bool can_live_reconfigure() const		{ return true; }

  void add_handlers();
  void take_state(Element *e, ErrorHandler *);


  bool _debug;

private:
  
  typedef HashMap<EtherAddress, Station> STable;
  typedef STable::const_iterator STIter;
  
  STable _table;

};

CLICK_ENDDECLS
#endif