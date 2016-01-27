/** CPU code
    \file SpdmCpuCode.hpp
 	\author Frederik Grüll (Frederik.Gruell@iri.uni-frankfurt.de)
**/


#ifndef SPDMCPUCODE_HPP
#define SPDMCPUCODE_HPP



//#include <stdlib.h>
//#include <stdint.h>
#include <stdexcept>

#include "tiff.h"



const int end_of_image = -1;	///< Indicates end of an image in the img field of estimator_result
const int last_pixel   = -2;	///< Indicates last pixel of input has been processed in the img field of estimator_result


/// Scalar values of the DFE configuration
struct dfe_scalars
{
	int total_images;					///< total number of images to process
	float nm_per_px;					///< size of an object that covers one pixel in nanometers
	long start_image;					///< first image to process, ignore earlier images
	long bg_threshold_factor;			///< threshold above image background for signal finder
	long img_width;						///< width in pixels of each image frame
	long img_height;					///< height in pixels of each image frame
	double separator_threshold_factor;	///< threshold for signal separator, signals below are discarded
};

/// Maxfile constants
struct dfe_config_constants
{
	long max_img_width;					///< maximum width of each image frame
	long max_img_height;				///< maximum length of each image frame
	long estimator_result_bitsize;		///< size in bytes of estimator result struct
};

/// Configuration of the DFE
class dfe_config
{
public:
	dfe_config() ;
	~dfe_config();
	max_file_t *maxfile() const;
	dfe_config_constants const& constants() const;

private:
	dfe_config(dfe_config const&);		// no copying
	dfe_config& operator=(const dfe_config&);
	max_file_t *maxfile_;
	dfe_config_constants constants_;
};

/// Dataflow engine with configuration
class dataflow_engine
{
public:
	dataflow_engine(dfe_config const& config, dfe_scalars const& scalars);
	~dataflow_engine();
	max_engine_t* handle() const;

private:
	dataflow_engine(dataflow_engine const&);		// no copying
	dataflow_engine& operator=(const dataflow_engine&);
	max_engine_t *engine_;
	static bool in_use_;
};

/// Result type as streamed from the signal estimator
struct estimator_result
{
	int32_t img;			///< Image (frame) number
	float Q;				///< Total charge / intensity
	float mu_x;				///< X position of the signal center with sub-pixel accuracy
	float mu_y;				///< Y position of the signal center with sub-pixel accuracy
	float sigma_x;			///< Width of the signal in x direction, squared
	float sigma_y;			///< Width of the signal in y direction, squared
	float delta_mu_x;		///< Confidence of the x position, squared
	float delta_mu_y;		///< Confidence of the y position, squared
};

/// Low-latency stream between, base class for stream from host to DFE or vice versa
template<class T>
class ll_stream
{
public:
	int slot_length() const;
	int slot_count() const;
	static const int alignment;

protected:
	ll_stream(dataflow_engine const& dfe, std::string name, int slot_length, int slot_count = 2);
	virtual ~ll_stream();
	max_llstream_t* handle() const;

private:
	ll_stream(ll_stream<T> const&);		// no copying
	ll_stream& operator=(const ll_stream&);
	int slot_length_;
	int slot_count_;
	void *buffer_;
	max_llstream_t *stream_;
};

/// Get the length of a slot for low-latency streaming in units of sizeof(T)
/** @return slot length
**/
template<class T>
int ll_stream<T>::slot_length() const
{
	return slot_length_;
}

/// Get the number of slots available
/** @return Number of slots available
**/
template<class T>
int ll_stream<T>::slot_count() const
{
	return slot_count_;
}

/// Alignment for transmission buffer
template<class T>
const int ll_stream<T>::alignment = 4096;

/// Create a low-latency stream
/** @param dfe The DFE to connect with
    @param name The name of the stream
    @param slot_length The length of each slot in units of T
    @param slot_count The number of slots to allocate
 **/
template<class T>
ll_stream<T>::ll_stream(dataflow_engine const& dfe, std::string name, int slot_length, int slot_count)
	: slot_length_(slot_length), slot_count_(slot_count), buffer_(0), stream_(0)
{
	std::cerr << "Setting up stream " << name << std::endl;

	slot_length_ = slot_length;
	slot_count_ = slot_count;
	int ret = posix_memalign(&buffer_, alignment, slot_count * slot_length * sizeof(T));
	if(ret != 0) {
		throw std::runtime_error(std::string("posix_memalign: ") + strerror(ret));
	}

	stream_ = max_llstream_setup(dfe.handle(), name.c_str(), slot_count, slot_length * sizeof(T), buffer_);
}

template<class T>
ll_stream<T>::~ll_stream()
{
	if(stream_) {
		std::cerr << "Releasing stream" << std::endl;
		max_llstream_release(stream_);
	}
}

/// Get the handle of the stream
/** @return The stream handle
 **/
template<class T>
max_llstream_t* ll_stream<T>::handle() const
{
	return stream_;
}

/// Low-latency stream for sending data from host to DFE
template<class T>
class ll_send_stream : public ll_stream<T>
{
public:
	ll_send_stream(dataflow_engine const& dfe, std::string name, int slot_length);
	virtual ~ll_send_stream();
	bool send(T* slot_data);

private:
	ll_send_stream(ll_send_stream<T> const&);	// no copying
	ll_send_stream& operator=(const ll_send_stream&);
};

/// Create a low-latency stream for sending data from host to DFE
/** @param dfe The DFE to connect with
    @param name The name of the stream
    @param slot_length The length of each slot in units of T
 **/
template<class T>
ll_send_stream<T>::ll_send_stream(dataflow_engine const& dfe, std::string name, int slot_length)
	: ll_stream<T>(dfe, name, slot_length)
{
}

template<class T>
ll_send_stream<T>::~ll_send_stream()
{}

/// Send data if a slot is available
/** @param slot_data The data to be send
    @return True iff the data was sent
 **/
template<class T>
bool ll_send_stream<T>::send(T* slot_data)
{
	T *write_ptr = 0;
	int slots = max_llstream_write_acquire(ll_stream<T>::handle(), 1, (void **) &write_ptr);
	if(slots == 0) {
//		std::cerr << "No space to send data" << std::endl;
		return false;
	} else {
//		std::cerr << "Sending data" << std::endl;
		std::copy(slot_data, slot_data + ll_stream<T>::slot_length(), write_ptr);
		max_llstream_write(ll_stream<T>::handle(), 1);
		return true;
	}
}


/// Low-latency stream for receiving data on the host from the DFE
template<class T>
class ll_recv_stream : public ll_stream<T>
{
public:
	ll_recv_stream(dataflow_engine const& dfe, std::string name, int slot_length);
	virtual ~ll_recv_stream();
	T* recv();

private:
	ll_recv_stream(ll_recv_stream<T> const&);	// no copying
	ll_recv_stream& operator=(const ll_recv_stream&);
	ssize_t outstanding_discards_;
};

/// Create a low-latency stream for receiving data on the host from the DFE
/** @param dfe The DFE to connect with
    @param name The name of the stream
    @param slot_length The length of each slot in units of T
 **/
template<class T>
ll_recv_stream<T>::ll_recv_stream(dataflow_engine const& dfe, std::string name, int slot_length)
	: ll_stream<T>(dfe, name, slot_length), outstanding_discards_(0)
{}

template<class T>
ll_recv_stream<T>::~ll_recv_stream()
{
	if(outstanding_discards_ && ll_stream<T>::handle()) {
		max_llstream_read_discard(ll_stream<T>::handle(), outstanding_discards_);
		outstanding_discards_ = 0;
	}
}

/// Receive data if available
/** @return Null if no data was available, slot with data otherwise
 **/
template<class T>
T* ll_recv_stream<T>::recv()
{

	if(outstanding_discards_) {
		max_llstream_read_discard(ll_stream<T>::handle(), outstanding_discards_);
		outstanding_discards_ = 0;
	}

	T *read_ptr = 0;
	ssize_t slots = max_llstream_read(ll_stream<T>::handle(), 1, (void **) &read_ptr);
	if(slots == 0) {
//		std::cerr << "No data received from stream" << std::endl;
		return 0;
	} else {
//		std::cerr << "Data received from stream" << std::endl;
		outstanding_discards_ += slots;
		return read_ptr;
	}
}


#endif /* SPDMCPUCODE_H */
