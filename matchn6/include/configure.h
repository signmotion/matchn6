#pragma once

#include <string>
#include <typelib/include/coord2.h>


namespace matchn {


/**
* Максимальное кол-во элементов по видам.
*
* # Выравниваем кол-во для оптимального быстродействия OpenCL.
*/
const cl_uint ALIGN_SIZE = 64;

const cl_uint CONTAINER_COUNT = 1000;
const cl_uint CONTAINER_OPTIMIZE_COUNT =
    (1 + CONTAINER_COUNT / ALIGN_SIZE) * ALIGN_SIZE;




/**
* Количество уникальных элементов, участвующих в игре.
*
* @todo extend Определять по количеству папок в папке 'media/element'.
*/




/**
* Характеристики движения контейнеров в зависимости от состояний.
* # Время задаётся в мс.
*/
static const int DURATION_DROP_CONTAINER = 1000;





/**
* Размер окна игры, согласно которому позиционируем элементы, пкс.
*/
static const int WINDOW_WIDTH  = 800;
static const int WINDOW_HEIGHT = 800;
static const float WINDOW_K_WIDTH_HEIGHT =
    static_cast< float >( WINDOW_WIDTH ) /
    static_cast< float >( WINDOW_HEIGHT );




/**
* Смещение центра координат визуального мира относительно логического, пкс.
*/
static const int CENTER_WINDOW_WIDTH  = WINDOW_WIDTH  / 2;
static const int CENTER_WINDOW_HEIGHT = WINDOW_HEIGHT / 2;




/**
* Максимальные и минимальные координаты визуального мира, пкс.
*/
static const int MIN_X_VISUAL_COORD = -WINDOW_WIDTH  / 2;
static const int MAX_X_VISUAL_COORD =  WINDOW_WIDTH  / 2;
static const int MIN_Y_VISUAL_COORD = -WINDOW_HEIGHT / 2;
static const int MAX_Y_VISUAL_COORD =  WINDOW_HEIGHT / 2;



/**
* Максимальное видимое расстояние от центра мира (от центра до угла).
*/
static const float MAX_VISUAL_CENTER_DISTANCE =
    typelib::vector2_t( MAX_X_VISUAL_COORD, MAX_Y_VISUAL_COORD ).length();




// # Ниже все координаты и размеры задаются относительно логического мира игры.
// # Направление координатных осей
//     - визуальный мир: X - вправо, Y - вниз
//     - логический мир: X - вправо, Y - вверх




/**
* Координаты центра области элементов, пкс.
* Задаются относит. логического мира игры - см. прим. выше.
*/
static const int ELEMENT_AREA_X = 0;
static const int ELEMENT_AREA_Y = 0;




/**
* Размер спрайта элемента (по умолчанию).
*/
static const typelib::size2Int_t  CONTAINER_SPRITE_SIZE( 270, 270 );



/**
* Пути к данным в файловой системе.
*/
static const std::string PATH_MEDIA = "media";

// # Ядра OpenCL храним в папке PATH_MEDIA.
static const std::string CL_PATH = PATH_MEDIA;




/**
* Для отладки.
*/
//#define _DEBUG_EVENT


#undef ASSERT
#define ASSERT(EXPR)  assert(EXPR);
#define DASSERT(EXPR)  if (!(EXPR)) __debugbreak();


// @see Console
#define CONSOLE  Console::instance()



#undef MATCHN_AS_OPEN_CL_STRUCT


} // matchn
