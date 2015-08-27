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


        cell( uint8_t Pos, uint8_t Board_Size ){
            _parent     = nullptr;
            _position   = Pos;
            _value      = 0;
            _board_size = Board_Size;
        }


        inline uint8_t row_num() const {
            return _position / _board_size;
        }


        inline uint8_t col_num() const {
            return _position % _board_size;
        }


        inline uint8_t pos() const{
            return _position;
        }


        inline void link_cluster( cluster* C ){
            // makes C the parent cluster of this cell
            _parent = C;
        }


        inline uint8_t value() const {
            return _value;
        }


        inline void set_value( uint8_t Value ){
            _value = Value;
        }


        inline bool is_neighbor( const cell* C ) const {
            return ( row_num() == C->row_num() )
                || ( col_num() == C->col_num() );
        }


        inline cluster* parent() const {
            return _parent;
        }


    };

}


#endif