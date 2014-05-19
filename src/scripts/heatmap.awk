#!/bin/awk -f
BEGIN{
    prev = -1;
    y = 0;
    delta = 0;
    x = -1;
    ymax = 0;
}
{
    if( prev != $1 ) {
        if( ymax < y )
            ymax = y;
        y = 0;
        prev = $1;
        x++;
    }

    heat[x][y++] = ($2 - $3);
}
END {
    for( i = 0; i < x; i++ ) {
        h = heat[i][0];
        for( j = 1; j < ymax; j++ ) {
            if( j in heat[i] ) {
                h = h "," heat[i][j];
            } else {
                h = h ",0";
            }
        }
        print h;
    }
}
