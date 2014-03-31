#pragma once

#include "../../configure.h"
#include "../../../media/structure.h"
#include "../../CL.h"


namespace matchn {


class Container;
class ManagerSprite;


/**
* ����� ��� ������ � ������� �����.
* ������ � ������ ��� ���� ����: ����������, ����������, ����� � ��..
*/
class World :
    public std::enable_shared_from_this< World >
{
public:
    /**
    * ���-�� ���������� ������ ��������� � ������ ����.
    *
    * # ������ ��������� �����������, �� ������ gett'���.
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
    // ��� ������������� this->shared_from_this()
    static std::shared_ptr< World > valueOf(
        size_t K,
        size_t N,
        size_t M
    );




    virtual ~World();




    /**
    * @return ������ ����.
    */
    inline typelib::size2Int_t size() const {
        return typelib::size2Int_t( N, M );
    }




    /**
    * @return ������������ ���������� ����������� � ����.
    */
    inline size_t volume() const {
        return N * M;
    }




    void pulse();




    /**
    * @return ���� ����.
    */
    inline const CL_DisplayWindow* visual() const {
        return mVisualWorld.get();
    }

    inline CL_DisplayWindow* visual() {
        return mVisualWorld.get();
    }




    /**
    * �������� ���.
    */
    void go();




    void draw();




    /**
    * @return ������� ����� � ����, ��.
    */
    static inline int currentTime() {
        return mCurrentTime;
    }




    typelib::size2Int_t needVisualSizeContainer() const;




    visualCoord_t visualByLogicCoordContainer( const logicCoord_t& ) const;




private:
    /**
    * �� ����� ������ ������ ���������� ����� ���������.
    */
    void incarnateOnEmptyCell();




    /**
    * ��������� ������� ����������� ����.
    */
    void onMouseDown( const CL_InputEvent&, const CL_InputState& );
    void onMouseMove( const CL_InputEvent&, const CL_InputState& );
    void onKeyboardDown( const CL_InputEvent&, const CL_InputState& );
    void onWindowClose();




    /**
    * @return ����� � ������� (Frame Per Second).
    */
    static size_t World::calcFPS( size_t frameTime );




private:
    /**
    * ��� ������ ����������.
    */
    std::shared_ptr< Container >  mContainer;


    /**
    * ������������ ���.
    */
    std::unique_ptr< CL_DisplayWindow >  mVisualWorld;


    /**
    * ������� �� ������.
    */
    CL_Slot  mSlotMouseDown;
    CL_Slot  mSlotMouseMove;
    CL_Slot  mSlotKeyboardDown;
    CL_Slot  mSlotWindowClose;


    static std::unique_ptr< ManagerSprite >  mManagerSprite;


    /**
    * ������� �����, ��.
    * ����� �������������� ��� ����������� ��������.
    */
    static int mCurrentTime;


    /**
    * ��� safe-�������.
    */
    mutable boost::mutex  mtxMouseAction;
};


} // matchn
