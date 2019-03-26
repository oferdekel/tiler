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
    // Stream mutator for indented new-line (use like std::endl)
    std::ostream& endil(std::ostream& stream);

    // Increases the global indent value for indented printing
    void IncreaseIndent();

    // Decreases the global indent value for indented printing
    void DecreaseIndent();

    // Formatted printing (like printf)
    template <typename ArgType, typename... ArgTypes>
    void PrintFormated(std::ostream& os, const char* format, const ArgType& arg, const ArgTypes&... args);

    // Substitution symbol for PrintFormat calls
    const char substitutionSymbol = '%';

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

    // internal
    void PrintFormated(std::ostream& os, const char* format);
}
