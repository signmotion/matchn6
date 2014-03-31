#include "../../../include/stdafx.h"
#include "../../../include/ge/world/World.h"
#include "../../../include/ge/container/Container.h"
#include "../../../include/ManagerSprite.h"


namespace matchn {


std::unique_ptr< ManagerSprite >  World::mManagerSprite(
    new ManagerSprite()
);

int World::mCurrentTime = 0;




World::World(
    size_t K, size_t N, size_t M
) :
    K( K ), N( N ), M( M ),
    mContainer( new Container() )
{
    ASSERT( (K >= 2 )
        && "Количество собираемых вместе элементов должно быть 2 или больше." );
    ASSERT( ( (N >= 3) && (M >= 3) )
        && "Размер поля не может быть меньше чем 3 x 3." );
    ASSERT( ( ((N % 2) != 0) && ((M % 2) != 0) )
        && "Размеры поля должны быть нечётными." );

    // *графический мир*
    // @source Example "SpritesRTS" from http://clanlib.org
    // @source Example "TileMap" from http://clanlib.org
    {
        // инициализируем ClanLib
        CL_SetupCore setup_core;
        CL_SetupDisplay setup_display;
        #ifdef USE_SOFTWARE_RENDERER
            CL_SetupSWRender setup_swrender;
        #endif
        #ifdef USE_OPENGL_1
            CL_SetupGL1 setup_gl1;
        #endif
        #ifdef USE_OPENGL_2
            CL_SetupGL setup_gl;
        #endif
        //CL_SetupSound setup_sound;

        std::stringstream  ss;
        ss << "Match" << K << "    " << N << " x " << M;
        CL_DisplayWindowDescription  desc( ss.str() );
        desc.set_size( CL_Size( WINDOW_WIDTH, WINDOW_HEIGHT ),  false );
        desc.set_allow_resize( false );
        mVisualWorld =
            std::unique_ptr< CL_DisplayWindow >( new CL_DisplayWindow( desc ) );
    }


    // *мир звуков*
    // @todo mAudiuWorld = ...


    // подключаем слушателей событий от игрока
    {
        mSlotMouseDown =
            mVisualWorld->get_ic().get_mouse().sig_key_down()
            .connect( this, &World::onMouseDown );
        mSlotMouseMove =
            mVisualWorld->get_ic().get_mouse().sig_pointer_move()
            .connect( this, &World::onMouseMove );
        mSlotKeyboardDown =
            mVisualWorld->get_ic().get_keyboard().sig_key_down()
            .connect( this, &World::onKeyboardDown );
        mSlotWindowClose =
            mVisualWorld->sig_window_close()
            .connect( this, &World::onWindowClose );
    }


    //mVisualWorld->hide_cursor();
}




std::shared_ptr< World >
World::valueOf( size_t K, size_t N, size_t M ) {
    auto r = std::shared_ptr< World >( new World( K, N, M ) );
    r->mContainer->mWorld = r;
    r->mContainer->prepareKernel( r );
    return r;
}




World::~World() {
}




void
World::pulse() {
    mContainer->pulse();
    incarnateOnEmptyCell();
}




void
World::go() {

    // показываем
    auto gc = mVisualWorld->get_gc();
    auto ic = mVisualWorld->get_ic();
    CL_Font  font( gc, "Tahoma", 16 );
    CL_BlendMode  blendTransparent;
    int lastTime = CL_System::get_time();
    for (auto keyboard = ic.get_keyboard();
         !keyboard.get_keycode( CL_KEY_ESCAPE );
    ) {
        // @source http://clanlib.org/wiki/MainDocs:Timing
        mCurrentTime = CL_System::get_time();
        int deltaTime = mCurrentTime - lastTime;
        // ждём: окно приложения могут перетаскивать
        if (deltaTime > 1000) { deltaTime = 1000; }
        // 'deltaTimeMs' можно использовать для корректного тайминга
        //const float deltaTimeMs = static_cast< float >( deltaTime );
        lastTime = mCurrentTime;


#ifdef _DEBUG
        // для FPS
        // @source http://clanlib.org/docs/clanlib-0.8.0/Overview/tipsandtricks.html
        const size_t fps = World::calcFPS( deltaTime );
#endif


        gc.clear( CL_Colorf::aquamarine );

        //gc.set_map_mode( cl_map_2d_upper_left );
        //gc.set_blend_mode( blendTransparent );


        pulse();
        draw();


#ifdef _DEBUG
        // покажем информацию для отладки
        {
            std::ostringstream ss;
            ss << fps;
            //const auto s = CL_StringHelp::int_to_text( fps );
            font.draw_text( gc, 10, 20, ss.str(), CL_Colorf::lightcoral );
        }
#endif

        mVisualWorld->flip( 1 );
        CL_KeepAlive::process();

        mCurrentTime = CL_System::get_time();
        // 10 ms (100 hz)
        static const int mainLoopRate = 10;
        const int timeToSleep = mainLoopRate - (mCurrentTime - lastTime);
        if (timeToSleep > 0) {
            CL_System::sleep( timeToSleep );
        }

    } // for (auto keyboard = ic.get_keyboard(); ...

}




void
World::draw() {

    auto& gc = visual()->get_gc();
    for (index_t i = 0; i < N * M; ++i) {
        const nameContainer_t nc = mContainer->mNameSet.get()[ i ];
        if (nc == 0) {
            // # Первый встретившийся пустой контейнер - дальше пусто.
            break;
        }

        const auto& vs = needVisualSizeContainer();
        const kindElement_t kind = "container";
        const nameElement_t name = std::string( 1, nc );
        const enum STATE_CONTAINER sc = mContainer->mStateSet.get()[ i ];
        const nameState_t state = convertState( sc );
        auto s = mManagerSprite->sprite( gc,  vs,  kind,  name,  state );
        DASSERT( s
            && "Спрайт для заданного события не найден." );

        const float rotation = mContainer->mVisualRotationSet.get()[ i ];
        s->set_angle( CL_Angle( rotation, cl_degrees ) );
        const float alpha = mContainer->mVisualAlphaSet.get()[ i ];
        s->set_alpha( alpha );
        const auto& coord = mContainer->mVisualCoordSet.get()[ i ];
        const auto& shift = mContainer->mVisualShiftSet.get()[ i ];
        const visualCoord_t vc = { coord.x + shift.x,  coord.y + shift.y };
        s->draw( gc, vc.x, vc.y );
        s->update();

#ifdef _DEBUG
        {
            // покажем состояние элемента
            const size_t fs = vs.x / 8;
            CL_Font  font( gc, "Tahoma", fs );
            const auto ts = font.get_text_size( gc, state );
            const auto center = ts / 2;
            font.draw_text_ellipsis(
                gc,
                vc.x - center.width,
                vc.y - center.height,
                CL_Rect(
                    vc.x - vs.x / 2,  vc.y - vs.y / 2,
                    vc.x + vs.x / 2,  vc.y + vs.y / 2
                ),
                state,
                CL_Colorf::lightcoral
            );
        }
#endif

    } // for (index_t i = 0; ...

}




typelib::size2Int_t
World::needVisualSizeContainer() const {
    // контейнер должен помещаться в сетке мира N x M
    const auto vs = 
        typelib::size2_t( WINDOW_WIDTH, WINDOW_HEIGHT ) /
        static_cast< typelib::size2_t >( CONTAINER_SPRITE_SIZE ) /
        typelib::size2_t( N, M ) * 0.99f;

    return static_cast< typelib::size2_t >( CONTAINER_SPRITE_SIZE ) * vs;
}




visualCoord_t
World::visualByLogicCoordContainer( const logicCoord_t&  lc ) const {

    const auto vss = needVisualSizeContainer();
    const auto vnlc =
        typelib::coord2_t( lc.x, lc.y ) *
        static_cast< typelib::coord2_t >( vss ) +
        static_cast< typelib::coord2_t >( vss / 2 );
    const visualCoord_t  vc = {
        static_cast< int >( vnlc.x ),
        static_cast< int >( vnlc.y )
    };

    return vc;
}




void
World::incarnateOnEmptyCell() {

    logicCoord_t lc = { 0, 0 };
    for ( ;  lc.x < static_cast< int >( N );  ++lc.x ) {
        const bool present = mContainer->present( lc, size() );
        if ( !present ) {
            mContainer->nextIncarnate( lc );
        }
    }
}




void
World::onMouseDown( const CL_InputEvent& ie, const CL_InputState& is ) {

    boost::mutex::scoped_lock  lock( mtxMouseAction );

    const auto attentionPoint =
        typelib::coord2Int_t( ie.mouse_pos.x, ie.mouse_pos.y );
    // @todo ...
}




void
World::onMouseMove( const CL_InputEvent& ie, const CL_InputState& is ) {

    boost::mutex::scoped_lock  lock( mtxMouseAction );

    const auto attentionPoint =
        typelib::coord2Int_t( ie.mouse_pos.x, ie.mouse_pos.y );
    // @todo ...
}




void
World::onKeyboardDown( const CL_InputEvent& ie, const CL_InputState& is ) {
}




void
World::onWindowClose() {
}




size_t
World::calcFPS( size_t frameTime ) {

    static size_t fpsResult  = 0;
    static size_t fpsCounter = 0;
    static size_t totalTime  = 0;
    
    totalTime += frameTime;
    if (totalTime >= 1000) {
        fpsResult = fpsCounter + 1;
        fpsCounter = totalTime = 0;
    }
    ++fpsCounter;

    return fpsResult;
}


} // matchn
