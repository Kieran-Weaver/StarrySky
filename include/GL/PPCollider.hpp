#ifndef PPCOLLIDER_H
#define PPCOLLIDER_H
class Sprite;
template<typename T> class Rect;

/* struct PPCollider
 *  Abstract Base Class for any object which can participate in
 *   pixel-perfect collision
*/

struct PPCollider {
	virtual bool collides(const Sprite& other) const = 0;
	virtual const Rect<float>& getAABB(void) const = 0;
};

#endif
