#pragma once
#include "GameObject.h"
#include "Enemy.h"
#include <stack>
#include "Font.h"
#include "Text.h"

class EnemyManager : public GameObject
{
public:
	EnemyManager(Renderer *renderer, Font *font, double *score_holder);
	virtual ~EnemyManager();

	virtual void update(double time);
	virtual std::vector<Rectangle *> get_collider();
	virtual void submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const;
	virtual void handle_external_collisions(const Rectangle *collider, const GameObject *other);
	virtual void pause();
	virtual void unpause();

private:
	Renderer *renderer;
	std::vector<Enemy *> enemies;
	std::vector<Rectangle *> colliders;

	const uint32_t max_enemies = 13;
	double *score;

	double spawn_time;

	Font *score_text_font;
	Text score_text;
	Text score_number_text;
};