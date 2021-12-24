include_directories(${CMAKE_CURRENT_SOURCE_DIR}/contrib/include common .)

add_library(common common/log.c common/log.h common/util.h)

add_library(disassemble
        cpu/disassemble.c cpu/disassemble.h)

add_library(core
        mem/addresses.h
        mem/bus.c mem/bus.h
        mem/ps1system.c mem/ps1system.h
        cpu/cpu.c cpu/cpu.h cpu/cpu_register_access.h
        cpu/mips_instructions.c cpu/mips_instructions.h
        cpu/mips_instruction_decode.h
        mem/mem_util.h
        mem/dma.c mem/dma.h)

TARGET_LINK_LIBRARIES(core disassemble)
if (NOT WIN32)
    TARGET_LINK_LIBRARIES(core m)
endif()

find_package(Capstone)

IF(Capstone_FOUND)
    if (Capstone_LIBRARIES MATCHES ".*\\.a$")
        message("Statically linking Capstone")
    else()
        message("Dynamically linking Capstone")
    endif()
    TARGET_LINK_LIBRARIES(disassemble ${Capstone_LIBRARIES})
    TARGET_COMPILE_DEFINITIONS(disassemble PRIVATE -DHAVE_CAPSTONE)
ELSE()
    message("Did not find Capstone.")
ENDIF()


add_executable(${PS1_TARGET} main.c)
target_link_libraries(${PS1_TARGET} core common)