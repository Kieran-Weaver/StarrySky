#include <game/Character.hpp>
#include <GL/Camera.hpp>
#include <imgui/imgui.h>
#include <GLFW/glfw3.h>
#define LASERW 25
#define LASERH 10
Character::Character(float x, float y, const std::string& mainsprite, const std::string& swordsprite, const std::string& sword2sprite, TextureAtlas& atlas): m_me(x, y), m_atlas(atlas) {
	this->texs.reserve(4);
	std::array<std::string,4> fnames= {mainsprite, swordsprite, sword2sprite, "shield"}; 
	for (auto& i : fnames){
		this->texs.emplace_back(atlas.findSubTexture(i));
	}
	this->m_spr.setTexture(this->texs[0]);

	Rect<float> tmpAABB = m_spr.getAABB();
	this->m_me.setSize(tmpAABB.right - tmpAABB.left, tmpAABB.bottom - tmpAABB.top);
	this->m_spr2.setTexture(this->texs[1]);
	this->m_shieldspr.setTexture(this->texs[3]);
	this->m_spr.setStencil(true);
	this->m_me.maxFallSpeed = 1300.f;

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
void Character::Update(float dt, const CMap& map, const std::vector<PPCollider*>& objects, Window& window) {
	this->m_me.Update(dt, map);
	const MEState& state = this->m_me.getState();
	WindowState& ws = window.getWindowState();
	bool onGround = state.atFloor;
	float yspd  = state.yspeed;
	float xspd  = state.xspeed;
	float xpos  = state.centerX();
	float ypos  = state.centerY();
	float width = state.width();
	
	if (yspd == 0) {
		onGround |= this->wasOnGround;
	}

	if (!ws.keyboardState[controls.jumpkey]){
		this->jumped = false;
	}

	this->m_me.dropFromOneWay = ws.keyboardState[controls.downkey];

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
	if (ws.keyboardState[controls.shieldkey] && onGround){
		shieldout = true;
		shieldmeter = shieldmeter - 1.0f;
	}else{
		shieldmeter = std::min(shieldmeter + 1.0f, shieldmax);
		shieldout = false;
	}
	yspd += this->gravity * dt;
	if (onGround){
		if (ws.keyboardState[controls.leftkey] == ws.keyboardState[controls.rightkey]) {
			xspd = 0.0f;
		}
		if (ws.keyboardState[controls.jumpkey]) {
			yspd = -this->jumpSpeed;
			this->jumped = true;
		}
		this->walljumpstate = WallType::FLOOR;
	}else{
		shieldout = false;
		if (yspd > 0.f){
			yspd = std::min(yspd, this->m_me.maxFallSpeed);
		}else if (!ws.keyboardState[controls.jumpkey]){
			yspd = std::max(yspd, this->lowJumpSpeed);
		}
		if (ws.keyboardState[controls.leftkey] == ws.keyboardState[controls.rightkey]){
			if (xspd < 0.f) {
				xspd = std::min(0.f, xspd+100.0f);
			} else if (xspd > 0.f) {
				xspd = std::max(0.f, xspd-100.0f);
			}
		}
		isOnLedge = false;
/*      ledge handling code temporarily disabled
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
		}*/
		if (ws.keyboardState[controls.jumpkey]){
			if ((walljumpstate == WallType::FLOOR)||(walljumpstate == WallType::RWALL)){
				if (state.atLeftWall&&(!this->jumped)){
					yspd = -this->jumpSpeed;
					this->jumped = true;
					this->walljumpstate = WallType::LWALL;
				}
			}
			if ((walljumpstate == WallType::FLOOR)||(walljumpstate == WallType::LWALL)){
				if (state.atRightWall&&(!this->jumped)){
					yspd = -this->jumpSpeed;
					this->jumped = true;
					this->walljumpstate = WallType::RWALL;
				}
			}
		}
	}
	if (ws.keyboardState[controls.leftkey] != ws.keyboardState[controls.rightkey]){
		if (ws.keyboardState[controls.leftkey]){
			xspd = this->minSpeed;
		}else{
			xspd = this->maxSpeed;
		}
	}

	if (!isOnLedge){
		if (ws.keyboardState[controls.leftkey]&&(!flipped)){
			flipped = true;
			this->m_spr.flip();
			this->m_spr2.flip();
		}else if (ws.keyboardState[controls.rightkey]&&flipped){
			flipped = false;
			this->m_spr.flip();
			this->m_spr2.flip();
		}
	}
	this->m_shieldspr.setPosition(xpos, ypos);
	int posx = xpos;
	int posy = ypos;
	Rect<float> tmp2 = m_spr2.getAABB();
	if (flipped){
		if (swordout){
			posx = posx + 12 - (tmp2.right - tmp2.left);
		}else{
			posx = posx - width/2 + ((tmp2.right - tmp2.left)/2);
			posy = posy - ((tmp2.bottom - tmp2.top)/2) + 12;
		}
	}else{
		if (swordout){
			posx = posx + width - 20;
		}else{
			posx = posx + (width/2) - ((tmp2.right - tmp2.left)/2) - 2;
			posy = posy + 12 - (tmp2.bottom - tmp2.top)/2;
		}
	}
	this->m_spr.setPosition(xpos, ypos);
	this->m_spr2.setPosition(posx, posy);
	if (invltimer()){
		for (auto& i : objects){
			if (i->collides(this->m_spr)){
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

	this->wasOnGround = state.atFloor;
	this->m_me.setSpeed(xspd, yspd);
//	ws.camera->ScrollTo({m_position.x - m_width/2.f, m_position.y - m_height/2.f, static_cast<float>(m_width), static_cast<float>(m_height)});
}

void Character::warpto(float x, float y) {
	this->m_me.warpto(x, y);
}

const Rect<float>& Character::getAABB() const {
	return this->m_me.getState().hitbox;
}

bool Character::collides(const Sprite& other) const {
	return this->swordout && this->m_spr2.collides(other);
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
