INCLUDE_DIRECTORIES(../include/operands ../include/instructions)
# add sources
SET(SRCS
    parser.cpp
    serializer.cpp
    operand.cpp
    operands/constant.cpp
    operands/symbol.cpp
    symbol_manager.cpp
    instruction.cpp
    instructions/fmt1_instruction.cpp
    instructions/fmt2_instruction.cpp
    instructions/fmt3_instruction.cpp
    instructions/fmt4_instruction.cpp
    instructions/directive.cpp
    instruction_factory.cpp
    main.cpp)

# the executable
ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
