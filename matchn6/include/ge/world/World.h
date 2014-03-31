#pragma once

#include "../../configure.h"
#include "../../../media/structure.h"
#include "../../CL.h"


namespace matchn {


class Container;
class ManagerSprite;


/**
* Класс для работы с игровым миром.
* Создаёт и хранит все миры игры: физический, визуальный, аудио и пр..
*/
class World :
    public std::enable_shared_from_this< World >
{
public:
    /**
    * Кол-во собираемых вместе элементов и размер мира.
    *
    * # Делаем открытыми константами, не плодим gett'еры.
    *
    * @see size()
    */
    const size_t  K;
    const size_t  N;
    const size_t  M;

    


protected:
    /**
    * @exception CL_Exception
    */
    World( size_t K, size_t N, size_t M );




public:
    // Для инициализации this->shared_from_this()
    static std::shared_ptr< World > valueOf(
        size_t K,
        size_t N,
        size_t M
    );




    virtual ~World();




    /**
    * @return Размер мира.
    */
    inline typelib::size2Int_t size() const {
        return typelib::size2Int_t( N, M );
    }




    /**
    * @return Максимальное количество контейнеров в мире.
    */
    inline size_t volume() const {
        return N * M;
    }




    void pulse();




    /**
    * @return Миры мира.
    */
    inline const CL_DisplayWindow* visual() const {
        return mVisualWorld.get();
    }

    inline CL_DisplayWindow* visual() {
        return mVisualWorld.get();
    }




    /**
    * Оживляет мир.
    */
    void go();




    void draw();




    /**
    * @return Текущее время в мире, мс.
    */
    static inline int currentTime() {
        return mCurrentTime;
    }




    typelib::size2Int_t needVisualSizeContainer() const;




    visualCoord_t visualByLogicCoordContainer( const logicCoord_t& ) const;




private:
    /**
    * На месте пустой ячейки появляется новый контейнер.
    */
    void incarnateOnEmptyCell();




    /**
    * Слушатели событий визуального мира.
    */
    void onMouseDown( const CL_InputEvent&, const CL_InputState& );
    void onMouseMove( const CL_InputEvent&, const CL_InputState& );
    void onKeyboardDown( const CL_InputEvent&, const CL_InputState& );
    void onWindowClose();




    /**
    * @return Кадры в секунду (Frame Per Second).
    */
    static size_t World::calcFPS( size_t frameTime );




private:
    /**
    * Мир хранит контейнеры.
    */
    std::shared_ptr< Container >  mContainer;


    /**
    * Отображаемый мир.
    */
    std::unique_ptr< CL_DisplayWindow >  mVisualWorld;


    /**
    * События от игрока.
    */
    CL_Slot  mSlotMouseDown;
    CL_Slot  mSlotMouseMove;
    CL_Slot  mSlotKeyboardDown;
    CL_Slot  mSlotWindowClose;


    static std::unique_ptr< ManagerSprite >  mManagerSprite;


    /**
    * Текущее время, мс.
    * Может использоваться для корректного тайминга.
    */
    static int mCurrentTime;


    /**
    * Для safe-потоков.
    */
    mutable boost::mutex  mtxMouseAction;
};


} // matchn
