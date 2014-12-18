#include "MeshRenderer.h"


MeshRenderer::MeshRenderer(Mesh* mesh)
{
	this->mesh = mesh;

	bounds = new Bounds(Vector3(), Vector3());
}

Bounds* MeshRenderer::getBounds(){ return bounds; }

//Recalculate the bounds of the mesh
void MeshRenderer::Update()
{
	calculateBounds();
}

//Draw everything in the VBOs
void MeshRenderer::Render()
{
	Material* material = this->getGameObject()->getComponent<Material>();
	GLuint shaderProgram = material->getShaderProgram();

	if (mesh->getTexture() != NULL)
		glBindTexture(GL_TEXTURE_2D, mesh->getTexture());

	GLuint vPosition = glGetAttribLocation(shaderProgram, "vPosition");
	GLuint vNormal = glGetAttribLocation(shaderProgram, "vNormal");
	GLuint vTexCoord = glGetAttribLocation(shaderProgram, "vTexCoord");

	glEnableVertexAttribArray(vPosition);
	glEnableVertexAttribArray(vNormal);
	glEnableVertexAttribArray(vTexCoord);

	int normalOffset = mesh->getPointSize();
	int texCoordOffset = normalOffset + mesh->getNormalSize();

	glBindBuffer(GL_ARRAY_BUFFER, mesh->getVBO());
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(normalOffset));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(texCoordOffset));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getIBO());

	// draw your shape
	glDrawElements(GL_TRIANGLES, mesh->getNumberOfVerts(), GL_UNSIGNED_SHORT, (void *)0);

	//Unbind
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void MeshRenderer::calculateBounds()
{
	float minX = bounds->getMinBound().getX();
	float minY = bounds->getMinBound().getY();
	float minZ = bounds->getMinBound().getZ();

	float maxX = bounds->getMaxBound().getX();
	float maxY = bounds->getMaxBound().getY();
	float maxZ = bounds->getMaxBound().getZ();

	float* points = mesh->getPoints();
	int numOfVerts = mesh->getNumberOfVerts();

	for(int i = 0; i < numOfVerts; i+=3)
	{
		float testX = points[i];
		float testY = points[i+1];
		float testZ = points[i+2];

		if(testX < minX)
			minX = testX;
		else if(testX > maxX)
			maxX = testX;

		if(testY < minY)
			minY = testY;
		else if(testY > maxY)
			maxY = testY;

		if(testZ < minZ)
			minZ = testZ;
		else if(testZ > maxZ)
			maxZ = testZ;
	}

	Vector3 min(minX, minY, minZ);
	Vector3 max(maxX, maxY, maxZ);

	float xWidth = max.getX() - min.getX();
	float yWidth = max.getY() - min.getY();
	float zWidth = max.getZ() - min.getZ();

	Vector3 center;

	GameObject* gameObject = getGameObject();
	if (gameObject)
	{
		Transform* transform = gameObject->getTransform();
		
		center = *transform->getPosition();
	}
	bounds = new Bounds(center, xWidth, yWidth, zWidth);
}

MeshRenderer::~MeshRenderer()
{
}