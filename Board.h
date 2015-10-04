#ifndef _KENKENBOARD_H_
#define _KENKENBOARD_H_

#include <algorithm>
#include <fstream>
#include <cmath>

#include "Cluster.h"


namespace KenKen{


    class board {


        std::vector< cluster* > _clusters;

        std::vector< cell* >    _cells;

        uint8_t                 _board_size;


        double cluster_combo_log();


      public:


        board( uint8_t Board_Size );


        static std::shared_ptr< board > get_from_file( const char* File_Name );


        void solve();


        bool is_solved() const;


        void print_solution();


        void print_combos() const;

    };


}


#endif