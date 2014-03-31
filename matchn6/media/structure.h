#ifndef MATCHN_AS_OPEN_CL_STRUCT

#pragma once

// #! Структуры OpenCL и C++ должны быть одинакового размера.
//    Особое внимание стоит обратить на структуры, которые содержат
//    вложенные структуры и атрибут "упаковать".
#pragma pack( 1 )


#include "../include/configure.h"
#include "../include/CL.h"


/**
* # Для звёздной системы расчёты желательно вести с большей точностью:
*   чистый float даёт ощутимую погрешность.
* # Координаты (~0, ~0, ~0) - центр звёздной системы.
* # Структуры организованы т. о., чтобы не дублировать их при включении
*   в код OpenCL.
*/
#ifndef __constant
#define __constant const
#endif

#ifndef __global
#define __global /*nothing*/
#endif

// Выравнивание для VC++ не требуется
#define __attribute__(x) /*nothing*/

namespace matchn {
    

/**
* Вид игрового элемента.
*
* @example background, container
*/
typedef std::string  kindElement_t;




/**
* Название игрового элемента.
*/
typedef std::string  nameElement_t;




/**
* Название состояния.
*/
typedef std::string  nameState_t;


#endif




/**
* UID (индекс) игрового элемента.
*/
typedef cl_uint  index_t;




/**
* Название контейнера.
*/
typedef cl_char  nameContainer_t;




typedef struct __attribute__ ((packed)) {
    cl_int  x;
    cl_int  y;
} logicCoord_t;


typedef logicCoord_t  visualCoord_t;




/**
* Состояния элементов.
*/
enum STATE_CONTAINER {
    // неопределённое состояние или отсутствует
    SC_NONE = 0,

    // падает
    SC_START_DROP,
    SC_DROP,
    SC_STOP_DROP,

    // обновляет сетку контейнеров
    SC_UPDATE,

    // ожидает
    SC_START_WAIT,
    SC_WAIT,
    SC_STOP_WAIT,

    // последнее
    GE_last
};




#ifndef MATCHN_AS_OPEN_CL_STRUCT






/**
* Соответствие числовых UID строковым для события.
*/
typedef boost::bimap< enum STATE_CONTAINER, std::string >  accordanceStateContainer_t;


static const accordanceStateContainer_t  accordanceStateContainer =
    boost::assign::list_of< accordanceStateContainer_t::relation >
        ( SC_NONE,    "undefined" )
        ( SC_DROP,    "drop" )
        ( SC_UPDATE,  "update" )
        ( SC_WAIT,    "wait" )
    ;


/**
* @return ID состояния по названию.
*/
static inline enum STATE_CONTAINER convertState( const std::string& name ) {
    const auto ftr = accordanceStateContainer.right.find( name );
    ASSERT( (ftr != accordanceStateContainer.right.end())
        && "Состояния с таким названием не существует." );
    return ftr->second;
}


/**
* @return Название состояния по ID.
*/
static inline std::string const&  convertState( enum STATE_CONTAINER state ) {
    const auto ftr = accordanceStateContainer.left.find( state );
    ASSERT( (ftr != accordanceStateContainer.left.end())
        && "Состояния с таким ID не существует." );
    return ftr->second;
}




/**
* Резервирует место для элементов в куче.
* Инициализирует структуру T нулями.
*/
template< class T >
static inline std::unique_ptr< T >  createContent( size_t N ) {
    std::unique_ptr< T >  content( new T[ N ] );
    std::memset( content.get(),  0,  sizeof( T ) * N );
    return std::move( content );
}


} // matchn

#endif
