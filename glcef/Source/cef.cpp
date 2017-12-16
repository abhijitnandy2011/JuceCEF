#include <stdio.h>
#include <iostream>
#include <sstream>



#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/cef_render_handler.h>
#include <include/cef_life_span_handler.h>
#include <include/cef_load_handler.h>
#include <include/wrapper/cef_helpers.h>

uint32 gBuffer[800 * 600 * 4];

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
		//OutputDebugStringW(L"GetViewRect()\n");
		rect = CefRect(0, 0, width, height);
		return true;
	}

	void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void * buffer, int w, int h)
	{
		//OutputDebugStringW(L"OnPaint()\n");

		memcpy(gBuffer, buffer, w * h * 4);
		
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
		/*if (texture)
		{
			SDL_DestroyTexture(texture);
		}*/

		//texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, w, h);
		//width = w;
		//height = h;
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
		//CEF_REQUIRE_UI_THREAD();

		OutputDebugStringW(L"OnAfterCreated\n");

		browser_id = browser->GetIdentifier();
	}

	bool DoClose(CefRefPtr<CefBrowser> browser)
	{
		// Must be executed on the UI thread.
		//CEF_REQUIRE_UI_THREAD();

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
		OutputDebugStringW(L"OnBeforeClose()\n");
	}

	void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
	{
		OutputDebugStringW(L"OnLoadEnd()\n");
		std::cout << "OnLoadEnd(" << httpStatusCode << ")" << std::endl;
		loaded = true;
	}

	bool OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefLoadHandler::ErrorCode errorCode, const CefString & failedUrl, CefString & errorText)
	{
		OutputDebugStringW(L"OnLoadError()\n");
		loaded = true;
	}

	void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward)
	{
		OutputDebugStringW(L"OnLoadingStateChange()\n");
	}

	void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
	{
		OutputDebugStringW(L"OnLoadStart()\n");
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


class MyRequestContextHandler :public CefRequestContextHandler
{
public:
	MyRequestContextHandler() {
		m_cookieMgr = CefCookieManager::CreateManager("D:/CefCookie", FALSE, nullptr);
	};
	~MyRequestContextHandler() {};
	CefRefPtr<CefCookieManager> GetCookieManager() override {
		return m_cookieMgr;
	}
private:
	CefRefPtr<CefCookieManager> m_cookieMgr;

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(MyRequestContextHandler);
};

CefRefPtr<CefRequestContext> rc;
CefRequestContextSettings reqsettings;

int startCEF(HWND hWnd)
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
		settings.multi_threaded_message_loop = true; // not supported, except windows
		//settings.single_process = true; // not supported, except windows
		//settings.remote_debugging_port = 8090;
		// CefString(&settings.browser_subprocess_path).FromASCII("sub_proccess path, by default uses and starts this executeable as child");
		// CefString(&settings.cache_path).FromASCII("");
		// CefString(&settings.log_file).FromASCII("");
		//settings.log_severity = LOGSEVERITY_DEFAULT;

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
	bool ret = false;
	{
		CefWindowInfo window_info;
		CefBrowserSettings browserSettings;

		// browserSettings.windowless_frame_rate = 60; // 30 is default

		window_info.SetAsWindowless(NULL); // false means no transparency (site background colour)

		browserClient = new BrowserClient(renderHandler);

		//browser = CefBrowserHost::CreateBrowserSync(window_info, browserClient.get(), "http://magpcss.org/ceforum/apidocs/projects/%28default%29/_cef_settings_t.html", browserSettings, nullptr);

		rc = CefRequestContext::CreateContext(reqsettings, new MyRequestContextHandler());
		ret = CefBrowserHost::CreateBrowser(window_info, browserClient.get(), "https://www.youtube.com/watch?v=ncp86cs3j8s", browserSettings, rc);

		// inject user-input by calling - non-trivial for non-windows - checkout the cefclient source and the platform specific cpp, like cefclient_osr_widget_gtk.cpp for linux
		// browser->GetHost()->SendKeyEvent(...);
		// browser->GetHost()->SendMouseMoveEvent(...);
		// browser->GetHost()->SendMouseClickEvent(...);
		// browser->GetHost()->SendMouseWheelEvent(...);
	}	

	return 0;
}

void doMessageLoopWork()
{
	CefDoMessageLoopWork();
}

uint32 *getBuffer()
{
	return gBuffer;
}

void stopCEF()
{
	//browser = nullptr;
	browserClient = nullptr;
	renderHandler = nullptr;
	CefShutdown();
}
