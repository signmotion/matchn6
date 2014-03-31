__kernel void stopDrop(
    __global enum STATE_CONTAINER*  stateSet,
    __global logicCoord_t*          logicCoordSet,
    __global visualCoord_t*         visualCoordSet,
    __global visualCoord_t*         visualShiftSet
) {
    // изменим логические координаты контейнера
    const uint i = get_global_id( 0 );
    if ( (i >= N * M) || (stateSet[ i ] != SC_STOP_DROP) ) {
        return;
    }

    if (logicCoordSet[ i ].y < (M - 2)) {
        printf( ". Change logic coord container %d [ %d, %d ]\n",
            i,  logicCoordSet[ i ].x,  logicCoordSet[ i ].y );
        ++logicCoordSet[ i ].y;
        visualCoordSet[ i ].y += visualShiftSet[ i ].y;
    }
    printf( "-> Wait container %d [ %d, %d ]\n",
        i,  logicCoordSet[ i ].x,  logicCoordSet[ i ].y );
    stateSet[ i ] = SC_WAIT;
}
