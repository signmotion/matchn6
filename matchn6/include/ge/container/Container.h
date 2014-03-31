#pragma once

#include "../../configure.h"
#include "../../../media/structure.h"
#include "../../CL.h"


namespace matchn {


class World;


struct OpenCLContainer {
    int  errorCL;
    cl::Buffer  bclFillCellSet;
    cl::Buffer  bclNameSet;
    cl::Buffer  bclLogicCoordSet;
    cl::Buffer  bclVisualCoordSet;
    cl::Buffer  bclVisualShiftSet;
    cl::Buffer  bclVisualRotationSet;
    cl::Buffer  bclVisualAlphaSet;
    cl::Buffer  bclStateSet;
    cl::Buffer  bclDurationTimeSet;
    cl::Buffer  bclStopTimeSet;

    OpenCLContainer();

    /**
    * Для работы с OpenCL.
    */
    std::unordered_map< std::string, cl::Kernel >  mKernelCL;
    cl::Context  mContextCL;
    std::vector< cl::Device >  mDeviceCL;
    cl::CommandQueue  mQueueCL;


    void prepareCL();


    void prepareKernel( const std::shared_ptr< World > );


    void compileCLKernel(
        const std::shared_ptr< World >,
        const std::vector< std::string >&  kernelKeys,
        const std::vector< std::string >&  includeHCL = std::vector< std::string >(),
        const std::string& options = ""
    );


    /**
    * Выполняет ядро OpenCL по имени 'key'.
    *
    * @param waitEvents  Ядро подождёт завершения этих событий.
    * @param event  Если указан, инициируется событием (для синхронизации
    *        потоков).
    */
    typedef VECTOR_CLASS< cl::Event >  vectorEventCL_t;

    template< size_t globalSize >
    void enqueueEventKernelCL(
        const std::string&  key,
        cl::Event*          event = nullptr
    );

    template< size_t globalSize >
    void enqueueEventKernelCL(
        const std::string&      key,
        const vectorEventCL_t&  waitEvents,
        cl::Event*              event = nullptr
    );


    std::string commonConstantCLKernel( std::shared_ptr< World > );

    static std::string commonOptionCLKernel();
};








/**
* Контейнер (что будем менять местами).
*/
class Container :
    private OpenCLContainer
{
protected:
    Container();




public:
    static std::shared_ptr< Container >  valueOf();




    virtual ~Container();




    std::shared_ptr< World >  world();

    std::shared_ptr< World >  world() const;




    void pulse();




    /**
    * Добавляет новый контейнер.
    */
    void nextIncarnate( const logicCoord_t& );




    /**
    * @return Ближайшая своб. ячейка для воплощения контейнера.
    */
    logicCoord_t nearFreeCoord();




    /**
    * @return След. своб. ячейка для записи контейнера.
    */
    index_t nextFreeCell();




    /**
    * @return В ячейке с заданными координатами есть контейнер.
    */
    bool present( const logicCoord_t&,  const typelib::size2Int_t& );

    bool present( const logicCoord_t& );

    bool present( index_t i );




    /**
    * @return Случайный контейнер.
    */
    static nameContainer_t nextRandom();




    /**
    * @return Количество доступных контейнеров.
    *
    * # Определяется по содержимому папки 'media/container'.
    */
    static size_t countAvailable();




    bool inside( const logicCoord_t& lc );

    bool inside( int x, int y );




    index_t ic( const logicCoord_t& );

    index_t ic( int x, int y );




    logicCoord_t ci( index_t );




    static inline typelib::coord2Int_t isc( size_t cell ) {
        using namespace typelib;
        ASSERT( (cell < 9)
            && "Попытка получить доступ за пределами мира." );
        const static coord2Int_t a[ 9 ] = {
            coord2Int_t(  0,   0 ),
            coord2Int_t(  0,  +1 ),
            coord2Int_t( +1,   0 ),
            coord2Int_t(  0,  -1 ),
            coord2Int_t( -1,   0 ),
            coord2Int_t( +1,  +1 ),
            coord2Int_t( +1,  -1 ),
            coord2Int_t( -1,  -1 ),
            coord2Int_t( -1,  +1 )
        };

        return a[ cell ];
    }




protected:
    std::weak_ptr< World >  mWorld;


    /**
    * Вспомогательная структура: отмечены ячейки, занятые контейнерами.
    */
    std::unique_ptr< cl_bool >  mFillCellSet;
    size_t  memsizeFillCellSet;


    /**
    * Поле контейнеров.
    */
    std::unique_ptr< nameContainer_t >       mNameSet;
    const size_t  memsizeNameSet;

    std::unique_ptr< logicCoord_t >          mLogicCoordSet;
    const size_t  memsizeLogicCoordSet;

    std::unique_ptr< visualCoord_t >         mVisualCoordSet;
    const size_t  memsizeVisualCoordSet;

    // для плавного перемещения между ячейками
    std::unique_ptr< visualCoord_t >         mVisualShiftSet;
    const size_t  memsizeVisualShiftSet;

    std::unique_ptr< cl_float >              mVisualRotationSet;
    const size_t  memsizeVisualRotationSet;

    std::unique_ptr< cl_float >              mVisualAlphaSet;
    const size_t  memsizeVisualAlphaSet;

    std::unique_ptr< enum STATE_CONTAINER >  mStateSet;
    const size_t  memsizeStateSet;

    std::unique_ptr< cl_int >                mDurationTimeSet;
    const size_t  memsizeDurationTimeSet;

    std::unique_ptr< cl_int >                mStopTimeSet;
    const size_t  memsizeStopTimeSet;


    /**
    * Кеш.
    */
    static size_t countAvailable_;


    friend World;
};


} // matchn
