inline index_t
ic( const int x, const int y ) {
    return ( index_t )( x + y * N );
}




inline logicCoord_t
ci( const index_t i ) {
    const size_t y = i / N;
    const size_t x = i - y * N;
    const logicCoord_t  lc = { x, y };
    return lc;
}




/**
* @return Сколько в диапазоне  [ 0.0 (началось); 1.0 (завершено) )
*         осталось до наступления времени 'stop'.
*/
inline float
nepperDeltatime( int duration, int current, int stop ) {
    const float u =
        1.0f - ( float )( stop - current ) / ( float )( duration );
    // @todo optimize Native.
    return (u > 1.0f) ? 1.0f : (u < 0.0f) ? 0.0f : u;
}
