#ifndef _KENKENCELL_H_
#define _KENKENCELL_H_

#include <vector>
#include <cstdint>
#include <iostream>


namespace KenKen{


    class cluster;


    class cell {


        cluster*               _parent;      // parent cluster

        uint8_t                _value;       // current value

        uint8_t                _position;    // position on board

        uint8_t                _board_size;  // board dimension


      public:


        cell( uint8_t Pos, uint8_t Board_Size );


        uint8_t row_num() const;


        uint8_t col_num() const;


        uint8_t pos() const;


        void link_cluster( cluster* C );
            // makes C the parent cluster of this cell


        uint8_t value() const;


        void set_value( uint8_t Value );


        bool is_neighbor( const cell* C ) const;


        cluster* parent() const;


    };

}


#endif