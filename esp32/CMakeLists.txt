cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS "./extra-components/")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# These project names are used for firmware detection, don't change without refactor
if(CONFIG_NSEC_BUILD_CTF)
    project(nsec-ctf)
elseif(CONFIG_NSEC_BUILD_ADDON)
    project(nsec-ctf-addon)
else() #default CONFIG_NSEC_BUILD_CONFERENCE
    project(nsec-badge)
endif()
