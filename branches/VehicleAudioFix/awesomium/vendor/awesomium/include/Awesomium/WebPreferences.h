///
/// @file WebPreferences.h
///
/// @brief The header for the WebPreferences class.
///
/// @author
///
/// This file is a part of Awesomium, a Web UI bridge for native apps.
///
/// Website: <http://www.awesomium.com>
///
/// Copyright (C) 2013 Awesomium Technologies LLC. All rights reserved.
/// Awesomium is a trademark of Awesomium Technologies LLC.
///
#ifndef AWESOMIUM_WEB_PREFERENCES_H_
#define AWESOMIUM_WEB_PREFERENCES_H_

#include <Awesomium/Platform.h>
#include <Awesomium/WebString.h>
#include <Awesomium/WebURL.h>

namespace Awesomium {

///
/// @brief  Use this class to specify preferences for each WebSession.
///
/// @see WebCore::CreateWebSession
///
#pragma pack(push)
#pragma pack(1)
struct OSM_EXPORT WebPreferences {
  ///
  /// Create a default set of Preferences.
  ///
  WebPreferences();

  ///
  /// Whether or not JavaScript should be enabled. (Default: true)
  ///
  bool enable_javascript;

  ///
  /// Whether or not Dart (experimental) should be enabled. (Default: true)
  ///
  bool enable_dart;

  ///
  /// Whether or not plugins (Flash, Silverlight) should be enabled.
  /// (Default: true)
  ///
  bool enable_plugins;

  ///
  /// Whether or not HTML5 Local Storage should be enabled. (Default: true)
  ///
  bool enable_local_storage;

  ///
  /// Whether or not HTML5 Databases should be enabled. (Default: false)
  ///
  bool enable_databases;

  ///
  /// Whether or not HTML5 App Cache should be enabled. (Default: true)
  ///
  bool enable_app_cache;

  ///
  /// Whether or not HTML5 WebAudio should be enabled. (Default: true)
  ///
  bool enable_web_audio;

  ///
  /// Whether or not HTML5 WebGL (experimental) should be enabled.
  /// (Default: false)
  ///
  bool enable_web_gl;

  ///
  /// Whether or not web security should be enabled (prevents cross-domain
  /// requests, for example). (Default: true)
  ///
  bool enable_web_security;

  ///
  /// Whether or not remote fonts should be enabled. (Default: true)
  ///
  bool enable_remote_fonts;

  ///
  /// Whether or not smooth scrolling should be enabled. (Default: false)
  ///
  bool enable_smooth_scrolling;

  ///
  /// Whether or not GPU accelerated compositing (experimental) should be
  /// enabled. This is only compatible with windowed WebViews at this time.
  /// (Default: false)
  ///
  bool enable_gpu_acceleration;

  ///
  /// User-defined CSS to be applied to all web-pages. This is useful for
  /// overriding default styles. (Default: empty)
  ///
  WebString user_stylesheet;

  ///
  /// Proxy configuration string.
  ///
  /// @note  Can either be: "auto" (use the OS proxy config), "none" (ignore
  ///        proxy settings), or you can specify a hardcoded proxy config
  ///        string, for example: "myproxyserver.com:80". (Default: "auto")
  ///
  WebString proxy_config;

  ///
  /// The accept-language for the browser (Default: "en-us,en")
  ///
  WebString accept_language;

  ///
  /// The accept-charset for the browser (Default: "iso-8859-1,*,utf-8")
  ///
  WebString accept_charset;

  ///
  /// The default encoding for the browser (Default: "iso-8859-1")
  ///
  WebString default_encoding;

  ///
  /// Whether or not standalone images should be shrunk to fit the view.
  /// (Default: true)
  ///
  bool shrink_standalone_images_to_fit;

  ///
  /// Whether or not images should be loaded automatically on the page.
  /// (Default: true)
  ///
  bool load_images_automatically;

  ///
  /// Whether or not scripts are allowed to open windows. (Default: true)
  ///
  bool allow_scripts_to_open_windows;

  ///
  /// Whether or not scripts are allowed to close windows. (Default: true)
  ///
  bool allow_scripts_to_close_windows;

  ///
  /// Whether or not scripts are allowed to access the clipboard.
  /// (Default: false)
  ///
  bool allow_scripts_to_access_clipboard;

  ///
  /// Whether or not local files can access other local files. (Default: false)
  ///
  bool allow_universal_access_from_file_url;

  ///
  /// Whether or not scripts are allowed to open windows. (Default: false)
  ///
  bool allow_file_access_from_file_url;

  ///
  /// Whether or not insecure content is displayed and/or run. (Default: true)
  ///
  bool allow_running_insecure_content;
};
#pragma pack(pop)

}  // namespace Awesomium

#endif  // AWESOMIUM_CONFIG_H_
