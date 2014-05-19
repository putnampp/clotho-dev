#!/bin/awk -f
BEGIN{
    prev = -1;
    y = 0;
    delta = 0;
    x = -1;
    ymax = 0;
    drange_min = 99999;
    drange_max = -999999;
}
{
    if( prev != $1 ) {
        if( ymax < y )
            ymax = y;
        y = 0;
        prev = $1;
        x++;
    }

    dro = ($2 - $3);
    heat[x][y++] = dro;
}
END {
    print "#ymax=" ymax;
    for( i = 0; i < x; i++ ) {
        h = heat[i][0];
        for( j = 1; j < ymax; j++ ) {
            if( j in heat[i] ) {
                if( heat[i][j] < 0 ) {
                    h = h "," (-1 * log( -1*heat[i][j]));
                } else {
                    h = h "," log(heat[i][j]);
                }
            } else {
                h = h ",0";
            }
        }
        print h;
    }
}
