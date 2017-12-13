/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"

static Image resizeImageToPowerOfTwo(Image image)
{
	if (!(isPowerOfTwo(image.getWidth()) && isPowerOfTwo(image.getHeight())))
		return image.rescaled(jmin(1024, nextPowerOfTwo(image.getWidth())),
			jmin(1024, nextPowerOfTwo(image.getHeight())));

	return image;
}

/*struct TextureFromFile
{
	TextureFromFile(const File& file)
	{
		name = file.getFileName();
		image = resizeImageToPowerOfTwo(ImageFileFormat::loadFrom(file));
	}

	Image image;
	String name;

	bool applyTo(OpenGLTexture& texture)
	{
		texture.loadImage(image);
		return false;
	}
};*/

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public OpenGLAppComponent
{
public:
    //==============================================================================
    MainContentComponent()
    {
        setSize (800, 600);
    }

    ~MainContentComponent()
    {
		m_texture.release();
		shutdownOpenGL();
    }

    void initialise() override
    {
		//logoImage = createLogoImage();
		//dynamicTextureImage = Image(Image::ARGB, 128, 128, true, OpenGLImageType());
		auto image = resizeImageToPowerOfTwo(ImageFileFormat::loadFrom(File("E:\\Code\\Juce\\Projects\\glcef\\Builds\\VisualStudio2015\\birds.png")));
		m_texture.loadImage(image);
    }

    void shutdown() override
    {
		logoImage = Image::null;
		dynamicTextureImage = Image::null;
    }

    void render() override
    {
        OpenGLHelpers::clear (Colours::black);

		const float desktopScale = (float)openGLContext.getRenderingScale();

		// Having used the juce 2D renderer, it will have messed-up a whole load of GL state, so
		// we'll put back any important settings before doing our normal GL 3D drawing..
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		openGLContext.extensions.glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);

		glViewport(0, 0, roundToInt(desktopScale * getWidth()), roundToInt(desktopScale * getHeight()));

		m_texture.bind();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Reset the element buffers so child Components draw correctly
		openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
		openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void paint (Graphics& g) override
    {
        // You can add your component specific drawing code here!
        // This will draw over the top of the openGL background.
    }

    void resized() override
    {
        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.
    }


private:
    //==============================================================================

    // private member variables
	juce::Image logoImage, dynamicTextureImage;
	OpenGLTexture m_texture;

	// Functions to create a couple of images to use as textures..
	static Image createLogoImage()
	{
		Image image(Image::ARGB, 256, 256, true, OpenGLImageType());

		if (image.isValid())
		{
			Graphics g(image);

			g.fillAll(Colours::lightgrey.withAlpha(0.8f));
			g.drawImageWithin(ImageFileFormat::loadFrom("birds.png", 3060),
				0, 0, image.getWidth(), image.getHeight(), RectanglePlacement::stretchToFit);			
		}

		return image;
	}


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()    { return new MainContentComponent(); }
