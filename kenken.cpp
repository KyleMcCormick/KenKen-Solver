// Compile with:
// g++ -o kenken kenken.cpp -std=c++14 -Wall [-O3]
//
// Run all tests with:
// ./kenken test_cases/*.test


#include <iostream>
#include <fstream>
#include <map>

#include "Board.h"


using namespace KenKen;


int main( int argc, char* argv[] ){

    if( argc < 2 ){
        std::cout << "Usage: ./test <board_file_names>\n";
        exit( 1 );
    }

    std::cout << '\n';

    for( int i = 1 ; i < argc ; i++ ){

        auto B = board::get_from_file( argv[ i ] );

        if( ! B ){
            std::cerr << "Error reading file `"
                      << argv[ i ]
                      << "`. Skipping...\n";
        }

        B-> solve();

        if( B-> is_solved() ){

            std::cout << argv[ i ] << ":\n";
            B-> print_solution();
            std::cout << '\n';

        } else {

            std::cout << "Failed to solve '" << argv[i] << "'\n\n";
            B-> print_combos();
            std::cout << "\n\n";

        }

    }

}