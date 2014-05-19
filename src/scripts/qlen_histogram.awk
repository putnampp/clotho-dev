#!/bin/awk -f
BEGIN {
    prevk = "";
    count = 0;
    max_node_idx = -1;
}
{
    if( $0 ~ /^[0-9]+/ ) {
    #    print $0;
        key = $1 "-" $2;    # time - root node index

        if( prevk ~ key ) {
            count++;
        } else {
            if( count in bins ) {
                bins[count]++;
            } else if( count != 0 ) {
                bins[count] = 1;
                queues[prevk] = count;
            }
            count = 1;
            prevk = key;
        }

        if( $5 == -1 ) { # last node of a list, restart counting
            if( count in bins ) {
                bins[count]++;
            } else if( count != 0 ) {
                bins[count] = 1;
                queues[prevk] = count;
            }
            count = 0;
            prevk = key;
        }

        if( $2 > max_node_idx ) max_node_idx = $2;
        if( $3 > max_node_idx ) max_node_idx = $3;
        if( $4 > max_node_idx ) max_node_idx = $4;
        if( $5 > max_node_idx ) max_node_idx = $5;
    }
}
END {
    print "Queue Length,Frequency";
    for( b in bins ) {
        print b "," bins[b];
    }

    print "\nTime-Root Node,Queue Length";
    for( k in queues ) {
        print k "," queues[k];
    }

    print "\nTotal Nodes allocated," (max_node_idx + 1);
}
