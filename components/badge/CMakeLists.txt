if(CONFIG_NSEC_BUILD_CONFERENCE)
        idf_component_register(SRCS "badge.cpp" "globals.cpp"
                INCLUDE_DIRS "." "../"
        )
endif()
