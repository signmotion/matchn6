#include "../../../include/stdafx.h"
#include "../../../include/ge/background/Background.h"
#include "../../../include/ge/world/World.h"


namespace matchn {


Background::Background(
    std::shared_ptr< World >  w,
    const nameElement_t&      name
) :
    // фон рисуем самым первым
    Incarnate( w, name, typelib::coord2Int_t::ZERO(), -1 )
{
}




Background::~Background() {
}




std::shared_ptr< Background >
Background::valueOf(
    std::shared_ptr< World >  w,
    const nameElement_t&      name
) {
    return std::shared_ptr< Background >( new Background( w, name ) );
}




void Background::draw( std::shared_ptr< World >  w ) const {
    // # Для фона существует только одно состояние.
    const nameState_t ns = "wait";
    const nameElement_t nc = name();
    drawSprite( w->visual()->get_gc(), needVisualSize(), "background", nc, ns );
}


} // matchn
