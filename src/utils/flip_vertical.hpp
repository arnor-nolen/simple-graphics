#pragma once

#include "utils/SDL.hpp"

auto flip_vertical(const sdl2::unique_ptr<SDL_Surface> &sfc)
    -> sdl2::unique_ptr<SDL_Surface> {
  auto result = sdl2::unique_ptr<SDL_Surface>(SDL_CreateRGBSurface(
      sfc->flags, sfc->w, sfc->h, sfc->format->BytesPerPixel * 8,
      sfc->format->Rmask, sfc->format->Gmask, sfc->format->Bmask,
      sfc->format->Amask));
  const auto pitch = sfc->pitch;
  const auto pxlength = pitch * (sfc->h - 1);
  auto *pixels = static_cast<unsigned char *>(sfc->pixels) + pxlength;
  auto *rpixels = static_cast<unsigned char *>(result->pixels);
  for (auto line = 0; line != sfc->h; ++line) {
    memcpy(rpixels, pixels, pitch);
    pixels -= pitch;
    rpixels += pitch;
  }
  return result;
}