/** CPU code
    \file SpdmCpuCode.cpp
 	\author Frederik Grüll (Frederik.Gruell@iri.uni-frankfurt.de)
**/

#include <cmath>
#include <iostream>
#include <iomanip>
#include <cerrno>
#include <cstring>

#include <unistd.h>

#include "tiff.hpp"

#include <MaxSLiCInterface.h>
#include "Spdm.h"

#include "SpdmCpuCode.hpp"


/// Look for last_pixel indicator in output stream to check whether the DFE is done
/** @param results Array with results
    @param length Length of the array
    @return True iff all images have been processed
**/
bool end_of_results(estimator_result *results, int length)
{
	return results[length - 1].img == last_pixel;
}


/// Print results as text, separated with tabs
/** @param results Array with results
    @param length Length of array
**/
void print_results(estimator_result *results, int length)
{
	int w = 10;
	for(int i = 0; i < length; i++) {
		estimator_result result = results[i];

		if(result.img >= 0) {
			std::cout << 0 << '\t'
				 << std::setw(w) << result.mu_y << '\t'
				 << std::setw(w) << result.mu_x << '\t'
				 << std::setw(w) << result.delta_mu_y << '\t'
				 << std::setw(w) << result.delta_mu_x << '\t'
				 << std::setw(w) << result.sigma_y << '\t'
				 << std::setw(w) << result.sigma_x << '\t'
				 << std::setw(w) << result.Q << '\t'
				 << std::setw(w) << result.img
			     << std::endl;
		}
	}
}

/// Get the DFE configuration
dfe_config::dfe_config()
{
	maxfile_ = Spdm_init();

	std::cerr << "Reading maxfile constants" << std::endl;

	constants_.max_img_width = max_get_constant_uint64t(maxfile_, "max_img_width");
	std::cerr << "max_img_width                              :  " << constants_.max_img_width << std::endl;

	constants_.max_img_height = max_get_constant_uint64t(maxfile_, "max_img_height");
	std::cerr << "max_img_height                             :  " << constants_.max_img_height << std::endl;

	constants_.estimator_result_bitsize = max_get_constant_uint64t(maxfile_, "estimator_result_bitsize");
	std::cerr << "estimator_result_bitsize                   :  " << constants_.estimator_result_bitsize << std::endl;
	if(constants_.estimator_result_bitsize != 8 * sizeof(estimator_result)) {
		throw std::runtime_error("constants_.estimator_result_bitsize != 8 * sizeof(estimator_result)");
	}

}

dfe_config::~dfe_config()
{
	std::cerr << "Freeing maxfile" << std::endl;
	max_file_free(maxfile_);
	maxfile_ = 0;
}

/// Get the maxfile handle of the configuration
max_file_t* dfe_config::maxfile() const
{
	return maxfile_;
}

/// Get the constants encoded in the maxfile
dfe_config_constants const& dfe_config::constants() const
{
	return constants_;
}

/// Configure the DFE
/** @param config The DFE configuration
    @param scalars Scalar values to be set before execution
**/
dataflow_engine::dataflow_engine(dfe_config const& config, dfe_scalars const& scalars)
{
	if(in_use_) {
		throw std::runtime_error("dataflow engine already in use");
	}
	in_use_ = true;

	std::cerr << "Configuring DFE" << std::endl;
	engine_ = max_load(config.maxfile(), "*");


	max_actions_t *action = max_actions_init(config.maxfile(), NULL);

	std::cerr << "SignalFinder.total_images                  :  " << scalars.total_images << std::endl;
	max_set_uint64t(action, "SignalFinder", "total_images", scalars.total_images);

	std::cerr << "SignalFinder.start_image                   :  " << scalars.start_image << std::endl;
	max_set_uint64t(action, "SignalFinder", "start_image", scalars.start_image);

	std::cerr << "SignalFinder.bg_threshold_factor           :  " << scalars.bg_threshold_factor << std::endl;
	max_set_uint64t(action, "SignalFinder", "bg_threshold_factor", scalars.bg_threshold_factor);

	std::cerr << "SignalFinder.img_width                     :  " << scalars.img_width << std::endl;
	max_set_uint64t(action, "SignalFinder", "img_width", scalars.img_width);

	std::cerr << "SignalFinder.img_height                    :  " << scalars.img_height << std::endl;
	max_set_uint64t(action, "SignalFinder", "img_height", scalars.img_height);

	std::cerr << "SignalFinder.img_width_offset              :  " << scalars.img_width << std::endl;
	max_set_offset(action, "SignalFinder", "img_width_offset", scalars.img_width);

	std::cerr << "SignalEstimator.separator_threshold_factor :  " << scalars.separator_threshold_factor << std::endl;
	max_set_double(action, "SignalEstimator", "separator_threshold_factor", scalars.separator_threshold_factor);

	std::cerr << "SignalEstimator.nm_per_px                  :  " << scalars.nm_per_px << std::endl;
	max_set_double(action, "SignalEstimator", "nm_per_px", scalars.nm_per_px);

	max_disable_stream_sync(action, "from_host");
	max_disable_stream_sync(action, "to_host");

	uint64_t pixel_count = scalars.img_width * scalars.img_height * scalars.total_images;
	uint64_t signal_finder_cyles = pixel_count;
	uint64_t signal_estimator_cycles = pixel_count * 2;

	std::cerr << "Kernel cycles of signal finder             :  " << signal_finder_cyles << std::endl;
	max_set_ticks(action, "SignalFinder", pixel_count);

	std::cerr << "Kernel cycles of signal finder             :  " << signal_estimator_cycles << std::endl;
	max_set_ticks(action, "SignalEstimator", signal_estimator_cycles);


	std::cerr << "Setting DFE scalars" << std::endl;
	max_run(engine_, action);
	max_actions_free(action);

}


dataflow_engine::~dataflow_engine()
{
	std::cerr << "Unloading engine" << std::endl;
	max_unload(engine_);
	engine_ = 0;
	in_use_ = false;
}

/// Get the engine handle
/** @return The engine handle
**/
max_engine_t* dataflow_engine::handle() const
{
	return engine_;
}

bool dataflow_engine::in_use_;

int main(int argc, char* argv[])
{
	if(argc != 2) {
		std::cerr << "Usage: " << argv[0] << " image.tif" << std::endl;
		exit(1);
	}
	char *filename = argv[1];

	std::cerr << "Opening Tiff file" << std::endl;
	tiff_container tiff(argv[1], "r");
	if(!tiff.good()) {
		std::cerr << "Could not open tiff file '" << filename << "'" << std::endl;
		exit(1);
	}
	int total_imgs = tiff.total_img_count();
	tiff_image16_ref img_ref = tiff.image(0);

	dfe_scalars scalars;
	scalars.total_images = total_imgs;
	scalars.nm_per_px = 102.0;
	scalars.start_image = 0;
	scalars.bg_threshold_factor = 4;
	scalars.img_width = img_ref.width();
	scalars.img_height = img_ref.height();
	scalars.separator_threshold_factor = 0.7;

	dfe_config config;

	if(scalars.img_width > config.constants().max_img_width || scalars.img_height > config.constants().max_img_height) {
		throw std::runtime_error("scalars.img_width > config.constants().max_img_width || scalars.img_height > config.constants().max_img_height");
	}

	dataflow_engine dfe(config, scalars);

	std::cerr << "Setting up input and output streams" << std::endl;

	const int slot_send_length = 2048;
	const int slot_recv_length = 16;
	ll_send_stream<int16_t> sender(dfe, "from_host", slot_send_length);
	ll_recv_stream<estimator_result> receiver(dfe, "to_host", slot_recv_length);

	if((scalars.img_height * scalars.img_width) % slot_send_length != 0) {
		throw std::runtime_error("(config.constants().img_height * config.constants().img_width) % slot_send_length != 0");
	}

	int img = 0;
	int pixel = 0;
	bool all_send = false;
	while(true) {		// active polling, required by the low-latency interface
		if(scalars.img_height != img_ref.height() || scalars.img_width != img_ref.width()) {
			throw std::runtime_error("scalars.img_height != img_ref.height() || scalars.img_width != img_ref.width()");
		}

		if(!all_send && sender.send(img_ref.data()[0] + pixel)) {
			pixel += sender.slot_length();
			if(pixel >= img_ref.height() * img_ref.width()) {
				pixel = 0;
				img++;
				if(img < total_imgs) {
					img_ref = tiff.image(img);
				} else {
					all_send = true;
				}
			}
//			std::cerr << "img: " << img << ", pixel: " << pixel << std::endl;
		}

		estimator_result* results = receiver.recv();
		if(results) {
//			std::cerr << "received results" << std::endl;
			print_results(results, receiver.slot_length());
			if(end_of_results(results, receiver.slot_length())) {
				break;
			}
		}

	}


	std::cerr << "Shutting down" << std::endl;

	return 0;
}



