target_sources(sfm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/features/cv/cv_detector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/features/cv/cv_descriptor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/features/cv/cv_matcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/features/sift/sift_pipeline.cpp
)
