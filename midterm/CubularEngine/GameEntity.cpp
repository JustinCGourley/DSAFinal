#include "GameEntity.h"
#include "glm/gtc/matrix_transform.hpp"

GameEntity::GameEntity(Mesh * mesh, 
    Material * material,
    glm::vec3 position, 
    glm::vec3 eulerAngles, 
    glm::vec3 scale,
	glm::vec3 color,
	bool applyPhysics,
	glm::vec3 collider,
	float weight)
{
    this->mesh = mesh;
    this->material = material;
    this->position = position;
    this->eulerAngles = eulerAngles;
    this->scale = scale;
	this->color = color;
    worldMatrix = glm::identity<glm::mat4>();

	this->applyPhysics = applyPhysics;
	this->collider = collider;
	this->applyGravity = true;
	this->weight = weight;
	this->velocity = glm::vec3(0, 0, 0);
}

GameEntity::~GameEntity()
{
}

void GameEntity::Update(std::vector<GameEntity*> entities, int num)
{

	if (this->applyPhysics)
	{
		this->UpdatePhysics();
		this->CheckCollisions(entities, num);
		this->UpdatePosition();
	}



	worldMatrix = glm::translate(glm::identity<glm::mat4>(),
		this->position);

	worldMatrix = glm::scale(worldMatrix, this->scale);

	worldMatrix = glm::rotate(worldMatrix,
		this->eulerAngles.y,
		glm::vec3(0.f, 1.f, 0.f)
	);
	worldMatrix = glm::rotate(worldMatrix,
		this->eulerAngles.x,
		glm::vec3(1.f, 0.f, 0.f)
	);
	worldMatrix = glm::rotate(worldMatrix,
		this->eulerAngles.z,
		glm::vec3(0.f, 0.f, 1.f)
	);

}

void GameEntity::UpdatePhysics()
{
	//apply physics calculations

	//apply graivty
	if (applyGravity)
	{
		this->velocity.y -= .0098;
	}

	//check collisions
}

void GameEntity::CheckCollisions(std::vector<GameEntity*> entities, int num)
{
	for (int i = 0; i < entities.size(); i++)
	{
		if (entities[i] != this)
		{
			if ((this->position.x + this->collider.x <= (entities[i]->position.x + entities[i]->collider.x) && this->position.x + this->collider.x >= (entities[i]->position.x - entities[i]->collider.x) ||
				this->position.x - this->collider.x <= (entities[i]->position.x + entities[i]->collider.x) && this->position.x - this->collider.x >= (entities[i]->position.x - entities[i]->collider.x))
				&&
				(this->position.y + this->collider.y <= (entities[i]->position.y + entities[i]->collider.y) && this->position.y + this->collider.y >= (entities[i]->position.y - entities[i]->collider.y) ||
					this->position.y - this->collider.y <= (entities[i]->position.y + entities[i]->collider.y) && this->position.y - this->collider.y >= (entities[i]->position.y - entities[i]->collider.y))
				&&
				(this->position.z + this->collider.z <= (entities[i]->position.z + entities[i]->collider.z) && this->position.z + this->collider.z >= (entities[i]->position.z - entities[i]->collider.z) ||
					this->position.z - this->collider.z <= (entities[i]->position.z + entities[i]->collider.z) && this->position.z - this->collider.z >= (entities[i]->position.z - entities[i]->collider.z)))
			{
				

				//glm::vec3 positionDiff = (entities[i]->position - this->position) - (entities[i]->position - (this->position + this->velocity));
				glm::vec3 positionDiff = (entities[i]->position - (this->position + this->velocity));
				

				if (positionDiff.x != 0)
				{
					if (positionDiff.x < 0 && this->velocity.x < 0)
					{
						float overshot = (this->position.x - this->collider.x) - (entities[i]->position.x + entities[i]->collider.x);
						this->position.x -= overshot;
						this->velocity.x = 0;
					}
					else if (positionDiff.x > 0 && this->velocity.x > 0)
					{
						float overshot = (this->position.x + this->collider.x) - (entities[i]->position.x - entities[i]->collider.x);
						this->position.x -= overshot;
						this->velocity.x = 0;
					}
				}


				if (positionDiff.y != 0)
				{
					if (positionDiff.y < 0 && this->velocity.y < 0)
					{
						float overshot = (this->position.y - this->collider.y) - (entities[i]->position.y + entities[i]->collider.y);
						this->position.y -= overshot;
						this->velocity.y = 0;
					}
					else if (positionDiff.y > 0 && this->velocity.y > 0)
					{
						float overshot = (this->position.y + this->collider.y) - (entities[i]->position.y - entities[i]->collider.y);
						this->position.y -= overshot;
						this->velocity.y = 0;
					}
				}

				if (positionDiff.z != 0)
				{
					if (positionDiff.z < 0 && this->velocity.z < 0)
					{
						float overshot = (this->position.z + this->collider.z) - (entities[i]->position.z - entities[i]->collider.z);
						this->position.z -= overshot;
						this->velocity.z = 0;
					}
					else if (positionDiff.z > 0 && this->velocity.z > 0)
					{
						float overshot = (this->position.z - this->collider.z) - (entities[i]->position.z + entities[i]->collider.z);
						this->position.z -= overshot;
						this->velocity.z = 0;
					}
				}
			}
		}
	}
}

void GameEntity::UpdatePosition()
{
	//apply to position
	this->position += this->velocity;

}

void GameEntity::ApplyForce(glm::vec3 force)
{
	this->velocity += force;
}

void GameEntity::Render(Camera* camera)
{
    material->Bind(camera, worldMatrix, color);
    mesh->Render();
}