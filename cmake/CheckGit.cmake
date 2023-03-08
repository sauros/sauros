#
# Obtain Git hash
#
execute_process(
   COMMAND git log -1 --format=%h
   WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
   OUTPUT_VARIABLE GIT_HASH
   OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_custom_target(AlwaysCheckGit COMMAND ${CMAKE_COMMAND}
   -DRUN_CHECK_GIT_VERSION=1
   -Dpre_configure_dir=${pre_configure_dir}
   -Dpost_configure_file=${post_configure_dir}
   -DGIT_HASH_CACHE=${GIT_HASH_CACHE}
   -P ${CURRENT_LIST_DIR}/CheckGit.cmake
   BYPRODUCTS ${post_configure_file}
)

# This command is used to save git commit hash.
file(WRITE ${CMAKE_BINARY_DIR}/git-state.txt ${GIT_HASH})
# This following command is used to retreive the git commit hash from the file.
if (EXISTS ${CMAKE_BINARY_DIR}/git-state.txt)
   file(STRINGS ${CMAKE_BINARY_DIR}/git-state.txt CONTENT)
   LIST(GET CONTENT 0 var)
   set(build_hash "\"${var}\"")
   add_compile_definitions(COMPILED_GIT_HASH=${build_hash})
endif ()