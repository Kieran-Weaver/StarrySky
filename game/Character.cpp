#include "Character.hpp"
#include <cmath>
#include <glm/gtx/string_cast.hpp>
#define LASERW 25
#define LASERH 10
Character::Character(float x, float y, ObjMap& map, const std::string& mainsprite, const std::string& swordsprite, const std::string& sword2sprite, TextureAtlas& atlas): MovingEntity(x, y, map), m_atlas(atlas) {
	this->texs.reserve(4);
	std::string fnames[] = {mainsprite, swordsprite, sword2sprite, "shield.png"}; 
	for (int i=0;i<4;i++){
		this->texs[i] = atlas.findSubTexture(fnames[i]);
	}
	this->spr.setTexture(this->texs[0]);
//	this->spr.transform(flipped_mat);
	Rect<float> tmpAABB = spr.getAABB();
	this->width = tmpAABB.width;
	this->height = tmpAABB.height;
	this->spr2.setTexture(this->texs[1]);
	this->shieldspr.setTexture(this->texs[3]);
}
void Character::Update(float dt, std::vector<MovingEntity*>& objects, GLFWwindow* window) {
	WindowState * ws = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
	if (!ws->keyboardState[controls.jumpkey]){
		this->jumped = false;
	}
	this->dropFromOneWay = ws->keyboardState[controls.downkey];
	if (this->swordtimer < 0){
		this->swordtimer++;
	}else if (this->swordtimer > 0){
		this->swordtimer--;
		if (this->swordtimer == 0){
			swordout = false;
			this->spr2.setTexture(this->texs[1]);
			if (flipped){
				this->spr2.transform(this->flipped_mat);
			}
			this->swordtimer = -6;
		}
	}else if ((ws->keyboardState[controls.swordkey])&&(!this->swordout)&&(!this->shieldout)){
		this->swordout = true;
		this->spr2.setTexture(this->texs[2]);
		if (flipped){
			this->spr2.transform(this->flipped_mat);
		}
		ws->keyboardState[controls.swordkey] = false;
		this->swordtimer = 30;
	}
	this->minSpeed = -this->walkSpeed;
	this->maxSpeed = this->walkSpeed;
	if (shieldbroken){
		shieldmeter = std::min(shieldmeter + 0.6f, shieldmax);
		if (shieldmeter >= shieldmax){
			shieldbroken = false;
		}
	}else if (!ws->keyboardState[controls.shieldkey]){
		shieldmeter = std::min(shieldmeter + 1.0f, shieldmax);
		shieldout = false;
	}else if (this->isOnGround){
		shieldout = true;
		shieldmeter = shieldmeter - 1.0f;
	}
	if (this->isOnGround){
		if (ws->keyboardState[controls.leftkey] == ws->keyboardState[controls.rightkey]) {
			this->speed.x = 0.0f;
		}
		if (ws->keyboardState[controls.jumpkey]) {
			this->speed.y = -this->jumpSpeed;
			this->isOnGround = false;
			this->jumped = true;
		}
		this->walljumpstate = WallType::FLOOR;
	}else{
		shieldout = false;
		this->speed.y += this->gravity * dt;
		if (this->speed.y > 0.f){
			this->speed.y = std::min(this->speed.y, this->maxFallSpeed);
		}else if (!ws->keyboardState[controls.jumpkey]){
			this->speed.y = std::max(this->speed.y, this->lowJumpSpeed);
		}
		if (ws->keyboardState[controls.leftkey] == ws->keyboardState[controls.rightkey]){
			if (this->speed.x < 0.f){
				this->speed.x = std::min(0.f,this->speed.x+100.0f);
			}else if (this->speed.x > 0.f){
				this->speed.x = std::max(0.f,this->speed.x-100.0f);
			}
		}
		isOnLedge = false;
		if (ws->keyboardState[controls.ledgekey]){
			Rect<float> thisrect = spr.getAABB();
			for (auto i : map.ledges){
				Rect<float> t(i.x,i.y,map.ledgewidth,map.ledgeheight);
				if (thisrect.Intersects(t)){
					isOnGround = true;
					this->speed = glm::vec2(0.f,0.f);
					this->position = glm::vec2(i.x,i.y);
					isOnLedge = true;
					break;
				}
			}
		}
		if (ws->keyboardState[controls.jumpkey]){
			if ((walljumpstate == WallType::FLOOR)||(walljumpstate == WallType::RWALL)){
				if (this->pushesLeftWall&&(!this->jumped)){
					this->speed.y = -this->jumpSpeed;
					this->jumped = true;
					this->walljumpstate = WallType::LWALL;
				}
			}
			if ((walljumpstate == WallType::FLOOR)||(walljumpstate == WallType::LWALL)){
				if (this->pushesRightWall&&(!this->jumped)){
					this->speed.y = -this->jumpSpeed;
					this->jumped = true;
					this->walljumpstate = WallType::RWALL;
				}
			}
		}
	}
	if (ws->keyboardState[controls.leftkey] != ws->keyboardState[controls.rightkey]){
		if (ws->keyboardState[controls.leftkey]){
			this->speed.x = this->minSpeed;
		}else{
			this->speed.x = this->maxSpeed;
		}
	}

	if (!isOnLedge){
		if (ws->keyboardState[controls.leftkey]&&(!flipped)){
			flipped = true;
			this->spr.transform(this->flipped_mat);
			this->spr2.transform(this->flipped_mat);
		}else if (ws->keyboardState[controls.rightkey]&&flipped){
			flipped = false;
			this->spr.transform(this->flipped_mat);
			this->spr2.transform(this->flipped_mat);
		}
	}
	this->spr.setPosition(this->position);
	this->shieldspr.setPosition(this->position);
	int posx = this->position.x;
	int posy = this->position.y;
	Rect<float> tmp2 = spr2.getAABB();
	if (flipped){
		if (swordout){
			posx = posx + 12 - tmp2.width;
		}else{
			posx = posx - width/2 + (tmp2.width/2);
			posy = posy - (tmp2.height/2) + 12;
		}
	}else{
		if (swordout){
			posx = posx + width - 20;
		}else{
			posx = posx + (width/2) - (tmp2.width/2) - 2;
			posy = posy + 12 - tmp2.height/2;
		}
	}
	this->spr2.setPosition(glm::vec2(posx,posy));
	MovingEntity::Update(dt);
	if (invltimer > 0){
		invltimer--;
	}
	for (auto& i : objects){
		if (m_atlas.PixelPerfectTest(this->spr,i->spr)){
			if (shieldout && !shieldbroken){
				if (invltimer > 0){
					shieldmeter = shieldmeter - 200.f;
					if (shieldmeter < 0.0f){
						shieldbroken = true;
						shieldmeter = 0.0f;
					}
					invltimer = 20;
				}
			}else{
				dead = true;
				break;
			}
		}
	}
	float hScrollSpd = std::max(std::fabs(this->speed.x)*dt,ws->camera->HScrollSpeed);
	float vScrollSpd = std::max(std::fabs(this->speed.y)*dt,ws->camera->VScrollSpeed);
	Rect<float> curScrollRect = ws->camera->scroll_bounds;
	curScrollRect.left += ws->camera->current_bounds.left;
	curScrollRect.top += ws->camera->current_bounds.top;
	glm::vec2 scrollVec(0.f,0.f);
	if (this->position.x > (curScrollRect.left + curScrollRect.width)){
		scrollVec.x = hScrollSpd;
	}else if (this->position.x < curScrollRect.left){
		scrollVec.x = -hScrollSpd;
	}
	if (this->position.y > (curScrollRect.top+curScrollRect.height)){
		scrollVec.y = vScrollSpd;
	}else if (this->position.y < curScrollRect.top){
		scrollVec.y = -vScrollSpd;
	}
	ws->camera->Scroll(scrollVec);
}
void Character::Draw(SpriteBatch& mframe) {
	mframe.Draw(&spr);
	mframe.Draw(&spr2);
	if ((shieldout)&&(!shieldbroken)){
		shieldspr.rotate(1.0f);
		mframe.Draw(&shieldspr);
	}
/*	ImGui::SetNextWindowPos(ImVec2(0.f,0.f),ImGuiCond_Always);
	ImGui::Begin("Shield");
	ImGui::ProgressBar(shieldmeter/shieldmax);
	ImGui::End();*/
}
