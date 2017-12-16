/*
==============================================================================

This file was auto-generated!

==============================================================================
*/

#ifndef MAINCONTENTCOMPONENT_H_INCLUDED
#define MAINCONTENTCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "cef.h"

/** Vertex data to be passed to the shaders.
For the purposes of this demo, each vertex will have a 3D position, a colour and a
2D texture co-ordinate. Of course you can ignore these or manipulate them in the
shader programs but are some useful defaults to work from.
*/

struct OpenGLDemoClasses
{
	struct Vertex
	{
		float position[3];
		float normal[3];
		float colour[4];
		float texCoord[2];
	};

	//==============================================================================
	// This class just manages the attributes that the demo shaders use.
	struct Attributes
	{
		Attributes(OpenGLContext& openGLContext, OpenGLShaderProgram& shader)
		{
			position = createAttribute(openGLContext, shader, "position");
			normal = createAttribute(openGLContext, shader, "normal");
			sourceColour = createAttribute(openGLContext, shader, "sourceColour");
			texureCoordIn = createAttribute(openGLContext, shader, "texureCoordIn");
		}

		void enable(OpenGLContext& openGLContext)
		{
			if (position != nullptr)
			{
				openGLContext.extensions.glVertexAttribPointer(position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
				openGLContext.extensions.glEnableVertexAttribArray(position->attributeID);
			}

			if (normal != nullptr)
			{
				openGLContext.extensions.glVertexAttribPointer(normal->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 3));
				openGLContext.extensions.glEnableVertexAttribArray(normal->attributeID);
			}

			if (sourceColour != nullptr)
			{
				openGLContext.extensions.glVertexAttribPointer(sourceColour->attributeID, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 6));
				openGLContext.extensions.glEnableVertexAttribArray(sourceColour->attributeID);
			}

			if (texureCoordIn != nullptr)
			{
				openGLContext.extensions.glVertexAttribPointer(texureCoordIn->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 10));
				openGLContext.extensions.glEnableVertexAttribArray(texureCoordIn->attributeID);
			}
		}

		void disable(OpenGLContext& openGLContext)
		{
			if (position != nullptr)       openGLContext.extensions.glDisableVertexAttribArray(position->attributeID);
			if (normal != nullptr)         openGLContext.extensions.glDisableVertexAttribArray(normal->attributeID);
			if (sourceColour != nullptr)   openGLContext.extensions.glDisableVertexAttribArray(sourceColour->attributeID);
			if (texureCoordIn != nullptr)  openGLContext.extensions.glDisableVertexAttribArray(texureCoordIn->attributeID);
		}

		ScopedPointer<OpenGLShaderProgram::Attribute> position, normal, sourceColour, texureCoordIn;

	private:
		static OpenGLShaderProgram::Attribute* createAttribute(OpenGLContext& openGLContext,
			OpenGLShaderProgram& shader,
			const char* attributeName)
		{
			if (openGLContext.extensions.glGetAttribLocation(shader.getProgramID(), attributeName) < 0)
				return nullptr;

			return new OpenGLShaderProgram::Attribute(shader, attributeName);
		}
	};

	//==============================================================================
	// This class just manages the uniform values that the demo shaders use.
	struct Uniforms
	{
		Uniforms(OpenGLContext& openGLContext, OpenGLShaderProgram& shader)
		{
			projectionMatrix = createUniform(openGLContext, shader, "projectionMatrix");
			viewMatrix = createUniform(openGLContext, shader, "viewMatrix");
			texture = createUniform(openGLContext, shader, "demoTexture");
			lightPosition = createUniform(openGLContext, shader, "lightPosition");
			bouncingNumber = createUniform(openGLContext, shader, "bouncingNumber");
		}

		ScopedPointer<OpenGLShaderProgram::Uniform> projectionMatrix, viewMatrix, texture, lightPosition, bouncingNumber;

	private:
		static OpenGLShaderProgram::Uniform* createUniform(OpenGLContext& openGLContext,
			OpenGLShaderProgram& shader,
			const char* uniformName)
		{
			if (openGLContext.extensions.glGetUniformLocation(shader.getProgramID(), uniformName) < 0)
				return nullptr;

			return new OpenGLShaderProgram::Uniform(shader, uniformName);
		}
	};

	//==============================================================================
	/** This loads a 3D model from an OBJ file and converts it into some vertex buffers
	that we can draw.
	*/
	struct Shape
	{
		Shape(OpenGLContext& openGLContext)
		{
			/* if (shapeFile.load(BinaryData::teapot_obj).wasOk())
			for (int i = 0; i < shapeFile.shapes.size(); ++i)*/
			vertexBuffers.add(new VertexBuffer(openGLContext));

		}

		void draw(OpenGLContext& openGLContext, Attributes& attributes)
		{

			VertexBuffer& vertexBuffer = *vertexBuffers.getUnchecked(0);
			vertexBuffer.bind();

			attributes.enable(openGLContext);
			glDrawElements(GL_QUADS, vertexBuffer.numIndices, GL_UNSIGNED_INT, 0);
			attributes.disable(openGLContext);

		}

	private:
		struct VertexBuffer
		{
			VertexBuffer(OpenGLContext& context) : openGLContext(context)
			{
				numIndices = 4;

				openGLContext.extensions.glGenBuffers(1, &vertexBuffer);
				openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

				Array<Vertex> vertices;
				Vertex v1 = { -10, 15, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1 };
				vertices.add(v1);

				Vertex v2 = { -10, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0 };
				vertices.add(v2);

				Vertex v3 = { 10, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0 };
				vertices.add(v3);

				Vertex v4 = { 10, 15, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1 };
				vertices.add(v4);

				//createVertexListFromMesh(shape.mesh, vertices, Colours::green);

				openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER, vertices.size() * (int) sizeof(Vertex),
					vertices.getRawDataPointer(), GL_STATIC_DRAW);

				Array<juce::uint32> indices;
				indices.add(0);
				indices.add(1);
				indices.add(2);
				indices.add(3);

				openGLContext.extensions.glGenBuffers(1, &indexBuffer);
				openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
				openGLContext.extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * (int) sizeof(juce::uint32),
					indices.getRawDataPointer(), GL_STATIC_DRAW);
			}

			~VertexBuffer()
			{
				openGLContext.extensions.glDeleteBuffers(1, &vertexBuffer);
				openGLContext.extensions.glDeleteBuffers(1, &indexBuffer);
			}

			void bind()
			{
				openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
				openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			}

			GLuint vertexBuffer, indexBuffer;
			int numIndices;
			OpenGLContext& openGLContext;

			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VertexBuffer)
		};

		//WavefrontObjFile shapeFile;
		OwnedArray<VertexBuffer> vertexBuffers;


	};

};


static Image resizeImageToPowerOfTwo(Image image)
{
	if (!(isPowerOfTwo(image.getWidth()) && isPowerOfTwo(image.getHeight())))
		return image.rescaled(jmin(1024, nextPowerOfTwo(image.getWidth())),
			jmin(1024, nextPowerOfTwo(image.getHeight())));

	return image;
}


struct TextureFromFile
{
	TextureFromFile(const File& file)
	{
		name = file.getFileName();
		image = resizeImageToPowerOfTwo(ImageFileFormat::loadFrom(file));
	}

	Image image;

	bool applyTo(OpenGLTexture& texture)
	{
		texture.loadImage(image);
		return false;
	}

	String name;
};


//==============================================================================
/*
This component lives inside our window, and this is where you should put all
your controls and content.
*/
class MainContentComponent : public Component, private OpenGLRenderer
{
public:
	//==============================================================================
	MainContentComponent();
	~MainContentComponent();

	void renderOpenGL() override;

	void newOpenGLContextCreated() override
	{
		// nothing to do in this case - we'll initialise our shaders + textures
		// on demand, during the render callback.
		freeAllContextObjects();

		dynamicTextureImage = Image(Image::ARGB, 800, 600, true, OpenGLImageType());
	}

	void openGLContextClosing() override
	{
		// When the context is about to close, you must use this callback to delete
		// any GPU resources while the context is still current.
		freeAllContextObjects();
	}

	void freeAllContextObjects()
	{
		shape = nullptr;
		shader = nullptr;
		attributes = nullptr;
		uniforms = nullptr;
	}

	void paint(Graphics& g) override
	{
		// You can add your component specific drawing code here!
		// This will draw over the top of the openGL background.
	}

	void resized() override
	{
		// This is called when the MainContentComponent is resized.
		// If you add any child components, this is where you should
		// update their positions.
		const float desktopScale = (float)openGLContext.getRenderingScale();
		glViewport(0, 0, roundToInt(desktopScale * getWidth()), roundToInt(desktopScale * getHeight()));
		draggableOrientation.setViewport(getLocalBounds());

	}


private:
	//==============================================================================

	// private member variables

	OpenGLContext openGLContext;
	String newVertexShader, newFragmentShader;
	ScopedPointer<OpenGLShaderProgram> shader;
	ScopedPointer<OpenGLDemoClasses::Shape> shape;
	ScopedPointer<OpenGLDemoClasses::Attributes> attributes;
	ScopedPointer<OpenGLDemoClasses::Uniforms> uniforms;

	OpenGLTexture texture;
	TextureFromFile* textureFromFile;

	Image dynamicTextureImage;

	Draggable3DOrientation draggableOrientation;

	Matrix3D<float> getProjectionMatrix() const;
	Matrix3D<float> getViewMatrix() const;

	void updateShader();
	void updateTextureImage();
	void drawScrollingMessage(Graphics& g, int y) const;
	float textScrollPos;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent() { return new MainContentComponent(); }


#endif  // MAINCONTENTCOMPONENT_H_INCLUDED
