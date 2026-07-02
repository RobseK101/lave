#ifndef REN_STRINGTOOLS_HPP
#define REN_STRINGTOOLS_HPP

/*
MIT License

Copyright (c) 2026 Dr. R. Klenk (a.k.a. RobseK, RobseK101)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <string>
#include <vector>
#include <cctype>
#include <cwctype>
#include <cstdarg>
#include <utility>

namespace ren
{
    namespace st
    {
        // Templated version of strlen()
        template <typename Char> size_t length(const Char* string);

        // Get a vector of words separated by whitespace (or other characters specified).
        template <class Str> std::vector<Str> dissect(const Str& input, typename Str::const_pointer whitespaceList);

        // Get a vector of words separated by whitespace, respects sections in specified grouping symbols (like ", (), etc.) as monolithic.
        template <class Str> std::vector<Str> dissect(const Str& input, typename Str::const_pointer whitespaceList, typename Str::const_pointer groupingSymbols);

        // Creates a vector of trimmed "tokens" from a list separated by a specified separator.
        template <class Str> std::vector<Str> tokenize(const Str& input, typename Str::value_type separator, typename Str::const_pointer whitespaceList);
        template <class Str> std::vector<Str> tokenize(const Str& input, typename Str::const_pointer separatorList, typename Str::const_pointer whitespaceList);

        // Creates a key-value-pair defined at the first occurence of the specified separator.
        template <class Str> std::pair<Str, Str> splitKeyValuePair(const Str& input, typename Str::value_type separator, typename Str::const_pointer whitespaceList);

        // Trim both ends of the string.
        template <class Str> Str trim(const Str& input, typename Str::const_pointer trimmingsList);
        
        // Converts all alphabetic characters to uppercase.
        template <class Str> Str toUpper(const Str& input);

        // Counts all digits.
        size_t digitCount(const std::string& input);

        // Counts all digits.
        size_t digitCount(const std::wstring& input);

        // Counts all letters.
        size_t alphaCount(const std::string& input);

        // Counts all letters.
        size_t alphaCount(const std::wstring& input);

        // Ratio of digits in the string.
        template <class Str> double digitRatio(const Str& input);

        // Ratio of letter in the string.
        template <class Str> double alphaRatio(const Str& input);

        // Converts an ANSI string to a wide character string.
        std::wstring strToWStr(const std::string& input);

        // Converts a wide character string to an ANSI string, replacing out-of-bounds
        // characters with a filler symbol.
        std::string wstrToStr(const std::wstring& input, char filler = '?');

        // Total count of the characters specified in the list.
        template <class Str> size_t countOccurences(const Str& input, typename Str::const_pointer needles);

        // Total count of the character specified.
        template <class Str> size_t countOccurences(const Str& input, typename Str::value_type needle);

        // Checks if the string only contains digits.
        bool isPureNumeric(const std::string& input);

        // Checks if the string only contains digits.
        bool isPureNumeric(const std::wstring& input);

        // Checks if the string only contains the specified characters.
        template <class Str> bool isPure(const Str& input, typename Str::const_pointer allowed);

        // Compare two strings case-insensitively.
        int stricmp(const std::string& string1, const std::string& string2);
        
        // Compare two strings case-insensitively.
        int stricmp(const std::wstring& string1, const std::wstring& string2);

        size_t stristr(const std::string& haystack, const std::string& needle);

        size_t stristr(const std::wstring& haystack, const std::wstring& needle);

        enum class Base { dec, hex };

        template <typename Char, size_t _size>
        struct Strbuf
        {
            static constexpr size_t size = _size;

            Char data[_size] = {};

            constexpr operator const Char* () const noexcept { return data; }
            constexpr operator Char* () noexcept { return data; }
        };

        template <typename Char> 
        using Strbuf32 = Strbuf<Char, 32>;

        template <typename Char>
        using Strbuf64 = Strbuf<Char, 64>;

        // Creates a string from an int with specifiable zero-padding.
        template <class Str> Str toStr(int value, unsigned totalWidth = 1);
        // template <> std::string toStr<std::string>(unsigned value, unsigned precision);
        // template <> std::wstring toStr<std::wstring>(int value, unsigned precision);

        // Creates a string from an unsigned int with specifiable zero-padding.
        template <class Str> Str toStr(unsigned value, unsigned totalWidth = 1, Base base = Base::dec);
        // template <> std::string toStr<std::string>(unsigned value, unsigned precision);
        // template <> std::wstring toStr<std::wstring>(unsigned value, unsigned precision);

        // Creates a string from a long long with specifiable zero-padding.
        template <class Str> Str toStr(long long value, unsigned totalWidth = 1);
        // template <> std::string toStr<std::string>(long long value, unsigned totalWidth);
        // template <> std::wstring toStr<std::wstring>(long long value, unsigned totalWidth);

        // Creates a string from an unsigned long long with specifiable zero-padding.
        template <class Str> Str toStr(unsigned long long value, unsigned totalWidth = 1, Base base = Base::dec);
        // template <> std::string toStr<std::string>(unsigned long long value, unsigned totalWidth);
        // template <> std::wstring toStr<std::wstring>(unsigned long long value, unsigned totalWidth);

        // Creates a string from a float with specifiable zero-padding and precision.
        template <class Str> Str toStr(float value, unsigned totalWidth = 1, unsigned precision = 5);
        // template <> std::string toStr<std::string>(float value, unsigned precisionLeft, unsigned precisionRight);
        // template <> std::wstring toStr<std::wstring>(float value, unsigned precisionLeft, unsigned precisionRight);

        // Create a formatted string. 
        // Follows the conventions of sprintf(). The buffer for this operation is 1023 characters long. 
        template <class Str> Str compose(typename Str::const_pointer format, ...);
        // template <> std::string compose<std::string>(const char* format, ...);
        // template <> std::wstring compose<std::wstring>(const wchar_t* format, ...);

        template <> 
        inline size_t length(const char* string)
        {
            return std::strlen(string);
        }

        template <>
        inline size_t length(const wchar_t* string)
        {
            return std::wcslen(string);
        }

        template <class Str>
        inline std::vector<Str> dissect(const Str& input, typename Str::const_pointer whitespaceList)
        {
            if (whitespaceList == nullptr || length(whitespaceList) == 0) {
                std::vector<Str> trivialOutput;
                trivialOutput.push_back(input);
                return trivialOutput;
            }

            std::vector<Str> output;
            size_t currentPosition = 0;
            while (currentPosition < input.size()) {
                size_t firstGlyph = input.find_first_not_of(whitespaceList, currentPosition);
                if (firstGlyph == input.npos) {
                    break;
                }
                size_t firstNonGlyph = input.find_first_of(whitespaceList, firstGlyph);
                if (firstNonGlyph == input.npos) { // necessary???
                    firstNonGlyph = input.size();
                }
                output.push_back(Str(input, firstGlyph, firstNonGlyph - firstGlyph));
                currentPosition = firstNonGlyph;
            }
            return output;
        }

        template <class Str>
        inline std::vector<Str> dissect(const Str& input, typename Str::const_pointer whitespaceList, typename Str::const_pointer groupingSymbols)
        {
            typename Str::value_type groupers[3] = {};
            if (groupingSymbols == nullptr) {
                return dissect(input, whitespaceList);
            }
            size_t grouperLength = length(groupingSymbols);
            if (grouperLength == 0) {
                return dissect(input, whitespaceList);
            }
            if (whitespaceList == nullptr || length(whitespaceList) == 0) {
                std::vector<Str> trivialOutput;
                trivialOutput.push_back(input);
                return trivialOutput;
            }

            if (grouperLength < 2) {
                groupers[0] = groupingSymbols[0];
                groupers[1] = groupingSymbols[0];
            }
            else {
                groupers[0] = groupingSymbols[0];
                groupers[1] = groupingSymbols[1];
            }

            std::vector<Str> output;
            size_t currentPosition = 0;
            while (currentPosition < input.size()) {
                size_t firstGlyph = input.find_first_not_of(whitespaceList, currentPosition);
                if (firstGlyph == input.npos) {
                    break;
                }
                size_t firstNonGlyph = firstGlyph;
                if (input[firstGlyph] == groupers[0]) {
                    size_t closingGrouper = input.find_first_of(groupers[1], firstGlyph + 1);
                    if (closingGrouper != input.npos) {
                        firstNonGlyph = closingGrouper + 1;
                        output.push_back(Str(input, firstGlyph + 1, (closingGrouper - firstGlyph) - 1));
                        currentPosition = firstNonGlyph;
                        continue;
                    }
                }
                firstNonGlyph = input.find_first_of(whitespaceList, firstGlyph);
                if (firstNonGlyph == input.npos) { // necessary???
                    firstNonGlyph = input.size();
                }
                output.push_back(Str(input, firstGlyph, firstNonGlyph - firstGlyph));
                currentPosition = firstNonGlyph;
            }
            return output;
        }

        template <class Str> 
        std::vector<Str> tokenize(const Str& input, typename Str::value_type separator, typename Str::const_pointer whitespaceList)
        {
            std::vector<Str> tokens;
            size_t currentPosition = 0;
            while (currentPosition < input.size()) {
                size_t startTokenArea = currentPosition;
                size_t endTokenArea = input.find_first_of(separator, startTokenArea);
                if (endTokenArea == input.npos) {
                    endTokenArea = input.size();
                }
                currentPosition = endTokenArea + 1;

                if (whitespaceList == nullptr || *whitespaceList == 0) {
                    // -> do not trim anything
                    tokens.push_back(Str(input, startTokenArea, endTokenArea - startTokenArea));
                    continue;
                }
                
                size_t firstTokenGlyph = input.find_first_not_of(whitespaceList, startTokenArea);
                // empty token:
                if (firstTokenGlyph == input.npos || firstTokenGlyph == endTokenArea) {
                    tokens.push_back(Str());
                    continue;
                }
                size_t firstTokenNonGlyph = input.find_first_of(whitespaceList, firstTokenGlyph);
                if (firstTokenNonGlyph > endTokenArea || firstTokenNonGlyph == input.npos) {
                    firstTokenNonGlyph = endTokenArea;
                }
                tokens.push_back(Str(input, firstTokenGlyph, firstTokenNonGlyph - firstTokenGlyph));
            }
            // not handled in the loop:
            if (input[input.size() - 1] == separator) {
                tokens.push_back(Str());
            }
            return tokens;
        }

        template <class Str> 
        std::vector<Str> tokenize(const Str& input, typename Str::const_pointer separatorList, typename Str::const_pointer whitespaceList)
        {
            std::vector<Str> tokens;
            size_t currentPosition = 0;
            while (currentPosition < input.size()) {
                size_t startTokenArea = currentPosition;
                size_t endTokenArea = input.find_first_of(separatorList, startTokenArea);
                if (endTokenArea == input.npos) {
                    endTokenArea = input.size();
                }
                currentPosition = endTokenArea + 1;

                if (whitespaceList == nullptr || *whitespaceList == 0) {
                    // -> do not trim anything
                    tokens.push_back(Str(input, startTokenArea, endTokenArea - startTokenArea));
                    continue;
                }

                size_t firstTokenGlyph = input.find_first_not_of(whitespaceList, startTokenArea);
                // empty token:
                if (firstTokenGlyph == input.npos || firstTokenGlyph == endTokenArea) {
                    tokens.push_back(Str());
                    continue;
                }
                size_t firstTokenNonGlyph = input.find_first_of(whitespaceList, firstTokenGlyph);
                if (firstTokenNonGlyph > endTokenArea || firstTokenNonGlyph == input.npos) {
                    firstTokenNonGlyph = endTokenArea;
                }
                tokens.push_back(Str(input, firstTokenGlyph, firstTokenNonGlyph - firstTokenGlyph));
            }
            // not handled in the loop:
            if (Str(separatorList).find_first_of(input[input.size() - 1]) != Str::npos) {
                tokens.push_back(Str());
            }
            return tokens;
        }

        template <class Str> 
        std::pair<Str, Str> splitKeyValuePair(const Str& input, typename Str::value_type separator, typename Str::const_pointer whitespaceList)
        {
            std::pair<std::string, std::string> keyValuePair = {};
            size_t separatorPosition = input.find(separator);
            if (separatorPosition == input.npos) {
                keyValuePair.first = trim(input, whitespaceList);
            }
            else {
                keyValuePair.first = trim(Str(input, 0, separatorPosition), whitespaceList);
                keyValuePair.second = trim(Str(input, separatorPosition + 1, input.size() - (separatorPosition + 1)), whitespaceList);
            }
            return keyValuePair;
        }

        template <class Str>
        inline Str trim(const Str& input, typename Str::const_pointer trimmingsList)
        {
            if (trimmingsList == nullptr || length(trimmingsList) == 0) {
                return input;
            }

            size_t firstGlyph = input.find_first_not_of(trimmingsList);
            if (firstGlyph == input.npos) {
                return Str();
            }
            size_t lastGlyph = input.find_last_not_of(trimmingsList);
            return Str(input, firstGlyph, (lastGlyph - firstGlyph) + 1);
        }

        template<>
        inline std::string toUpper<std::string>(const std::string& input)
        {
            std::string output(input.size(), ' ');
            for (size_t i = 0; i < input.size(); i++) {
                output[i] = std::toupper(input[i]);
            }
            return output;
        }

        template<>
        inline std::wstring toUpper<std::wstring>(const std::wstring& input)
        {
            std::wstring output(input.size(), L' ');
            for (size_t i = 0; i < input.size(); i++) {
                output[i] = std::towupper(input[i]);
            }
            return output;
        }

        inline size_t digitCount(const std::string& input)
        {
            size_t digitCount = 0;
            for (size_t i = 0; i < input.size(); i++) {
                if (std::isdigit(input[i])) {
                    digitCount++;
                }
            }
            return digitCount;
        }

        inline size_t digitCount(const std::wstring& input) 
        {
            size_t digitCount = 0;
            for (size_t i = 0; i < input.size(); i++) {
                if (std::iswdigit(input[i])) {
                    digitCount++;
                }
            }
            return digitCount;
        }

        inline size_t alphaCount(const std::string& input)
        {
            size_t alphaCount = 0;
            for (size_t i = 0; i < input.size(); i++) {
                if (std::isalpha(input[i])) {
                    alphaCount++;
                }
            }
            return alphaCount;
        }

        inline size_t alphaCount(const std::wstring& input) 
        {
            size_t alphaCount = 0;
            for (size_t i = 0; i < input.size(); i++) {
                if (std::iswalpha(input[i])) {
                    alphaCount++;
                }
            }
            return alphaCount;
        }

        template <class Str>
        inline double digitRatio(const Str& input)
        {
            return (double)digitCount(input) / (double)input.size();
        }

        template <class Str>
        inline double alphaRatio(const Str& input)
        {
            return (double)alphaCount(input) / (double)input.size();
        }

        inline std::wstring strToWStr(const std::string& input)
        {
            std::wstring output(input.size(), L' ');
            for (size_t i = 0; i < input.size(); i++) {
                output[i] = input[i];
            }
            return output;
        }

        inline std::string wstrToStr(const std::wstring& input, char filler)
        {
            std::string output(input.size(), ' ');
            for (size_t i = 0; i < input.size(); i++) {
                output[i] = (input[i] < 128) ? static_cast<char>(input[i]) : filler;
            }
            return output;
        }

        template <class Str>
        inline size_t countOccurences(const Str& input, typename Str::const_pointer needles)
        {
            size_t occurences = 0;
            size_t lastOccurence = input.find_first_of(needles, 0);
            while (lastOccurence != input.npos) {
                occurences++;
                if (lastOccurence + 1 < input.size()) {
                    lastOccurence = input.find_first_of(needles, lastOccurence + 1);
                }
                else {
                    break;
                }
            }
            return occurences;
        }

        template <class Str>
        inline size_t countOccurences(const Str& input, typename Str::value_type needle)
        {
            size_t occurences = 0;
            size_t lastOccurence = input.find_first_of(needle, 0);
            while (lastOccurence != input.npos) {
                occurences++;
                if (lastOccurence + 1 < input.size()) {
                    lastOccurence = input.find_first_of(needle, lastOccurence + 1);
                }
                else {
                    break;
                }
            }
            return occurences;
        }

        inline bool isPureNumeric(const std::string& input)
        {
            for (size_t i = 0; i < input.size(); i++) {
                if (!std::isdigit(input[i])) {
                    return false;
                }
            }
            return true;
        }

        inline bool isPureNumeric(const std::wstring& input)
        {
            for (size_t i = 0; i < input.size(); i++) {
                if (!std::iswdigit(input[i])) {
                    return false;
                }
            }
            return true;
        }

        template <class Str>
        inline bool isPure(const Str& input, typename Str::const_pointer allowed)
        {
            return (input.find_first_not_of(allowed) == input.npos) ? true : false;
        }

        inline int stricmp(const std::string& string1, const std::string& string2) 
        {
            std::string string1Upper = toUpper(string1);
            std::string string2Upper = toUpper(string2);
            return std::strcmp(string1Upper.c_str(), string2Upper.c_str());
        }

        inline int stricmp(const std::wstring& string1, const std::wstring& string2) 
        {
            std::wstring string1Upper = toUpper(string1);
            std::wstring string2Upper = toUpper(string2);
            return std::wcscmp(string1Upper.c_str(), string2Upper.c_str());
        }

        inline size_t stristr(const std::string& haystack, const std::string& needle)
        {
            std::string haystackUpper = toUpper(haystack);
            std::string needleUpper = toUpper(needle);
            const char* result = std::strstr(haystackUpper.c_str(), needleUpper.c_str());
            if (result) {
                return result - haystackUpper.c_str();
            }
            else {
                return std::string::npos;
            }
        }

        inline size_t stristr(const std::wstring& haystack, const std::wstring& needle)
        {
            std::wstring haystackUpper = toUpper(haystack);
            std::wstring needleUpper = toUpper(needle);
            const wchar_t* result = std::wcsstr(haystackUpper.c_str(), needleUpper.c_str());
            if (result) {
                return result - haystackUpper.c_str();
            }
            else {
                return std::wstring::npos;
            }
        }

        template<>
        inline std::string toStr<std::string>(int value, unsigned totalWidth)
        {
            constexpr size_t bufferCount = 32;
            char buffer[bufferCount] = {};
            std::snprintf(buffer, bufferCount, "%0*d", totalWidth, value);
            return std::string(buffer);
        }

        template<>
        inline std::wstring toStr<std::wstring>(int value, unsigned totalWidth)
        {
            constexpr size_t bufferCount = 32;
            wchar_t buffer[bufferCount] = {};
            std::swprintf(buffer, bufferCount, L"%0*d", totalWidth, value);
            return std::wstring(buffer);
        }

        template<>
        inline std::string toStr<std::string>(unsigned value, unsigned totalWidth, Base base)
        {
            constexpr size_t bufferCount = 32;

            char buffer[bufferCount] = {};
            std::snprintf(buffer, bufferCount, (base == Base::hex) ? "0x%0*X" : "%0*u", totalWidth, value);
            return std::string(buffer);
        }

        template<>
        inline std::wstring toStr<std::wstring>(unsigned value, unsigned totalWidth, Base base)
        {
            constexpr size_t bufferCount = 32;
            wchar_t buffer[bufferCount] = {};
            std::swprintf(buffer, bufferCount, (base == Base::hex) ? L"0x%0*X" : L"%0*u", totalWidth, value);
            return std::wstring(buffer);
        }

        template <> 
        inline std::string toStr<std::string>(long long value, unsigned totalWidth)
        {
            constexpr size_t bufferCount = 64;
            char buffer[bufferCount] = {};
            std::snprintf(buffer, bufferCount, "%0*lld", totalWidth, value);
            return std::string(buffer);
        }

        template <>
        inline std::wstring toStr<std::wstring>(long long value, unsigned totalWidth)
        {
            constexpr size_t bufferCount = 64;
            wchar_t buffer[bufferCount] = {};
            std::swprintf(buffer, bufferCount, L"%0*lld", totalWidth, value);
            return std::wstring(buffer);
        }

        template <> 
        inline std::string toStr<std::string>(unsigned long long value, unsigned totalWidth, Base base)
        {
            constexpr size_t bufferCount = 64;
            char buffer[bufferCount] = {};
            std::snprintf(buffer, bufferCount, (base == Base::hex) ? "0x%0*llX" : "%0*llu", totalWidth, value);
            return std::string(buffer);
        }

        template <> 
        inline std::wstring toStr<std::wstring>(unsigned long long value, unsigned totalWidth, Base base)
        {
            constexpr size_t bufferCount = 64;
            wchar_t buffer[bufferCount] = {};
            std::swprintf(buffer, bufferCount, (base == Base::hex) ? L"0x%0*llX" : L"%0*llu", totalWidth, value);
            return std::wstring(buffer);
        }

        template <> 
        inline std::string toStr<std::string>(float value, unsigned totalWidth, unsigned precision)
        {
            constexpr size_t bufferCount = 48;
            char buffer[bufferCount] = {};
            std::snprintf(buffer, bufferCount, "%0*.*f", totalWidth, precision, value);
            return std::string(buffer);
        }

        template <> 
        inline std::wstring toStr<std::wstring>(float value, unsigned totalWidth, unsigned precision)
        {
            constexpr size_t bufferCount = 48;
            wchar_t buffer[bufferCount] = {};
            std::swprintf(buffer, bufferCount, L"%0*.*f", totalWidth, precision, value);
            return std::wstring(buffer);
        }

        template<>
        inline std::string compose<std::string>(const char* format, ...)
        {
            constexpr size_t bufferCount = 1024;

            va_list args;
            va_start(args, format);
            char buffer[bufferCount] {};
            std::vsnprintf(buffer, bufferCount, format, args);
            va_end(args);

            return std::string(buffer);
        }

        template<>
        inline std::wstring compose<std::wstring>(const wchar_t* format, ...)
        {
            constexpr size_t bufferCount = 1024;

            va_list args;
            va_start(args, format);
            wchar_t buffer[bufferCount] {};
            std::vswprintf(buffer, bufferCount, format, args);
            va_end(args);

            return std::wstring(buffer);
        }
    }
}

#endif