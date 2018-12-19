#include "GameEntity.h"
#include "glm/gtc/matrix_transform.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

GameEntity::GameEntity(
	Mesh * mesh, 
    Material * material,
    glm::vec3 position, 
    glm::vec3 eulerAngles, 
    glm::vec3 scale,
	glm::vec3 color,
	bool applyPhysics,
	glm::vec3 collider,
	float weight,
	std::string tag)
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
	this->tag = tag;
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
	else {
		this->velocity = glm::vec3(0, 0, 0);
	}

	worldMatrix = glm::translate(glm::identity<glm::mat4>(),
		this->position);

	/*worldMatrix = glm::rotate(worldMatrix,
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
	);*/

	glm::mat4 rotationMatrix = glm::toMat4(glm::quat(glm::vec3(eulerAngles.x, eulerAngles.y, eulerAngles.z)));

	worldMatrix = worldMatrix * rotationMatrix;

	worldMatrix = glm::scale(worldMatrix, this->scale);

	

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
				
				if (entities[i]->tag == std::string("Floor")) {
					entities[i]->velocity = glm::vec3(0.f, 0.f, 0.f);
					entities[i]->weight = this->weight;
				}

				//glm::vec3 positionDiff = (entities[i]->position - this->position) - (entities[i]->position - (this->position + this->velocity));
				glm::vec3 positionDiff = ((entities[i]->position + entities[i]->velocity) - (this->position + this->velocity));
				
				if (entities[i]->tag != std::string("Floor")) {
					if (positionDiff.x != 0 && abs(positionDiff.x) > abs(positionDiff.y) && abs(positionDiff.x) > abs(positionDiff.z))
					{
						if (positionDiff.x < 0 && this->velocity.x < 0)
						{
							float overshot = (this->position.x - this->collider.x) - (entities[i]->position.x + entities[i]->collider.x);
							this->position.x -= overshot;

							float thisVel = UpdateLinearMomentum(this->velocity.x, this->weight, entities[i]->velocity.x, entities[i]->weight);
							float entityVel = UpdateLinearMomentum(entities[i]->velocity.x, entities[i]->weight, this->velocity.x, this->weight);
							this->velocity.x = thisVel;
							entities[i]->velocity.x = entityVel;
						}
						else if (positionDiff.x > 0 && this->velocity.x > 0)
						{
							float overshot = (this->position.x + this->collider.x) - (entities[i]->position.x - entities[i]->collider.x);
							this->position.x -= overshot;

							float thisVel = UpdateLinearMomentum(this->velocity.x, this->weight, entities[i]->velocity.x, entities[i]->weight);
							float entityVel = UpdateLinearMomentum(entities[i]->velocity.x, entities[i]->weight, this->velocity.x, this->weight);
							this->velocity.x = thisVel;
							entities[i]->velocity.x = entityVel;
						}
					}
				}

				if ((positionDiff.y != 0 && abs(positionDiff.y) > abs(positionDiff.x) && abs(positionDiff.y) > abs(positionDiff.z)) || entities[i]->tag == std::string("Floor"))
				{
					if (positionDiff.y < 0 && this->velocity.y < 0)
					{
						float overshot = (this->position.y - this->collider.y) - (entities[i]->position.y + entities[i]->collider.y);
						this->position.y -= overshot;
						
						//mass dependent
						float thisVel = UpdateLinearMomentum(this->velocity.y, this->weight, entities[i]->velocity.y, entities[i]->weight);
						float entityVel = UpdateLinearMomentum(entities[i]->velocity.y, entities[i]->weight, this->velocity.y, this->weight);
						this->velocity.y = thisVel;
						entities[i]->velocity.y = entityVel;

						//mass independent
						/*float vel = this->velocity.y;
						this->velocity.y = entities[i]->velocity.y;
						entities[i]->velocity.y = vel;*/
					}
					else if (positionDiff.y > 0 && this->velocity.y > 0)
					{
						float overshot = (this->position.y + this->collider.y) - (entities[i]->position.y - entities[i]->collider.y);
						this->position.y -= overshot;
						
						float thisVel = UpdateLinearMomentum(this->velocity.y, this->weight, entities[i]->velocity.y, entities[i]->weight);
						float entityVel = UpdateLinearMomentum(entities[i]->velocity.y, entities[i]->weight, this->velocity.y, this->weight);
						this->velocity.y = thisVel;
						entities[i]->velocity.y = entityVel;
					}
				}

				if (positionDiff.z != 0 && abs(positionDiff.z) > abs(positionDiff.x) && abs(positionDiff.z) > abs(positionDiff.x))
				{
					if (positionDiff.z < 0 && this->velocity.z < 0)
					{
						float overshot = (this->position.z + this->collider.z) - (entities[i]->position.z - entities[i]->collider.z);
						this->position.z -= overshot;

						float thisVel = UpdateLinearMomentum(this->velocity.z, this->weight, entities[i]->velocity.z, entities[i]->weight);
						float entityVel = UpdateLinearMomentum(entities[i]->velocity.z, entities[i]->weight, this->velocity.z, this->weight);
						this->velocity.z = thisVel;
						entities[i]->velocity.z = entityVel;
					}
					else if (positionDiff.z > 0 && this->velocity.z > 0)
					{
						float overshot = (this->position.z - this->collider.z) - (entities[i]->position.z + entities[i]->collider.z);
						this->position.z -= overshot;
						
						float thisVel = UpdateLinearMomentum(this->velocity.z, this->weight, entities[i]->velocity.z, entities[i]->weight);
						float entityVel = UpdateLinearMomentum(entities[i]->velocity.z, entities[i]->weight, this->velocity.z, this->weight);
						this->velocity.z = thisVel;
						entities[i]->velocity.z = entityVel;
					}
				}
			}
		}
	}
}

float GameEntity::UpdateLinearMomentum(float vel1, float mass1, float vel2, float mass2) {
	float numerator = (mass1 - mass2)*vel1 + (2 * mass2*vel2);
	float denominator = mass1 + mass2;
	//std::cout << "Numerator: " << numerator<< std::endl;
	//std::cout << "Denominator: " << denominator << std::endl;
	return numerator / denominator;
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