add_executable(OurPaintApp main.cc)
target_include_directories(OurPaintApp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Lib/src)
target_link_libraries(OurPaintApp PRIVATE OurPaint::Lib)
