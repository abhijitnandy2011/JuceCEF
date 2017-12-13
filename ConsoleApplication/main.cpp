#include <stdio.h>
#include <iostream>
#include <sstream>



#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/cef_render_handler.h>
#include <include/cef_life_span_handler.h>
#include <include/cef_load_handler.h>
#include <include/wrapper/cef_helpers.h>



class RenderHandler :
	public CefRenderHandler
{
public:
	RenderHandler(/*SDL_Renderer * renderer,*/ int w, int h) :
		width(w),
		height(h)
		//renderer(renderer)
	{
		//texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, w, h);
	}

	~RenderHandler()
	{
		/*if (texture)
		{
		SDL_DestroyTexture(texture);
		}
		renderer = nullptr;*/
	}

	bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
	{
		std::cout << "GetViewRect\n";
		rect = CefRect(0, 0, width, height);
		return true;
	}

	void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void * buffer, int w, int h)
	{
		std::cout << "OnPaint\n";
		
		/*if (texture)
		{
		unsigned char * texture_data = NULL;
		int texture_pitch = 0;

		SDL_LockTexture(texture, 0, (void **)&texture_data, &texture_pitch);
		memcpy(texture_data, buffer, w * h * 4);
		SDL_UnlockTexture(texture);
		}*/
	}

	void resize(int w, int h)
	{
		std::cout << "resize\n";
		/*if (texture)
		{
		SDL_DestroyTexture(texture);
		}*/

		//texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, w, h);
		width = w;
		height = h;
	}

	void render()
	{
		//SDL_RenderCopy(renderer, texture, NULL, NULL);
	}

private:
	int width;
	int height;

	IMPLEMENT_REFCOUNTING(RenderHandler);
};

// for manual render handler
class BrowserClient :
	public CefClient,
	public CefLifeSpanHandler,
	public CefLoadHandler
{
public:
	BrowserClient(CefRefPtr<CefRenderHandler> ptr) :
		handler(ptr)
	{
	}

	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler()
	{
		return this;
	}

	virtual CefRefPtr<CefLoadHandler> GetLoadHandler()
	{
		return this;
	}

	virtual CefRefPtr<CefRenderHandler> GetRenderHandler()
	{
		return handler;
	}

	// CefLifeSpanHandler methods.
	void OnAfterCreated(CefRefPtr<CefBrowser> browser)
	{
		// Must be executed on the UI thread.
		CEF_REQUIRE_UI_THREAD();

		browser_id = browser->GetIdentifier();
	}

	bool DoClose(CefRefPtr<CefBrowser> browser)
	{
		// Must be executed on the UI thread.
		CEF_REQUIRE_UI_THREAD();

		// Closing the main window requires special handling. See the DoClose()
		// documentation in the CEF header for a detailed description of this
		// process.
		if (browser->GetIdentifier() == browser_id)
		{
			// Set a flag to indicate that the window close should be allowed.
			closing = true;
		}

		// Allow the close. For windowed browsers this will result in the OS close
		// event being sent.
		return false;
	}

	void OnBeforeClose(CefRefPtr<CefBrowser> browser)
	{
	}

	void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
	{
		std::cout << "OnLoadEnd(" << httpStatusCode << ")" << std::endl;
		loaded = true;
	}

	bool OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefLoadHandler::ErrorCode errorCode, const CefString & failedUrl, CefString & errorText)
	{
		std::cout << "OnLoadError()" << std::endl;
		loaded = true;
	}

	void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward)
	{
		std::cout << "OnLoadingStateChange()" << std::endl;
	}

	void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
	{
		std::cout << "OnLoadStart()" << std::endl;
	}

	bool closeAllowed() const
	{
		return closing;
	}

	bool isLoaded() const
	{
		return loaded;
	}

private:
	int browser_id;
	bool closing = false;
	bool loaded = false;
	CefRefPtr<CefRenderHandler> handler;

	IMPLEMENT_REFCOUNTING(BrowserClient);
};


CefRefPtr<RenderHandler> renderHandler;

// create browser-window
CefRefPtr<CefBrowser> browser;
CefRefPtr<BrowserClient> browserClient;


int main(int argc, char * argv[])
{
	CefMainArgs args;

	{
		int result = CefExecuteProcess(args, nullptr, nullptr);
		// checkout CefApp, derive it and set it as second parameter, for more control on
		// command args and resources.
		if (result >= 0) // child proccess has endend, so exit.
		{
			return result;
		}
		else if (result == -1)
		{
			// we are here in the father proccess.
		}
	}

	{
		CefSettings settings;

		// CefString(&settings.resources_dir_path).FromASCII("");
		/*{
		std::ostringstream ss;
		ss << SDL_GetBasePath() << "locales/";
		CefString(&settings.locales_dir_path) = ss.str();
		}*/

		// CefString(&settings.locales_dir_path).FromASCII("");
		//CefString(&settings.resources_dir_path) = SDL_GetBasePath();

		// checkout detailed settings options http://magpcss.org/ceforum/apidocs/projects/%28default%29/_cef_settings_t.html
		// nearly all the settings can be set via args too.
		// settings.multi_threaded_message_loop = true; // not supported, except windows
		// CefString(&settings.browser_subprocess_path).FromASCII("sub_proccess path, by default uses and starts this executeable as child");
		// CefString(&settings.cache_path).FromASCII("");
		// CefString(&settings.log_file).FromASCII("");
		// settings.log_severity = LOGSEVERITY_DEFAULT;

		bool result = CefInitialize(args, settings, nullptr, nullptr);
		// CefInitialize creates a sub-proccess and executes the same executeable, as calling CefInitialize, if not set different in settings.browser_subprocess_path
		// if you create an extra program just for the childproccess you only have to call CefExecuteProcess(...) in it.
		if (!result)
		{
			// handle error
			return -1;
		}
	}



	int width = 800;
	int height = 600;


	renderHandler = new RenderHandler(width, height);

	// create browser-window
	{
		CefWindowInfo window_info;
		CefBrowserSettings browserSettings;

		// browserSettings.windowless_frame_rate = 60; // 30 is default

		window_info.SetAsWindowless(NULL); // false means no transparency (site background colour)

		browserClient = new BrowserClient(renderHandler);

		browser = CefBrowserHost::CreateBrowserSync(window_info, browserClient.get(), "http://github.com", browserSettings, nullptr);

		// inject user-input by calling - non-trivial for non-windows - checkout the cefclient source and the platform specific cpp, like cefclient_osr_widget_gtk.cpp for linux
		// browser->GetHost()->SendKeyEvent(...);
		// browser->GetHost()->SendMouseMoveEvent(...);
		// browser->GetHost()->SendMouseClickEvent(...);
		// browser->GetHost()->SendMouseWheelEvent(...);
	}

	while (true) {
		CefDoMessageLoopWork();
	}

	return 0;
}
