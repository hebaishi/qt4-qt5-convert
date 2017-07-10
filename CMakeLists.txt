project(qt4-qt5-convert)
set(SOURCES main.cpp NamespaceResolver.cpp)
set(HEADERS TypeMatchers.h NamespaceResolver.h)

add_clang_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} clangTooling)
