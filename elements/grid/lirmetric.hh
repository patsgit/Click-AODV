#ifndef LIRMETRIC_HH
#define LIRMETRIC_HH
#include <click/element.hh>
#include "gridgenericmetric.hh"
CLICK_DECLS

/*
 * =c
 * LIRMetric(GridGenericRouteTable)
 * =s Grid
 * =io
 * None
 * =d
 *
 * Child class of GridGenericMetric that implements the `Least
 * Interference Routing' metric.  The metric is the sum of the number
 * 1-hop neighbors of each node in the route.  This node's number of
 * 1-hop neighbors is obtained from the GridGenericRouteTable
 * argument.  Smaller metric values are better.
 *
 * LIR is described in `Spatial Reuse Through Dynamic Power and
 * Routing Control in Common-Channel Random-Access Packet Radio
 * Networks', James Almon Stevens, Ph.D. Thesis, University of Texas
 * at Dallas, 1988.
 *
 * =a HopcountMetric, LinkStat, ETXMetric, E2ELossMetric */

class GridGenericRouteTable;

class LIRMetric : public GridGenericMetric {
  
public:

  LIRMetric();
  ~LIRMetric();

  const char *class_name() const { return "LIRMetric"; }
  const char *processing() const { return AGNOSTIC; }
  LIRMetric *clone()  const { return new LIRMetric; } 

  int configure(Vector<String> &, ErrorHandler *);
  bool can_live_reconfigure() const { return false; }

  void add_handlers();

  void *cast(const char *);

  // generic metric methods
  bool metric_val_lt(const metric_t &, const metric_t &) const;
  metric_t get_link_metric(const EtherAddress &n) const;
  metric_t append_metric(const metric_t &, const metric_t &) const;

private:

  GridGenericRouteTable *_rt;

};

CLICK_ENDDECLS
#endif