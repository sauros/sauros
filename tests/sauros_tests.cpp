
#include "sauros/list.hpp"
#include "sauros/front/parser.hpp"
#include "sauros/environment.hpp"
#include "sauros/processor/processor.hpp"

#include <string>
#include <vector>

#include <CppUTest/TestHarness.h>

TEST_GROUP(sauros_tests){};

TEST(sauros_tests, all) {
 
   struct test_case_t {
      std::string input;
      std::string expected_output;
   };


   std::vector<test_case_t> tests = {
      {"[+ 3 4]", "7"},
      {"[- 3 4]", "-1"},
      {"[* 3 4 2]", "24"},
      {"[/ 10 5]", "2.000000"}
   };

   
   size_t line_no = 0;
   std::shared_ptr<sauros::environment_c> env = std::make_shared<sauros::environment_c>();
   sauros::processor_c proc;

   for (auto tc : tests) {

      auto result = sauros::parser::parse_line(env, "test", line_no++, tc.input);

      CHECK_FALSE(result.error_info);

      auto proc_result = proc.process(result.cell, env);

      CHECK_TRUE(proc_result.returned_value.has_value());

      std::string stringed_result;
      proc.cell_to_string(stringed_result, (*proc_result.returned_value), env, false);

      CHECK_EQUAL(tc.expected_output, stringed_result);
   }

}