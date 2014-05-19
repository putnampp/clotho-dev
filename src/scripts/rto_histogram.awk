#!/bin/awk -f
{
    if( $0 ~ /^[0-9]+/ ) {
        d_r_o = $2 - $3;

        if( d_r_o in rto_distance ) {  #root to object (rto)
            rto_distance[d_r_o]++;
        } else {
            rto_distance[d_r_o]=1;
        }
    }
}
END {
    print "#RTO distance,Frequency";
    for( r in rto_distance ) {
        print r "," rto_distance[r];
    }
}
