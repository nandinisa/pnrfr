#' Computes the moving average of a given channel within the PNRF file
#'
#' @param file_path Path to the PNRF file location, full path or current directory
#' @param channel_no Channel number to retrieve data
#' @param window_time Moving average time interval
#' @param data_type Type of data to retrieve : continuous 1, sweeps = 2, mixed = 3 (default = 3)
#' @param sampling If sampling is required (default = F)
#' @param sampling_rate Rate of sampling (default = 100), applicable if sampling = T
#' @examples
#' compute_moving_average("example.PNRF", 3, 0.01)
#' @export
compute_moving_average <-
  function(file_path,
           channel_no,
           window_time, ...){
    file_path = check('f', file_path)
    channel_no = check('c', channel_no)

    arguments <- list(...)
    data_type <- get_data_type(arguments)
    sampling_list <- get_sampling_rate(arguments)

    channel_meta <- .C("readFileChannelMeta",
                       filePath = as.character(file_path),
                       record = as.integer(0),
                       channel = as.integer(channel_no),
                       dataType = as.integer(data_type),
                       name = as.character(''),
                       type = as.character(''),
                       yunit = as.character(''),
                       startTime = as.double(0),
                       endTime = as.double(0),
                       noOfSamples = as.integer(0),
                       sampleInterval = as.double(0),
                       segCount = as.integer(0),
                       msg = as.character(''),
                       out = as.integer(1),
                       PACKAGE = 'pnrfr')
    if(channel_meta$out != 0){
      stop_err(channel_meta$msg)
    }

    # Reads the PNRF file and returns the data
    c_detail <- .C("getChannelDataSize",
                   filePath = as.character(file_path),
                   channel = as.integer(channel_no),
                   dataType = as.integer(data_type),
                   size = as.integer(0),
                   sampling = as.logical(sampling_list[[1]]),
                   samplingRate = as.integer(sampling_list[[2]]),
                   msg = as.character(''),
                   out = as.integer(1),
                   PACKAGE = 'pnrfr')
    if (c_detail$out != 0) {
      stop_err(c_detail$msg)
    }

    mov_avg_data <- .C(
      "computeMovingAverage",
      filePath = as.character(file_path),
      channel = as.integer(channel_no),
      dataType = as.integer(data_type),
      timeArray = as.double(rep(0, c_detail$size)),
      dataArray = as.double(rep(0, c_detail$size)),
      windowTime = as.double(window_time),
      sampling = as.logical(sampling_list[[1]]),
      samplingRate = as.integer(sampling_list[[2]]),
      msg = as.character(''),
      out = as.integer(1),
      PACKAGE = 'pnrfr'
    )

    if (mov_avg_data$out != 0) {
      stop_err(mov_avg_data$msg)
    }

    result <- create_pnrfr_obj(file_path,
                               channel_meta, c_detail, data_type,
                               sampling_list, 'rms_signal', mov_avg_data)

    return(result)
  }

