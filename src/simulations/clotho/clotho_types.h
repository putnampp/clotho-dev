#ifndef CLOTHO_TYPES_H_
#define CLOTHO_TYPES_H_

#include <ostream>
#include <list>
#include <vector>
#include <set>
#include "engine/event_interface.h"
#include "engine/event_functor.h"
#include "common_strings.h"
#include "sex.h"
//#include "AlleleTuple.h"

//#include "engine/FSBAllocator.hh"

//#include "pooled_event_set.h"

//typedef std::list< const event *, FSBAllocator< const event * > > ClothoEventSet;
//typedef std::vector< const event * > ClothoEventSet;
typedef std::multiset< const event *, ltsf_event_order/*, FSBAllocator< const event * >*/ > ClothoEventSet;
//
//typedef pooled_event_set ClothoEventSet;


#endif  // CLOTHO_TYPES_H_
