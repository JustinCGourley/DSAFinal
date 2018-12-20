#pragma once
#include "stdafx.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"

/// <summary>
/// Represents one 'renderable' objet
/// </summary>
class GameEntity
{
private:
    Mesh* mesh;        
    Material* material;

    //TODO - maybe this should be in a transform class?
    glm::mat4 worldMatrix;

	glm::vec3 velocity;
	glm::vec3 collider;

	void UpdatePhysics();
	void CheckCollisions(std::vector<GameEntity*>, int num);
	float UpdateLinearMomentum(float vel1, float mass1, float vel2, float mass2);
	void UpdatePosition();


public: 
    /// <summary>
    /// Basic paramterized constructor for most of our private vars
    /// </summary>
    GameEntity(
        Mesh* mesh,
        Material* material,
        glm::vec3 position,
        glm::vec3 eulerAngles,
        glm::vec3 scale,
		glm::vec3 color,
		bool applyPhysics,
		glm::vec3 collider,
		float weight,
		std::string tag
    );

	glm::vec3 position;
	glm::vec3 eulerAngles;
	glm::vec3 scale;
	glm::vec3 color;
	bool applyPhysics;
	bool applyGravity;
	float weight;
	float alpha;
	std::string tag;
    /// <summary>
    /// Destruction
    /// </summary>
    virtual ~GameEntity();

    /// <summary>
    /// Updates the worldMatrix
    /// </summary>

    /// <summary>
    /// Renders the gameEntity based on a camera
    /// </summary>
    void Render(Camera* camera);

	virtual void Update(std::vector<GameEntity*>, int num);

	void ApplyForce(glm::vec3 force);

};

