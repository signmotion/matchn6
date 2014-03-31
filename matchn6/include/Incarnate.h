#pragma once

#include "LIncarnate.h"
#include "VIncarnate.h"


namespace matchn {


class Incarnate :
    public std::enable_shared_from_this< Incarnate >,
    public LIncarnate,
    public VIncarnate
{
protected:
    Incarnate(
        std::shared_ptr< World >,
        const std::string&  name,
        const typelib::coord2Int_t&  logicCoord,
        int  visualOrder
    );




public:
    virtual ~Incarnate();




    /**
    * @return Имя воплощённого элемента.
    */
    inline std::string const& name() const {
        return mName;
    }




private:
    /**
    * Имя воплощённого элемента.
    */
    const std::string  mName;
};


} // matchn








namespace std {

std::ostream& operator<<( std::ostream&,  const matchn::Incarnate& );

} // std
