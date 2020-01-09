#ifndef CHARACTER_HPP
#define CHARACTER_HPP
#include <core/MovingEntity.hpp>
#include <core/Timer.hpp>
#include <GL/Sprite.hpp>
#include <GL/SpriteBatch.hpp>
#include <GL/TextureAtlas.hpp>
#include <GL/Window.hpp>
#include <core/Map.hpp>
#include <cmath>
#include <array>
#include <vector>
struct Controls{
	int upkey = GLFW_KEY_UP;
	int downkey = GLFW_KEY_DOWN;
	int leftkey = GLFW_KEY_LEFT;
	int rightkey = GLFW_KEY_RIGHT;
	int swordkey = GLFW_KEY_Z;
	int jumpkey = GLFW_KEY_X;
	int shieldkey = GLFW_KEY_LEFT_SHIFT;
	int ledgekey = GLFW_KEY_C;
};
class Character : public MovingEntity {
public:
	Sprite m_spr2;
	Sprite m_shieldspr;
	bool swordout = false;

	Character(const Character&) = delete;
	Character& operator=(const Character&) = delete;
	Character(float x, float y, ObjMap& map, const std::string& mainsprite, const std::string& swordsprite, const std::string& sword2sprite, TextureAtlas& atlas);
	void Update(float dt, const std::vector<MovingEntity *>& objects, const Window& window);
	void Draw(SpriteBatch& mframe);
private:
	const Controls defaultcontrols;
	const float jumpSpeed = 1500.0f;
	const float walkSpeed = 900.0f;
	const float lowJumpSpeed = -400.0f;
	float minSpeed = 0.0f;
	float maxSpeed = 0.0f;
	float gravity = 5000.0f;
	const float shieldmax = 600.f;
	TextureAtlas& m_atlas;
	std::vector<Texture> texs; //1,2,3,shield
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
