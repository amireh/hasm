INCLUDE_DIRECTORIES(../include/operands ../include/instructions)
# add sources
SET(SRCS
    parser.cpp
    serializer.cpp
    control_section.cpp
    operand.cpp
    operand_factory.cpp
    operands/constant.cpp
    operands/expression.cpp
    operands/symbol.cpp
    program_block.cpp
    symbol_manager.cpp
    instruction.cpp
    instructions/fmt1_instruction.cpp
    instructions/fmt2_instruction.cpp
    instructions/fmt3_instruction.cpp
    instructions/fmt4_instruction.cpp
    instructions/directive.cpp
    instructions/literal.cpp
    instruction_factory.cpp
    main.cpp)

# the executable
ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
