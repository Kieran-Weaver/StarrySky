#include <game/Character.hpp>
#include <GL/Camera.hpp>
#include <imgui/imgui.h>
#include <GLFW/glfw3.h>
#define LASERW 25
#define LASERH 10
Character::Character(float x, float y, ObjMap& map, const std::string& mainsprite, const std::string& swordsprite, const std::string& sword2sprite, TextureAtlas& atlas): MovingEntity(x, y, map), m_atlas(atlas) {
	this->texs.reserve(4);
	std::array<std::string,4> fnames= {mainsprite, swordsprite, sword2sprite, "shield"}; 
	for (auto& i : fnames){
		this->texs.emplace_back(atlas.findSubTexture(i));
	}
	this->m_spr.setTexture(this->texs[0]);

	Rect<float> tmpAABB = m_spr.getAABB();
	this->m_width = tmpAABB.right - tmpAABB.left;
	this->m_height = tmpAABB.bottom - tmpAABB.top;
	this->m_spr2.setTexture(this->texs[1]);
	this->m_shieldspr.setTexture(this->texs[3]);
	this->m_spr.setStencil(true);
	this->maxFallSpeed = 1300.f;

	defaultcontrols.upkey = GLFW_KEY_UP;
	defaultcontrols.downkey = GLFW_KEY_DOWN;
	defaultcontrols.leftkey = GLFW_KEY_LEFT;
	defaultcontrols.rightkey = GLFW_KEY_RIGHT;
	defaultcontrols.swordkey = GLFW_KEY_Z;
	defaultcontrols.jumpkey = GLFW_KEY_X;
	defaultcontrols.shieldkey = GLFW_KEY_LEFT_SHIFT;
	defaultcontrols.ledgekey = GLFW_KEY_C;

	controls = defaultcontrols;
}
void Character::Update(float dt, const std::vector<MovingEntity*>& objects, Window& window) {
	MovingEntity::Update(dt);
	if (this->m_speed.y == 0){
		this->isOnGround |= this->wasOnGround;
	}
	WindowState& ws = window.getWindowState();
	if (!ws.keyboardState[controls.jumpkey]){
		this->jumped = false;
	}
	this->dropFromOneWay = ws.keyboardState[controls.downkey];
	if (swordtimer()){
		swordout = false;
		this->m_spr2.setTexture(this->texs[1]);
		this->swordtimer.setDelay(6);
		this->swordtimer.setTime(-1);
	} else if (!swordout && this->swordtimer.getDelay() == 0) {
		if ((ws.keyboardState[controls.swordkey])&&(!this->shieldout)){
			this->swordout = true;
			this->m_spr2.setTexture(this->texs[2]);
			ws.keyboardState[controls.swordkey] = false;
			this->swordtimer.setTime(30);
		}
	}
	this->minSpeed = -this->walkSpeed;
	this->maxSpeed = this->walkSpeed;
	if (shieldbroken){
		shieldmeter = std::min(shieldmeter + 0.6f, shieldmax);
		if (shieldmeter >= shieldmax){
			shieldbroken = false;
		}
	}
	if (ws.keyboardState[controls.shieldkey] && this->isOnGround){
		shieldout = true;
		shieldmeter = shieldmeter - 1.0f;
	}else{
		shieldmeter = std::min(shieldmeter + 1.0f, shieldmax);
		shieldout = false;
	}
	this->m_speed.y += this->gravity * dt;
	if (this->isOnGround){
		if (ws.keyboardState[controls.leftkey] == ws.keyboardState[controls.rightkey]) {
			this->m_speed.x = 0.0f;
		}
		if (ws.keyboardState[controls.jumpkey]) {
			this->m_speed.y = -this->jumpSpeed;
			this->isOnGround = false;
			this->jumped = true;
		}
		this->walljumpstate = WallType::FLOOR;
	}else{
		shieldout = false;
		if (this->m_speed.y > 0.f){
			this->m_speed.y = std::min(this->m_speed.y, this->maxFallSpeed);
		}else if (!ws.keyboardState[controls.jumpkey]){
			this->m_speed.y = std::max(this->m_speed.y, this->lowJumpSpeed);
		}
		if (ws.keyboardState[controls.leftkey] == ws.keyboardState[controls.rightkey]){
			if (this->m_speed.x < 0.f){
				this->m_speed.x = std::min(0.f,this->m_speed.x+100.0f);
			}else if (this->m_speed.x > 0.f){
				this->m_speed.x = std::max(0.f,this->m_speed.x-100.0f);
			}
		}
		isOnLedge = false;
		if (ws.keyboardState[controls.ledgekey]){
			Rect<float> thisrect = m_spr.getAABB();
			for (auto i : m_map.ledges){
				Rect<float> t = {i.x,i.y,m_map.ledgewidth,m_map.ledgeheight};
				if (thisrect.Intersects(t)){
					isOnGround = true;
					this->m_speed = glm::vec2(0.f,0.f);
					this->m_position = glm::vec2(i.x,i.y);
					isOnLedge = true;
					break;
				}
			}
		}
		if (ws.keyboardState[controls.jumpkey]){
			if ((walljumpstate == WallType::FLOOR)||(walljumpstate == WallType::RWALL)){
				if (this->pushesLeftWall&&(!this->jumped)){
					this->m_speed.y = -this->jumpSpeed;
					this->jumped = true;
					this->walljumpstate = WallType::LWALL;
				}
			}
			if ((walljumpstate == WallType::FLOOR)||(walljumpstate == WallType::LWALL)){
				if (this->pushesRightWall&&(!this->jumped)){
					this->m_speed.y = -this->jumpSpeed;
					this->jumped = true;
					this->walljumpstate = WallType::RWALL;
				}
			}
		}
	}
	if (ws.keyboardState[controls.leftkey] != ws.keyboardState[controls.rightkey]){
		if (ws.keyboardState[controls.leftkey]){
			this->m_speed.x = this->minSpeed;
		}else{
			this->m_speed.x = this->maxSpeed;
		}
	}

	if (!isOnLedge){
		if (ws.keyboardState[controls.leftkey]&&(!flipped)){
			flipped = true;
			this->m_spr.transform(this->flipped_mat);
			this->m_spr2.transform(this->flipped_mat);
		}else if (ws.keyboardState[controls.rightkey]&&flipped){
			flipped = false;
			this->m_spr.transform(this->flipped_mat);
			this->m_spr2.transform(this->flipped_mat);
		}
	}
	this->m_shieldspr.setPosition(this->m_position);
	int posx = this->m_position.x;
	int posy = this->m_position.y;
	Rect<float> tmp2 = m_spr2.getAABB();
	if (flipped){
		if (swordout){
			posx = posx + 12 - (tmp2.right - tmp2.left);
		}else{
			posx = posx - m_width/2 + ((tmp2.right - tmp2.left)/2);
			posy = posy - ((tmp2.bottom - tmp2.top)/2) + 12;
		}
	}else{
		if (swordout){
			posx = posx + m_width - 20;
		}else{
			posx = posx + (m_width/2) - ((tmp2.right - tmp2.left)/2) - 2;
			posy = posy + 12 - (tmp2.bottom - tmp2.top)/2;
		}
	}
	this->m_spr2.setPosition(glm::vec2(posx,posy));
	if (invltimer()){
		for (auto& i : objects){
			if (this->m_spr.PPCollidesWith(i->m_spr)){
				if (shieldout && !shieldbroken){
					shieldmeter = shieldmeter - 200.f;
					if (shieldmeter < 0.0f){
						shieldbroken = true;
						shieldmeter = 0.0f;
					}
					invltimer.setTime(20);
				} else {
					dead = true;
					shieldmeter = shieldmax;
					break;
				}
			}
		}
	}

	ws.camera->ScrollTo({m_position.x - m_width/2.f, m_position.y - m_height/2.f, static_cast<float>(m_width), static_cast<float>(m_height)});
}
void Character::Draw(SpriteBatch& mframe) {
	mframe.Draw(m_spr);
	mframe.Draw(m_spr2);
	std::string colstr = "no";
	if ((shieldout)&&(!shieldbroken)){
		m_shieldspr.rotate(0.1f);
		mframe.Draw(m_shieldspr);
		colstr = "yes";
	}
	ImGui::SetNextWindowPos(ImVec2(0.f,0.f),ImGuiCond_Always);
	ImGui::Begin("Shield");
	ImGui::ProgressBar(shieldmeter/shieldmax);
	ImGui::Text("Shield Out and not Broken: %s",colstr.c_str());
	ImGui::End();
	ImGui::Begin("InvlTimer");
	ImGui::ProgressBar(invltimer.getTime()/20.f);
	ImGui::End();
	
}
