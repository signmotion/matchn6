#include "include/stdafx.h"
#include "include/configure.h"
#include "include/ge/world/World.h"




void run();




/**
* Переписанная через OpenCL демо-версия MatchN5.
*/
int
main( int argc, char** argv ) {

    using namespace matchn;

            
    setlocale( LC_ALL, "Russian" );
    setlocale( LC_NUMERIC, "C" );


    // создаём мир
    int code = 0;
    try {
        run();
        
    } catch ( const Exception& ex ) {
        std::cerr << std::endl << ex.what() << std::endl;
        code = -1;

    } catch ( const CL_Exception& ex ) {
        CL_ConsoleWindow  console( "Console", 100, 200 );
        CL_Console::write_line(
            "\n(!) Exception caught: " + ex.get_message_and_stack_trace()
        );
        console.display_close_message();
        code = -2;

    } catch ( const std::exception& ex ) {
        std::cerr << std::endl << ex.what() << std::endl;
        code = -3;

    } catch ( const std::string& ex ) {
        std::cerr << std::endl << ex << std::endl;
        code = -100;

    } catch ( const char* const ex ) {
        std::cerr << std::endl << *ex << std::endl;
        code = -200;

    } catch ( ... ) {
        std::cerr << std::endl << "(!) Unknown exception." << std::endl;
        code = -300;
    }

    std::cout << "\n^\n";
    //std::cin.ignore();

    return code;

}; // main()








inline void
run() {

    using namespace matchn;

    std::shared_ptr< World >  w = World::valueOf( 3, 5, 5 );
    w->go();
}
