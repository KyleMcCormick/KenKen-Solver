#ifndef _KENKENBOARD_H_
#define _KENKENBOARD_H_

#include <algorithm>
#include <fstream>
#include <cmath>

#include "KenKenCluster.h"


namespace KenKen{


    class board {


        std::vector< cluster* > _clusters;

        std::vector< cell* >    _cells;

        uint8_t                 _board_size;


        double cluster_combo_log(){
            double result = 0.0;
            for( auto& c : _clusters )
                result += log( c-> num_combos() );
            return result;
        }


      public:



        board( uint8_t Board_Size ){
            _board_size = Board_Size;
        }



        static board* get_from_file( const char* File_Name ){
            std::ifstream infile( File_Name );

            int board_size;
            infile >> board_size;

            auto result = new board( board_size );

            char cluster_id;
            char cluster_type;
            int  value;
            std::map< char, cluster* > clusters;
            std::string waste;

            for( int i = 0 ; i < board_size ; i++ )
                infile >> waste;

            while( infile >> cluster_id ){
                infile >> cluster_type;
                infile >> value;
                cluster* c;

                switch( cluster_type ){
                    case '+':
                        c = new addition_cluster( value, board_size, cluster_id );
                        break;
                    case '-':
                        c = new subtraction_cluster( value, board_size, cluster_id );
                        break;
                    case '*': case 'x': case 'X':
                        c = new multiplication_cluster( value, board_size, cluster_id );
                        break;
                    case '/': case '%':
                        c = new division_cluster( value, board_size, cluster_id );
                        break;
                }

                clusters.insert( std::pair< char, cluster* > ( cluster_id, c ) );
            }

            infile.clear();
            infile.seekg( 0 );

            infile >> board_size;

            for( int pos = 0 ; pos < board_size * board_size ; pos++ ){
                infile >> cluster_id;
                if( clusters.count( cluster_id ) )
                    clusters.at( cluster_id )-> add_cell( new cell( pos, board_size ) );
                else{
                    std::cerr << "\nCluster " << cluster_id
                            << " isn't listed after the board diagram\n";
                    exit( 2 );
                }
            }

            // Add all clusters to the board, with their cells
            for( auto& cl : clusters ){
                result-> _clusters.push_back( cl.second );
                result-> _cells.insert(
                            result-> _cells.end(),
                            cl.second->cells().begin(),
                            cl.second->cells().end()
                        );
            }

            // Sort the cells by location ( not by cluster )
            std::sort(
                result-> _cells.begin(),
                result-> _cells.end(),
                []( cell* A,cell* B ){ return A->pos() < B->pos(); }
            );

            return result;
        }



        void solve(){

            for( const auto& cl : _clusters )
                cl-> calculate_combos();

            double logarithmic_combo_product = cluster_combo_log();

            while( logarithmic_combo_product > 0.1 ){

                while( logarithmic_combo_product > 0.1 ){

                    for( size_t i = 0 ; i < _clusters.size() ; i++ ){
                        auto first = _clusters[ i ];

                        for( size_t j = i + 1 ; j < _clusters.size() ; j++ )
                            if( first-> has_overlap_with( _clusters[ j ] ) )
                                first-> reduce_with( _clusters[ j ] );

                    }

                    volatile double tmp = cluster_combo_log();

                    if( logarithmic_combo_product <= tmp )
                        break;

                    logarithmic_combo_product = tmp;
                }

                if( logarithmic_combo_product > 0.1 ){

                    for( size_t i = 0 ; i < _clusters.size() ; i++ ){
                        auto first = _clusters[ i ];

                        for( size_t j = i + 1 ; j < _clusters.size() ; j++ ){
                            auto second = _clusters[ j ];

                            if( ! first-> has_overlap_with( second ) )
                                continue;

                            for( size_t k = j + 1 ; k < _clusters.size() ; k++ ){
                                auto third = _clusters[ k ];

                                if(     first-> has_overlap_with( third )
                                    && second-> has_overlap_with( third )
                                  )
                                    first-> reduce_with( second, third );
                            }
                        }
                    }

                    double tmp = cluster_combo_log();

                    if( logarithmic_combo_product <= tmp )
                        return;

                    logarithmic_combo_product = tmp;
                }

            }
        }



        bool is_solved() const {
            for( auto& cl : _clusters )
                if( cl->num_combos() != 1 )
                    return false;
            return true;
        }



        void debug_print() const {
            std::cout << (int) _board_size << '\n' << std::flush;
            int pos = 0;

            for( size_t i = 0 ; i < _cells.size() ; i++ ){
                if( _cells[ i ]-> pos() != pos ){
                    std::cerr << "\nError at cell " << i << "\n";
                    exit( 3 );
                }
                std::cout << _cells[ i ]-> parent()-> id() << std::flush;
                if( ( (pos + _board_size + 1) % _board_size ) == 0 )
                    std::cout << '\n';
                pos++;
            }

            for( auto& c : _clusters )
                std::cout << c->id()
                          << c->type_id()
                          << c->value()    << '\n';
        }



        void print_solution(){

            for( auto& cl : _clusters ){
                if( cl->num_combos() == 1 )
                    cl->map_combo( 0 );
                else{
                    std::cerr << "Not solved!\n";
                    return;
                }
            }

            for( size_t i = 0 ; i < _cells.size() ; i++ ){
                std::cout << (int) _cells[ i ]-> value() << ' ';
                if( ( (i + _board_size + 1) % _board_size ) == 0 )
                    std::cout << '\n';
            }
        }



        void print_combos() const {
            for( const auto& c : _clusters ){
                std::cout << c -> id() << ' ';
                c-> print_combos();
                std::cout << '\n';
            }
        }


    };


}


#endif