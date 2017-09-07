#pragma once

#if !HAS_MAKEUNIQUE

#include <memory>

namespace std
{

    template <typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

}

#endif

#if !HAS_PI
namespace std
{

    constexpr double PI = 3.14159265359;

}

#endif
