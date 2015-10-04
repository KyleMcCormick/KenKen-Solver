#ifndef _KENKENCLUSTER_H_
#define _KENKENCLUSTER_H_

#include "Cell.h"

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
            // Takes a set of cell pairs and returns true if and only if all
            // pairs have non-matching values
            std::vector< std::pair< cell*, cell* > >& List
        );


      protected:


        int                                    _value;

        size_t                                 _board_size;

        std::vector< std::vector< uint8_t > >  _combos;

        std::vector< cell* >                   _cells;

        char                                   _id;



        size_t min_dim() const;


        void remove_invalid_combos();



        void find_legal_combos(
            bool (*Valid)( int candidate, int goal ),
                        // Returns true iff the candidate
                        // might work for the remaining goal
            int  (*New_Val)( int candidate, int goal )
                        // Returns the new updated goal value after
                        // removing the candidate from the goal value
        );



        void find_legal_pairs(
            uint8_t (*Next_Val)( uint8_t LowVal, uint8_t ClusterVal )
                        // Returns the HigherValue for the pair, with the
                        // given lower value and cluster
        );



      public:


        virtual void calculate_combos() = 0;


        virtual char type_id() = 0;


        cluster( int Value, size_t Board_Size, char Id );


        void add_cell( cell* Cell );


        char id() const;


        int value() const;


        const std::vector< cell* >& cells();


        size_t num_combos() const;


        bool has_overlap_with( cluster* C ) const;


        void set_to_combo( int Combo_Num );


        std::shared_ptr< std::vector< std::pair< cell*, cell* > > >
            conflicting_cells( const cluster* C ) const;


        void reduce_with( cluster* C );


        void reduce_with( cluster* B, cluster* C );


        void print_combos() const;

    };




    class addition_cluster : public cluster{

      public:

        addition_cluster( int Value, size_t Board_Size, char Id )
            : cluster( Value, Board_Size, Id ){}

        void calculate_combos() override {
            find_legal_combos(
                    []( int candidate, int goal )-> bool {
                            return goal >= candidate;
                    },
                    []( int candidate, int goal )-> int  {
                            return goal - candidate;
                    }
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
            find_legal_pairs(
                []( uint8_t candidate, uint8_t goal )-> uint8_t {
                        return candidate + goal;
                }
            );
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
                []( int candidate, int goal )-> bool {
                        return goal % candidate == 0;
                },
                []( int candidate, int goal )-> int {
                        return goal / candidate;
                }
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
            find_legal_pairs( []( uint8_t candidate, uint8_t goal )-> uint8_t {
                    return candidate * goal;
            } );
        }

        char type_id() override {
            return '/';
        }

    };



}


#endif