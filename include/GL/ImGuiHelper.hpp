#ifndef IMHELPER_HPP
#define IMHELPER_HPP
#ifndef NO_IMGUI
#include <GL/Texture.hpp>
void ImImage(const Texture& tex, float width, float height);
bool ImImageButton(const std::string& id, const Texture& tex, float width, float height);
#endif
#endif
