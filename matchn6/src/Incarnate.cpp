#include "../include/stdafx.h"
#include "../include/Incarnate.h"
#include "../include/ge/world/World.h"


namespace matchn {


Incarnate::Incarnate(
    std::shared_ptr< World >     w,
    const std::string&           name,
    const typelib::coord2Int_t&  logicCoord,
    int  order
) :
    GE( w ),
    mName( name ),
    LIncarnate( logicCoord ),
    // для World w не передаётся
    VIncarnate(
        w
          ? w->fsm()->visualByLogicCoordContainer( logicCoord )
          : typelib::coord2Int_t::ZERO(),
        order
    )
{
    ASSERT( !mName.empty()
        && "Название элемента должно быть указано." );
}




Incarnate::~Incarnate() {
}


} // matchn








namespace std {

std::ostream& operator<<( std::ostream& out,  const matchn::Incarnate& ie ) {
    out << ie.uid() << " " << ie.logicCoord();
    return out;
}

} // std
