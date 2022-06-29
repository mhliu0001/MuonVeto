cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(MuonVetoProject)

find_package(Geant4 REQUIRED ui_all vis_all)

# CXX_STANDARD may be changed, according to the environment
set(CMAKE_CXX_STANDARD 17)

include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${Geant4_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(MuonVeto MuonVeto.cc ${sources} ${headers})
target_link_libraries(MuonVeto ${Geant4_LIBRARIES} )

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/output/)

set(MUONVETO_SCRIPTS
    ./macro/vis.mac
    ./macro/init_vis.mac
    ./macro/gui.mac
    ./macro/muon_z.mac
    ./macro/muon_x.mac
)

foreach(_script ${MUONVETO_SCRIPTS})
    configure_file(
        ${PROJECT_SOURCE_DIR}/${_script}
        ${PROJECT_BINARY_DIR}/${_script}
        COPYONLY
    )
endforeach()

add_custom_target(MuonVetoProject DEPENDS MuonVeto)
