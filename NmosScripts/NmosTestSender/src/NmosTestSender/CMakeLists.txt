# ~~~~~
# 
# Code Sample License Agreement
# Effective Date: 4/15/2025
# Cinnafilm, Inc. ("Licensor") grants Finn Thomas ("Licensee") a non-exclusive, 
# non-transferable, revocable license to use the provided code samples ("Code") 
# under the following terms:
# - Permitted Use: Licensee may use the Code solely for personal, non-commercial 
# purposes, such as inclusion in a portfolio or demonstration during job 
# interviews.
# - Restrictions: Licensee may not: (a) use the Code for any commercial purpose; 
# (b) distribute, sell, sublicense, or otherwise share the Code with third 
# parties; (c) modify the Code for purposes beyond personal demonstration; or 
# (d) claim ownership of the Code.
# - Ownership: The Code remains the exclusive property of Cinnafilm, Inc.
# - Termination: This license may be terminated by Licensor at any time with 
# written notice to Licensee, after which Licensee must cease all use of the Code.
# No Warranty: The Code is provided "as is," with no warranties of any kind.
# By using the Code, Licensee agrees to these terms.
# 
# ~~~~~

cmake_minimum_required(VERSION 3.24)

project(NmosTestSender VERSION 1.0 LANGUAGES CXX)

# Add source to this project's executable.
add_executable (NmosTestSender "NmosTestSender.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET NmosTestSender PROPERTY CXX_STANDARD 20)
endif()

 message(STATUS "This is the cur dir [${CMAKE_CURRENT_SOURCE_DIR}]")
# windows
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(nvnmos SHARED IMPORTED GLOBAL)
set_property(TARGET nvnmos PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/nvnmos.dll)
set_property(TARGET nvnmos PROPERTY IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/nvnmos.lib)

target_link_libraries(${PROJECT_NAME} PUBLIC
	nvnmos
)

# Copy DLLs at install time
install(IMPORTED_RUNTIME_ARTIFACTS 
    nvnmos
    DESTINATION bin
)

#COPY nvnmos.dll to the output directory
add_custom_command(TARGET NmosTestSender POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:nvnmos> $<TARGET_FILE_DIR:NmosTestSender>
)
