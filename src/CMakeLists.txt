set(pngtotxt_sources 
	main.c
	pngtotxt.c
	imgbuf.c
)

add_executable(pngtotxt main.c pngtotxt.c imgbuf.c)
target_include_directories(pngtotxt PRIVATE ${CMAKE_BINARY_DIR}/include ${CMAKE_SOURCE_DIR}/include ${Iconv_INCLUDE_DIR} ${PNG_INCLUDE_DIR})
target_link_libraries(pngtotxt PRIVATE ${Iconv_LIBRARIES} ${PNG_LIBRARIES})

install(TARGETS pngtotxt RUNTIME DESTINATION bin)


