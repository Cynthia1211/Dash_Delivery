#pragma once

#ifdef PLATFORM_WEB
#define ASSET_PATH(filename) "assets/" filename
#else
#define ASSET_PATH(filename) "../assets/" filename
#endif
