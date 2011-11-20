# add sources
SET(SRCS instruction.cpp parser.cpp symbol.cpp symbol_manager.cpp main.cpp)

# the executable
ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
