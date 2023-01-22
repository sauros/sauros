
#include "sauros.hpp"

#include <iostream>
#include <string>
#include <vector>

#include <CppUTest/TestHarness.h>

TEST_GROUP(sauros_tests){};

TEST(sauros_tests, piecemeal) {

   struct test_case_t {
      std::string input;
      std::string expected_output;
   };

   std::vector<test_case_t> tests = {
       {"[+ 3 4]", "7"},
       {"[- 3 4]", "-1"},
       {"[+ -1 1]", "0"},
       {"[- -1 -1]", "0"},
       {"[+ -1 -1]", "-2"},
       {"[* 3 4 2]", "24"},
       {"[/ 10 5]", "2.000000"},
       {"[% 10 9]", "1"},
       {"[+ [* 2 100] [* 1 10]]", "210"},
       {"[var x 3]", "3"},
       {"[var y [+ x 4]]", "7"},
       {"[var z 10]", "10"},
       {"[x]", "3"},
       {"[y]", "7"},
       {"[z]", "10"},
       {"[+ x y z]", "20"},
       {"[var new_list [list x y z]", "[3 7 10]"},
       {"[new_list]", "[3 7 10]"},
       {"[var l [lambda [p] [p]]", "<lambda>"},
       {"[l z]", "10"},
       {"[l x]", "3"},
       {"[l 420]", "420"},
       {"[var g [lambda [a b c] [+ a b c]]", "<lambda>"},
       {"[g 10 3 200]", "213"},
       {"[set x 4.5]", "4.5"},
       {"[block [var item 1] [set item [+ item 1]] [+ item 1]]", "3"},
       {"[if [0] [1] [0] ]", "0"},
       {"[if [1] [1] [0] ]", "1"},
       {"[if [< 1 2] [1] [0] ]", "1"},
       {"[if [< 4 2] [1] [0] ]", "0"},
       {"[if [> 10 2] [1] [0] ]", "1"},
       {"[if [> 2 12] [1] [0] ]", "0"},
       {"[if [>= 2 2] [1] [0] ]", "1"},
       {"[if [>= 3 2] [1] [0] ]", "1"},
       {"[if [>= 2 3] [1] [0] ]", "0"},
       {"[if [<= 2 2] [1] [0] ]", "1"},
       {"[if [<= 2 3] [1] [0] ]", "1"},
       {"[if [<= 4 3] [1] [0] ]", "0"},
       {"[if [== 4 4] [1] [0] ]", "1"},
       {"[if [== 4 3] [1] [0] ]", "0"},
       {"[if [!= 4 4] [1] [0] ]", "0"},
       {"[if [!= 4 3] [1] [0] ]", "1"},
       {"[if [!= 4 3] [1] ]", "1"},
       {"[var this \"A String\"", "A String"},
       {"[var that \"A Longer String\"", "A Longer String"},
       {"[if [seq this \"A String\"] [1] [0] ]", "1"},
       {"[if [seq this this] [1] [0] ]", "1"},
       {"[if [seq this that] [1] [0] ]", "0"},
       {"[if [sneq this this] [1] [0] ]", "0"},
       {"[if [sneq this that] [1] [0] ]", "1"},
       {"[if [sneq this \"A Longer String\"] [1] [0] ]", "1"},
       {"[front [list 1 2 3]]", "1"},
       {"[block [var shark [list 1 2 3]] [at 1 shark]]", "2"},
       {"[block [clear shark] [shark]]", "[]"},
       {"[block [var target] [push target 1] [push target 2] [push target 3] "
        "[target] ]",
        "[1 2 3]"},
       {"[block [pop target] [target]]", "[1 2]"},
       {"[back [list 1 2 3]]", "3"},
       {"[len [list 1 2 3]]", "3"},
       {"[front [list [list 5 6 7] 2 3]]", "[5 6 7]"},
       {"[block [var q [list 1 2 3]] [var r 3.4] [list q r]]", "[[1 2 3] 3.4]"},
       {"[block [var monkey [list 1]] [pop monkey] [len monkey]]", "0"},
       {"[var is_true 1]", "1"},
       {"[var is_false 0]", "0"},
       {"[not 1]", "0"},
       {"[not 0]", "1"},
       {"[not is_true]", "0"},
       {"[not is_false]", "1"},
       {"[assert \"is true\" is_true]", "1"},
       {"[assert \"not is_false\" [not is_false]]", "1"},
       {"[assert \"Direct string\" \"Any non empty string should produce a "
        "1\"]",
        "1"},
       {"[assert \"a integer\" 420]", "1"},
       {"[assert \"a double\" 2.1828]", "1"},
       {"[block [var x 1] [loop [] [< x 10] [] [[block [set x [+ x 1]] ]]] "
        "[x]]",
        "10"},
       {"[block [var x \"A string!\"] [type x] ]", "string"},
       {"[block [var x 42] [type x] ]", "integer"},
       {"[block [var x 2.1828] [type x] ]", "real"},
       {"[block [var x [list 1 2 3]] [type x] ]", "list"},
       {"[block [var x [lambda [x] [x]] ] [type x] ]", "lambda"},
       {"[block [var lhs 1] [var rhs 0] [if [or lhs rhs] [1] [0] ]]", "1"},
       {"[block [var lhs 0] [var rhs 1] [if [or lhs rhs] [1] [0] ]]", "1"},
       {"[block [var lhs 1] [var rhs 1] [if [or lhs rhs] [1] [0] ]]", "1"},
       {"[block [var lhs 0] [var rhs 0] [if [or lhs rhs] [1] [0] ]]", "0"},
       {"[block [var lhs 1] [var rhs 0] [if [and lhs rhs] [1] [0] ]]", "0"},
       {"[block [var lhs 0] [var rhs 1] [if [and lhs rhs] [1] [0] ]]", "0"},
       {"[block [var lhs 1] [var rhs 1] [if [and lhs rhs] [1] [0] ]]", "1"},
       {"[block [var lhs 0] [var rhs 0] [if [and lhs rhs] [1] [0] ]]", "0"},
       {"[block [var lhs 1] [var rhs 0] [if [xor lhs rhs] [1] [0] ]]", "1"},
       {"[block [var lhs 0] [var rhs 1] [if [xor lhs rhs] [1] [0] ]]", "1"},
       {"[block [var lhs 1] [var rhs 1] [if [xor lhs rhs] [1] [0] ]]", "0"},
       {"[block [var lhs 0] [var rhs 0] [if [xor lhs rhs] [1] [0] ]]", "0"},
   };

   size_t line_no = 0;
   std::shared_ptr<sauros::environment_c> env =
       std::make_shared<sauros::environment_c>();
   sauros::processor_c proc;

   for (auto tc : tests) {
      auto result = sauros::parser::parse_line("test", line_no++, tc.input);
      try {
         auto cell_result = proc.process_cell(result, env);

         std::string stringed_result;
         proc.cell_to_string(stringed_result, cell_result, env, false);

         //   CHECK_EQUAL(tc.expected_output, stringed_result);
      } catch (sauros::processor_c::runtime_exception_c &e) {
         std::cout << e.what() << std::endl;
         FAIL("exception");
      } catch (sauros::processor_c::assertion_exception_c &e) {
         std::cout << e.what() << std::endl;
         FAIL("exception");
      } catch (sauros::environment_c::unknown_identifier_c &e) {
         std::cout << e.what() << " : " << e.get_id() << std::endl;
         FAIL("exception");
      } catch (sauros::parser::parser_exception_c &e) {
         std::cout << e.what() << std::endl;
         FAIL("exception");
      }
   }
}