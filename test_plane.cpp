#include "plane.h"
#include "vec3.h"
#include <vector>

int main()
{
    vec3 a(0, 0, 0), b(1, 0, 0), c(1, 0, 1), d(0, 0, 1);

    plane p(a, b, c, d, NULL);

    std::vector<vec3> vectors{{.5, 0, .5}, {.5, .5, 1}, {1,0,1}, {1, 2, 0}};

    for (auto v : vectors)
    {
        std::cout << p.isInside(v) << std::endl;
    }
    return 0;
}