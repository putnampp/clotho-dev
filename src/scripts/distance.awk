#!/bin/awk -f 
{ 
    line=$0; 
    if($0 ~ /Time/) { 
        line=line ",D(R:H),D(R:N_R),D(H:N_H)" 
    } else if( $0 ~ /^[0-9]+/) {    # select only those lines which start with a number
        if( $3 != -1) { 
            line = line "," ($3-$2);
        } else {
            line = line ",0";
        }

        if( $4 != -1 ) {
            line = line "," ($4 - $2);
        } else {
            line = line ",0";
        }

        if( $5 != -1 ) {
            line = line "," ($3 - $5);
        } else {
            line = line ",0";
        }
    }
    print line;
}
