__kernel void startDrop(
    __global enum STATE_CONTAINER*  stateSet,
    __global int*                   durationTimeSet,
    __global int*                   stopTimeSet,
    const int                       currentTime,
    const int                       duration
) {
    // запомним время начала падения контейнера
    const uint i = get_global_id( 0 );
    if ( (i >= N * M) || (stateSet[ i ] != SC_START_DROP) ) {
        return;
    }

    durationTimeSet[ i ] = duration;
    stopTimeSet[ i ] = currentTime + duration;
    printf( "-> Drop container %d\n",  i );
    stateSet[ i ] = SC_DROP;
}
