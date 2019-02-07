#include <iostream>
#include <memory>
#include <cstddef>
#include <cstring>

#include <Magick++.h>

#include "rawimage.h"

namespace rawimage {

RawImage::RawImage(const RawImage::ByteOrder &_byteOrder,
                   const RawImage::PixFormat &_pixFormat)
    : byteOrder(_byteOrder), pixFormat(_pixFormat) {}

RawImage &RawImage::operator=(const RawImage &orig) {

  if (this == &orig)
    return *this;

  // before copying anything, make sure there was nothing created before (delete
  // if it was)
  wipe();
  // now we can copy things
  byteOrder = orig.getByteOrder();
  pixFormat = orig.getPixFormat();
  // copy raw data only if it has meaningful size
  if (orig.getW() * orig.getH() * getDepth() * getCompSize()) {
    w = orig.getW();
    h = orig.getH();
    switch (getPixFormat()) {
    case RawImage::PixFormat::chr:
      raw.chr = new unsigned char[getW() * getH() * getDepth()];
      memcpy(raw.chr, orig.raw.chr,
             getW() * getH() * getDepth() * sizeof(unsigned char));
      break;
    case RawImage::PixFormat::flo:
      raw.flo = new float[getW() * getH() * getDepth()];
      memcpy(raw.flo, orig.raw.flo,
             getW() * getH() * getDepth() * sizeof(float));
      break;
    }
  }
  return *this;
}

RawImage::~RawImage() { wipe(); }

void RawImage::wipe() {
  switch (getPixFormat()) {
  case RawImage::PixFormat::chr:
    if (raw.chr)
      delete[] raw.chr;
    raw.chr = nullptr;
    break;
  case RawImage::PixFormat::flo:
    if (raw.flo)
      delete[] raw.flo;
    raw.flo = nullptr;
    break;
  }
  w = 0U;
  h = 0U;
}

void RawImage::create(size_t _w, size_t _h) {
  // before creating anything, make sure there was nothing created before
  // (delete if it was)
  wipe();
  // create anything only if it has meaningful size
  if (_w * _h * getDepth() * getCompSize()) {
    w = _w;
    h = _h;
    switch (getPixFormat()) {
    case RawImage::PixFormat::chr:
      raw.chr = new unsigned char[getW() * getH() * getDepth()];
      memset(raw.chr, 0, getW() * getH() * getDepth() * sizeof(unsigned char));
      break;
    case RawImage::PixFormat::flo:
      raw.flo = new float[getW() * getH() * getDepth()];
      memset(raw.flo, 0, getW() * getH() * getDepth() * sizeof(float));
      break;
    }
  }
}

void RawImage::readImpl(const char *fname) {
  Magick::Image mimg(fname);

  create(mimg.columns(), mimg.rows());
  switch (getPixFormat()) {
  case RawImage::PixFormat::chr:
    if (raw.chr)
      mimg.write(0U, 0U, getW(), getH(), getByteMap(), Magick::CharPixel,
                 raw.chr);
    break;
  case RawImage::PixFormat::flo:
    if (raw.flo)
      mimg.write(0U, 0U, getW(), getH(), getByteMap(), Magick::FloatPixel,
                 raw.flo);
    break;
  }
}

void RawImage::read(const char *fname) {
  if (RawImage::ByteOrder::gray == getByteOrder()) {
    byteOrder = RawImage::ByteOrder::rgb;
    readImpl(fname);
    toGray();
  } else {
    readImpl(fname);
  }
}

void RawImage::save(const char *fname) const {
  size_t imgSize = getW() * getH();

  switch (getPixFormat()) {
  case RawImage::PixFormat::chr:
    if (raw.chr) {
      if (RawImage::ByteOrder::gray == getByteOrder()) {
        unsigned char *chr = new unsigned char[getW() * getH() * 3U];
        unsigned char *ptr = chr;

        for (size_t u = 0U; u < imgSize; u++) {
          ptr[0] = raw.chr[u];
          ptr[1] = raw.chr[u];
          ptr[2] = raw.chr[u];
          ptr += 3U;
        }
        Magick::Image img(getW(), getH(), "RGB", Magick::CharPixel, chr);
        img.write(fname);
        delete[] chr;
      } else {
        Magick::Image img(getW(), getH(), getByteMap(), Magick::CharPixel,
                          raw.chr);
        img.write(fname);
      }
    }
    break;
  case RawImage::PixFormat::flo:
    if (raw.flo) {
      if (RawImage::ByteOrder::gray == getByteOrder()) {
        float *flo = new float[getW() * getH() * 3U];
        float *ptr = flo;

        for (size_t u = 0U; u < imgSize; u++) {
          ptr[0] = raw.flo[u];
          ptr[1] = raw.flo[u];
          ptr[2] = raw.flo[u];
          ptr += 3U;
        }
        Magick::Image img(getW(), getH(), "RGB", Magick::FloatPixel, flo);
        img.write(fname);
        delete[] flo;
      } else {
        Magick::Image img(getW(), getH(), getByteMap(), Magick::FloatPixel,
                          raw.flo);
        img.write(fname);
      }
    }
    break;
  }
}

void RawImage::toGray() {
  size_t imgSize = getW() * getH();

  if (3U > getDepth())
    return;
  // NOTE that no new array is allocated for the gray image, current one is
  // reused, some of it will be just left unused
  switch (getPixFormat()) {
  case RawImage::PixFormat::chr:
    if (raw.chr) {
      unsigned char *ptr = raw.chr;
      for (size_t u = 0U; u < imgSize; u++) {
        raw.chr[u] = static_cast<unsigned char>(
            (ptr[2] * 0.3) + (ptr[1] * 0.59) + (ptr[0] * 0.11));
        ptr += getDepth();
      }
    }
    break;
  case RawImage::PixFormat::flo:
    if (raw.flo) {
      float *ptr = raw.flo;
      for (size_t u = 0U; u < imgSize; u++) {
        raw.flo[u] = (ptr[2] * 0.3) + (ptr[1] * 0.59) + (ptr[0] * 0.11);
        ptr += getDepth();
      }
    }
    break;
  }
  byteOrder = RawImage::ByteOrder::gray;
}

size_t RawImage::getW() const { return w; }

size_t RawImage::getH() const { return h; }

RawImage::ByteOrder RawImage::getByteOrder() const { return byteOrder; }

RawImage::PixFormat RawImage::getPixFormat() const { return pixFormat; }

const char *RawImage::getByteMap() const {
  switch (getByteOrder()) {
  case RawImage::ByteOrder::gray:
    break;
  case RawImage::ByteOrder::rgb:
    return "RGB";
  case RawImage::ByteOrder::rgba:
    return "RGBA";
  }
  return nullptr;
}

size_t RawImage::getDepth() const {
  switch (getByteOrder()) {
  case RawImage::ByteOrder::gray:
    return 1U;
  case RawImage::ByteOrder::rgb:
    return 3U;
  case RawImage::ByteOrder::rgba:
    return 4U;
  }
  return 0U;
}

size_t RawImage::getCompSize() const {
  switch (getPixFormat()) {
  case RawImage::PixFormat::chr:
    return sizeof(unsigned char);
  case RawImage::PixFormat::flo:
    return sizeof(float);
  }
  return 0U;
}

std::unique_ptr<RawImage>
RawImage::imgFactory(size_t w, size_t h, const RawImage::ByteOrder &byteOrder,
                     const RawImage::PixFormat &pixFormat) {
  std::unique_ptr<RawImage> img(new RawImage(byteOrder, pixFormat));

  img->create(w, h);
  return img;
}

std::ostream &operator<<(std::ostream &os, const RawImage &img) {
  for (size_t row = 0U; row < img.getH(); row++) {
    for (size_t col = 0U; col < img.getW(); col++) {
      os << col << '\t' << row << '\t';
      for (size_t component = 0U; component < img.getDepth(); component++) {
        switch (img.getPixFormat()) {
        case RawImage::PixFormat::chr:
          if (img.raw.chr)
            os << static_cast<unsigned>(
                      img.raw.chr[(row * (img.getW() * img.getDepth())) +
                                  (col * img.getDepth()) + component]) << '\t';
          break;
        case RawImage::PixFormat::flo:
          if (img.raw.flo)
            os << img.raw.flo[(row * (img.getW() * img.getDepth())) +
                              (col * img.getDepth()) + component] << '\t';
          break;
        }
      }
      os << std::endl;
    }
  }
  return os;
}

void init(int argc, char **argv) { Magick::InitializeMagick(*argv); }

} /* namespace rawimage */
