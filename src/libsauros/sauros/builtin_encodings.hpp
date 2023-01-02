#ifndef SAUROS_BUILTIN_ENCODINGS_HPP
#define SAUROS_BUILTIN_ENCODINGS_HPP

#include <cstdint>
#include <string>
#include <unordered_map>

// Built in items are stored in an indexed list to avoid
// having to hash their string representations and search them
// in a map every time they are called

namespace sauros {
constexpr uint8_t BUILTIN_DEFAULT_VAL = 0xFF;
constexpr uint8_t BUILTIN_ENTRY_COUNT = 60;

constexpr uint8_t BUILTIN_FRONT = 0;
constexpr uint8_t BUILTIN_BACK = 1;
constexpr uint8_t BUILTIN_PUSH = 2;
constexpr uint8_t BUILTIN_POP = 3;
constexpr uint8_t BUILTIN_LIST = 4;
constexpr uint8_t BUILTIN_SET = 5;
constexpr uint8_t BUILTIN_LAMBDA = 6;
constexpr uint8_t BUILTIN_BLOCK = 7;
constexpr uint8_t BUILTIN_LEN = 8;
constexpr uint8_t BUILTIN_PUT = 9;
constexpr uint8_t BUILTIN_PUTLN = 10;
constexpr uint8_t BUILTIN_IF = 11;
constexpr uint8_t BUILTIN_EQ_EQ = 12;
constexpr uint8_t BUILTIN_NOT_EQ = 13;
constexpr uint8_t BUILTIN_LT_EQ = 14;
constexpr uint8_t BUILTIN_GT_EQ = 15;
constexpr uint8_t BUILTIN_LT = 16;
constexpr uint8_t BUILTIN_GT = 17;
constexpr uint8_t BUILTIN_SEQ = 18;
constexpr uint8_t BUILTIN_SNEQ = 19;
constexpr uint8_t BUILTIN_ASSERT = 20;
constexpr uint8_t BUILTIN_LOOP = 21;
constexpr uint8_t BUILTIN_TYPE = 22;
constexpr uint8_t BUILTIN_IMPORT = 23;
constexpr uint8_t BUILTIN_USE = 24;
constexpr uint8_t BUILTIN_ITER = 25;
constexpr uint8_t BUILTIN_NOT = 26;
constexpr uint8_t BUILTIN_OR = 27;
constexpr uint8_t BUILTIN_AND = 28;
constexpr uint8_t BUILTIN_XOR = 29;
constexpr uint8_t BUILTIN_BREAK = 30;
constexpr uint8_t BUILTIN_AT = 31;
constexpr uint8_t BUILTIN_CLEAR = 32;
constexpr uint8_t BUILTIN_COMPOSE = 33;
constexpr uint8_t BUILTIN_DECOMPOSE = 34;
constexpr uint8_t BUILTIN_BOX = 35;
constexpr uint8_t BUILTIN_TRUE = 36;
constexpr uint8_t BUILTIN_FALSE = 37;
constexpr uint8_t BUILTIN_IS_NIL = 38;
constexpr uint8_t BUILTIN_NIL = 39;
constexpr uint8_t BUILTIN_ADD = 40;
constexpr uint8_t BUILTIN_SUB = 41;
constexpr uint8_t BUILTIN_DIV = 42;
constexpr uint8_t BUILTIN_MUL = 43;
constexpr uint8_t BUILTIN_MOD = 44;
constexpr uint8_t BUILTIN_VAR = 45;
constexpr uint8_t BUILTIN_EXIT = 46;
constexpr uint8_t BUILTIN_YIELD = 47;
constexpr uint8_t BUILTIN_TRY = 48;
constexpr uint8_t BUILTIN_AS_INT = 49;
constexpr uint8_t BUILTIN_AS_STR = 50;
constexpr uint8_t BUILTIN_AS_REAL = 51;
constexpr uint8_t BUILTIN_THROW = 52;
constexpr uint8_t BUILTIN_BITWISE_AND = 53;
constexpr uint8_t BUILTIN_BITWISE_OR = 54;
constexpr uint8_t BUILTIN_BITWISE_LSH = 55;
constexpr uint8_t BUILTIN_BITWISE_RSH = 56;
constexpr uint8_t BUILTIN_BITWISE_XOR = 57;
constexpr uint8_t BUILTIN_BITWISE_NOT = 58;
constexpr uint8_t BUILTIN_REV = 59;

// A map that ties a string representation ot the encoding
// that the parser can leverage to ensure that all translations
// and map accesses for these functions are done at parse time
// rather than execution time
static std::unordered_map<std::string, uint8_t> BUILTIN_STRING_TO_ENCODING{
    {"front", BUILTIN_FRONT},
    {"back", BUILTIN_BACK},
    {"push", BUILTIN_PUSH},
    {"pop", BUILTIN_POP},
    {"list", BUILTIN_LIST},
    {"set", BUILTIN_SET},
    {"lambda", BUILTIN_LAMBDA},
    {"block", BUILTIN_BLOCK},
    {"len", BUILTIN_LEN},
    {"put", BUILTIN_PUT},
    {"putln", BUILTIN_PUTLN},
    {"if", BUILTIN_IF},
    {"==", BUILTIN_EQ_EQ},
    {"!=", BUILTIN_NOT_EQ},
    {"<=", BUILTIN_LT_EQ},
    {">=", BUILTIN_GT_EQ},
    {"<", BUILTIN_LT},
    {">", BUILTIN_GT},
    {"seq", BUILTIN_SEQ},
    {"sneq", BUILTIN_SNEQ},
    {"assert", BUILTIN_ASSERT},
    {"loop", BUILTIN_LOOP},
    {"type", BUILTIN_TYPE},
    {"import", BUILTIN_IMPORT},
    {"use", BUILTIN_USE},
    {"not", BUILTIN_NOT},
    {"or", BUILTIN_OR},
    {"and", BUILTIN_AND},
    {"break", BUILTIN_BREAK},
    {"at", BUILTIN_AT},
    {"clear", BUILTIN_CLEAR},
    {"compose", BUILTIN_COMPOSE},
    {"decompose", BUILTIN_DECOMPOSE},
    {"box", BUILTIN_BOX},
    {"true", BUILTIN_TRUE},
    {"false", BUILTIN_FALSE},
    {"is_nil", BUILTIN_IS_NIL},
    {"nil", BUILTIN_NIL},
    {"var", BUILTIN_VAR},
    {"+", BUILTIN_ADD},
    {"-", BUILTIN_SUB},
    {"/", BUILTIN_DIV},
    {"*", BUILTIN_MUL},
    {"%", BUILTIN_MOD},
    {"exit", BUILTIN_EXIT},
    {"iter", BUILTIN_ITER},
    {"yield", BUILTIN_YIELD},
    {"try", BUILTIN_TRY},
    {"as_int", BUILTIN_AS_INT},
    {"as_str", BUILTIN_AS_STR},
    {"as_real", BUILTIN_AS_REAL},
    {"throw", BUILTIN_THROW},
    {"xor", BUILTIN_XOR},
    {"bw_and", BUILTIN_BITWISE_AND},
    {"bw_or", BUILTIN_BITWISE_OR},
    {"bw_lsh", BUILTIN_BITWISE_LSH},
    {"bw_rsh", BUILTIN_BITWISE_RSH},
    {"bw_xor", BUILTIN_BITWISE_XOR},
    {"bw_not", BUILTIN_BITWISE_NOT},
    {"rev", BUILTIN_REV}};
} // namespace sauros

#endif