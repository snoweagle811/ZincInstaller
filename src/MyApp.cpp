#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "MyApp.h"
#include <Windows.h>
#include <iostream>
#include <Windows.h>
#include <WinInet.h>
#include <CommCtrl.h>
#include <filesystem>
#include <7zpp/7zpp.h>

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;

HWND window = nullptr;
RefPtr<Overlay> overlay_;

MyApp::MyApp() {

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

  ///
  /// Register our MyApp instance as a ViewListener so we can handle the
  /// View's OnChangeCursor and OnChangeTitle events below.
  ///
  overlay_->view()->set_view_listener(this);
}

MyApp::~MyApp() {
}

void MyApp::Run() {
  app_->Run();
}

void MyApp::OnUpdate() {
  ///
  /// This is called repeatedly from the application's update loop.
  ///
  /// You should update any app logic here.
  ///
}

void MyApp::OnClose() {
}

void MyApp::OnResize(uint32_t width, uint32_t height) {
  ///
  /// This is called whenever the window changes size (values in pixels).
  ///
  /// We resize our overlay here to take up the entire window.
  ///
  overlay_->Resize(width, height);
}

void MyApp::OnFinishLoading(ultralight::View* caller,
                            uint64_t frame_id,
                            bool is_main_frame,
                            const String& url) {
  ///
  /// This is called when a frame finishes loading on the page.
  ///

  ShowWindow(window, SW_SHOW);
  CenterWindow();
}

void MyApp::OnDOMReady(ultralight::View* caller,
                       uint64_t frame_id,
                       bool is_main_frame,
                       const String& url) {
  ///
  /// This is called when a frame's DOM has finished loading on the page.
  ///
  /// This is the best time to setup any JavaScript bindings.
  ///

  Ref<JSContext> context = caller->LockJSContext();
  SetJSContext(context.get());

  JSObject global = JSGlobalObject();

  global["SendMessage"] = BindJSCallback(&MyApp::Message);

}

void MyApp::OnChangeCursor(ultralight::View* caller,
                           Cursor cursor) {
  ///
  /// This is called whenever the page requests to change the cursor.
  ///
  /// We update the main window's cursor here.
  ///
  window_->SetCursor(cursor);
}

void MyApp::OnChangeTitle(ultralight::View* caller,
                          const String& title) {
  ///
  /// This is called whenever the page requests to change the title.
  ///
  /// We update the main window's title here.
  ///
  window_->SetTitle(title.utf8().data());
}

JSValue MyApp::Message(const JSObject thisObj, const JSArgs& args) {
  JSString str = args[0];
  auto length = JSStringGetLength(str);
  auto buffer = new char[length];
  JSStringGetUTF8CString(str, buffer, length);

  const char* url = buffer;
  std::string filePath = std::filesystem::temp_directory_path().string() + "Zinc.zip";

  std::cout << "Downloading File..." << std::endl;

  DeleteUrlCacheEntry(url);

  HRESULT hr = URLDownloadToFile(nullptr, url, filePath.c_str(), 0, nullptr);

  if (SUCCEEDED(hr)) {
    std::cout << "Downloaded OK" << std::endl;

    SevenZip::SevenZipLibrary lib;
    lib.Load();
    
    SevenZip::SevenZipExtractor extractor(lib, SevenZip::TString(std::wstring(filePath.begin(), filePath.end())));
    extractor.SetCompressionFormat(SevenZip::CompressionFormat::Zip);
    SevenZip::TString path = filePath;
    SevenZip::TString destPath = filePath;
    std::string destPath = std::filesystem::temp_directory_path().string() + "Zinc";
    extractor.ExtractArchive(SevenZip::TString(std::wstring(destPath.begin(), destPath.end())), nullptr);
  } else {
    std::cout << "Download Failed" << std::endl;
  }

  return JSValue(NULL);
}

 void CenterWindow() {
   RECT rect;
  GetWindowRect(window, &rect);
  LPRECT prc = &rect;

  // Get main monitor
  HMONITOR hMonitor = MonitorFromPoint({ 1,1 }, MONITOR_DEFAULTTONEAREST);

  MONITORINFO mi;
  RECT        rc;
  int         w = prc->right - prc->left;
  int         h = prc->bottom - prc->top;

  mi.cbSize = sizeof(mi);
  GetMonitorInfo(hMonitor, &mi);

  rc = mi.rcMonitor;

  prc->left = rc.left + (rc.right - rc.left - w) / 2;
  prc->top = rc.top + (rc.bottom - rc.top - h) / 2;
  prc->right = prc->left + w;
  prc->bottom = prc->top + h;

  SetWindowPos(window, NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
 }