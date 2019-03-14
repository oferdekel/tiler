////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     IndentedOutputStream.h
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <memory>
#include <ostream>

namespace tiler
{
    // Stuct used to simulate std::endl
    struct Endl {};
    static Endl endl;

    // A class that behaves like an output stream with persistent indentation
    class IndentedOutputStream
    {
    public:
        IndentedOutputStream(std::ostream& stream);

        // Casting operator that returns a reference to an ostream. This allows us to use IndentedOutputStream in most places where an ostream would be accepted.
        operator std::ostream&() & noexcept { return _stream; }

        // Casting operator that returns a reference to an ostream. This allows us to use IndentedOutputStream in most places where an ostream would be accepted.
        operator std::ostream const&() const& noexcept { return _stream; }

        IndentedOutputStream& operator<<(Endl);

        // Output operator that sends the given value to the output stream.
        template <typename T>
        IndentedOutputStream& operator<<(T&& value);

        // Returns the current level of indentation 
        size_t GetIndent() { return _indent; }

        // Increased the level of indentation
        void IncreaseIndent() { ++_indent; }

        // Decreased the level of indentation
        void DecreaseIndent() { if(_indent>0) --_indent; }

    private:
        std::reference_wrapper<std::ostream> _stream;
        size_t _indent = 0;
        bool _lineBeginning = true;
    };

    //
    //
    //

    template <typename T>
    IndentedOutputStream& IndentedOutputStream::operator<<(T&& value)
    {
        if(_lineBeginning)
        {
            _stream.get() << std::string(_indent * 4, ' ');
        }

        _lineBeginning = false;
        _stream.get() << value;
        return *this;
    }
}