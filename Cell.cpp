#include "Cell.h"

namespace KenKen{

    cell::cell( uint8_t Pos, uint8_t Board_Size ){
        _parent     = nullptr;
        _position   = Pos;
        _value      = 0;
        _board_size = Board_Size;
    }


    uint8_t cell::row_num() const {
        return _position / _board_size;
    }


    uint8_t cell::col_num() const {
        return _position % _board_size;
    }


    uint8_t cell::pos() const{
        return _position;
    }


    void cell::link_cluster( cluster* C ){
        // makes C the parent cluster of this cell
        _parent = C;
    }


    uint8_t cell::value() const {
        return _value;
    }


    void cell::set_value( uint8_t Value ){
        _value = Value;
    }


    bool cell::is_neighbor( const cell* C ) const {
        return ( row_num() == C->row_num() )
            || ( col_num() == C->col_num() );
    }


    cluster* cell::parent() const {
        return _parent;
    }

}