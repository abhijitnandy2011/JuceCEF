/*
==============================================================================

This file was auto-generated!

==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"



MainContentComponent::MainContentComponent() :
	textureFromFile(nullptr),
	textScrollPos(200)
{
	setSize(800, 600);

	openGLContext.setRenderer(this);
	openGLContext.attachTo(*this);
	openGLContext.setContinuousRepainting(true);

	newVertexShader =
		"attribute vec4 position;\n"
		"attribute vec4 normal;\n"
		"attribute vec4 sourceColour;\n"
		"attribute vec2 texureCoordIn;\n"
		"\n"
		"uniform mat4 projectionMatrix;\n"
		"uniform mat4 viewMatrix;\n"
		"uniform vec4 lightPosition;\n"
		"\n"
		"varying vec4 destinationColour;\n"
		"varying vec2 textureCoordOut;\n"
		"varying float lightIntensity;\n"
		"\n"
		"void main()\n"
		"{\n"
		"    destinationColour = sourceColour;\n"
		"    textureCoordOut = texureCoordIn;\n"
		"\n"
		"    vec4 light = viewMatrix * lightPosition;\n"
		"    lightIntensity = dot (light, normal);\n"
		"\n"
		"    gl_Position = projectionMatrix * viewMatrix * position;\n"
		"}\n";

	newFragmentShader =
		"varying vec4 destinationColour;\n"
		"varying vec2 textureCoordOut;\n"
		"varying float lightIntensity;\n"
		"\n"
		"uniform sampler2D demoTexture;\n"
		"\n"
		"void main()\n"
		"{\n"
		"    float l = max (0.3, lightIntensity * 0.3);\n"
		"    vec4 colour = vec4 (1, 1, 1, 1.0);\n"
		"    gl_FragColor = colour * texture2D (demoTexture, textureCoordOut);\n"
		"}\n";

}

MainContentComponent::~MainContentComponent()
{
	// texture.release();
	//openGLContext.detach();
	//openGLContext.deactivateCurrentContext();	
}


void MainContentComponent::renderOpenGL()
{
	jassert(OpenGLHelpers::isContextActive());
	OpenGLHelpers::clear(Colours::lightblue);

	updateTextureImage();

	// if (textureFromFile != nullptr)
	//   if (!textureFromFile->applyTo(texture))
	//     textureFromFile = nullptr;

	updateShader();
	if (shader == nullptr)
		return;

	// Having used the juce 2D renderer, it will have messed-up a whole load of GL state, so
	// we need to initialise some important settings before doing our normal GL 3D drawing..
	// glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	openGLContext.extensions.glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	//texture.bind();

	juce::OpenGLFrameBuffer* tex = OpenGLImageType::getFrameBufferFrom(dynamicTextureImage);

	uint32 *pBuffer = getBuffer();
	tex->writePixels((const juce::PixelARGB*)pBuffer, juce::Rectangle<int>(0, 0, 800, 600));

	glBindTexture(GL_TEXTURE_2D, tex->getTextureID());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	shader->use();

	if (uniforms->projectionMatrix != nullptr)
		uniforms->projectionMatrix->setMatrix4(getProjectionMatrix().mat, 1, false);

	if (uniforms->viewMatrix != nullptr)
		uniforms->viewMatrix->setMatrix4(getViewMatrix().mat, 1, false);

	if (uniforms->texture != nullptr)
		uniforms->texture->set((GLint)0);

	//if (uniforms->lightPosition != nullptr)
	//  uniforms->lightPosition->set(-15.0f, 10.0f, 15.0f, 0.0f);

	shape->draw(openGLContext, *attributes);

	// Reset the element buffers so child Components draw correctly
	openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
	openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	textScrollPos += 1.f;

}


/** Returns a new matrix from the given frustrum values. */
Matrix3D<float> orthoProjection(
	float left, float right, float bottom, float top, float nearDistance, float farDistance)
{
	return Matrix3D<float>(
		2.0f / (right - left),  // m00
		0.0f, // m10
		0.0f, // m20
		-(right + left) / (right - left),  // m30

		0.0f, // m01
		2.0f / (top - bottom), // m11
		0.0f,  // m21
		-(top + bottom) / (top - bottom),  // m31

		0.0f, // m02
		0.0f, // m12
		2.0f / (farDistance - nearDistance), // m22
		-(farDistance + nearDistance) / (farDistance - nearDistance),  // m32

		0.0f, // m03
		0.0f,  // m13
		0.0f,  // m23
		1.0f); // m33
}

Matrix3D<float> MainContentComponent::getProjectionMatrix() const
{
	float w = 1.0f / (0.5 + 0.1f);
	float h = w * getLocalBounds().toFloat().getAspectRatio(false);
	//return Matrix3D<float>::fromFrustum(-w, w, h, -h, 4.0f, 30.0f);
	return orthoProjection(w, -w, h, -h, 4.0, 30.f);
}


Matrix3D<float> MainContentComponent::getViewMatrix() const
{
	Matrix3D<float> viewMatrix = /*draggableOrientation.getRotationMatrix()
								 * */ Vector3D<float>(0.0f, 7.5f, -5.0f);

	Matrix3D<float> rotationMatrix = viewMatrix.rotated(Vector3D<float>(0, 0, float_Pi));

	return rotationMatrix * viewMatrix;
}


//==============================================================================
void MainContentComponent::updateShader()
{
	if (newVertexShader.isNotEmpty() || newFragmentShader.isNotEmpty())
	{
		ScopedPointer<OpenGLShaderProgram> newShader(new OpenGLShaderProgram(openGLContext));
		String statusText;

		if (newShader->addVertexShader(OpenGLHelpers::translateVertexShaderToV3(newVertexShader))
			&& newShader->addFragmentShader(OpenGLHelpers::translateFragmentShaderToV3(newFragmentShader))
			&& newShader->link())
		{
			shape = nullptr;
			attributes = nullptr;
			uniforms = nullptr;

			shader = newShader;
			shader->use();

			shape = new OpenGLDemoClasses::Shape(openGLContext);
			attributes = new OpenGLDemoClasses::Attributes(openGLContext, *shader);
			uniforms = new OpenGLDemoClasses::Uniforms(openGLContext, *shader);

			statusText = "GLSL: v" + String(OpenGLShaderProgram::getLanguageVersion(), 2);
		}
		else
		{
			statusText = newShader->getLastError();
		}

		newVertexShader = String::empty;
		newFragmentShader = String::empty;

		/*textureFromFile = new TextureFromFile(
		File("D:/anandy/Code/cef/cefgl/Builds/VisualStudio2013/Ambar.png"));*/
	}
}

void MainContentComponent::updateTextureImage()
{
	// This image is a special framebuffer-backed image, so when we draw to it, the context
	// will render directly into its framebuffer

	if (dynamicTextureImage.isValid())
	{
		dynamicTextureImage.clear(dynamicTextureImage.getBounds(),
			Colours::red.withRotatedHue(fabsf(::sinf(0.0 / 300.0f))).withAlpha(0.7f));

		Graphics g(dynamicTextureImage);

		g.setFont(dynamicTextureImage.getHeight() / 3.0f);
		g.setColour(Colours::black);
		drawScrollingMessage(g, dynamicTextureImage.getHeight() / 2);
	}
}


void MainContentComponent::drawScrollingMessage(Graphics& g, int y) const
{
	/*g.drawSingleLineText("The background, foreground and texture are all being drawn using the OpenGLGraphicsContext class, which "
		"lets you use a standard JUCE 2D graphics context to render directly onto an OpenGL window or framebuffer...  ",
		(int)-std::fmod(textScrollPos, 2500.0f), y);*/
}
