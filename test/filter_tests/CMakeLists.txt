## App name
set( APP_NAME  filter_tests )

include( ${CMAKE_CURRENT_SOURCE_DIR}/../etc/Unity/Unity.cmake )

## Depends on libraries
list( APPEND  DEP_LIBS_XCORE  ""  )
list( APPEND  DEP_LIBS_Linux  m   )

list( APPEND  DEP_LIBS        
    lib_xs3_math 
    test_shared 
)

list( APPEND  DEP_LIBS ${DEP_LIBS_${CMAKE_SYSTEM_NAME}} )

## Sources
file( GLOB_RECURSE SOURCES_C  src/*.c )
file( GLOB_RECURSE SOURCES_XC src/*.xc )

list( APPEND  SOURCES         ${SOURCES_C}     )
list( APPEND  SOURCES_XCORE   ${SOURCES_XC}    )
list( APPEND  SOURCES   ${SOURCES_${CMAKE_SYSTEM_NAME}} )


# set( XSCOPE_CONFIG config.xscope )
get_filename_component(XSCOPE_CONFIG ../etc/config.xscope ABSOLUTE)

## Compile flags
unset(COMPILE_FLAGS)
list( APPEND  COMPILE_FLAGS   -Wno-unused-variable )

unset(COMPILE_FLAGS_XCORE)
list( APPEND  COMPILE_FLAGS_XCORE   "")

unset(COMPILE_FLAGS_Linux)
list( APPEND  COMPILE_FLAGS_Linux   "" )

list( APPEND  COMPILE_FLAGS ${COMPILE_FLAGS_${CMAKE_SYSTEM_NAME}} )

list(APPEND   COMPILE_FLAGS ${TEST_DEFS} )

## Linker flags
unset(LINKER_FLAGS)
list( APPEND  LINKER_FLAGS  "" )

unset(LINKER_FLAGS_XCORE)
list( APPEND  LINKER_FLAGS_XCORE  "-target=${XCORE_TARGET}"     )
list( APPEND  LINKER_FLAGS_XCORE  "-report"                     )
list( APPEND  LINKER_FLAGS_XCORE  "-fcmdline-buffer-bytes=1024" )
list( APPEND  LINKER_FLAGS_XCORE  "${XSCOPE_CONFIG}"            )

unset(LINKER_FLAGS_Linux)
list( APPEND  LINKER_FLAGS_Linux  ""                            )

list( APPEND  LINKER_FLAGS ${LINKER_FLAGS_${CMAKE_SYSTEM_NAME}} )

#########


if ( ${Python3_FOUND} )

  # Run the python scripts for filter generation. This is just to ensure that the script runs
  # correctly and produces code that compiles correctly.

  set( SCRIPT_DIR  ${CMAKE_SOURCE_DIR}/lib_xs3_math/script   )
  set( GEN_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/src/gen.local )
  set( COEF_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/etc           )

  list( APPEND SOURCES_C  ${CMAKE_CURRENT_SOURCE_DIR}/src/gen.local/test_fir_s32.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/src/gen.local/test_fir_s16.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/src/gen.local/test_biquad_s32.c )

  add_custom_command(
    OUTPUT ${GEN_DIR}/test_fir_s32.h ${GEN_DIR}/test_fir_s32.c
    COMMAND ${Python3_EXECUTABLE} ${SCRIPT_DIR}/gen_fir_filter_s32.py test_fir_s32 --out-dir ${GEN_DIR} ${COEF_DIR}/coefs.txt
    DEPENDS ${COEF_DIR}/coefs.txt
    COMMENT "Generating test_fir_s32 from script" )

  add_custom_command(
    OUTPUT ${GEN_DIR}/test_fir_s16.h ${GEN_DIR}/test_fir_s16.c
    COMMAND ${Python3_EXECUTABLE} ${SCRIPT_DIR}/gen_fir_filter_s16.py test_fir_16 --out-dir ${GEN_DIR} ${COEF_DIR}/coefs.txt
    DEPENDS ${COEF_DIR}/coefs.txt
    COMMENT "Generating test_fir_s16 from script" )

  add_custom_command(
    OUTPUT ${GEN_DIR}/test_biquad_s32.h ${GEN_DIR}/test_biquad_s32.c
    COMMAND ${Python3_EXECUTABLE} ${SCRIPT_DIR}/gen_biquad_filter_s32.py test_biquad_s32 --out-dir ${GEN_DIR} ${COEF_DIR}/biquad_coefs.csv
    DEPENDS ${COEF_DIR}/coefs.txt
    COMMENT "Generating test_biquad_s32 from script" )

endif()
#########

add_executable( ${APP_NAME} ${SOURCES} )

target_include_directories( ${APP_NAME} PRIVATE src )

target_link_libraries( ${APP_NAME} ${DEP_LIBS})

target_compile_options( ${APP_NAME} PRIVATE ${COMPILE_FLAGS} )

#(because otherwise the set_target_properties command fails)
string(REPLACE ";" " " LINKER_FLAGS_STR "${LINKER_FLAGS}")
set_target_properties( ${APP_NAME} PROPERTIES LINK_FLAGS "${LINKER_FLAGS_STR}" )

if ( XCORE )
  set_target_properties( ${APP_NAME} PROPERTIES   
      SUFFIX ".xe" 
      LINK_DEPENDS  ${XSCOPE_CONFIG}
  )
endif()

add_unity_to_target( ${APP_NAME} CONFIG)
