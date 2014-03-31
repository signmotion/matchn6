#include "../include/stdafx.h"
#include "../include/VIncarnate.h"
#include "../include/ge/world/World.h"
#include "../include/ManagerSprite.h"


namespace matchn {


std::unique_ptr< ManagerSprite >  VIncarnate::mManagerSprite(
    new ManagerSprite()
);




VIncarnate::VIncarnate(
    const typelib::coord2Int_t&  vc,
    int  order
) :
    mCoord( vc ),
    mRotation( 0 ),
    mAlpha( 1.0f ),
    mOrder( order )
{
}




VIncarnate::~VIncarnate() {
}




void
VIncarnate::drawSprite(
    CL_GraphicContext&          gc,
    const typelib::size2Int_t&  needVisualSize,
    const kindElement_t&        kind,
    const nameElement_t&        name,
    const nameState_t&          ns
) const {
    auto s = mManagerSprite->sprite( gc, needVisualSize, kind, name, ns );
    DASSERT( s
        && "Спрайт для заданного события не найден." );

    s->set_angle( CL_Angle( visualRotation< float >(), cl_degrees ) );
    s->set_alpha( visualAlpha() );
    const auto& vc = visualCoord();
    s->draw( gc, vc.x, vc.y );
    s->update();

#ifdef _DEBUG
    {
        // покажем состояние элемента
        const size_t fs = needVisualSize.x / 8;
        CL_Font  font( gc, "Tahoma", fs );
        const auto ts = font.get_text_size( gc, ns );
        const auto center = ts / 2;
        font.draw_text_ellipsis(
            gc,
            vc.x - center.width,
            vc.y - center.height,
            CL_Rect(
                vc.x - needVisualSize.x / 2,  vc.y - needVisualSize.y / 2,
                vc.x + needVisualSize.x / 2,  vc.y + needVisualSize.y / 2
            ),
            ns,
            CL_Colorf::lightcoral
        );

    }
#endif
}


} // matchn
