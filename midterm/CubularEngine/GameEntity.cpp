#include "GameEntity.h"
#include "glm/gtc/matrix_transform.hpp"

GameEntity::GameEntity(Mesh * mesh, 
    Material * material,
    glm::vec3 position, 
    glm::vec3 eulerAngles, 
    glm::vec3 scale,
	glm::vec3 color)
{
    this->mesh = mesh;
    this->material = material;
    this->position = position;
    this->eulerAngles = eulerAngles;
    this->scale = scale;
	this->color = color;
    worldMatrix = glm::identity<glm::mat4>();

	this->applyPhysics = true;
}

GameEntity::~GameEntity()
{
}

void GameEntity::Update()
{
	



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

void GameEntity::Render(Camera* camera)
{
    material->Bind(camera, worldMatrix, color);
    mesh->Render();
}