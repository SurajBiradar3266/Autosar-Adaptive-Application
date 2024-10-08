

cmake_minimum_required(VERSION 3.15)
get_filename_component(PROJECTNAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECTNAME} VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(AUTOSAR/apd)

# Set these variables (like in the yocto recipe) to build the application as a specific process on a specific machine
if(NOT TARGET_MACHINE)
    set(TARGET_MACHINE "RadarFusionMachine")
endif()
if(NOT APP_NAME)
    set(APP_NAME "DataSender")
endif()
if(NOT PROC_NAME)
    set(PROC_NAME "DataSender")
endif()

# Generic stuff (ara generator and arxmls)
find_package(ara-gen REQUIRED)

find_package(ara-arxmls REQUIRED)
find_package(ara-adi-sensoritf-arxmls REQUIRED)
find_package(apd-applications-arxmls REQUIRED)
find_package(apd-common-machine-arxmls REQUIRED)
find_package(apd-interfaces-arxmls REQUIRED)
find_package(apd-network-arxmls REQUIRED)
find_package(apd-dlt-arxmls REQUIRED)
 
string(TOLOWER ${TARGET_MACHINE} TARGET_MACHINE_LOWER)
find_package(apd-${TARGET_MACHINE_LOWER}-arxmls REQUIRED)

# Functional Cluster (APIs)
find_package(Threads REQUIRED)
find_package(ara-com REQUIRED)
find_package(ara-core REQUIRED)
find_package(ara-log REQUIRED)
find_package(ara-exec-execution-client REQUIRED)

# Protocol Binding
# It's not possible to enable some/ip and opendds at the same time
if((HAS_VSOMEIP_BINDING OR HAS_WRSOMEIP_BINDING) AND HAS_OPENDDS_BINDING)
    message(FATAL_ERROR "Both SOMEIP and OPENDDS Bindings are enabled")
endif()

if(HAS_VSOMEIP_BINDING AND HAS_WRSOMEIP_BINDING)
    message(FATAL_ERROR "Both VSOMEIP and WRSOMEIP Bindings are enabled")
endif()

# use vsomeip binding by default
if(NOT HAS_VSOMEIP_BINDING AND NOT HAS_WRSOMEIP_BINDING AND NOT HAS_OPENDDS_BINDING)
    set(HAS_VSOMEIP_BINDING ON CACHE BOOL "Use communication over some/ip via vsomeip" FORCE)
endif(NOT HAS_VSOMEIP_BINDING AND NOT HAS_WRSOMEIP_BINDING AND NOT HAS_OPENDDS_BINDING)

if(HAS_OPENDDS_BINDING)
    find_package(OpenDDS REQUIRED)
    set(COMMUNICATION_LIBRARY ara_ddsidlbinding)
    add_compile_definitions(HAS_OPENDDS_BINDING)
endif(HAS_OPENDDS_BINDING)

if(HAS_VSOMEIP_BINDING)
    set(COMMUNICATION_LIBRARY ara_vsomeipbinding)
    add_compile_definitions(HAS_VSOMEIP_BINDING)
endif(HAS_VSOMEIP_BINDING)

if(HAS_WRSOMEIP_BINDING)
    set(COMMUNICATION_LIBRARY ara_wrsomeipbinding)
    add_compile_definitions(HAS_WRSOMEIP_BINDING)
endif(HAS_WRSOMEIP_BINDING)

# List all applications that will run on the machine
list(TRANSFORM APD_APPLICATION_ARXMLS PREPEND ${APD_APPLICATIONS_ARXMLS_DIR}/)

# List all interfaces that will be used by applications on the machine
list(TRANSFORM APD_INTERFACES_ARXMLS PREPEND ${APD_INTERFACES_ARXMLS_DIR}/)

# List all adi sensor interfaces that will be used by applications on the machine
list(TRANSFORM ARA_ADI_SENSORITF_ARXMLS PREPEND ${ARA_ADI_SENSORITF_ARXMLS_DIR}/)

# List all processes that will run on the machine
list(TRANSFORM APD_PROCESS_ARXMLS PREPEND ${APD_MACHINE_ARXMLS_DIR}/)

# List all other files of the machine (like machine manifest)
list(TRANSFORM APD_MACHINE_ARXMLS PREPEND ${APD_MACHINE_ARXMLS_DIR}/)

# List all dlt related files that will be used by machine or applications on the machine
list(TRANSFORM APD_DLT_ARXMLS PREPEND ${APD_DLT_ARXMLS_DIR}/)

# List all files of the network
set(APD_NETWORK_ARXMLS
  ${APD_NETWORK_ARXMLS_DIR}/network.arxml
)

# List all files required from ara-api
set(ARA_COMMON_ARXMLS
  ${ARA_ARXMLS_DIR}/AUTOSAR_MOD_StandardTypes.arxml
  ${ARA_ARXMLS_DIR}/package_management.arxml
  ${ARA_ARXMLS_DIR}/routine.arxml
  ${ARA_ARXMLS_DIR}/shared_types.arxml
  ${ARA_ARXMLS_DIR}/sm_types.arxml
  ${ARA_ARXMLS_DIR}/system_components.arxml
  ${ARA_ARXMLS_DIR}/trigger_in_out.arxml
  ${ARA_ARXMLS_DIR}/trigger_in.arxml
  ${ARA_ARXMLS_DIR}/trigger_out.arxml
  ${ARA_ARXMLS_DIR}/ucmtypes.arxml
  ${ARA_ARXMLS_DIR}/update_request.arxml
  ${ARA_ARXMLS_DIR}/vehicle_package_management.arxml
)

# Make use of common machine elements
set(APD_COMMON_MACHINE_ARXMLS
  ${APD_COMMON_MACHINE_ARXMLS_DIR}/execution_manifest.arxml
  ${APD_COMMON_MACHINE_ARXMLS_DIR}/machine_manifest.arxml
  ${APD_COMMON_MACHINE_ARXMLS_DIR}/service_instance_manifest.arxml
)

set(ARXML_SET
  ${ARA_COMMON_ARXMLS}
  ${APD_APPLICATION_ARXMLS}
  ${APD_INTERFACES_ARXMLS}
  ${APD_COMMON_MACHINE_ARXMLS}
  ${APD_MACHINE_ARXMLS}
  ${APD_PROCESS_ARXMLS}
  ${APD_NETWORK_ARXMLS}
  ${APD_DLT_ARXMLS}
  ${ARA_ADI_SENSORITF_ARXMLS}
)

# ------------------------------------------------------------------------------------------------------------------

set(GEN_DEST ${PROJECT_BINARY_DIR}/gen)

set(SWC_FQN /apd/AdaptiveApplicationSwComponentTypes/${APP_NAME})
set(PROCESS_FQN /apd/Machines/${TARGET_MACHINE}/Processes/${PROC_NAME})

set(INTERFACES_MAKE_TARGET ${CMAKE_PROJECT_NAME}InterfacesGenerated)
set(PROCESS_ARTIFACTS_MAKE_TARGET ${PROC_NAME}ProcessGenerated)

# DDS generator output directory to avoid conflict with process related generated parts
set(OPENDDS_GEN_DEST ${PROJECT_BINARY_DIR}/dds_gen)

set(OPENDDS_MAKE_TARGET ${APP_NAME}Generated)

set(APPLICATION_src
  ${PROJECT_SOURCE_DIR}/src/DataSender_component.cpp
  ${PROJECT_SOURCE_DIR}/src/VehicleData.cpp
  ${PROJECT_SOURCE_DIR}/src/main.cpp
)

if(HAS_VSOMEIP_BINDING OR HAS_WRSOMEIP_BINDING)
    add_middleware_interfaces(
        DESTINATION ${GEN_DEST}
        TARGET ${INTERFACES_MAKE_TARGET}
        SWC_FQN ${SWC_FQN}
        ARXMLS ${ARXML_SET}
    )
endif()

if(HAS_OPENDDS_BINDING)
    add_opendds(
        DESTINATION ${OPENDDS_GEN_DEST}
        TARGET ${OPENDDS_MAKE_TARGET}
        PROCESS_FQN ${PROCESS_FQN}
        ARXMLS ${ARXML_SET}
        APP_NAME ${APP_NAME}
    )
    list(APPEND APPLICATION_src $<TARGET_OBJECTS:${OPENDDS_MAKE_TARGET}Lib>)
endif()

add_process_artifacts(
    DESTINATION ${GEN_DEST}
    TARGET ${PROCESS_ARTIFACTS_MAKE_TARGET}
    PROCESS_FQN ${PROCESS_FQN}
    ARXMLS ${ARXML_SET}
)

if(HAS_VSOMEIP_BINDING OR HAS_WRSOMEIP_BINDING)
    list(APPEND APPLICATION_src ${${PROCESS_ARTIFACTS_MAKE_TARGET}_SOURCES})
endif()

add_executable(${CMAKE_PROJECT_NAME} ${APPLICATION_src})

if(HAS_VSOMEIP_BINDING OR HAS_WRSOMEIP_BINDING)
    add_dependencies(${CMAKE_PROJECT_NAME} ${INTERFACES_MAKE_TARGET} ${PROCESS_ARTIFACTS_MAKE_TARGET})

    target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        ${GEN_DEST}/includes
        ${ARA_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/include
    )
endif()

if(HAS_OPENDDS_BINDING)
    add_dependencies(${CMAKE_PROJECT_NAME} ${OPENDDS_MAKE_TARGET} ${PROCESS_ARTIFACTS_MAKE_TARGET})

    include_directories(
        ${OPENDDS_GEN_DEST}/includes
        ${OPENDDS_GEN_DEST}/net-bindings/opendds
        ${ARA_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/include
    )
endif()

target_link_libraries(${CMAKE_PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} ${ARA_LIBRARIES} ${COMMUNICATION_LIBRARY} ara_com e2exf e2e ara::log ara::exec_execution_client ara::core)

apd_install_app_executable(${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME})

set(EM_CONFIG_FILENAME ${GEN_DEST}/processes/${PROC_NAME}_MANIFEST.json)
apd_install_app_execution_manifest(${CMAKE_PROJECT_NAME} ${EM_CONFIG_FILENAME})

set(SI_MANIFEST_FILENAME ${GEN_DEST}/processes/${PROC_NAME}_SI_MANIFEST.json)
apd_install_app_service_instance_manifest(${CMAKE_PROJECT_NAME} ${SI_MANIFEST_FILENAME})

# E2E protection configs
#apd_install_app_file(${CMAKE_PROJECT_NAME} etc ${PROJECT_SOURCE_DIR}/etc/e2e_dataid_mapping.json)
#apd_install_app_file(${CMAKE_PROJECT_NAME} etc ${PROJECT_SOURCE_DIR}/etc/e2e_statemachines.json)
