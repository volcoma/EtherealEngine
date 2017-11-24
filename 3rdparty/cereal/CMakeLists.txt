set( THREAD_SAFE ON CACHE BOOL "Use mutexes to ensure thread safety" FORCE)
set( JUST_INSTALL_CEREAL ON CACHE BOOL "Don't do anything besides installing the library" FORCE)

add_subdirectory( cereal )
