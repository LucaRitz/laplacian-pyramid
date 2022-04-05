target_sources(${PROJECT_NAME}
        PUBLIC
        ../include/laplacian_pyramid/laplacian_pyramid.hpp
        ../include/laplacian_pyramid/type.hpp

        PRIVATE
        laplacian_pyramid.cpp
        type.cpp)
