/** Functions for access to TIFF image stacks
    \file tiff.hpp
 	\author Frederik Grüll (Frederik.Gruell@iri.uni-frankfurt.de)
**/

#ifndef TIFF_HPP
#define TIFF_HPP


#include <string>
#include <tiffio.h>
#include <ostream>

/// A gray-scale image with 16bit encoding from a TIFF container
class tiff_image16_ref {
  friend class tiff_container;

  public:
    tiff_image16_ref(int height, int width, int bits_per_pixel, int dir_number);
    tiff_image16_ref(tiff_image16_ref const& image);
    ~tiff_image16_ref();

    tiff_image16_ref copy();
    int16 *const *data();
    int16 const *const *data() const;

    int height() const;
    int width() const;
    int scanline_size() const;
    int bytes_per_pixel() const;
    int bits_per_pixel() const;
    int dir_number() const;

		tiff_image16_ref& operator=(tiff_image16_ref const& image);

    tiff_image16_ref& operator<<=(int shift);
    tiff_image16_ref& operator>>=(int shift);
    tiff_image16_ref& operator*=(double d);
    tiff_image16_ref& operator-=(tiff_image16_ref const& image);

    tiff_image16_ref& operator+=(tiff_image16_ref const& image);

    tiff_image16_ref& operator-=(int subtrahend);

    tiff_image16_ref& apply_highpass(int fir_radius);

    int subtr_and_update_bg(tiff_image16_ref const& bg, float img_weight = 1.0/16);

    int substract_meanvalue();
    int calc_meanvalue();

    int histogram();

    void write_as_csv(std::ostream &out);

  private:
    tiff_image16_ref(int16 *const *data, int height, int width,
                 int scanline_size, int bits_per_pixel, int dir_number);

    int sum_halo(int row, int col, int fir_radius, int& halo_count);

    int16 *const *data_;        ///< data array of image
    int height_;                ///< height of the image
    int width_;                 ///< width of the image
    int scanline_size_;         ///< size of a line in bytes
    int bits_per_pixel_;        ///< bits used to store a pixel value
    int *ref_count_;            ///< nr of references that point to same data
    int dir_number_;            ///< number of the image in its tiff container
};

/// A TIFF container
class tiff_container {
  public:
    tiff_container(std::string path, std::string mode);
    ~tiff_container();

    bool good();
    bool writeable();
    std::string const& mode();
    std::string const& path();

    int total_img_count();

    tiff_image16_ref image(int i);
    void append_image(tiff_image16_ref const& image);
    void append_as_8bit_image(tiff_image16_ref const& image, int shift = 0);

  private:

    static void TIFFWarningHandler(const char* module, const char* fmt, va_list ap);

    TIFF *tiff_;              ///< tiff file handle
    std::string path_;        ///< path of the tiff file
    std::string mode_;        ///< opening mode of the tiff file
    bool good_;               ///< indicated wether tiff file could be opened

};


#endif
