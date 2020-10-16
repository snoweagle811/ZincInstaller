#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "MyApp.h"

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;

HWND window = nullptr;
RefPtr<Overlay> overlay_;
ultralight::RefPtr<ultralight::View> view;

MyApp::MyApp()
{

  ultralight::Config config;
  ultralight::Settings settings;

  config.override_ram_size = 30000000;

  settings.app_name = "Zinc Installer";
  settings.developer_name = "Zinc DevTeam";

  ///
  /// Create our main App instance.
  ///
  app_ = App::Create(settings, config);

  ///
  /// Create a resizable window by passing by OR'ing our window flags with
  /// kWindowFlags_Resizable.
  ///
  window_ = Window::Create(app_->main_monitor(), WINDOW_WIDTH, WINDOW_HEIGHT,
                           false, kWindowFlags_Titled | kWindowFlags_Resizable | kWindowFlags_Maximizable);

  window = reinterpret_cast<HWND>(window_->native_handle());

  ShowWindow(window, SW_HIDE);

  (*window_).SetTitle("Loading Zinc Installer...");

  ///
  /// Tell our app to use 'window' as our main window.
  ///
  /// This call is required before creating any overlays or calling App::Run
  ///
  app_->set_window(*window_.get());

  ///
  /// Create our HTML overlay-- we don't care about its initial size and
  /// position because it'll be calculated when we call OnResize() below.
  ///
  overlay_ = Overlay::Create(*window_.get(), 1, 1, 0, 0);

  ///
  /// Force a call to OnResize to perform size/layout of our overlay.
  ///
  OnResize(window_->width(), window_->height());

  ///
  /// Load a page into our overlay's View
  ///
  overlay_->view()->LoadURL("file:///index.html");
  view = overlay_->view();

  ///
  /// Register our MyApp instance as an AppListener so we can handle the
  /// App's OnUpdate event below.
  ///
  app_->set_listener(this);

  ///
  /// Register our MyApp instance as a WindowListener so we can handle the
  /// Window's OnResize event below.
  ///
  window_->set_listener(this);

  ///
  /// Register our MyApp instance as a LoadListener so we can handle the
  /// View's OnFinishLoading and OnDOMReady events below.
  ///
  overlay_->view()->set_load_listener(this);
  auto view_ = &overlay_->view().get();

  ///
  /// Register our MyApp instance as a ViewListener so we can handle the
  /// View's OnChangeCursor and OnChangeTitle events below.
  ///
  overlay_->view()->set_view_listener(this);
}

MyApp::~MyApp()
{
}

void MyApp::Run()
{
  app_->Run();
}

void MyApp::OnUpdate()
{
  ///
  /// This is called repeatedly from the application's update loop.
  ///
  /// You should update any app logic here.
  ///
}

void MyApp::OnClose()
{
}

void MyApp::OnResize(uint32_t width, uint32_t height)
{
  ///
  /// This is called whenever the window changes size (values in pixels).
  ///
  /// We resize our overlay here to take up the entire window.
  ///
  overlay_->Resize(width, height);
}

void MyApp::OnFinishLoading(ultralight::View *caller,
                            uint64_t frame_id,
                            bool is_main_frame,
                            const String &url)
{
  ///
  /// This is called when a frame finishes loading on the page.
  ///

  ShowWindow(window, SW_SHOW);
  CenterWindow();
}

void MyApp::OnDOMReady(ultralight::View *caller,
                       uint64_t frame_id,
                       bool is_main_frame,
                       const String &url)
{
  ///
  /// This is called when a frame's DOM has finished loading on the page.
  ///
  /// This is the best time to setup any JavaScript bindings.
  ///

  Ref<JSContext> context = caller->LockJSContext();
  JSContextRef ctx = context.get();
  
  JSStringRef name = JSStringCreateWithUTF8CString("SendMessage");

  JSObjectRef func = JSObjectMakeFunctionWithCallback(ctx, name, Message);

  JSObjectSetProperty(ctx, JSContextGetGlobalObject(ctx), name, func, 0, 0);

  JSStringRelease(name);
}

void MyApp::OnChangeCursor(ultralight::View *caller,
                           Cursor cursor)
{
  ///
  /// This is called whenever the page requests to change the cursor.
  ///
  /// We update the main window's cursor here.
  ///
  window_->SetCursor(cursor);
}

void MyApp::OnChangeTitle(ultralight::View *caller,
                          const String &title)
{
  ///
  /// This is called whenever the page requests to change the title.
  ///
  /// We update the main window's title here.
  ///
  window_->SetTitle(title.utf8().data());
}

JSValueRef Message(JSContextRef ctx, JSObjectRef function,
  JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[],
  JSValueRef* exception)
{
  JSStringRef argstr = JSValueToStringCopy(ctx, arguments[0], NULL);

  size_t buffer_size = JSStringGetMaximumUTF8CStringSize(argstr);
  char* url = new char[buffer_size];
  JSStringGetUTF8CString(argstr, url, buffer_size);

  std::thread downloadThread(DownloadProcess, (const char*)url, view, GetExePath(), window);

  downloadThread.detach();

  return JSValueRef(NULL);
}

void DownloadProcess(const char* url, ultralight::RefPtr<ultralight::View> view, const char* exePath, HWND hwnd) {
  HRESULT hr;
  std::string filePath = std::filesystem::temp_directory_path().string() + "Zinc.zip";

  DeleteUrlCacheEntry(url);

  hr = URLDownloadToFile(nullptr, url, filePath.c_str(), 0, nullptr);
  if (SUCCEEDED(hr))
  {
    std::filesystem::path exePath(GetExePath());
    std::string cmd = exePath.parent_path().string(); //No ending backsplash
    cmd += "\\7zip";
    cmd += "\\7z x ";
    cmd += filePath;
    cmd += " -y -o\"";
    cmd += std::filesystem::temp_directory_path().string();
    cmd += "\"";
    WinExec(cmd.c_str(), SW_HIDE);

    MessageBox(window, cmd.c_str(), "DEBUG", MB_OK | MB_ICONINFORMATION);
  }

  return;
}

const char* GetExePath() {
  char* pathBuffer = new char[MAX_PATH];
  GetModuleFileName(NULL, pathBuffer, MAX_PATH);

  return (const char*)pathBuffer;

  free(pathBuffer);
}

void CenterWindow()
{
  RECT rect;
  GetWindowRect(window, &rect);
  LPRECT prc = &rect;

  // Get main monitor
  HMONITOR hMonitor = MonitorFromPoint({1, 1}, MONITOR_DEFAULTTONEAREST);

  MONITORINFO mi;
  RECT rc;
  int w = prc->right - prc->left;
  int h = prc->bottom - prc->top;

  mi.cbSize = sizeof(mi);
  GetMonitorInfo(hMonitor, &mi);

  rc = mi.rcMonitor;

  prc->left = rc.left + (rc.right - rc.left - w) / 2;
  prc->top = rc.top + (rc.bottom - rc.top - h) / 2;
  prc->right = prc->left + w;
  prc->bottom = prc->top + h;

  SetWindowPos(window, NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}