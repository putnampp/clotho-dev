#ifndef SEX_H_
#define SEX_H_

#include <ostream>

enum Sex { MALE, FEMALE, UNKNOWN, UNASSIGNED };

std::ostream & operator<<( std::ostream &, const Sex s );

#endif  // SEX_H_
