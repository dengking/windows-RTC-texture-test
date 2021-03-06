#include "include/windows_texture_test/windows_texture_test_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <memory>
#include <sstream>
#include <thread>
#include <iostream>

namespace
{
  void FillColorBars(FlutterDesktopPixelBuffer *buffer, bool reverse)
  {
    const size_t num_bars = 8;
    const uint32_t bars[num_bars] = {0xFFFFFFFF, 0xFF00C0C0, 0xFFC0C000, 0xFF00C000, 0xFFC000C0, 0xFF0000C0, 0xFFC00000, 0xFF000000};

    uint32_t *word = (uint32_t *)buffer->buffer;

    auto width = buffer->width;
    auto height = buffer->height;
    auto column_width = width / num_bars;

    for (size_t y = 0; y < height; y++)
    {
      for (size_t x = 0; x < width; x++)
      {
        auto index = x / column_width;
        *(word++) = reverse ? bars[num_bars - index - 1] : bars[index];
      }
    }
  }

  void StartTimer(int interval, std::function<void(void)> func)
  {
    std::thread([=]()
                {
      while (true)
      {
        func();
        std::this_thread::sleep_for(
            std::chrono::milliseconds(interval));
      } })
        .detach();
  }

  class ColorBarTexture
  {
  public:
    ColorBarTexture(size_t width, size_t height);
    virtual ~ColorBarTexture();
    const FlutterDesktopPixelBuffer *CopyPixelBuffer(size_t width, size_t height);

  private:
    std::unique_ptr<FlutterDesktopPixelBuffer> buffer1_;
    std::unique_ptr<FlutterDesktopPixelBuffer> buffer2_;
    std::unique_ptr<uint8_t> pixels1_;
    std::unique_ptr<uint8_t> pixels2_;
    size_t request_count_ = 0;
  };

  ColorBarTexture::ColorBarTexture(size_t width, size_t height)
  {
    size_t size = width * height * 4;

    pixels1_.reset(new uint8_t[size]);
    pixels2_.reset(new uint8_t[size]);

    buffer1_ = std::make_unique<FlutterDesktopPixelBuffer>();
    buffer1_->buffer = pixels1_.get();
    buffer1_->width = width;
    buffer1_->height = height;

    buffer2_ = std::make_unique<FlutterDesktopPixelBuffer>();
    buffer2_->buffer = pixels2_.get();
    buffer2_->width = width;
    buffer2_->height = height;

    FillColorBars(buffer1_.get(), false);
    FillColorBars(buffer2_.get(), true);
  }

  const FlutterDesktopPixelBuffer *ColorBarTexture::CopyPixelBuffer(size_t width, size_t height)
  {
    // if (request_count_++ % 2 == 0)
    // {
    //   return buffer2_.get();
    // }

    return buffer1_.get();
  }

  ColorBarTexture::~ColorBarTexture() {}

  class WindowsTextureTestPlugin : public flutter::Plugin
  {
  public:
    static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

    WindowsTextureTestPlugin(flutter::TextureRegistrar *textures);

    virtual ~WindowsTextureTestPlugin();

  private:
    // Called when a method is called on this plugin's channel from Dart.
    void HandleMethodCall(
        const flutter::MethodCall<flutter::EncodableValue> &method_call,
        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

    flutter::TextureRegistrar *textures_;
    std::unique_ptr<flutter::TextureVariant> texture_;
    std::unique_ptr<ColorBarTexture> color_bar_texture_;
    size_t test_request_count_ = 1;
  };

  // static
  void WindowsTextureTestPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarWindows *registrar)
  {
    auto channel =
        std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
            registrar->messenger(), "windows_texture_test",
            &flutter::StandardMethodCodec::GetInstance());

    auto plugin = std::make_unique<WindowsTextureTestPlugin>(registrar->texture_registrar());

    channel->SetMethodCallHandler(
        [plugin_pointer = plugin.get()](const auto &call, auto result)
        {
          plugin_pointer->HandleMethodCall(call, std::move(result));
        });

    registrar->AddPlugin(std::move(plugin));
  }

  WindowsTextureTestPlugin::WindowsTextureTestPlugin(flutter::TextureRegistrar *textures) : textures_(textures) {}

  WindowsTextureTestPlugin::~WindowsTextureTestPlugin() {}

  void WindowsTextureTestPlugin::HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    const std::string &method_name = method_call.method_name();

    if (method_name.compare("initialize") == 0)
    {
      color_bar_texture_ = std::make_unique<ColorBarTexture>(1920, 1080);

      texture_ = std::make_unique<flutter::TextureVariant>(flutter::PixelBufferTexture([this](size_t width, size_t height) -> const FlutterDesktopPixelBuffer *
                                                                                       { return color_bar_texture_->CopyPixelBuffer(width, height); }));

      int64_t texture_id = textures_->RegisterTexture(texture_.get());

      auto response = flutter::EncodableValue(flutter::EncodableMap{
          {flutter::EncodableValue("textureId"),
           flutter::EncodableValue(texture_id)},
      });

      result->Success(response);

      // Update the texture @ 30 Hz
      // Setting this to 60 Hz might cause epileptic shocks :D
      StartTimer(1000 / 50, [&, texture_id]()
                 { textures_->MarkTextureFrameAvailable(texture_id); });
    }
    else if (method_name.compare("test") == 0)
    {
      auto response = flutter::EncodableValue(flutter::EncodableMap{
          {flutter::EncodableValue("response"),
           flutter::EncodableValue(std::to_string(test_request_count_))},
      });
      ++test_request_count_;
      result->Success(response);
    }
    else
    {
      result->NotImplemented();
    }
  }

} // namespace

void WindowsTextureTestPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar)
{
  WindowsTextureTestPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
