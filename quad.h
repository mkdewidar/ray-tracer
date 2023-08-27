#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"
#include "logger.h"

// a representation of a four sided geometrical shape
// Q represents the bottom left corner of the quad, u and v are vectors that take
// you from Q to the two other adjacent corners, and adding both u and v takes you
// to the corner opposite Q
// for a quad/plane, any point in the world (x,y,z) that matches the equation Ax + By + Cy + D = 0 is inside the plane/quad
class Quad : public Hittable {
    public:

        Quad(Point3 const & q, Vec3 const & u, Vec3 const & v, std::shared_ptr<Material> const & m);

        bool hit(Ray const & ray, Interval const & rayLimits, HitResult & result) const override;

        Aabb bounding_box() const override;

    private:
        Point3 _q;
        Vec3 _u;
        Vec3 _v;
        std::shared_ptr<Material> _material;
        Aabb _boundingBox;
        Vec3 _normal;
        // the D constant in the plane's equation, see collision later
        double _constantD;
        // a constant that we use in intersection calculations that we can pre-calculate to save some time
        // see hit's documentation for more info
        Vec3 _w;
};

// ------

Quad::Quad(Point3 const & q, Vec3 const & u, Vec3 const & v, std::shared_ptr<Material> const & m)
           : _q(q), _u(u), _v(v), _material(m), _boundingBox(Aabb(this->_q, this->_q + this->_u + this->_v).pad()) {
    auto uvNormal = this->_u.cross(this->_v);

    this->_normal = uvNormal.unit();
    this->_constantD = this->_normal.dot(this->_q);

    // note this isn't using the unit vector normal
    this->_w = uvNormal / uvNormal.dot(uvNormal);
}

// Step 1: find the plane equation for the that the quad is on
//         in re-odering the quad equation we get an equation that just so happens to be similar to the
//         dot product of the normal and any vector on the plane:
//                    Ax + By + Cz = D = normal . pointOnPlane         normal = (A, B, C)  pointOnPlane = (x, y, z)
//         since we have a point on the plane (q) and the normal is the cross product of u and v we can find D
// Step 2: use the constant we have found to solve the equation for intersection with a ray
//         t = (D - normal . P) / (normal . d)        where P and d come from the standard ray equation P(t) = P + td
//         assuming the ray intersects with the plane, P(t) gives a point on the plane, and so satisfies the plane equation
//         If the ray does not hit (cause its parallel to the plane), then normal . d gives 0
//         If the ray does hit, we get a value of t
// Step 3: check if we actually hit inside the quad, or did we just hit the plane as a whole
//         we know the point of intersection must be on the plane, and must be inside the bounds of q + u and q + v so
//         P(t) - q must be a vector of components (q + Xu, q + Yv) so:
//                    P(t) = q + (Alpha * u) + (Beta * v)
//                P(t) - q = (Alpha * u) + (Beta * v) = p
//         by applying cross product twice to both sides, once u and once v, we can get two equations:
//                u x p = u x (Alpha * u) + u x (Beta * v) = Alpha (u x u) + Beta (u x v) = Beta (u x v)
//                v x p = v x (Alpha * u) + v x (Beta * v) = Alpha (v x u) + Beta (v x v) = Alpha (v x u)
//         we then apply dot products of (u x v), which we know is synonymous to the (not unit) normal, to either side
//         to make the sides scalar values that can be moved around easily, then re-order to get:
//                Alpha = normal . (v x p) / normal . (v x u)
//                Beta = normal . (u x p) / normal . (u x v)
//         because a x b = -b x a, we can inverse the cross product on the top and bottom of the alpha equation to
//         have the same bottom half of the equation (the negatives cancel out giving us the same alpha value),
//         allowing us to pre-compute and cache it for both calculations we cache that as w, leading to the final equation:
//                w = normal / normal . (u x v) = normal / normal . normal
//                alpha = w . (p x v)
//                beta = w . (u x p)
bool Quad::hit(Ray const & ray, Interval const & rayLimits, HitResult & result) const {
    LOG(
        std::clog << "Quad intersection check for quad q: " << this->_q << ", u: " << this->_u << ", v: " << this->_v
                  << ", normal: " << this->_normal << ", w: " << this->_w << "\n";
    )

    double normalDotRayDirection = this->_normal.dot(ray.dir);
    // some leeway to capture things that are almost parallel but not technically
    if (fabs(normalDotRayDirection) < 0.00000001) {
        // no hit, the ray is parallel to the plane
        LOG(
            std::clog << "Ray does not hit quad, normal . ray direction: " << normalDotRayDirection << "\n";
        )
        return false;
    }

    double t = (this->_constantD - this->_normal.dot(ray.orig)) / normalDotRayDirection;
    // make sure we're within the limits of the ray
    if (!rayLimits.contains(t)) {
        LOG(
            std::clog << "Ray hits quad at " << t << " but this is not in ray limits: " << rayLimits.min << " " << rayLimits.max << "\n";
        )
        return false;
    }

    Point3 planeIntersectionPoint = ray.at(t);
    Point3 intersectionPointFromQ = planeIntersectionPoint - this->_q;

    // we know it hit the plane, but now to figure out if it hit the plane within our quad or not

    // how many u vectors would it take to reach the intersection point
    double alpha = this->_w.dot(intersectionPointFromQ.cross(this->_v));
    // how many v vectors would it take to reach the intersection point
    double beta = this->_w.dot(this->_u.cross(intersectionPointFromQ));

    LOG(
        std::clog << "Ray alpha w: " << this->_w << ", intersection point x v: " << intersectionPointFromQ.cross(this->_v) << "\n";
        std::clog << "Ray beta w: " << this->_w << ", u x intersection point: " << this->_u.cross(intersectionPointFromQ) << "\n";
    )

    if ((alpha < 0) || (alpha > 1) || (beta < 0) || (beta > 1)) {
        // intersected with the plane, but not within the bounds of this quad
        LOG(
            std::clog << "Ray hits quad at " << t << ", intersection point: " << planeIntersectionPoint
                      << " but has a u factor of " << alpha << " and v factor of " << beta << " so is being rejected\n";
        )
        return false;
    }

    result.t = t;
    result.point = planeIntersectionPoint;
    result.material = this->_material;
    result.set_face_normal(ray, this->_normal);
    result.u = alpha;
    result.v = beta;

    LOG(
        std::clog << "Ray hits quad at " << t << " which is the point " << planeIntersectionPoint
                  << ", with u factor of " << alpha << " and v factor of "
                  << beta << " \n";
    )

    return true;
}

Aabb Quad::bounding_box() const {
    return this->_boundingBox;
}

#endif
