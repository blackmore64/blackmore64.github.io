///////////////////////////////////////////////////////////////////////////////
// shadermanager.cpp
// ============
// manage the loading and rendering of 3D scenes
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager *pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = nullptr;			// Changing this entry (and the one below it) from "NULL" to "nullptr"
	delete m_basicMeshes;
	m_basicMeshes = nullptr;
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;
		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.ambientColor = m_objectMaterials[index].ambientColor;
			material.ambientStrength = m_objectMaterials[index].ambientStrength;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationX * rotationY * rotationZ * scale;

	if (nullptr != m_pShaderManager)			// Making a change here, from "NULL" to "nullptr"
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (nullptr != m_pShaderManager)			// Was originally "NULL", changing to "nullptr"
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (nullptr != m_pShaderManager)			// Changing this "NULL" to "nullptr"
	{
		m_pShaderManager->setIntValue(g_UseTextureName, 1);
		// Adding a line here to make the wine bottle work
		m_pShaderManager->setIntValue(g_UseLightingName, true);

		// Adding a "default" material here, for the textures to use.
		OBJECT_MATERIAL def;
		FindMaterial("default", def);
		m_pShaderManager->setVec3Value("material.ambientColor", def.ambientColor);
		m_pShaderManager->setFloatValue("material.ambientStrength", def.ambientStrength);
		m_pShaderManager->setVec3Value("material.diffuseColor", def.diffuseColor);
		m_pShaderManager->setVec3Value("material.specularColor", def.specularColor);
		m_pShaderManager->setFloatValue("material.shininess", def.shininess);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (nullptr != m_pShaderManager)			// Making another "NULL" -> "nullptr" change here.
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	m_pShaderManager->setIntValue("bUseTexture", false);

	// Adding a line here, to try and fix the wine bottle
	m_pShaderManager->setIntValue(g_UseLightingName, true);
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.ambientColor", material.ambientColor);
			m_pShaderManager->setFloatValue("material.ambientStrength", material.ambientStrength);
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/
// Adding in a DefineObjectMaterials helper here for creating a wine bottle (with wine)
void SceneManager::DefineObjectMaterials() {

	// Adding a "default" material here, for the textures to use.
	OBJECT_MATERIAL def;
	def.ambientColor = glm::vec3(1.0);
	def.ambientStrength = 0.0f;
	def.diffuseColor = glm::vec3(1.0f);
	def.specularColor = glm::vec3(0.1f);
	def.shininess = 16.0f;
	def.tag = "default";
	m_objectMaterials.push_back(def);

	// Adding a wine bottle material
	OBJECT_MATERIAL wine;
	wine.ambientColor = glm::vec3(0.2f, 0.005f, 0.2f);  // Creating a dark color
	wine.ambientStrength = 0.15f;
	wine.diffuseColor = glm::vec3(0.1f, 0.05f, 0.1f);  // Creating a purple hue
	wine.specularColor = glm::vec3(0.5f, 0.5f, 0.5f);
	wine.shininess = 180.0f;   // making the glass shiny
	wine.tag = "wineBottle";
	m_objectMaterials.push_back(wine);

	// Adding 3 new materials here (to avoid hitting the texture cap) for the three books on the desk;
	// Each will have similar lighting properties (going for shinier, leather book covers), and each one
	// Will be of a different color (royal blue, red, and a rich, coffee brown).
	OBJECT_MATERIAL royalBlueLeather;
	royalBlueLeather.ambientColor = glm::vec3(0.1f, 0.1f, 0.4f);
	royalBlueLeather.ambientStrength = 0.1f;
	royalBlueLeather.diffuseColor = glm::vec3(0.1f, 0.1f, 0.4f); // Going for a royal blue color
	royalBlueLeather.specularColor = glm::vec3(0.7f, 0.7f, 1.0f);  // Going for a bluish sort of shine
	royalBlueLeather.shininess = 64.0f;
	royalBlueLeather.tag = "royalBlueLeather";
	m_objectMaterials.push_back(royalBlueLeather);

	OBJECT_MATERIAL redLeather;
	redLeather.ambientColor = glm::vec3(0.1f, 0.2f, 0.2f);
	redLeather.ambientStrength = 0.1f;
	redLeather.diffuseColor = glm::vec3(0.5f, 0.05f, 0.05f);
	redLeather.specularColor = glm::vec3(1.0f, 0.05f, 0.05f); // Going for a warmer specular here
	redLeather.shininess = 64.0f;
	redLeather.tag = "redLeather";
	m_objectMaterials.push_back(redLeather);

	OBJECT_MATERIAL coffeeLeather;
	coffeeLeather.ambientColor = glm::vec3(0.1f, 0.05f, 0.025f);
	coffeeLeather.ambientStrength = 0.3f;
	coffeeLeather.diffuseColor = glm::vec3(0.4f, 0.2f, 0.1f);  // Going for a rich brown color here
	coffeeLeather.specularColor = glm::vec3(0.6f, 0.4f, 0.3f);
	coffeeLeather.shininess = 64.0f;
	coffeeLeather.tag = "coffeeLeather";
	m_objectMaterials.push_back(coffeeLeather);

}

// Adding in a SetupSceneLights() helper here for creating lighting for the scene.
void SceneManager::SetupSceneLights() {

	// Turning on the lights
	m_pShaderManager->setBoolValue("bUseLighting", true);

	// Creating the first light (Light [0] ) to create a very soft, white fill - similar
	// To soft, natural sunlight.
	m_pShaderManager->setVec3Value("lightSources[0].position", 20.0f, 30.0f, 3.0f);
	m_pShaderManager->setVec3Value("lightSources[0].ambientColor", 0.1f, 0.1f, 0.1f);
	m_pShaderManager->setVec3Value("lightSources[0].diffuseColor", 0.3f, 0.3f, 0.3f);
	m_pShaderManager->setVec3Value("lightSources[0].specularColor", 0.0f, 0.0f, 0.0f);
	m_pShaderManager->setFloatValue("lightSources[0].focalStrength", 32.0f);
	m_pShaderManager->setFloatValue("lightSources[0].specularIntensity", 0.0f);

	// Creating the second light (Light [1] ) to create a faint, soft, yellowish light,
	// Hoping to emulate the faint hint of sunlight without being overpowering.
	m_pShaderManager->setVec3Value("lightSources[1].position", 50.0f, 0.0f, 20.0f);
	m_pShaderManager->setVec3Value("lightSources[1].ambientColor", 0.1f, 0.08f, 0.04f);
	m_pShaderManager->setVec3Value("lightSources[1].diffuseColor", 0.9f, 0.75f, 0.4f);
	m_pShaderManager->setVec3Value("lightSources[1].specularColor", 0.8f, 0.7f, 0.5f);
	m_pShaderManager->setFloatValue("lightSources[1].focalStrength", 1.0f);
	m_pShaderManager->setFloatValue("lightSources[1].specularIntensity", 0.0f);

}

/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene 
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	// only one instance of a particular mesh needs to be
	// loaded in memory no matter how many times it is drawn
	// in the rendered 3D scene

	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadBoxMesh();  // Loading Box Mesh method to create Desk and other objects
	m_basicMeshes->LoadTaperedCylinderMesh();  // Loading a Tapered Cylinder Mesh, to create a vase for flowers
	m_basicMeshes->LoadCylinderMesh();  // Loading a Cylinder Mesh, to create the stems for the flowers
	m_basicMeshes->LoadPyramid4Mesh();  // Loading a Pyramid Mesh, for the PC monitor's base
	m_basicMeshes->LoadSphereMesh();  // Loading a Sphere mesh, to use for a half-sphere for the wine bottle

	// Creating the use of the textures to be used for the various objects
	CreateGLTexture("../../Utilities/textures/floor_tile.jpg", "floorTile");
	CreateGLTexture("../../Utilities/textures/desk_wood.jpg", "deskWood");
	CreateGLTexture("../../Utilities/textures/desk_metal.jpg", "deskBlotter");
	CreateGLTexture("../../Utilities/textures/flower_stem.png", "stem");
	CreateGLTexture("../../Utilities/textures/clay_vase.png", "clay");
	CreateGLTexture("../../Utilities/textures/red_petal.png", "red_petal");
	CreateGLTexture("../../Utilities/textures/blue_petal.png", "blue_petal");
	CreateGLTexture("../../Utilities/textures/pc_desktop.png", "pc_desktop");
	CreateGLTexture("../../Utilities/textures/pc_plastic.png", "pc_plastic");
	CreateGLTexture("../../Utilities/textures/knife_handle.jpg", "knife_handle");
	CreateGLTexture("../../Utilities/textures/stainless_end.jpg", "stainless");
	CreateGLTexture("../../Utilities/textures/bottle_holder.png", "bottle_holder");
	CreateGLTexture("../../Utilities/textures/white_paint.png", "white_paint");
	CreateGLTexture("../../Utilities/textures/white_paint_2.png", "white_accent");
	CreateGLTexture("../../Utilities/textures/book_pages.png", "book_pages");

	// Calling the helper DefineOjectMaterials() to load in the wine bottle's material.
	DefineObjectMaterials();

	// Calling the helper SetupSceneLights() here to load in the lighting for the scene.
	SetupSceneLights();

}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by 
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{

	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_pShaderManager->use();
	

	BindGLTextures();

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/
	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(20.0f, 1.0f, 10.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);


	// Creating the floor plane with texture
	scaleXYZ = glm::vec3(20.0f, 1.0f, 10.0f);
	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	//SetShaderMaterial("def");
	SetShaderTexture("floorTile");
	SetTextureUVScale(4.0f, 4.0f);  // Making a tile floor, 4x4

	// draw the mesh with transformation values
	m_basicMeshes->DrawPlaneMesh();
	/****************************************************************/

	// Creation of TV Stand / Desk object, using a Box (now with texture)
	scaleXYZ = glm::vec3(20.0f, 8.0f, -1.5f); // Width, Height, and Depth
	positionXYZ = glm::vec3(0.0f, -0.5f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("deskWood");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawBoxMesh();

	// Creating a plane to sit on top of the desk, to appear as a kind of ink blotter
	scaleXYZ = glm::vec3(9.5f, 3.0f, 0.62f); 
	positionXYZ = glm::vec3(0.0f, 3.54f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("deskBlotter");
	SetTextureUVScale(6.0f, 1.0f);
	m_basicMeshes->DrawPlaneMesh();


	// Creating a Tapered Cylinder to sit on the desk, to represent a vase
	scaleXYZ = glm::vec3(0.7f, 0.7f, 0.5f);
	positionXYZ = glm::vec3(-6.0f, 4.25f, -0.25f);
	SetTransformations(scaleXYZ, 180.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("clay");
	SetTextureUVScale(5.0f, 5.0f);
	m_basicMeshes->DrawTaperedCylinderMesh();


	// Creating a set of two slim cylinders, to represent flower stems (to go into the vase)
	scaleXYZ = glm::vec3(0.03f, 2.5f, 0.03f);
	positionXYZ = glm::vec3(-6.4f, 4.25f, -0.1f);
	SetTransformations(scaleXYZ, 5.0f, 0.0f, 20.0f, positionXYZ);
	SetShaderTexture("stem");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawCylinderMesh();

	scaleXYZ = glm::vec3(0.03f, 2.5f, 0.03f);
	positionXYZ = glm::vec3(-5.5f, 4.25f, -0.1f);
	SetTransformations(scaleXYZ, -5.0f, 0.0f, -20.0f, positionXYZ);
	SetShaderTexture("stem");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawCylinderMesh();

	// Creating two tapered cylinders to go with the flower stems (to act as flower bulbs / petals)
	scaleXYZ = glm::vec3(0.4f, 0.4f, 0.4f);
	positionXYZ = glm::vec3(-7.5f, 6.8f, 0.18f);
	SetTransformations(scaleXYZ, -160.0f, 0.0f, -40.0f, positionXYZ);
	SetShaderTexture("red_petal");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawTaperedCylinderMesh();

	scaleXYZ = glm::vec3(0.4f, 0.4f, 0.4f);
	positionXYZ = glm::vec3(-4.55f, 6.8f, -0.3f);
	SetTransformations(scaleXYZ, 150.0f, 60.0f, 40.0f, positionXYZ);
	SetShaderTexture("blue_petal");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawTaperedCylinderMesh();

	// Creating a pyramid for the PC monitor, using a gray plastic texture
	scaleXYZ = glm::vec3(1.0f, 1.0f, 1.0f);
	positionXYZ = glm::vec3(0.0f, 4.05f, -0.29f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("pc_plastic");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawPyramid4Mesh();

	// Creating a plane for the PC monitor's screen
	scaleXYZ = glm::vec3(2.0f, 1.0f, 1.0f);
	positionXYZ = glm::vec3(0.0f, 5.0f, -0.29f);
	SetTransformations(scaleXYZ, 70.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("pc_desktop");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawPlaneMesh();

	// Creating a large box to go beneath the desk, to serve as a kind of pull out drawer
	scaleXYZ = glm::vec3(10.0f, 4.0f, 1.0f);
	positionXYZ = glm::vec3(0.0f, 0.0f, 0.5f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("knife_handle");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawBoxMesh();

	// Creating two cylinders here, which will serve as handles for the pull-out drawer beneath the desk
	scaleXYZ = glm::vec3(0.1f, 2.0f, 0.1f);
	positionXYZ = glm::vec3(-2.0f, 1.0f, 1.1f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 90.0f, positionXYZ);
	SetShaderTexture("stainless");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawCylinderMesh();

	scaleXYZ = glm::vec3(0.1f, 2.0f, 0.1f);
	positionXYZ = glm::vec3(4.0f, 1.0f, 1.1f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 90.0f, positionXYZ);
	SetShaderTexture("stainless");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawCylinderMesh();

	// Creating a plane for the back wall, and using a white paint texture
	scaleXYZ = glm::vec3(20.0f, 100.0f, 10.0f);
	positionXYZ = glm::vec3(0.0f, 10.0f, -2.0f);
	SetTransformations(scaleXYZ, 90.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("white_paint");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawPlaneMesh();

	// Creating 4 Cylinders here (the first two are top and bottom, the last two are the sides)
	// For the rectangular molding on the back wall
	scaleXYZ = glm::vec3(0.3f, 17.0f, 0.3f);
	positionXYZ = glm::vec3(8.0f, 13.0f, -1.8f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 90.0f, positionXYZ);
	SetShaderTexture("white_accent");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawCylinderMesh();

	scaleXYZ = glm::vec3(0.3f, 17.0f, 0.3f);
	positionXYZ = glm::vec3(8.0f, 8.0f, -1.8f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 90.0f, positionXYZ);
	SetShaderTexture("white_accent");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawCylinderMesh();

	// Now for the sides
	scaleXYZ = glm::vec3(0.3f, 5.0f, 0.3f);
	positionXYZ = glm::vec3(-9.0f, 8.0f, -1.8f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("white_accent");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawCylinderMesh();

	scaleXYZ = glm::vec3(0.3f, 5.0f, 0.3f);
	positionXYZ = glm::vec3(8.0f, 8.0f, -1.8f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("white_accent");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawCylinderMesh();

	// Creating a small box here for a stand for the wine bottle
	scaleXYZ = glm::vec3(0.2f, 0.7f, 1.0f);
	positionXYZ = glm::vec3(5.0f, 3.9f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("bottle_holder");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawBoxMesh();

	// Creating several shapes for a wine bottle: a cylinder for the base, a half-sphere for the shoulder,
	// And another cylinder for the neck.  Going to probably have to use a created material for this,
	// To represent a dark, purplish glass (reflecting the wine within).
	
	scaleXYZ = glm::vec3(0.3f, 1.8f, 0.3f);
	positionXYZ = glm::vec3(3.8f, 3.8f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, -70.0f, positionXYZ);
	SetShaderMaterial("wineBottle");
	m_basicMeshes->DrawCylinderMesh();

	scaleXYZ = glm::vec3(0.3f, 0.3f, 0.3f);
	positionXYZ = glm::vec3(5.45f, 4.4f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, -70.0f, positionXYZ);
	SetShaderMaterial("wineBottle");
	m_basicMeshes->DrawHalfSphereMesh();

	scaleXYZ = glm::vec3(0.15f, 0.8f, 0.15f);
	positionXYZ = glm::vec3(5.5f, 4.45f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, -70.0f, positionXYZ);
	SetShaderMaterial("wineBottle");
	m_basicMeshes->DrawCylinderMesh();

	// This next section is going to be creating the three books that are on the right side of the desk,
	// Their covers, spines, and the pages therein.

	// Bottom Cover (Coffee Book)
	scaleXYZ = glm::vec3(1.0f, 0.05f, 1.0f);
	positionXYZ = glm::vec3(9.0f, 3.57f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderMaterial("coffeeLeather");
	m_basicMeshes->DrawBoxMesh();

	scaleXYZ = glm::vec3(0.95f, 0.25f, 0.8f);
	positionXYZ = glm::vec3(9.0f, 3.7f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("book_pages");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawBoxMesh();

	// Top Cover (Coffee Book)
	scaleXYZ = glm::vec3(1.0f, 0.05f, 1.0f);
	positionXYZ = glm::vec3(9.0f, 3.85f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderMaterial("coffeeLeather");
	m_basicMeshes->DrawBoxMesh();

	// Book Spine (Coffee Book)
	scaleXYZ = glm::vec3(0.05f, 0.28f, 1.0f);
	positionXYZ = glm::vec3(8.5f, 3.71f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderMaterial("coffeeLeather");
	m_basicMeshes->DrawBoxMesh();

	// Now for the second book (red), sitting on top of the first.
	// Bottom Cover (Red Book)
	scaleXYZ = glm::vec3(1.0f, 0.05f, 1.0f);
	positionXYZ = glm::vec3(9.0f, 3.9f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, -30.0f, 0.0f, positionXYZ);
	SetShaderMaterial("redLeather");
	m_basicMeshes->DrawBoxMesh();

	scaleXYZ = glm::vec3(0.95f, 0.25f, 0.8f);
	positionXYZ = glm::vec3(9.0f, 4.0f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, -30.0f, 0.0f, positionXYZ);
	SetShaderTexture("book_pages");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawBoxMesh();

	// Top Cover (Red Book)
	scaleXYZ = glm::vec3(1.0f, 0.05f, 1.0f);
	positionXYZ = glm::vec3(9.0f, 4.15f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, -30.0f, 0.0f, positionXYZ);
	SetShaderMaterial("redLeather");
	m_basicMeshes->DrawBoxMesh();

	// Book Spine (Red Book)
	scaleXYZ = glm::vec3(0.05f, 0.28f, 1.0f);
	positionXYZ = glm::vec3(8.56f, 4.03f, -0.25f);
	SetTransformations(scaleXYZ, 0.0f, -30.0f, 0.0f, positionXYZ);
	SetShaderMaterial("redLeather");
	m_basicMeshes->DrawBoxMesh();

	// Lastly, we have the blue book, sitting on top of the other two
	// Bottom Cover (Blue Book)
	scaleXYZ = glm::vec3(1.0f, 0.05f, 1.0f);
	positionXYZ = glm::vec3(9.0f, 4.2f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderMaterial("royalBlueLeather");
	m_basicMeshes->DrawBoxMesh();

	scaleXYZ = glm::vec3(0.95f, 0.25f, 0.8f);
	positionXYZ = glm::vec3(9.0f, 4.3f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("book_pages");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawBoxMesh();

	// Top Cover (Blue Book)
	scaleXYZ = glm::vec3(1.0f, 0.05f, 1.0f);
	positionXYZ = glm::vec3(9.0f, 4.45f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderMaterial("royalBlueLeather");
	m_basicMeshes->DrawBoxMesh();

	// Book Spine (Blue Book)
	scaleXYZ = glm::vec3(0.05f, 0.28f, 1.0f);
	positionXYZ = glm::vec3(8.5f, 4.33f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderMaterial("royalBlueLeather");
	m_basicMeshes->DrawBoxMesh();


}
