#include <AppCore/AppCore.h>
#include <Windows.h>
#include <iostream>
#include <WinInet.h>
#include <CommCtrl.h>
#include <filesystem>
#include <thread>
#include <future>

#pragma once

using namespace ultralight;

class MyApp : public AppListener,
              public WindowListener,
              public LoadListener,
              public ViewListener {
public:
  MyApp();

  virtual ~MyApp();

  // Start the run loop.
  virtual void Run();

  // This is called continuously from the app's main loop.
  virtual void OnUpdate() override;

  // This is called when the window is closing.
  virtual void OnClose() override;

  // This is called whenever the window resizes.
  virtual void OnResize(uint32_t width, uint32_t height) override;

  // This is called when the page finishes a load in one of its frames.
  virtual void OnFinishLoading(ultralight::View* caller,
                               uint64_t frame_id,
                               bool is_main_frame,
                               const String& url) override;

  // This is called when the DOM has loaded in one of its frames.
  virtual void OnDOMReady(ultralight::View* caller,
                          uint64_t frame_id,
                          bool is_main_frame,
                          const String& url) override;

  // This is called when the page requests to change the Cursor.
  virtual void OnChangeCursor(ultralight::View* caller,
    Cursor cursor) override;

  virtual void OnChangeTitle(ultralight::View* caller,
    const String& title) override;

protected:
  RefPtr<App> app_;
  RefPtr<Window> window_;
};
const char* GetExePath();
void DownloadProcess(const char* url, ultralight::RefPtr<ultralight::View> view, const char* exePath, HWND hwnd);
void CenterWindow();
LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM lParam);

JSValueRef Message(JSContextRef ctx, JSObjectRef function,
  JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[],
  JSValueRef* exception);