#ifndef CHARACTER_HPP
#define CHARACTER_HPP
#include <core/MovingEntity.hpp>
#include <core/Timer.hpp>
#include <GL/PPCollider.hpp>
#include <GL/Sprite.hpp>
#include <GL/SpriteBatch.hpp>
#include <GL/TextureAtlas.hpp>
#include <GL/Window.hpp>
#include <core/Map.hpp>
struct Controls{
	int upkey;
	int downkey;
	int leftkey;
	int rightkey;
	int swordkey;
	int jumpkey;
	int shieldkey;
	int ledgekey;
};
class Character : public PPCollider {
public:
	Character(const Character&) = delete;
	Character& operator=(const Character&) = delete;
	Character(float x, float y, const std::string& mainsprite, const std::string& swordsprite, const std::string& sword2sprite, TextureAtlas& atlas);
	void Update(float dt, const ObjMap& map, const std::vector<PPCollider *>& objects, Window& window);
	void Draw(SpriteBatch& mframe);
	void warpto(float x, float y);
	bool collides(const Sprite& Object2) const override;
	const Rect<float>& getAABB() const override;
public:
	bool dead = false;
private:
	MovingEntity m_me;
	Sprite m_spr;
	Sprite m_spr2;
	Sprite m_shieldspr;
	bool swordout = false;
	Controls defaultcontrols;
	const float jumpSpeed = 1500.0f;
	const float walkSpeed = 900.0f;
	const float lowJumpSpeed = -400.0f;
	float minSpeed = 0.0f;
	float maxSpeed = 0.0f;
	float gravity = 5000.0f;
	const float shieldmax = 600.f;
	TextureAtlas& m_atlas;
	std::vector<Texture> texs; //1,2,3,shield
	bool wasOnGround = false;
	bool isOnLedge = false;
	bool jumped = false;
	Timer swordtimer;
	Timer invltimer;
	bool flipped = false;
	bool shieldout = false;
	bool shieldbroken = false;
	float shieldmeter = 600.f;
	Controls controls;
	WallType walljumpstate;
};
#endif
