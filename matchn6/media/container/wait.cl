__kernel void wait(
    __global enum STATE_CONTAINER*  stateSet,
    __global const logicCoord_t*    logicCoordSet,
    __global const bool*            fillCellSet
) {
    // проверим, что контейнер не собирается падать
    const uint i = get_global_id( 0 );
    if ( (i >= N * M) || (stateSet[ i ] != SC_WAIT) ) {
        return;
    }
    
    if (logicCoordSet[ i ].y == (M - 2)) {
        // # Нижний контейнер не падает.
        return;
    }

    logicCoord_t  current = { 0, 0 };
    for ( ; current.y < M; ++current.y) {
        const index_t j = ic( current.x, current.y );
        if ( !fillCellSet[ j ] ) {
            // контейнер должен упасть
            printf( "-> Start drop container %d [ %d, %d ]\n",
                i,  logicCoordSet[ i ].x,  logicCoordSet[ i ].y );
            stateSet[ i ] = SC_START_DROP;
            break;
        }
    }

    // контейнер неподвижен
}
