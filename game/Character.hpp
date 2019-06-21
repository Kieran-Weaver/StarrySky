#ifndef CHARACTER_HPP
#define CHARACTER_HPP
#include <core/MovingEntity.hpp>
#include <GL/Sprite.h>
#include <GL/SpriteBatch.h>
#include <GL/TextureAtlas.h>
#include <core/Map.hpp>
#include "Enemy.hpp"
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
	const float jumpSpeed = 1500.0f;
	const float walkSpeed = 900.0f;
	const float lowJumpSpeed = -400.0f;
	float minSpeed = 0.0f;
	float maxSpeed = 0.0f;
	float gravity = 5000.0f;
	float maxFallSpeed = 1300.0f;
public:
	Sprite spr2;
	Sprite shieldspr;
	std::vector<Texture> texs; //1,2,3,shield
	bool swordout = false;
	bool isOnLedge = false;
	bool jumped = false;
	int swordtimer = 0;
	bool flipped = false;
	const glm::mat4 flipped_mat = glm::scale(glm::mat4(1.f),glm::vec3(-1.f,1.f,1.f));
	bool shieldout = false;
	bool shieldbroken = false;
	int invltimer = 0;
	float shieldmeter = 600.f;
	const float shieldmax = 600.f;
	Controls controls;
	const Controls defaultcontrols;
	WallType walljumpstate;
public:
	Character(const Character&) = delete;
	Character& operator=(const Character&) = delete;
	Character(float x, float y, ObjMap& map, const std::string& mainsprite, const std::string& swordsprite, const std::string& sword2sprite, TextureAtlas& atlas);
	void Update(float dt, std::vector<MovingEntity *>& objects, GLFWwindow* frame);
	void Draw(SpriteBatch& mframe);
public:
	TextureAtlas& m_atlas;
};
#endif
