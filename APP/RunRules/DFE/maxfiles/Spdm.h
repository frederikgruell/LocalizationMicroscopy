/**\file */
#ifndef SLIC_DECLARATIONS_Spdm_H
#define SLIC_DECLARATIONS_Spdm_H
#include "MaxSLiCInterface.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define Spdm_estimator_result_bitsize (256)
#define Spdm_max_img_width (512)
#define Spdm_max_img_height (512)


/*----------------------------------------------------------------------------*/
/*---------------------------- Interface default -----------------------------*/
/*----------------------------------------------------------------------------*/




/**
 * \brief Basic static function for the interface 'default'.
 * 
 * \param [in] offset_SignalFinder_img_width_offset Offset parameter "img_width_offset" for kernel "SignalFinder".
 * \param [in] ticks_SignalEstimator The number of ticks for which kernel "SignalEstimator" will run.
 * \param [in] ticks_SignalFinder The number of ticks for which kernel "SignalFinder" will run.
 * \param [in] inscalar_SignalEstimator_nm_per_px Input scalar parameter "SignalEstimator.nm_per_px".
 * \param [in] inscalar_SignalEstimator_separator_threshold_factor Input scalar parameter "SignalEstimator.separator_threshold_factor".
 * \param [in] inscalar_SignalFinder_bg_threshold_factor Input scalar parameter "SignalFinder.bg_threshold_factor".
 * \param [in] inscalar_SignalFinder_img_height Input scalar parameter "SignalFinder.img_height".
 * \param [in] inscalar_SignalFinder_img_width Input scalar parameter "SignalFinder.img_width".
 * \param [in] inscalar_SignalFinder_start_image Input scalar parameter "SignalFinder.start_image".
 * \param [in] inscalar_SignalFinder_total_images Input scalar parameter "SignalFinder.total_images".
 * \param [in] instream_from_host Stream "from_host".
 * \param [in] instream_size_from_host The size of the stream instream_from_host in bytes.
 * \param [out] outstream_to_host Stream "to_host".
 * \param [in] outstream_size_to_host The size of the stream outstream_to_host in bytes.
 */
void Spdm(
	long offset_SignalFinder_img_width_offset,
	uint64_t ticks_SignalEstimator,
	uint64_t ticks_SignalFinder,
	double inscalar_SignalEstimator_nm_per_px,
	double inscalar_SignalEstimator_separator_threshold_factor,
	uint64_t inscalar_SignalFinder_bg_threshold_factor,
	uint64_t inscalar_SignalFinder_img_height,
	uint64_t inscalar_SignalFinder_img_width,
	uint64_t inscalar_SignalFinder_start_image,
	uint64_t inscalar_SignalFinder_total_images,
	const void *instream_from_host,
	size_t instream_size_from_host,
	void *outstream_to_host,
	size_t outstream_size_to_host);

/**
 * \brief Basic static non-blocking function for the interface 'default'.
 * 
 * Schedule to run on an engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 * 
 * 
 * \param [in] offset_SignalFinder_img_width_offset Offset parameter "img_width_offset" for kernel "SignalFinder".
 * \param [in] ticks_SignalEstimator The number of ticks for which kernel "SignalEstimator" will run.
 * \param [in] ticks_SignalFinder The number of ticks for which kernel "SignalFinder" will run.
 * \param [in] inscalar_SignalEstimator_nm_per_px Input scalar parameter "SignalEstimator.nm_per_px".
 * \param [in] inscalar_SignalEstimator_separator_threshold_factor Input scalar parameter "SignalEstimator.separator_threshold_factor".
 * \param [in] inscalar_SignalFinder_bg_threshold_factor Input scalar parameter "SignalFinder.bg_threshold_factor".
 * \param [in] inscalar_SignalFinder_img_height Input scalar parameter "SignalFinder.img_height".
 * \param [in] inscalar_SignalFinder_img_width Input scalar parameter "SignalFinder.img_width".
 * \param [in] inscalar_SignalFinder_start_image Input scalar parameter "SignalFinder.start_image".
 * \param [in] inscalar_SignalFinder_total_images Input scalar parameter "SignalFinder.total_images".
 * \param [in] instream_from_host Stream "from_host".
 * \param [in] instream_size_from_host The size of the stream instream_from_host in bytes.
 * \param [out] outstream_to_host Stream "to_host".
 * \param [in] outstream_size_to_host The size of the stream outstream_to_host in bytes.
 * \return A handle on the execution status, or NULL in case of error.
 */
max_run_t *Spdm_nonblock(
	long offset_SignalFinder_img_width_offset,
	uint64_t ticks_SignalEstimator,
	uint64_t ticks_SignalFinder,
	double inscalar_SignalEstimator_nm_per_px,
	double inscalar_SignalEstimator_separator_threshold_factor,
	uint64_t inscalar_SignalFinder_bg_threshold_factor,
	uint64_t inscalar_SignalFinder_img_height,
	uint64_t inscalar_SignalFinder_img_width,
	uint64_t inscalar_SignalFinder_start_image,
	uint64_t inscalar_SignalFinder_total_images,
	const void *instream_from_host,
	size_t instream_size_from_host,
	void *outstream_to_host,
	size_t outstream_size_to_host);

/**
 * \brief Advanced static interface, structure for the engine interface 'default'
 * 
 */
typedef struct { 
	long offset_SignalFinder_img_width_offset; /**<  [in] Offset parameter "img_width_offset" for kernel "SignalFinder". */
	uint64_t ticks_SignalEstimator; /**<  [in] The number of ticks for which kernel "SignalEstimator" will run. */
	uint64_t ticks_SignalFinder; /**<  [in] The number of ticks for which kernel "SignalFinder" will run. */
	double inscalar_SignalEstimator_nm_per_px; /**<  [in] Input scalar parameter "SignalEstimator.nm_per_px". */
	double inscalar_SignalEstimator_separator_threshold_factor; /**<  [in] Input scalar parameter "SignalEstimator.separator_threshold_factor". */
	uint64_t inscalar_SignalFinder_bg_threshold_factor; /**<  [in] Input scalar parameter "SignalFinder.bg_threshold_factor". */
	uint64_t inscalar_SignalFinder_img_height; /**<  [in] Input scalar parameter "SignalFinder.img_height". */
	uint64_t inscalar_SignalFinder_img_width; /**<  [in] Input scalar parameter "SignalFinder.img_width". */
	uint64_t inscalar_SignalFinder_start_image; /**<  [in] Input scalar parameter "SignalFinder.start_image". */
	uint64_t inscalar_SignalFinder_total_images; /**<  [in] Input scalar parameter "SignalFinder.total_images". */
	const void *instream_from_host; /**<  [in] Stream "from_host". */
	size_t instream_size_from_host; /**<  [in] The size of the stream instream_from_host in bytes. */
	void *outstream_to_host; /**<  [out] Stream "to_host". */
	size_t outstream_size_to_host; /**<  [in] The size of the stream outstream_to_host in bytes. */
} Spdm_actions_t;

/**
 * \brief Advanced static function for the interface 'default'.
 * 
 * \param [in] engine The engine on which the actions will be executed.
 * \param [in,out] interface_actions Actions to be executed.
 */
void Spdm_run(
	max_engine_t *engine,
	Spdm_actions_t *interface_actions);

/**
 * \brief Advanced static non-blocking function for the interface 'default'.
 *
 * Schedule the actions to run on the engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 *
 * 
 * \param [in] engine The engine on which the actions will be executed.
 * \param [in] interface_actions Actions to be executed.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *Spdm_run_nonblock(
	max_engine_t *engine,
	Spdm_actions_t *interface_actions);

/**
 * \brief Group run advanced static function for the interface 'default'.
 * 
 * \param [in] group Group to use.
 * \param [in,out] interface_actions Actions to run.
 *
 * Run the actions on the first device available in the group.
 */
void Spdm_run_group(max_group_t *group, Spdm_actions_t *interface_actions);

/**
 * \brief Group run advanced static non-blocking function for the interface 'default'.
 * 
 *
 * Schedule the actions to run on the first device available in the group and return immediately.
 * The status of the run must be checked with ::max_wait. 
 * Note that use of ::max_nowait is prohibited with non-blocking running on groups:
 * see the ::max_run_group_nonblock documentation for more explanation.
 *
 * \param [in] group Group to use.
 * \param [in] interface_actions Actions to run.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *Spdm_run_group_nonblock(max_group_t *group, Spdm_actions_t *interface_actions);

/**
 * \brief Array run advanced static function for the interface 'default'.
 * 
 * \param [in] engarray The array of devices to use.
 * \param [in,out] interface_actions The array of actions to run.
 *
 * Run the array of actions on the array of engines.  The length of interface_actions
 * must match the size of engarray.
 */
void Spdm_run_array(max_engarray_t *engarray, Spdm_actions_t *interface_actions[]);

/**
 * \brief Array run advanced static non-blocking function for the interface 'default'.
 * 
 *
 * Schedule to run the array of actions on the array of engines, and return immediately.
 * The length of interface_actions must match the size of engarray.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 *
 * \param [in] engarray The array of devices to use.
 * \param [in] interface_actions The array of actions to run.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *Spdm_run_array_nonblock(max_engarray_t *engarray, Spdm_actions_t *interface_actions[]);

/**
 * \brief Converts a static-interface action struct into a dynamic-interface max_actions_t struct.
 *
 * Note that this is an internal utility function used by other functions in the static interface.
 *
 * \param [in] maxfile The maxfile to use.
 * \param [in] interface_actions The interface-specific actions to run.
 * \return The dynamic-interface actions to run, or NULL in case of error.
 */
max_actions_t* Spdm_convert(max_file_t *maxfile, Spdm_actions_t *interface_actions);

/**
 * \brief Initialise a maxfile.
 */
max_file_t* Spdm_init(void);

/* Error handling functions */
int Spdm_has_errors(void);
const char* Spdm_get_errors(void);
void Spdm_clear_errors(void);
/* Free statically allocated maxfile data */
void Spdm_free(void);
/* These are dummy functions for hardware builds. */
int Spdm_simulator_start(void);
int Spdm_simulator_stop(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* SLIC_DECLARATIONS_Spdm_H */

