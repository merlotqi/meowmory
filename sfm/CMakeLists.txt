find_package(OpenCV REQUIRED COMPONENTS core features2d imgcodecs imgproc highgui)

add_library(sfm STATIC "")
target_include_directories(sfm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(sfm PUBLIC
    opencv_core
    opencv_features2d
    opencv_imgproc
    opencv_imgcodecs
)
target_compile_features(sfm PUBLIC cxx_std_17)

add_subdirectory(src)
add_subdirectory(app)
add_subdirectory(tests)
