#include "sex.h"

#include "common_strings.h"

std::ostream & operator<<( std::ostream & out, const Sex s ) {
    switch( s ) {
    case MALE:
        out << MALE_K;
    case FEMALE:
        out << FEMALE_K;
    case UNASSIGNED:
        out << UNASSIGNED_K;
    default:
        out << UNKNOWN_K;
    }
    return out;
}
