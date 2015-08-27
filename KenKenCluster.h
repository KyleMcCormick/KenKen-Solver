#ifndef _KENKENCLUSTER_H_
#define _KENKENCLUSTER_H_

#include "KenKenCell.h"

#include <map>
#include <algorithm>
#include <memory>


namespace KenKen{


    class cluster {


        static std::vector< std::vector< uint8_t > >* legal_combos(
            size_t,size_t,size_t,size_t,bool(*)(int,int),int(*)(int,int),size_t
        );


        static std::vector< std::vector< uint8_t > >* legal_pairs(
            size_t,size_t,uint8_t(*)(uint8_t,uint8_t)
        );


        static size_t mode_frequency( std::vector< uint8_t >& Array );


        static bool all_pairs_unmatching(
            std::vector< std::pair< cell*, cell* > >& List
        ){
            for( const auto& cell_pair : List )
                if( cell_pair.first->value() == cell_pair.second->value() )
                    return false;
            return true;
        }


      protected:


        int                                    _value;

        size_t                                 _board_size;

        std::vector< std::vector< uint8_t > >  _combos;

        std::vector< cell* >                   _cells;

        char                                   _id;



        size_t min_dim() const {
            std::map< uint8_t, uint8_t > tmp_X;
            std::map< uint8_t, uint8_t > tmp_Y;
            size_t width  = 0;
            size_t height = 0;
            for( auto& c : _cells ){
                if( tmp_X.insert(
                        std::pair< uint8_t, uint8_t >( c-> col_num(),0 )
                    ).second
                  )
                    width++;
                if( tmp_Y.insert(
                        std::pair< uint8_t, uint8_t >( c-> row_num(),0 )
                    ).second
                  )
                    height++;
            }
            return width > height ? height : width;
        }



        void remove_invalid_combos(){
            using cell_pair = std::pair< cell*, cell* >;
            std::vector< cell_pair > neighbors;

            for( size_t i = 0 ; i < _cells.size() ; i++ )
                for( size_t j = i + 1 ; j < _cells.size() ; j++ )
                    if( _cells[ i ]->is_neighbor( _cells[ j ] ) )
                        neighbors.push_back( cell_pair( _cells[ i ], _cells[ j ] ) );

            for( size_t i = 0 ; i < _combos.size() ; i++ ){
                map_combo( i );
                if( ! all_pairs_unmatching( neighbors ) ){
                    _combos.erase( _combos.begin() + i );
                    i--;
                }
            }
        }



        void find_legal_combos(
            bool (*Valid)(int,int),
            int  (*New_Val)(int,int)
        ){
            auto tmp = *legal_combos(
                _board_size,
                _value,
                _cells.size(),
                min_dim(),
                Valid,
                New_Val,
                1
            );

            _combos.clear();

            for( auto& set : tmp )
                do{
                    _combos.push_back( set );
                }while( std::prev_permutation( set.begin(), set.end() ) );

            if( min_dim() > 1 )
                remove_invalid_combos();
        }



        void find_legal_pairs(
            uint8_t (*Next_Val)(uint8_t,uint8_t)
        ){
            auto tmp = *legal_pairs(
                _board_size,
                _value,
                Next_Val
            );

            _combos.clear();

            for( auto& set : tmp )
                do{
                    _combos.push_back( set );
                }while( std::next_permutation( set.begin(), set.end() ) );
        }



      public:


        virtual void calculate_combos() = 0;


        virtual char type_id() = 0;



        cluster( int Value, size_t Board_Size, char Id ){
            _value      = Value;
            _board_size = Board_Size;
            _id         = Id;
        }



        inline void add_cell( cell* Cell ){
            _cells.push_back( Cell );
            Cell-> link_cluster( this );
        }



        inline char id() const {
            return _id;
        }



        inline int value() const {
            return _value;
        }



        inline std::vector< cell* >& cells(){
            return _cells;
        }



        inline size_t num_combos() const {
            return _combos.size();
        }



        bool has_overlap_with( cluster* C ) const {
            std::vector< bool > rows( _board_size, false );
            std::vector< bool > cols( _board_size, false );

            for( auto& c : _cells ){
                rows[ c-> row_num() ] = true;
                cols[ c-> col_num() ] = true;
            }

            for( auto& c : C-> _cells )
                if( rows[ c-> row_num() ] || cols[ c-> col_num() ] )
                    return true;

            return false;
        }



        void map_combo( int Combo_Num ){
            for( size_t i = 0 ; i < _cells.size() ; i++ )
                _cells[ i ]-> set_value( _combos[ Combo_Num ][ i ] );
        }



        std::shared_ptr< std::vector< std::pair< cell*, cell* > > >
            conflicting_cells(
                const cluster* C
        ) const {
            using cell_pair = std::pair< cell*, cell* >;
            auto result = std::make_shared< std::vector< cell_pair > > ();
            for( auto& c1 : _cells )
                for( auto& c2 : C->_cells )
                    if( c1-> is_neighbor( c2 ) )
                        result-> push_back( cell_pair( c1, c2 ) );
            return result;
        }



        void reduce_with( cluster* C ){
            auto conflicts = conflicting_cells( C );

            std::vector< bool > valid1 ( _combos.size(), false );
            std::vector< bool > valid2 ( C->_combos.size(), false );

            for( size_t i = 0 ; i < _combos.size() ; i++ ){
                map_combo( i );
                for( size_t j = 0 ; j < C->_combos.size() ; j++ ){
                    C-> map_combo( j );
                    if( all_pairs_unmatching( *conflicts ) )
                        valid1[ i ] = valid2[ j ] = true;
                }
            }

            std::vector< std::vector< uint8_t > > tmp;
            for( int i = 0 ; i < (int) _combos.size() ; i++ ){
                if( valid1[ i ] )
                    tmp.push_back( _combos[ i ] );
            }
            _combos = tmp;

            tmp.clear();
            for( int i = 0 ; i < (int) C-> _combos.size() ; i++ ){
                if( valid2[ i ] )
                    tmp.push_back( C-> _combos[ i ] );
            }
            C-> _combos = tmp;
        }



        void reduce_with( cluster* B, cluster* C ){
            auto A = this;

            auto conflicts = A-> conflicting_cells( B );
            auto temp      = A-> conflicting_cells( C );
            conflicts-> insert( conflicts->end(), temp->begin(), temp->end() );
            temp           = B-> conflicting_cells( C );
            conflicts-> insert( conflicts->end(), temp->begin(), temp->end() );

            std::vector< bool > validA ( A->_combos.size(), false );
            std::vector< bool > validB ( B->_combos.size(), false );
            std::vector< bool > validC ( C->_combos.size(), false );

            for( size_t i = 0 ; i < A-> _combos.size() ; i++ ){
                A-> map_combo( i );
                for( size_t j = 0 ; j < B-> _combos.size() ; j++ ){
                    B-> map_combo( j );
                    for( size_t k = 0 ; k < C-> _combos.size() ; k++ ){
                        C-> map_combo( k );
                        if( all_pairs_unmatching( *conflicts ) )
                            validA[ i ] = validB[ j ] = validC[ k ] = true;
                    }
                }
            }

            std::vector< std::vector< uint8_t > > tmp;
            for( int i = 0 ; i < (int) A-> _combos.size() ; i++ ){
                if( validA[ i ] )
                    tmp.push_back( A-> _combos[ i ] );
            }
            A->_combos = tmp;

            tmp.clear();
            for( int i = 0 ; i < (int) B-> _combos.size() ; i++ ){
                if( validB[ i ] )
                    tmp.push_back( B-> _combos[ i ] );
            }
            B->_combos = tmp;

            tmp.clear();
            for( int i = 0 ; i < (int) C-> _combos.size() ; i++ ){
                if( validC[ i ] )
                    tmp.push_back( C-> _combos[ i ] );
            }
            C-> _combos = tmp;
        }



        void print_combos() const {
            for( const auto& p : _combos ){
                std::cout << " ( ";
                for( const auto& v : p )
                    std::cout << (int) v << ' ';
                std::cout << ")  ";
            }
        }


    };




    std::vector< std::vector< uint8_t > >* cluster::legal_combos(
            size_t  Board_Size,
            size_t  Goal,
            size_t  Num_Cells,
            size_t  Min_Dim,
            bool (*Valid)(int,int),
            int  (*New_Val)(int,int),
            size_t  Min_Val = 1
    ){

        std::vector< std::vector< uint8_t > >* result = nullptr;

        if( ! Num_Cells ){
            std::cerr << "Zero cell legal_combo call??\n";
            exit( 72 );
            return nullptr;
        }

        if( Num_Cells == 1 ){
            if( Min_Val <= Goal && Goal <= Board_Size ){
                result = new std::vector< std::vector< uint8_t > >;
                result-> push_back( std::vector< uint8_t > ( 1, Goal ) );
            }

            return result;
        }

        for( size_t candidate = Min_Val ; candidate <= Board_Size ; candidate++ ){

            if( ! Valid( candidate, Goal ) )
                continue;

            auto subsets = legal_combos(
                                        Board_Size,
                                        New_Val( candidate, Goal ),
                                        Num_Cells - 1,
                                        Min_Dim,
                                        Valid,
                                        New_Val,
                                        candidate
                                    );

            if( subsets == nullptr )
                continue;

            for( auto& s : *subsets ){
                s.push_back( candidate );
                if( mode_frequency( s ) <= Min_Dim ){
                    if( ! result )
                        result = new std::vector< std::vector< uint8_t > >;
                    result-> push_back( s );
                }
            }

            delete subsets;
        }

        return result ? result : new std::vector< std::vector< uint8_t > >;
    }



    std::vector< std::vector< uint8_t > >* cluster::legal_pairs(
        size_t Board_Size,
        size_t Val,
        uint8_t (*Next)(uint8_t,uint8_t)
    ){
        auto result = new std::vector< std::vector< uint8_t > >;
        for( uint8_t i = 1 ; Next( i, Val ) <= Board_Size ; i++ ){
            auto tmp = std::vector< uint8_t >( { i, Next( i, Val ) } );
            //tmp.push_back( next( i, val ) );
            result-> push_back( tmp );
        }
        return result;
    }



    size_t cluster::mode_frequency( std::vector< uint8_t >& Array ){

            if( ! Array.size() )
                return 0;

            int prev    = Array[ 0 ];
            size_t ct   = 1;
            size_t max  = 1;

            auto item = Array.begin();
            item++;

            for( ; item != Array.end() ; item++ ){
                if( *item == prev ){
                    ct++;
                    max = ct > max ? ct : max;
                }else{
                    prev = * item;
                    ct = 1;
                }
            }

            return max;
        }



    class addition_cluster : public cluster{

      public:

        addition_cluster( int Value, size_t Board_Size, char Id )
            : cluster( Value, Board_Size, Id ){}

        void calculate_combos() override {
            find_legal_combos(
                    [](int c,int g)->bool{return g>=c;},
                    [](int c,int g)->int {return g-c;}
            );
        }

        char type_id() override {
            return '+';
        }

    };



    class subtraction_cluster : public cluster{

      public:

        subtraction_cluster( int Value, size_t Board_Size, char Id )
            : cluster( Value, Board_Size, Id ){}

        void calculate_combos() override {
            find_legal_pairs( [](uint8_t c,uint8_t v)->uint8_t{return c+v;} );
        }

        char type_id() override {
            return '-';
        }

    };



    class multiplication_cluster : public cluster{

      public:

        multiplication_cluster( int Value, size_t Board_Size, char Id )
            : cluster( Value, Board_Size, Id ){}

        void calculate_combos() override {
            find_legal_combos(
                [](int c,int g)->bool{return !(g%c);},
                [](int c,int g)->int {return g/c;}
            );
        }

        char type_id() override {
            return '*';
        }

    };



    class division_cluster : public cluster{

      public:

        division_cluster( int Value, size_t Board_Size, char Id )
            : cluster( Value, Board_Size, Id ){}

        void calculate_combos() override {
            find_legal_pairs( [](uint8_t c,uint8_t v)->uint8_t{return c*v;} );
        }

        char type_id() override {
            return '/';
        }

    };



}


#endif