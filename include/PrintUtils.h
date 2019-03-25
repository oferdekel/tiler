////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  tiler
//  File:     PrintUtils.h
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <memory>
#include <ostream>
#include <string>

namespace tiler
{
    std::ostream& endil(std::ostream& stream);

    void IncreaseIndent();

    void DecreaseIndent();

    const char substitutionSymbol = '%';

    template <typename ArgType, typename... ArgTypes>
    void PrintFormated(std::ostream& os, const char* format, const ArgType& arg, const ArgTypes&... args);

    void PrintFormated(std::ostream& os, const char* format);

    //
    //
    //

    template <typename ArgType, typename... ArgTypes>
    void PrintFormated(std::ostream& os, const char* format, const ArgType& arg, const ArgTypes&... args)
    {
        if (*format == '\0')
        {
            return;
        }

        while (*format != substitutionSymbol && *format != '\0')
        {
            os << *format;
            ++format;
        }

        if (*format == substitutionSymbol)
        {
            ++format;
            os << arg;
        }

        PrintFormated(os, format, args...);
    }
}
