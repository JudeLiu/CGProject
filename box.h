#ifndef BOXH
#define BOXH

#include "object_list.h"
#include "aarect.h"

class box: public object {
    public:
    vec3 pmin, pmax;
    object *list_ptr;
    box() {}
    box(const vec3 &p0, const vec3 &p1, material *m);
    virtual bool hit(const ray& r, float t0, float t1, hit_record &rec) const;
    virtual bool bounding_box(float t0, float t1, aabb& box) const {
               box =  aabb(pmin, pmax);
               return true; }
};


box::box(const vec3& p0, const vec3& p1, material *ptr) {
    pmin = p0;
    pmax = p1;
    object **list = new object*[6];
    list[0] = new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr);
    list[1] = new flip_normals(new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));
    list[2] = new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr);
    list[3] = new flip_normals(new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));
    list[4] = new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr);
    list[5] = new flip_normals(new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
    list_ptr = new object_list(list,6);
}

bool box::hit(const ray& r, float t0, float t1, hit_record& rec) const {
    return list_ptr->hit(r, t0, t1, rec);
}

#endif // BOXH