__kernel void drop(
    __global enum STATE_CONTAINER*  stateSet,
    __global visualCoord_t*         visualShiftSet,
    __global const int*             durationTimeSet,
    __global const int*             stopTimeSet,
    const int                       currentTime
) {
    // контейнер будет менять свою виз. коорд. в зависимости от тек. времени
    const uint i = get_global_id( 0 );
    if ( (i >= N * M) || (stateSet[ i ] != SC_DROP) ) {
        return;
    }

    const float nepper = nepperDeltatime(
        durationTimeSet[ i ],  currentTime,  stopTimeSet[ i ]
    );
    //printf( "... Drop container %d %e\n",  i, nepper );

    const float shift = nepper * HEIGHT_CELL;
    visualShiftSet[ i ].y = shift;

    if (nepper >= 1.0f) {
        printf( "-> Stop drop container %d\n",  i );
        stateSet[ i ] = SC_STOP_DROP;
    }
}
