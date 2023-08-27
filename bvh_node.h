#ifndef BVH_NODE_H
#define BVH_NODE_H

#include <algorithm>

#include "interval.h"
#include "hittable.h"
#include "random.h"

// represents a node in the BVH tree, which is a hittable AABB that encompasses up to two other child hittable objects
class BvhNode : public Hittable {
    public:
        // a special constructor that will open up the list of hittables and subdivide them into more BVH nodes
        BvhNode(HittableList const & inputList);
        // startIndex is inclusive, endIndex is exclusive (i.e after the last object by 1)
        BvhNode(std::vector<std::shared_ptr<Hittable>> const & srcObjects, size_t startIndex, size_t endIndex);

        virtual bool hit(Ray const & ray, Interval const & rayLimits, HitResult & result) const override;

        virtual Aabb bounding_box() const override;

    private:
        std::shared_ptr<Hittable> _leftNode;
        std::shared_ptr<Hittable> _rightNode;
        Aabb _boundingBox;

        static bool box_x_compare(std::shared_ptr<Hittable> const & a, std::shared_ptr<Hittable> const & b);
        static bool box_y_compare(std::shared_ptr<Hittable> const & a, std::shared_ptr<Hittable> const & b);
        static bool box_z_compare(std::shared_ptr<Hittable> const & a, std::shared_ptr<Hittable> const & b);
};

// ------

BvhNode::BvhNode(HittableList const & srcHittables) : BvhNode(srcHittables.objects, 0, srcHittables.objects.size()) { }

BvhNode::BvhNode(std::vector<std::shared_ptr<Hittable>> const & srcObjects, size_t startIndex, size_t endIndex) {
    LOG(
        std::clog << "Creating BVH node from indices " << startIndex << " and " << endIndex << "\n";
    )

    auto objects = srcObjects; // create a copy of the array, but one that references the same objects

    // choose an axis that we want to sort the objects by before we split them
    int chosenAxis = random_int(0, 2);
    auto comparator = chosenAxis == 0 ? BvhNode::box_x_compare
                    : chosenAxis == 1 ? BvhNode::box_y_compare
                                      : BvhNode::box_z_compare;

    auto numOfObjectsToSplit = endIndex - startIndex;
    if (numOfObjectsToSplit == 1) {
        // there's only one object to be contained by this node
        _leftNode = _rightNode = srcObjects[startIndex];
    } else if (numOfObjectsToSplit == 2) {
        // there's two objects to be contained by this node, lets put one on the left and one on the right
        // but lets keep the order cause why not
        if (comparator(srcObjects[startIndex], srcObjects[startIndex + 1])) {
            _leftNode = srcObjects[startIndex];
            _rightNode = srcObjects[startIndex + 1];
        } else {
            _leftNode = srcObjects[startIndex + 1];
            _rightNode = srcObjects[startIndex];
        }
    } else {
        // there's more than two objects to be contained by this node, so we'll have to create more BVH nodes
        // as children
        std::sort(objects.begin() + startIndex, objects.begin() + endIndex, comparator);

        size_t middleIndex = startIndex + (numOfObjectsToSplit / 2);
        _leftNode = std::make_shared<BvhNode>(objects, startIndex, middleIndex);
        _rightNode = std::make_shared<BvhNode>(objects, middleIndex, endIndex);
    }

    this->_boundingBox = Aabb(this->_leftNode->bounding_box(), this->_rightNode->bounding_box());
}

bool BvhNode::hit(Ray const & ray, Interval const & rayLimits, HitResult & result) const {
    if (!this->_boundingBox.hit(ray, rayLimits)) {
        return false;
    }

    // since the left and right nodes can overlap, we must compute both, not just one side
    bool hitLeft = this->_leftNode->hit(ray, rayLimits, result);
    // if we hit something in the left subtree, then we can re-use the ray max distance here to save even
    // more time processing the nodes in this subtree
    bool hitRight = this->_rightNode->hit(ray, Interval(rayLimits.min, hitLeft ? result.t : rayLimits.max), result);

    return hitLeft || hitRight;
}

Aabb BvhNode::bounding_box() const {
    return this->_boundingBox;
}

bool BvhNode::box_x_compare(std::shared_ptr<Hittable> const & a, std::shared_ptr<Hittable> const & b) {
    return a->bounding_box().xBounds.min < b->bounding_box().xBounds.min;
}

bool BvhNode::box_y_compare(std::shared_ptr<Hittable> const & a, std::shared_ptr<Hittable> const & b) {
    return a->bounding_box().yBounds.min < b->bounding_box().yBounds.min;
}

bool BvhNode::box_z_compare(std::shared_ptr<Hittable> const & a, std::shared_ptr<Hittable> const & b) {
    return a->bounding_box().zBounds.min < b->bounding_box().zBounds.min;
}

#endif