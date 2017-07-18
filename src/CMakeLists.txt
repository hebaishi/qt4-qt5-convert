project(qt4-qt5-convert)

set(
  SOURCES
  main.cpp
  TypeMatchers.cpp
  ContextMatchers.cpp
  NamespaceResolver.cpp
  FunctionMatcher.cpp
  CustomPrinter.cpp
)

set(
  HEADERS
  TypeMatchers.h
  ContextMatchers.h
  NamespaceResolver.h
  FunctionMatcher.h
  CustomPrinter.h
)

add_clang_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} clangTooling)
