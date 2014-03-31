#ifndef MATCHN_AS_OPEN_CL_STRUCT

#pragma once

// #! ��������� OpenCL � C++ ������ ���� ����������� �������.
//    ������ �������� ����� �������� �� ���������, ������� ��������
//    ��������� ��������� � ������� "���������".
#pragma pack( 1 )


#include "../include/configure.h"
#include "../include/CL.h"


/**
* # ��� ������� ������� ������� ���������� ����� � ������� ���������:
*   ������ float ��� �������� �����������.
* # ���������� (~0, ~0, ~0) - ����� ������� �������.
* # ��������� ������������ �. �., ����� �� ����������� �� ��� ���������
*   � ��� OpenCL.
*/
#ifndef __constant
#define __constant const
#endif

#ifndef __global
#define __global /*nothing*/
#endif

// ������������ ��� VC++ �� ���������
#define __attribute__(x) /*nothing*/

namespace matchn {
    

/**
* ��� �������� ��������.
*
* @example background, container
*/
typedef std::string  kindElement_t;




/**
* �������� �������� ��������.
*/
typedef std::string  nameElement_t;




/**
* �������� ���������.
*/
typedef std::string  nameState_t;


#endif




/**
* UID (������) �������� ��������.
*/
typedef cl_uint  index_t;




/**
* �������� ����������.
*/
typedef cl_char  nameContainer_t;




typedef struct __attribute__ ((packed)) {
    cl_int  x;
    cl_int  y;
} logicCoord_t;


typedef logicCoord_t  visualCoord_t;




/**
* ��������� ���������.
*/
enum STATE_CONTAINER {
    // ������������� ��������� ��� �����������
    SC_NONE = 0,

    // ������
    SC_START_DROP,
    SC_DROP,
    SC_STOP_DROP,

    // ��������� ����� �����������
    SC_UPDATE,

    // �������
    SC_START_WAIT,
    SC_WAIT,
    SC_STOP_WAIT,

    // ���������
    GE_last
};




#ifndef MATCHN_AS_OPEN_CL_STRUCT






/**
* ������������ �������� UID ��������� ��� �������.
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
* @return ID ��������� �� ��������.
*/
static inline enum STATE_CONTAINER convertState( const std::string& name ) {
    const auto ftr = accordanceStateContainer.right.find( name );
    ASSERT( (ftr != accordanceStateContainer.right.end())
        && "��������� � ����� ��������� �� ����������." );
    return ftr->second;
}


/**
* @return �������� ��������� �� ID.
*/
static inline std::string const&  convertState( enum STATE_CONTAINER state ) {
    const auto ftr = accordanceStateContainer.left.find( state );
    ASSERT( (ftr != accordanceStateContainer.left.end())
        && "��������� � ����� ID �� ����������." );
    return ftr->second;
}




/**
* ����������� ����� ��� ��������� � ����.
* �������������� ��������� T ������.
*/
template< class T >
static inline std::unique_ptr< T >  createContent( size_t N ) {
    std::unique_ptr< T >  content( new T[ N ] );
    std::memset( content.get(),  0,  sizeof( T ) * N );
    return std::move( content );
}


} // matchn

#endif
