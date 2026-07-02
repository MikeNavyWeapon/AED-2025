cmake_minimum_required(VERSION 3.16)
project(ctikg LANGUAGES CXX)

# ----------------------------------------------------------------- linguagem
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# --------------------------------------------------------------- dependências
# SQLite (Nível 2). O SQLite vem instalado com o MSYS2, ao lado do compilador.
# Procuramo-lo junto do g++ e ligamos a versão estática (evita depender de DLL
# em runtime). Se não estiver lá, recorre-se ao módulo FindSQLite3 do CMake.
get_filename_component(_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
get_filename_component(_mingw_root "${_compiler_dir}" DIRECTORY)
set(_sqlite_static "${_mingw_root}/lib/libsqlite3.a")

if(EXISTS "${_sqlite_static}" AND EXISTS "${_mingw_root}/include/sqlite3.h")
  add_library(sqlite3_local STATIC IMPORTED)
  set_target_properties(sqlite3_local PROPERTIES
    IMPORTED_LOCATION "${_sqlite_static}"
    INTERFACE_INCLUDE_DIRECTORIES "${_mingw_root}/include")
  set(SQLITE_TARGET sqlite3_local)
  message(STATUS "SQLite (estático) encontrado: ${_sqlite_static}")
else()
  find_package(SQLite3 REQUIRED)
  set(SQLITE_TARGET SQLite3::SQLite3)
endif()

# ------------------------------------------------------ biblioteca do grafo
# Toda a lógica vive numa biblioteca reutilizada pela app de consola e pelos
# testes. Nos próximos níveis (API) acrescentam-se aqui mais ficheiros.
add_library(ctikg_lib
  src/Schema.cpp
  src/Graph.cpp
  src/SyntheticData.cpp
  src/Database.cpp
  src/Analytics.cpp
  src/Json.cpp
  src/GraphService.cpp
)
target_include_directories(ctikg_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(ctikg_lib PUBLIC ${SQLITE_TARGET})
target_compile_options(ctikg_lib PRIVATE -Wall -Wextra)

# ----------------------------------------------- aplicação de demonstração
add_executable(ctikg src/main.cpp)
target_link_libraries(ctikg PRIVATE ctikg_lib)

# ---------------------------------------------------------- API REST (Nível 4)
# As rotas vivem numa pequena biblioteca, partilhada pelo servidor e pelos
# testes de integração. Usa cpp-httplib (header único em third_party) + winsock.
add_library(ctikg_api STATIC src/ApiRoutes.cpp)
target_include_directories(ctikg_api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
target_link_libraries(ctikg_api PUBLIC ctikg_lib ws2_32)
# cpp-httplib é enorme; com informação de depuração completa (-g) os objetos
# ficam gigantes e o linker do MinGW pode esgotar memória. -g0 nos alvos que
# incluem o httplib reduz drasticamente esse risco (não se depura dentro da lib).
target_compile_options(ctikg_api PRIVATE -g0)

add_executable(ctikg_api_server src/api_main.cpp)
target_link_libraries(ctikg_api_server PRIVATE ctikg_api)
target_compile_options(ctikg_api_server PRIVATE -g0)
# Caminho absoluto da interface web, para o servidor a encontrar a partir de
# qualquer diretório de trabalho.
target_compile_definitions(ctikg_api_server
  PRIVATE CTIKG_WEB_DIR="${CMAKE_CURRENT_SOURCE_DIR}/web")

# ------------------------------------------------------------------- testes
enable_testing()

add_executable(ctikg_tests tests/test_graph.cpp)
target_link_libraries(ctikg_tests PRIVATE ctikg_lib)
add_test(NAME ctikg_tests COMMAND ctikg_tests)

add_executable(ctikg_tests_persistence tests/test_persistence.cpp)
target_link_libraries(ctikg_tests_persistence PRIVATE ctikg_lib)
add_test(NAME ctikg_tests_persistence COMMAND ctikg_tests_persistence)

add_executable(ctikg_tests_algorithms tests/test_algorithms.cpp)
target_link_libraries(ctikg_tests_algorithms PRIVATE ctikg_lib)
add_test(NAME ctikg_tests_algorithms COMMAND ctikg_tests_algorithms)

add_executable(ctikg_tests_json tests/test_json.cpp)
target_link_libraries(ctikg_tests_json PRIVATE ctikg_lib)
add_test(NAME ctikg_tests_json COMMAND ctikg_tests_json)

add_executable(ctikg_tests_service tests/test_service.cpp)
target_link_libraries(ctikg_tests_service PRIVATE ctikg_lib)
add_test(NAME ctikg_tests_service COMMAND ctikg_tests_service)

add_executable(ctikg_tests_api tests/test_api.cpp)
target_link_libraries(ctikg_tests_api PRIVATE ctikg_api)
target_compile_options(ctikg_tests_api PRIVATE -g0)
add_test(NAME ctikg_tests_api COMMAND ctikg_tests_api)
