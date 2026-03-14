#pragma once 
#include <cwctype>
#include "antlr4-runtime.h"

class CaseCaptilizeInputStream : public antlr4::ANTLRInputStream {
public:
    // Re-use all constructors from the base ANTLRInputStream
    using antlr4::ANTLRInputStream::ANTLRInputStream;

    /*
    - Overrides the Original Look-Ahead (LA) method.
    - The Lexer calls this method of INPUT STREAM to "see" characters.
    - The overridden method LA() returns similarrly just captilize each character.
      Making Language Case Independent.
    */ 
    size_t LA(ssize_t i) override {
        const size_t character = antlr4::ANTLRInputStream::LA(i);

        // End Of File or null is not captilized just returned immeditely
        // Tells LEXER the end of Input Stream
        if (character == antlr4::IntStream::EOF || character == 0) {
            return character;
        }

        // Convert the looked-ahead character to uppercase (CAPTILIZED).
        return static_cast<size_t>(std::toupper(static_cast<int>(character)));
    }
};