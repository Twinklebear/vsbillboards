add_executable(FastBillboards main.cpp camera.cpp util.cpp gl_core_3_3.c)
target_link_libraries(FastBillboards ${SDL2_LIBRARY} ${OPENGL_LIBRARIES} ${lfwatch_LIBRARY})
	
install(TARGETS FastBillboards DESTINATION ${FastBillboards_INSTALL_DIR})

