#include "../../../include/stdafx.h"
#include "../../../include/ge/container/Container.h"
#include "../../../include/ge/world/World.h"


namespace matchn {


size_t Container::countAvailable_ = 0;




OpenCLContainer::OpenCLContainer(
) :
    errorCL( CL_SUCCESS )
{
    // # Инициализацию сделаем при вызове Container().
}




void
OpenCLContainer::prepareCL() {

    std::vector< cl::Platform >  mPlatformCL;
    cl::Platform::get( &mPlatformCL );
    if ( mPlatformCL.empty() ) {
        throw Exception( "Не найдено ни одного устройства с поддержкой OpenCL." );
    }

    cl_context_properties properties[] = {
        CL_CONTEXT_PLATFORM,
        ( cl_context_properties )( mPlatformCL.front() )(),
        0
    };
    mContextCL = cl::Context( CL_DEVICE_TYPE_CPU, properties );
    mDeviceCL = mContextCL.getInfo< CL_CONTEXT_DEVICES >();
    mQueueCL = cl::CommandQueue( mContextCL, mDeviceCL[ 0 ],  0,  &errorCL );

#ifdef _DEBUG
    std::cout << "\nВыбрано устройство OpenCL:\n";
    printCLInfo( mPlatformCL.front()() );
#endif
}




void
OpenCLContainer::prepareKernel( const std::shared_ptr< World >  w ) {
    // # Контекст и очередь команд уже инициализированы.

    static const std::string element = "container";
    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( element + "/startDrop" )
        ( element + "/drop" )
        ( element + "/stopDrop" )
        ( element + "/wait" )
    ;

    // Компилируем ядра OpenCL
    compileCLKernel( w, kernelKeys );
}




void
OpenCLContainer::compileCLKernel(
    const std::shared_ptr< World >  w,
    const std::vector< std::string >&  kernelKeys,
    const std::vector< std::string >&  includeHCL,
    const std::string&  options
) {
    // # Контекст и очередь команд инициализированы в конструкторе.

#ifdef _DEBUG
    std::cout << std::endl << std::endl;
#endif

    std::ostringstream commonOptions;
    commonOptions
        // лечим точность для float
        << std::fixed
        << " " << options
        << commonConstantCLKernel( w )
        << commonOptionCLKernel()
        << "";

    // здесь соберём код необходимых ядру библиотек
    std::string kernelLibraryCode = "";

    // подключаем общие библиотеки и структуры
    // #! Важен порядок подключения.
    std::vector< std::string >  hcl = boost::assign::list_of
        ( CL_PATH + "/pragma.hcl" )
        ( CL_PATH + "/restruct.hcl" )
        ( CL_PATH + "/structure.h" )
        ( CL_PATH + "/helper.hcl" )
    ;
    hcl.insert( hcl.end(), includeHCL.cbegin(), includeHCL.cend() );
    for (auto itr = hcl.cbegin(); itr != hcl.cend(); ++itr) {
        const std::string& pathAndName = *itr;
#ifdef _DEBUG
        const std::string shortName =
            itr->substr( pathAndName.find_last_of( '/' ) + 1 );
        std::cout << "Собираем \"" << shortName << "\" ..";
#endif
        const std::ifstream  file( pathAndName.c_str() );
        ASSERT( file.is_open()
            && "Файл не найден." );
        std::stringstream buffer;
        buffer << file.rdbuf();
        kernelLibraryCode +=
            (std::string)"\n\n\n\n\n" +
            "// #include " + pathAndName + "\n" +
            buffer.str();
#ifdef _DEBUG
    std::cout << " ОК" << std::endl;
#endif
    } // for (auto itr = hcl.cbegin(); ...


    // собираем исходники ядер в один файл и компилируем
    for (auto itr = std::begin( kernelKeys ); itr != std::end( kernelKeys ); ++itr) {
        //   # Последнее за "/" название является именем ядра.
        const std::string kernelKey = *itr;
        const std::string kernelName =
            itr->substr( itr->find_last_of( '/' ) + 1 );

        const std::string fileKernel = kernelKey + ".cl";
        const std::string pathAndName = CL_PATH + "/" + fileKernel;
#ifdef _DEBUG
        std::cout << "\"" << fileKernel << "\" ..";
#endif
        const std::ifstream  file( pathAndName.c_str() );
        ASSERT( file.is_open()
            && "Файл ядра не найден." );
        std::stringstream buffer;
        buffer << file.rdbuf();
        const std::string kernelSC = buffer.str();
#ifdef _DEBUG
    std::cout << " ОК" << std::endl;
#endif

        const std::string kernelSourceCode =
            kernelLibraryCode + "\n\n\n\n\n" +
            "// @include kernel " + pathAndName + "\n" +
            kernelSC;

#ifdef _DEBUG
        // сохраняем полный код ядра в файл
        const std::string fn =
            boost::replace_all_copy( kernelKey, "/", "_" ) + ".debug.cl";
        std::ofstream  out( fn.c_str() );
        out << kernelSourceCode;
        out.close();
#endif

        // @todo optimize Загружать уже скомпилированные ядра.
        const cl::Program::Sources  source( 1,  std::make_pair(
            kernelSourceCode.c_str(),  kernelSourceCode.length()
        ) );

        // компилируем каждое ядро в контексте
#ifdef _DEBUG
        std::cout << "Опции OpenCL для ядра \"" << kernelKey << "\"" <<
            std::endl << commonOptions.str() <<
        std::endl;
#endif

        const cl::Program  program( mContextCL, source );
        try {
            program.build( mDeviceCL,  commonOptions.str().c_str() );

        } catch ( const cl::Error& ex ) {
            if (ex.err() == CL_BUILD_PROGRAM_FAILURE) {
                std::cerr <<
                    program.getBuildInfo< CL_PROGRAM_BUILD_LOG >( mDeviceCL.front() ) <<
                std::endl;
            }
            throw Exception( ex.what() );
        }

        const cl::Kernel  kernel( program,  kernelName.c_str(),  &errorCL );
        mKernelCL[ kernelKey ] = kernel;

    } // for (auto itr ...

}




template< size_t globalSize >
void
OpenCLContainer::enqueueEventKernelCL(
    const std::string&  key,
    cl::Event*          event
) {
    static const auto EMPTY_WAIT = vectorEventCL_t();
    enqueueEventKernelCL< globalSize >(
        key,  EMPTY_WAIT,  event
    );
}




template< size_t globalSize >
void
OpenCLContainer::enqueueEventKernelCL(
    const std::string&      key,
    const vectorEventCL_t&  waitEvents,
    cl::Event*              event
) {
    const auto ftr = mKernelCL.find( key );
    ASSERT( (ftr != mKernelCL.cend())
        && "Ядро OpenCL не найдено." );

    mQueueCL.enqueueNDRangeKernel(
        ftr->second,
        cl::NullRange,
        cl::NDRange( globalSize ),
        cl::NullRange,
        waitEvents.empty() ? nullptr : &waitEvents,
        event
    ); 
}




std::string
OpenCLContainer::commonConstantCLKernel( std::shared_ptr< World >  w ) {

    std::ostringstream options;
    options
        // лечим точность
        << std::fixed

        // структуры и методы h-файлов на С++ станут годными для OpenCL
        << " -D MATCHN_AS_OPEN_CL_STRUCT"

        << " -D K=" << w->K
        << " -D N=" << w->N
        << " -D M=" << w->M

        << " -D HEIGHT_CELL=" << w->needVisualSizeContainer().y
        << " -D CONTAINER_COUNT=" << CONTAINER_COUNT

        << "";

    return options.str();
}




std::string
OpenCLContainer::commonOptionCLKernel() {

    std::ostringstream options;
    options
        // лечим точность для float
        //<< std::fixed

        // предупреждения в ядрах считаем ошибками
        << " -Werror"
#if 0
        // оптимизация
        // #i ~10% прирост только с включением опций ниже.
        // @todo optimize Тонкая оптимизация OpenCL.
        //       http://khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/clBuildProgram.html
        << " -cl-fast-relaxed-math"
        << " -cl-mad-enable"
#endif
#if 0
        // внимательная отладка
        // #i Включать только при вылавливании блох: более чем
        //    20-кратное замедление.
        << " -cl-opt-disable"
#endif
        << "";

    return options.str();
}








Container::Container(
) :
    OpenCLContainer(),

    // # Будет (пере)инициализирован при создании World.
    mWorld(),

    // фиксированные размеры структур
    // @todo fine Одной строкой: memsize*Set( createContent( ... ) ).
    mFillCellSet( createContent< cl_bool >( CONTAINER_OPTIMIZE_COUNT ) ),
    memsizeFillCellSet( sizeof( cl_bool ) * CONTAINER_OPTIMIZE_COUNT ),

    mNameSet( createContent< nameContainer_t >( CONTAINER_OPTIMIZE_COUNT ) ),
    memsizeNameSet( sizeof( nameContainer_t ) * CONTAINER_OPTIMIZE_COUNT ),

    mLogicCoordSet( createContent< logicCoord_t >( CONTAINER_OPTIMIZE_COUNT ) ),
    memsizeLogicCoordSet( sizeof( logicCoord_t ) * CONTAINER_OPTIMIZE_COUNT ),

    mVisualCoordSet( createContent< visualCoord_t >( CONTAINER_OPTIMIZE_COUNT ) ),
    memsizeVisualCoordSet( sizeof( visualCoord_t ) * CONTAINER_OPTIMIZE_COUNT ),

    mVisualShiftSet( createContent< visualCoord_t >( CONTAINER_OPTIMIZE_COUNT ) ),
    memsizeVisualShiftSet( sizeof( visualCoord_t ) * CONTAINER_OPTIMIZE_COUNT ),

    mVisualRotationSet( createContent< cl_float >( CONTAINER_OPTIMIZE_COUNT ) ),
    memsizeVisualRotationSet( sizeof( cl_float ) * CONTAINER_OPTIMIZE_COUNT ),

    mVisualAlphaSet( createContent< cl_float >( CONTAINER_OPTIMIZE_COUNT ) ),
    memsizeVisualAlphaSet( sizeof( cl_float ) * CONTAINER_OPTIMIZE_COUNT ),

    mStateSet( createContent< enum STATE_CONTAINER >( CONTAINER_OPTIMIZE_COUNT ) ),
    memsizeStateSet( sizeof( enum STATE_CONTAINER ) * CONTAINER_OPTIMIZE_COUNT ),

    mDurationTimeSet( createContent< cl_int >( CONTAINER_OPTIMIZE_COUNT ) ),
    memsizeDurationTimeSet( sizeof( cl_int ) * CONTAINER_OPTIMIZE_COUNT ),

    mStopTimeSet( createContent< cl_int >( CONTAINER_OPTIMIZE_COUNT ) ),
    memsizeStopTimeSet( sizeof( cl_int ) * CONTAINER_OPTIMIZE_COUNT )
{
    // подготавливаем контекст и очередь команд для работы с OpenCL
    prepareCL();

    bclFillCellSet = cl::Buffer(
        mContextCL,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeFillCellSet,
        mFillCellSet.get(),
        &errorCL
    );

    bclNameSet = cl::Buffer(
        mContextCL,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeNameSet,
        mNameSet.get(),
        &errorCL
    );

    bclLogicCoordSet = cl::Buffer(
        mContextCL,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeLogicCoordSet,
        mLogicCoordSet.get(),
        &errorCL
    );

    bclVisualCoordSet = cl::Buffer(
        mContextCL,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeVisualCoordSet,
        mVisualCoordSet.get(),
        &errorCL
    );

    bclVisualShiftSet = cl::Buffer(
        mContextCL,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeVisualShiftSet,
        mVisualShiftSet.get(),
        &errorCL
    );

    bclVisualRotationSet = cl::Buffer(
        mContextCL,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeVisualRotationSet,
        mVisualRotationSet.get(),
        &errorCL
    );

    bclVisualAlphaSet = cl::Buffer(
        mContextCL,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeVisualAlphaSet,
        mVisualAlphaSet.get(),
        &errorCL
    );

    bclStateSet = cl::Buffer(
        mContextCL,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeStateSet,
        mStateSet.get(),
        &errorCL
    );

    bclDurationTimeSet = cl::Buffer(
        mContextCL,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeDurationTimeSet,
        mDurationTimeSet.get(),
        &errorCL
    );

    bclStopTimeSet = cl::Buffer(
        mContextCL,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeStopTimeSet,
        mStopTimeSet.get(),
        &errorCL
    );
}




std::shared_ptr< Container >
Container::valueOf() {
    return std::shared_ptr< Container >( new Container() );
}




Container::~Container() {
}




std::shared_ptr< World >
Container::world() {
    return mWorld.lock();
}




std::shared_ptr< World >
Container::world() const {
    return mWorld.lock();
}




void
Container::pulse() {
    ASSERT( (mWorld.lock() && mWorld.lock()->visual())
        && "Мир должен быть инициализирован." );

    // # Перевод в др. состояние обеспечивают ядра OpenCL.
    {
        const std::string key = "container/startDrop";
        const auto& kp = mKernelCL.find( key );
        DASSERT( kp != mKernelCL.cend() );
        auto& kernel = kp->second;
        kernel.setArg( 0, bclStateSet );
        kernel.setArg( 1, bclDurationTimeSet );
        kernel.setArg( 2, bclStopTimeSet );
        kernel.setArg( 3, World::currentTime() );
        kernel.setArg( 4, DURATION_DROP_CONTAINER );
        enqueueEventKernelCL< CONTAINER_OPTIMIZE_COUNT >( key );
        mQueueCL.finish();
    }

    {
        const std::string key = "container/drop";
        const auto& kp = mKernelCL.find( key );
        DASSERT( kp != mKernelCL.cend() );
        auto& kernel = kp->second;
        kernel.setArg( 0, bclStateSet );
        kernel.setArg( 1, bclVisualShiftSet );
        kernel.setArg( 2, bclDurationTimeSet );
        kernel.setArg( 3, bclStopTimeSet );
        kernel.setArg( 4, World::currentTime() );
        enqueueEventKernelCL< CONTAINER_OPTIMIZE_COUNT >( key );
        mQueueCL.finish();
    }

    {
        const std::string key = "container/stopDrop";
        const auto& kp = mKernelCL.find( key );
        DASSERT( kp != mKernelCL.cend() );
        auto& kernel = kp->second;
        kernel.setArg( 0, bclStateSet );
        kernel.setArg( 1, bclLogicCoordSet );
        kernel.setArg( 2, bclVisualCoordSet );
        kernel.setArg( 3, bclVisualShiftSet );
        enqueueEventKernelCL< CONTAINER_OPTIMIZE_COUNT >( key );
        mQueueCL.finish();
    }

    {
        const std::string key = "container/wait";
        const auto& kp = mKernelCL.find( key );
        DASSERT( kp != mKernelCL.cend() );
        auto& kernel = kp->second;
        // @todo optimize Связать аргументы при создании ядра.
        kernel.setArg( 0, bclStateSet );
        kernel.setArg( 1, bclLogicCoordSet );
        kernel.setArg( 2, bclFillCellSet );
        enqueueEventKernelCL< CONTAINER_OPTIMIZE_COUNT >( key );
        mQueueCL.finish();
    }

    // обновляем данные
    // @todo optimize Ядра могут отмечать, были ли изменения. Здесь можем
    //       читать только изменённые списки.
    mQueueCL.enqueueReadBuffer(
        bclNameSet,
        CL_TRUE,
        0,
        memsizeNameSet,
        mNameSet.get()
    );

    mQueueCL.enqueueReadBuffer(
        bclLogicCoordSet,
        CL_TRUE,
        0,
        memsizeLogicCoordSet,
        mLogicCoordSet.get()
    );

    mQueueCL.enqueueReadBuffer(
        bclVisualCoordSet,
        CL_TRUE,
        0,
        memsizeVisualCoordSet,
        mVisualCoordSet.get()
    );

    mQueueCL.enqueueReadBuffer(
        bclVisualShiftSet,
        CL_TRUE,
        0,
        memsizeVisualShiftSet,
        mVisualShiftSet.get()
    );

    mQueueCL.enqueueReadBuffer(
        bclVisualRotationSet,
        CL_TRUE,
        0,
        memsizeVisualRotationSet,
        mVisualRotationSet.get()
    );

    mQueueCL.enqueueReadBuffer(
        bclVisualAlphaSet,
        CL_TRUE,
        0,
        memsizeVisualAlphaSet,
        mVisualAlphaSet.get()
    );

    mQueueCL.enqueueReadBuffer(
        bclNameSet,
        CL_TRUE,
        0,
        memsizeNameSet,
        mNameSet.get()
    );

    mQueueCL.enqueueReadBuffer(
        bclFillCellSet,
        CL_TRUE,
        0,
        memsizeFillCellSet,
        mFillCellSet.get()
    );
}




void
Container::nextIncarnate( const logicCoord_t& lc ) {

    // находим ближайшую своб. ячейку и создаём контейнер
    const index_t j = nextFreeCell();
    ASSERT( (j != INT_MAX)
        && "Нет свободных ячеек." );

#ifdef _DEBUG
    CONSOLE << "Incarnate container " <<
        typelib::coord2Int_t( lc.x, lc.y ) << std::endl;
#endif

    // попутно обновляем буферы OpenCL
    mNameSet.get()[ j ] = nextRandom();
    mQueueCL.enqueueWriteBuffer(
        bclNameSet,
        CL_TRUE,
        0,
        memsizeNameSet,
        mNameSet.get()
    );

    mLogicCoordSet.get()[ j ] = lc;
    mQueueCL.enqueueWriteBuffer(
        bclLogicCoordSet,
        CL_TRUE,
        0,
        memsizeLogicCoordSet,
        mLogicCoordSet.get()
    );

    mVisualCoordSet.get()[ j ] =
        mWorld.lock()->visualByLogicCoordContainer( lc );
    mQueueCL.enqueueWriteBuffer(
        bclVisualCoordSet,
        CL_TRUE,
        0,
        memsizeVisualCoordSet,
        mVisualCoordSet.get()
    );

    static const visualCoord_t vcZero = { 0, 0 };
    mVisualShiftSet.get()[ j ] = vcZero;
    mQueueCL.enqueueWriteBuffer(
        bclVisualShiftSet,
        CL_TRUE,
        0,
        memsizeVisualShiftSet,
        mVisualShiftSet.get()
    );

    mVisualRotationSet.get()[ j ] = 0.0f;
    mQueueCL.enqueueWriteBuffer(
        bclVisualRotationSet,
        CL_TRUE,
        0,
        memsizeVisualRotationSet,
        mVisualRotationSet.get()
    );

    mVisualAlphaSet.get()[ j ] = 1.0f;
    mQueueCL.enqueueWriteBuffer(
        bclVisualAlphaSet,
        CL_TRUE,
        0,
        memsizeVisualAlphaSet,
        mVisualAlphaSet.get()
    );

    mStateSet.get()[ j ] = SC_WAIT;
    mQueueCL.enqueueWriteBuffer(
        bclStateSet,
        CL_TRUE,
        0,
        memsizeStateSet,
        mStateSet.get()
    );

    const index_t i = ic( lc );
    mFillCellSet.get()[ i ] = true;
    mQueueCL.enqueueWriteBuffer(
        bclFillCellSet,
        CL_TRUE,
        0,
        memsizeFillCellSet,
        mFillCellSet.get()
    );
}




logicCoord_t
Container::nearFreeCoord() {
    ASSERT( mWorld.lock() );
    ASSERT( mFillCellSet );

    // воспользуемся картой контейнеров
    for (index_t i = 0; i < mWorld.lock()->volume(); ++i) {
        if ( !present( i ) ) {
            return ci( i );
        }
    }

    static const logicCoord_t UNDEFINED = { INT_MAX, INT_MAX };
    return UNDEFINED;
}




index_t
Container::nextFreeCell() {
    ASSERT( mNameSet );

    // пройдём по всем воплощённым контейнерам
    // @todo optimize Ввести указатель на след. своб. ячейку.
    for (index_t i = 0; i < mWorld.lock()->volume(); ++i) {
        const nameContainer_t& nc = mNameSet.get()[ i ];
        if (nc == 0) {
            return i;
        }
    }

    return INT_MAX;
}




bool
Container::present(
    const logicCoord_t&         lc,
    const typelib::size2Int_t&  sw
) {
    ASSERT( mFillCellSet );
    const index_t i = ic( lc );
    return present( i );
}


bool
Container::present( const logicCoord_t& lc ) {
    return present( lc, mWorld.lock()->size() );
}


bool
Container::present( index_t i ) {
    ASSERT( i < mWorld.lock()->volume() );
    return (mFillCellSet.get()[ i ] != 0);
}




nameContainer_t
Container::nextRandom() {

    static boost::mt19937  gen;
    // # Последним всегда идёт бонусный элемент: его надо заслужить, собирая
    //   вместе первые. Поэтому: -2.
    const size_t ne = countAvailable();
    ASSERT( (ne >= 2)
        && "В папке 'media/element' должно быть больше одного элемента." );
    static boost::uniform_int<>   uni( 'a', 'a' + ne - 2 );
    static boost::variate_generator< boost::mt19937&, boost::uniform_int<> >
        random( gen, uni );

    const auto ch = static_cast< nameContainer_t >( random() );
    return ch;
}




size_t
Container::countAvailable() {
    if (countAvailable_ == 0) {
        std::set< std::string >  folder;
        typelib::file::listFolder( &folder, PATH_MEDIA + "/container" );
        countAvailable_ = folder.size();
        ASSERT( (countAvailable_ > 0)
            && "В папке 'media/container' не обнаружено ни одного элемента." );
    }

    return countAvailable_;
}




bool
Container::inside( const logicCoord_t& lc ) {
    return inside( lc.x, lc.y );
}


bool
Container::inside( int x, int y ) {
    const auto sw = mWorld.lock()->size();
    return (x >= 0) && (x < sw.x)
        && (y >= 0) && (y < sw.y);
}




index_t
Container::ic( const logicCoord_t& lc ) {
    return ic( lc.x, lc.y );
}


index_t
Container::ic( int x, int y ) {
    const auto sw = mWorld.lock()->size();
    return static_cast< index_t >( x + y * sw.x );
}




logicCoord_t
Container::ci( index_t i ) {

    const auto sw = mWorld.lock()->size();
    const size_t N = static_cast< size_t >( sw.x );
    const size_t y = i / N;
    const size_t x = i - y * N;
    const logicCoord_t  lc = { x, y };
    ASSERT( inside( lc )
        && "Вычисленная 2D координата лежит за пределами мира." );

    return lc;
}


} // matchn
