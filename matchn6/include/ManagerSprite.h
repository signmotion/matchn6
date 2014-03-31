#pragma once

#include "configure.h"
#include "../media/structure.h"


namespace matchn {


/**
* Управляет спрайтами игры.
*/
class ManagerSprite {
public:
    /**
    * Известные менеджеру спрайты.
    * Спрайты сгруппированы по состояниям.
    * Каждое состояние может иметь один спрайт (образ для визуализации).
    */
    typedef std::string  pathSprite_t;
    typedef std::unordered_map<
        pathSprite_t,
        std::unique_ptr< CL_Sprite >
    > knownSprite_t;


    /**
    * Известные менеджеру фреймы спрайтов.
    */
    typedef std::string  pathFrame_t;
    typedef boost::unordered_map<
        pathFrame_t,
        std::unique_ptr< CL_PixelBuffer >
    >  knownFrame_t;




public:
    ManagerSprite();




    virtual ~ManagerSprite();




    /**
    * @return Спрайт для элемента с учётом тек. состояния.
    *         nullptr, если спрайт загрузить на удалось.
    *
    * # Фреймы спрайтов сгруппированы по папкам состояний.
    * # Фреймы спрайтов должны быть одного размера.
    *
    * @see pathSprite()
    */
    CL_Sprite* sprite(
        CL_GraphicContext&,
        const typelib::size2Int_t&  needVisualSize,
        const kindElement_t&,
        const nameElement_t&,
        const nameState_t&
    );




    /**
    * @return Загруженный по указанному пути фрейм в формате ClanLib.
    *         nullptr, если фрейм загрузить на удалось.
    */
    CL_PixelBuffer* frame( const std::string& path );




    /**
    * @return Путь к спрайту элемента с учётом состояния.
    */
    static inline pathSprite_t pathSprite(
        const kindElement_t&  kind,
        const nameElement_t&  name,
        const nameState_t&    ns
    ) {
        const std::string path = PATH_MEDIA + "/" + kind + "/" + name + "/" + ns;
        return path;
    }




private:
    /**
    * Собирает спрайт из фреймов по указанному пути.
    */
    CL_Sprite* loadSprite(
        CL_GraphicContext&,
        const typelib::size2Int_t&  needVisualSize,
        const pathSprite_t&
    );




private:
    knownSprite_t  mKnownSprite;
    knownFrame_t   mKnownFrame;
};


} // matchn
