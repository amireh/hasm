# add sources
SET(SRCS
    parser.cpp
    symbol.cpp
    symbol_manager.cpp
    instruction.cpp
    fmt1_instruction.cpp
    fmt2_instruction.cpp
    fmt3_instruction.cpp
    fmt4_instruction.cpp
    directive.cpp
    instruction_factory.cpp
    main.cpp)

# the executable
ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
