#include "sex.h"

#include "common_strings.h"

std::ostream & operator<<( std::ostream & out, const Sex s ) {
    switch( s ) {
    case MALE:
        out << MALE_K;
        break;
    case FEMALE:
        out << FEMALE_K;
        break;
    case UNASSIGNED:
        out << UNASSIGNED_K;
        break;
    default:
        out << UNKNOWN_K;
        break;
    }
    return out;
}
