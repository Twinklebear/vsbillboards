add_executable(vsbillboards main.cpp camera.cpp util.cpp gl_core_3_3.c)
target_link_libraries(vsbillboards ${SDL2_LIBRARY} ${OPENGL_LIBRARIES} ${lfwatch_LIBRARY})
	
install(TARGETS vsbillboards DESTINATION ${vsbillboards_INSTALL_DIR})

