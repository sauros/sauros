link_directories(${CPPUTEST_LIBRARIES})

include_directories(
  ${CPPUTEST_INCLUDE_DIRS}
  ../
)

add_executable(sauros_unit_tests
        sauros_tests.cpp
        ${SAUROS_SOURCES}
        main.cpp)

target_link_libraries(sauros_unit_tests
  ${CPPUTEST_LDFLAGS}
  dl
)

add_custom_command(TARGET sauros_unit_tests COMMAND ./sauros_unit_tests POST_BUILD)