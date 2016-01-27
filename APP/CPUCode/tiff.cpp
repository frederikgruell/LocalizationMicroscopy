/** Functions for access to TIFF image stacks
    \file tiff.cpp
 	\author Frederik Grüll (Frederik.Gruell@iri.uni-frankfurt.de)
**/


#include "tiff.hpp"

#include <iostream>
#include <algorithm>
#include <cstring>   // memset
#include <cassert>
#include <cmath>


/********************** tiff_image **********************************/
// public

/// Create an empty image
/** @param height height of the image in pixels
    @param width  width of the image in pixels
    @param bits_per_pixel range of a pixel value in bits
    @param dir_number Number of the image in the tiff stack
**/
tiff_image16_ref::tiff_image16_ref(int height, int width, int bits_per_pixel,  int dir_number)
  : height_(height), width_(width), scanline_size_(width * sizeof(uint16)),
    bits_per_pixel_(bits_per_pixel), ref_count_(new int), dir_number_(dir_number)
{
	/*
  int16 **data = new int16*[height];
  for(int row = 0; row < height; row++) {
    data[row] = new int16[width];
    memset(data[row], '\0', scanline_size_);
  }*/


  int16 **data = new int16*[height];
  int16 *data_rows = new int16[height * width];

  for(int row = 0; row < (int) height; row++) {
    data[row] =  &data_rows[row * width];
    memset(data[row], '\0', scanline_size_);
  }

  assert(scanline_size_ == width * (int) sizeof(int16));

  data_ = data;
  *ref_count_ = 1;
}

/// Copy an image reference without copying the image
/** @param image The reference to copy
**/
tiff_image16_ref::tiff_image16_ref(tiff_image16_ref const& image)
{
  if(data_ == image.data_) return;

  data_ = image.data_;
  height_ = image.height_;
  width_ = image.width_;
  scanline_size_ = image.scanline_size_;
  bits_per_pixel_ = image.bits_per_pixel_;
  dir_number_ = image.dir_number_;
  ref_count_ = image.ref_count_;

  (*ref_count_)++;
}

/// Image destructor, delete data if last reference to it gets destructed
tiff_image16_ref::~tiff_image16_ref()
{
  (*ref_count_)--;


  if(*ref_count_ == 0) {
    delete[] data_[0];
    delete[] data_;
    delete ref_count_;
  }

  /*
  if(*ref_count_ == 0) {
    for(int row = 0; row < height_; row++) {
      delete[] data_[row];
    }
    delete[] data_;
  }*/


}

/// Copy an image by copying its data
tiff_image16_ref tiff_image16_ref::copy()
{

	int16 **data = new int16*[height_];
	int16 *data_rows = new int16[height_ * width_];
	for(int row = 0; row < height_; row++) {
		data[row] = &data_rows[row * width_];
		memcpy(data[row], data_[row], scanline_size_);
	}

	/*
  int16 **data = new int16*[height_];
  for(int row = 0; row < height_; row++) {
    data[row] = new int16[width_];
    memcpy(data[row], data_[row], scanline_size_);
  }*/

  assert(scanline_size_ == width_ * (int) sizeof(int16));

  return tiff_image16_ref(data, height_, width_, scanline_size_, bits_per_pixel_, dir_number_);
}

/// Return data array of the image
int16 *const *tiff_image16_ref::data()
{
  return data_;
}

/// Return constant array of the data
int16 const* const *tiff_image16_ref::data() const
{
  return data_;
}

/// Return the height of the image
/** @return The height of the image in pixels
**/
int tiff_image16_ref::height() const
{
  return height_;
}

/// Return the width of the image
/** @return The width of the image in pixels
**/
int tiff_image16_ref::width() const
{
  return width_;
}

/// Return the directory number of the image in the tiff stack
/** @return The directory number of the image in the tiff stack
**/
int tiff_image16_ref::dir_number() const
{
  return dir_number_;
}

/// Return the size of one line in bytes of the image
/** @return The size of one line in bytes of the image
**/
int tiff_image16_ref::scanline_size() const
{
  return scanline_size_;
}

/// Return the number of bytes used to store one pixel
/** @return The number of bytes used to store one pixel
**/
int tiff_image16_ref::bytes_per_pixel() const
{
  return scanline_size_ / width_;
}

/// Retunr the range of a pixel value in bits
int tiff_image16_ref::bits_per_pixel() const
{
  return bits_per_pixel_;
}

/// Assignment operator
tiff_image16_ref& tiff_image16_ref::operator=(tiff_image16_ref const& image)
{
	if(data_ == image.data_) return *this;

	(*ref_count_)--;
	if(*ref_count_ == 0) {
    delete[] data_[0];
    delete[] data_;
    delete ref_count_;
  }

  /*
  if(*ref_count_ == 0) {
    for(int row = 0; row < height_; row++) {
      delete[] data_[row];
    }
    delete[] data_;
  }*/

  data_ = image.data_;
  height_ = image.height_;
  width_ = image.width_;
  scanline_size_ = image.scanline_size_;
  bits_per_pixel_ = image.bits_per_pixel_;
  dir_number_ = image.dir_number_;
  ref_count_ = image.ref_count_;

  (*ref_count_)++;

  return *this;
}

/// Shift all pixel values to the left
/** @param shift shift amount
**/
tiff_image16_ref& tiff_image16_ref::operator<<=(int shift)
{
  for(int row = 0; row < height_; row++) {
    for(int col = 0; col < width_; col++) {
      data_[row][col] <<= shift;
    }
  }

  return *this;
}

/// Shift all pixel values to the right
/** @param shift shift amount
**/
tiff_image16_ref& tiff_image16_ref::operator>>=(int shift)
{
  for(int row = 0; row < height_; row++) {
    for(int col = 0; col < width_; col++) {
      data_[row][col] >>= shift;
    }
  }

  return *this;
}


/// Subtract an image from this image
/** @param image The image to subtract
**/
tiff_image16_ref& tiff_image16_ref::operator-=(tiff_image16_ref const& image)
{
  assert(height_ == image.height_ && width_ == image.width_);

  for(int row = 0; row < height_; row++) {
    for(int col = 0; col < width_; col++) {
      data_[row][col] = std::max(data_[row][col] - image.data_[row][col],0);
    }
  }

  return *this;
}

/// Add an image to this image
/** @param image The image to add
**/
tiff_image16_ref& tiff_image16_ref::operator+=(tiff_image16_ref const& image)
{
  assert(height_ == image.height_ && width_ == image.width_);

  for(int row = 0; row < height_; row++) {
    for(int col = 0; col < width_; col++) {
      data_[row][col] = data_[row][col] + image.data_[row][col];
    }
  }

  return *this;
}

/// Multiply every pixel value by a factor
/** @param d Factor to multiply each pixel value with
**/

tiff_image16_ref& tiff_image16_ref::operator*=(double d)
{

  for(int row = 0; row < height_; row++) {
    for(int col = 0; col < width_; col++) {
      data_[row][col] = (int16) (data_[row][col] * d);
    }
  }

  return *this;
}


/// Subtract background from image, update background, and return mean background
/** @param bg Image of the background
    @param img_weight Defines how much of the current image will form the updated background
    @return Mean value of picture
**/
int tiff_image16_ref::subtr_and_update_bg(tiff_image16_ref const& bg, float img_weight)
{
  assert(height_ == bg.height_ && width_ == bg.width_);
  assert(img_weight >= 0 && img_weight <= 1);

  int meanbg = 0;
  float bg_weight = 1.0 - img_weight;

  for(int row = 0; row < height_; row++) {
    for(int col = 0; col < width_; col++) {
      int16 img_data = data_[row][col];
      int16 bg_data = bg.data_[row][col];

      data_[row][col] = std::max(img_data - bg_data, 0);                      // subtract background
      bg.data_[row][col] = (int16) (bg_data * bg_weight  + img_data * img_weight);      // update background
      meanbg += img_data;
    }
  }
  meanbg /= (height_ * width_);

  return meanbg;
}


/// Subtract a value from all pixel values
/** @param subtrahend The value to subtract from each pixel value
**/
tiff_image16_ref& tiff_image16_ref::operator-=(int subtrahend)
{
  for(int row = 0; row < height_; row++) {
    for(int col = 0; col < width_; col++) {
      data_[row][col] = std::max(data_[row][col] - subtrahend, 0);
    }
  }

  return *this;
}


/// Apply a 2D high-pass filter to the image
/** @param fir_radius The radius of the square where the average is calculated from
**/
tiff_image16_ref& tiff_image16_ref::apply_highpass(int fir_radius)
{
  for(int row = 0; row < height_; row++) {
    for(int col = 0; col < width_; col++) {

      int halo_count = 0;
      int halo_acc = sum_halo(row, col, fir_radius, halo_count);

      data_[row][col] = data_[row][col] -halo_acc / halo_count;
    }
  }

  return *this;
}

/// Print a histogram of all pixel values to stdout
int tiff_image16_ref::histogram()
{
  const int HISTSIZE = 5000;
  int *values = new int[HISTSIZE];
  memset(values, '\0', HISTSIZE );
  int maxvalue=0;


  for(int i=0;i < height_;i++)
  {
    for(int j=0;j < width_;j++)
    {
      if(data_[i][j] >= HISTSIZE)
      {
        maxvalue=HISTSIZE;
        values[HISTSIZE-1]++;
      } else {
        values[data_[i][j]]++;
        if(data_[i][j] > maxvalue) {
          maxvalue = data_[i][j];
        }
      }
    }
  }
  /* output */
  for(int i = 0;i <= maxvalue; i++)
  {
    std::cout << i << "\t" << values[i] << std::endl;
  }
  delete values;

  return maxvalue;
}

/// Calculate and subtract the mean pixel value from the image
/** @return The mean pixel value
**/
int tiff_image16_ref::substract_meanvalue()
{

  int meanbg = 0;

  for(int i = 0; i<height_; i++)
  {
    for(int j = 0; j<width_; j++)
    {
      meanbg += data_[i][j];
    }
  }
  meanbg = meanbg / (height_ * width_);

  for(int i = 0; i < height_; i++)
  {
    for(int j = 0; j < width_; j++)
    {
      data_[i][j] = std::max(data_[i][j] - meanbg, 0);
      //data_[i][j] -= value;
    }
  }

  return (int) meanbg;
}


/// Calculate the mean pixel value from the image
/** @return The mean pixel value
**/
int tiff_image16_ref::calc_meanvalue()
{
 int value = 0;

  for(int i = 0; i<height_; i++)
  {
    for(int j = 0; j<width_; j++)
    {
      value += data_[i][j];
    }
  }
  value = value / (height_ * width_);

  //std::cerr << "tiff_image16_ref::calc_meanvalue(): height is " << height_ << ", width is " << width_ << std::endl;

  return value;
}

/// Output the image as CSV
/**  @param out The output stream to print the values to
**/
void tiff_image16_ref::write_as_csv(std::ostream &out)
{
  for(int row = 0; row < height_; row++)
  {
    for(int col = 0; col < width_; col++)
    {
      out << data_[row][col] << "; ";
    }
    out << std::endl;
  }
}


// private
/// Create an image reference from a tiff container
tiff_image16_ref::tiff_image16_ref(int16 *const *data, int height, int width,
                           int scanline_size, int bits_per_pixel, int dir_number)
  : data_(data), height_(height), width_(width), scanline_size_(scanline_size),
    bits_per_pixel_(bits_per_pixel), ref_count_(new int), dir_number_(dir_number)
{
  *ref_count_ = 1;
}

/// Calculate the sum of all pixel values in a halo
int tiff_image16_ref::sum_halo(int row, int col, int fir_radius, int& halo_count)
{
  int halo_acc = 0;
  halo_count = 0;

  for(int row_halo = row - fir_radius; row_halo < row + fir_radius + 1; row_halo++) {
    for(int col_halo = col - fir_radius; col_halo < col + fir_radius + 1; col_halo++) {
      if(row_halo > 0 && row_halo < height_ && col_halo > 0 && col_halo < width_) {
        halo_acc += data_[row_halo][col_halo];
        halo_count++;
      }
    }
  }

  return halo_acc;
}


/********************** tiff_container **********************************/
// public

/// Create a tiff container object from a tiff file
tiff_container::tiff_container(std::string path, std::string mode)
  : path_(path), mode_(mode)
{
  TIFFSetWarningHandler(&TIFFWarningHandler);
  tiff_ = TIFFOpen(path.c_str(), mode.c_str());
  good_ = (bool) tiff_;
}

/// Close a tiff container
tiff_container::~tiff_container()
{
  if(good_) {
    TIFFClose(tiff_);
  }
}

/// Indicate wheter the tiff container could be opened
/** @return true iff tiff file could be read and object is valie
**/
bool tiff_container::good()
{
  return good_;
}

/// Return wheter the images in the container can be saved after modifications
/** true iff the images in the container can be saved after modifications
**/
bool tiff_container::writeable()
{
  return mode_.find('w') != std::string::npos;
}

/// Return the opening mode of the tiff file
/** @return the opening mode of the tiff file
**/
std::string const& tiff_container::mode()
{
  return mode_;
}

/// Get the path of the tiff file
/** @return the path of the tiff file
**/
std::string const& tiff_container::path()
{
  return path_;
}

/// Get the number of images in the tiff container
/** @return the number of images in the tiff container
**/
int tiff_container::total_img_count()
{
  int count = 0;
  TIFFSetDirectory(tiff_, 0);

  do {
    count++;
  } while (TIFFReadDirectory(tiff_));

  TIFFSetDirectory(tiff_, 0);
  return count;
}

/// Get an image from the tiff container
/** @param i The number of the image
    @return a refernce to the requested image
**/
tiff_image16_ref tiff_container::image(int i)
{
  TIFFSetDirectory(tiff_, i);

  tsize_t scanline_size = TIFFScanlineSize(tiff_);

  uint32 height;
  TIFFGetField(tiff_,  TIFFTAG_IMAGELENGTH, &height);

  uint32 width;
  TIFFGetField(tiff_,  TIFFTAG_IMAGEWIDTH, &width);

  uint16 bits_per_pixel;
  TIFFGetField(tiff_, TIFFTAG_BITSPERSAMPLE, &bits_per_pixel);

  assert(scanline_size == (tsize_t) (sizeof(int16) * width));

  int16 **data = new int16*[height];
  int16 *data_rows = new int16[height * width];

  for(int row = 0; row < (int) height; row++) {
    data[row] =  &data_rows[row * width];
    TIFFReadScanline(tiff_, data[row], row);
  }

  // rely on return value optimization here,
  // copy-constructor should not be called

  return tiff_image16_ref(data, height, width, scanline_size, bits_per_pixel, i);
}

/// Append an image to the end of the tiff container
/** @param image the image to append
**/
void tiff_container::append_image(tiff_image16_ref const& image)
{
  int height = image.height();

  TIFFSetField(tiff_, TIFFTAG_IMAGEWIDTH, image.width());      	   // set the width of the image
  TIFFSetField(tiff_, TIFFTAG_IMAGELENGTH, image.height());    	   // set the height of the image
  TIFFSetField(tiff_, TIFFTAG_SAMPLESPERPIXEL, 1);                 // set number of channels per pixel
  TIFFSetField(tiff_, TIFFTAG_BITSPERSAMPLE, 16);                  // set the size of the channels
  TIFFSetField(tiff_, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);   // set the origin of the image
  TIFFSetField(tiff_, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
  TIFFSetField(tiff_, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tiff_, image.scanline_size()));

  for(int row = 0; row < height; row++) {
    TIFFWriteScanline(tiff_, (tdata_t) image.data()[row], row);
  }

  TIFFWriteDirectory(tiff_);

}

/// Append an image to the end of the tiff container after converting it to 8 bits
/** @param image the image to append
    @param shift the amount of pixels the image should be shifted to the left before appending
**/
void tiff_container::append_as_8bit_image(tiff_image16_ref const& image, int shift)
{
  int height = image.height();
  int width  = image.width();

  TIFFSetField(tiff_, TIFFTAG_IMAGEWIDTH, image.width());  // set the width of the image
  TIFFSetField(tiff_, TIFFTAG_IMAGELENGTH, image.height());    // set the height of the image
  TIFFSetField(tiff_, TIFFTAG_SAMPLESPERPIXEL, 1);   // set number of channels per pixel
  TIFFSetField(tiff_, TIFFTAG_BITSPERSAMPLE, 8);    // set the size of the channels
  TIFFSetField(tiff_, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);    // set the origin of the image
  TIFFSetField(tiff_, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
  TIFFSetField(tiff_, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tiff_, width * sizeof(uint8)));

  uint8 *row_8bit = new uint8[width];

  for(int row = 0; row < height; row++) {
    for(int col = 0; col < width; col++) {
      row_8bit[col] = image.data()[row][col] >> shift;
    }
    TIFFWriteScanline(tiff_, (tdata_t) row_8bit, row);
  }

  delete[] row_8bit;

  TIFFWriteDirectory(tiff_);
}


void tiff_container::TIFFWarningHandler(const char* /*module*/, const char* /*fmt*/, va_list /*ap*/)
{

/* do nothing*/
}
