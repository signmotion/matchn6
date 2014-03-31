#pragma once

#include "GE.h"


namespace matchn {


class World;
struct WorldFSM;
class ManagerSprite;


/**
* Объект, зарегистрированный в визуальном движке (в мире).
* # В качестве виз. движка используется ClanLib.
*
* @see http://clanlib.org
*/
class VIncarnate {
protected:
    VIncarnate(
        const typelib::coord2Int_t&  coord,
        int  order
    );



    virtual ~VIncarnate();




public:
    /**
    * Визуализирует элемент.
    */
    virtual void draw( std::shared_ptr< World > ) const = 0;




    /**
    * Отображает спрайт для элемента с учётом состояния.
    */
    void drawSprite(
        CL_GraphicContext&,
        const typelib::size2Int_t&  needVisualSize,
        const kindElement_t&,
        const nameElement_t&,
        const nameState_t&
    ) const;




    /**
    * @return Размер образа в пкс, который нужно показать на экране.
    *         Спрайт будет подогнан под этот размер.
    */
    virtual typelib::size2Int_t needVisualSize() const = 0;




    /**
    * @return Координаты образа.
    *
    * # Названа с приставкой, чтобы проще получать доступ из элементов игры.
    */
    template< typename T1, typename T2 >
    inline void visualCoord( T1 x, T1 y ) {
        visualCoord( coord2Int_t( x, y ) );
    };


    inline const typelib::coord2Int_t& visualCoord() const {
        return mCoord;
    };


    inline void visualCoord( const typelib::coord2Int_t& c ) {
        mCoord = c;
    };




    /**
    * @return Угол поворота элемента в градусах.
    */
    template< typename T >
    inline void visualRotation( T a ) {
        mRotation = static_cast< int >( a );
    };


    template< typename T >
    inline T visualRotation() const {
        return static_cast< T >( mRotation );
    };


    inline int visualRotation() const {
        return mRotation;
    };




    /**
    * @return Прозрачность элемента [0.0; 1.0].
    */
    template< typename T >
    inline void visualAlpha( T a ) {
        mAlpha = static_cast< float >( a );
    };


    inline float visualAlpha() const {
        return mAlpha;
    };




    /**
    * @return Порядок отрисовки элемента. Меньше - раньше.
    */
    inline int visualOrder() const {
        return mOrder;
    }




private:
    typelib::coord2Int_t  mCoord;
    int                   mRotation;
    float                 mAlpha;

    /**
    * Порядок отрисовки элемента. Меньше - раньше.
    */
    int  mOrder;

    static std::unique_ptr< ManagerSprite >  mManagerSprite;
};



} // matchn
