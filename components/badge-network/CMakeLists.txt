if(CONFIG_NSEC_BUILD_CONFERENCE)
        idf_component_register(SRCS "network_handler.cpp"
                        INCLUDE_DIRS "." "../"
        )
endif()