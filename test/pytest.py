#RTree collision test for MEL python binding
from MEL import FloatRect, FloatRTree

def makeRect(a, b, c, d):
	r = FloatRect()
	r.left = a
	r.top = b
	r.right = c
	r.bottom = d
	return r

tree = FloatRTree()

boxes = [
	makeRect(0, 0, 4, 4),     makeRect(1, 1, 4, 6),     makeRect(5, 5, 7, 8),     makeRect(10, 0, 15, 4),
	makeRect(0, 10, 15, 14),  makeRect(12, 4, 16, 8),   makeRect(50, 0, 54, 6),   makeRect(3, 1, 9, 5),
	makeRect(15, 3, 20, 4),   makeRect(10, 0, 16, 4),   makeRect(49, 64, 57, 76), makeRect(57, 27, 71, 40),
	makeRect(59, 11, 72, 24), makeRect(33, 7, 44, 11),  makeRect(12, 60, 21, 76), makeRect(60, 22, 61, 33),
	makeRect(55, 63, 58, 67), makeRect(39, 47, 48, 63), makeRect(60, 25, 67, 33), makeRect(25, 51, 41, 60),
	makeRect(61, 1, 71, 15),  makeRect(5, 28, 6, 37),   makeRect(18, 54, 21, 66), 
	makeRect(9, 38, 15, 54),  makeRect(53, 39, 59, 47)
]

djBox = makeRect(99, 99, 100, 100)
bbBox = makeRect(0, 0, 100, 100)
IDtoArrIndex = {}
for i in range(len(boxes)):
	ident = tree.insert(boxes[i])
	assert(ident not in IDtoArrIndex)
	IDtoArrIndex[ident] = i

assert(len(tree.intersect(djBox)) == 0)

collided = tree.intersect(bbBox)
assert(len(collided) == len(boxes))

for ident in collided:
	assert(ident in IDtoArrIndex)
	assert(boxes[IDtoArrIndex[ident]].Intersects(bbBox))
