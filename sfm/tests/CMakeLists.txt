include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.1
)
FetchContent_MakeAvailable(Catch2)

add_executable(sfm_tests
    test_pipeline.cpp
)
target_link_libraries(sfm_tests PRIVATE sfm Catch2::Catch2WithMain opencv_highgui)
target_compile_features(sfm_tests PRIVATE cxx_std_17)

include(Catch)
catch_discover_tests(sfm_tests)
