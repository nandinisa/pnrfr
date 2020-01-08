#' pnrfr: R package for reading a PNRF file.
#'
#' The pnrf package provides two categories of functions:
#' PNRF file read related functions and computing moving average on a given channel.
#'
#' @section PNRF file read functions:
#' read_file_meta - Reads the meta data of a PNRF file
#' read_channel_data_size - Reads the specified channel data size from a PNRF file
#' read_channel_data - Reads the specified channel data from a PNRF file (at a given samplinng rate)
#' read_all_channel_data - Reads all available channel data from a PNRF file (at a given samplinng rate)
#' read_segment_data - Reads the segment data corresponding to a channel from a PNRF file (at a given samplinng rate)
#' save_csv -
#'
#' @section PNRF file computation functions:
#' compute_moving_average - Computes the moving average for a channel in a PNRF file given a moving window
#'
#' @docType package
#' @name pnrfr
