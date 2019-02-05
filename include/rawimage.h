#ifndef __RAWIMAGE_H
#define __RAWIMAGE_H

#include <iostream>
#include <memory>
#include <cstddef>

namespace rawimage {

class RawImage {
public:
  enum class ByteOrder {
    gray,
    rgb,
    rgba
  };
  enum class PixFormat {
    chr,
    flo
  };
  union {
    unsigned char *chr;
    float *flo;
  } raw{ nullptr };

  RawImage(const RawImage &) = delete;
  RawImage(RawImage &&) = delete;
  RawImage &operator=(const RawImage &);
  RawImage &operator=(RawImage &&) = delete;
  RawImage() = default;
  RawImage(const ByteOrder &byteOrder, const PixFormat &pixFormat);
  virtual ~RawImage();
  void wipe();
  void create(size_t w, size_t h);
  void read(const char *fname);
  void save(const char *fname) const;
  void toGray();
  size_t getW() const;
  size_t getH() const;
  ByteOrder getByteOrder() const;
  PixFormat getPixFormat() const;
  const char *getByteMap() const;
  size_t getDepth() const;
  size_t getCompSize() const;
  static std::unique_ptr<RawImage>
  imgFactory(size_t w, size_t h, const ByteOrder &byteOrder = ByteOrder::rgb,
             const PixFormat &pixFormat = PixFormat::chr);

private:
  size_t w{ 0U };
  size_t h{ 0U };
  ByteOrder byteOrder{ ByteOrder::rgb };
  PixFormat pixFormat{ PixFormat::chr };

  void readImpl(const char *fname);
};

void init(int argc, char **argv);

} /* namespace rawimage */

std::ostream &operator<<(std::ostream &os, const rawimage::RawImage &img);

#endif /* __RAWIMAGE_H */
