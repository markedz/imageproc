#include <iostream>
#include <sstream>
#include <ios>     // std::fixed
#include <iomanip> // std::setprecision
#include <string>
#include <array>

#include <rawimage.h>
#include <imageproc.h>

using namespace imageproc;

int main(int argc, char **argv) {

  using RawIm = rawimage::RawImage;
  const RawIm::ByteOrder byteOrder = RawIm::ByteOrder::rgb;
  const RawIm::PixFormat pixFormat = RawIm::PixFormat::chr;

  rawimage::init(argc, argv);

  // Ensure proper usage
  if (argc < 2) {
    std::cerr << "Input file not supplied.\n";
    return 1;
  }

  for (int i = 1; i < argc; i++) {
    std::cout << argv[i] << '\n';
    std::string input(argv[i]);

    RawIm img{ byteOrder, pixFormat };
    img.read(input.c_str());

    RawIm img_out{ byteOrder, pixFormat };

    img_out.create(img.getW(), img.getH());

    std::array<unsigned char, 3> sigmas{ 50U, 100U, 150U };
    for (auto sig : sigmas) {
      std::stringstream sigma_out("");

      sigma_filter(img.raw.chr, img_out.raw.chr, img.getW(), img.getH(),
                   img.getDepth(), sig);
      sigma_out << input << ".__sigma_filter_sig=" << static_cast<int>(sig)
                << ".png";
      std::cout << '>' << sigma_out.str() << '\n';
      img_out.save(sigma_out.str().c_str());
      img_out.create(img.getW(), img.getH());
    }

    std::array<float, 3> angles{ 0.5, 1., 1.5 };
    for (auto angle : angles) {
      std::stringstream rotated_out("");
      std::stringstream rotated_fxp_out("");
      rotate(img.raw.chr, img_out.raw.chr, img.getW(), img.getH(),
             img.getDepth(), angle);
      rotated_out << input << "._rotated_ang=" << std::fixed
                  << std::setprecision(2) << angle << ".png";
      std::cout << '>' << rotated_out.str() << '\n';
      img_out.save(rotated_out.str().c_str());
      img_out.create(img.getW(), img.getH());

      rotate_fxp(img.raw.chr, img_out.raw.chr, img.getW(), img.getH(),
                 img.getDepth(), angle);
      rotated_fxp_out << input << "._rotated_fxp_ang=" << std::fixed
                      << std::setprecision(2) << angle << ".png";
      std::cout << '>' << rotated_fxp_out.str() << '\n';
      img_out.save(rotated_fxp_out.str().c_str());
      img_out.create(img.getW(), img.getH());
    }
  }

  return 0;
}
