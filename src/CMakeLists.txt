project(qt4-qt5-convert)

set(
  SOURCES
  main.cpp
  TypeMatchers.cpp
  ContextMatchers.cpp
  NamespaceResolver.cpp
  MethodMatcher.cpp
  CustomPrinter.cpp
  QtConvertAction.cpp
  QtConvertConsumer.cpp
  QtConvertVisitor.cpp
)

set(
  HEADERS
  TypeMatchers.h
  ContextMatchers.h
  NamespaceResolver.h
  MethodMatcher.h
  CustomPrinter.h
  QtConvertAction.h
  QtConvertConsumer.h
  QtConvertVisitor.h
)

add_clang_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} clangTooling)
