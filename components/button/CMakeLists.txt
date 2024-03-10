if(CONFIG_NSEC_BUILD_CONFERENCE)
        idf_component_register(SRCS "watcher.cpp"
                INCLUDE_DIRS "." "../"
        )
endif()
