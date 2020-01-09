#' PNRF file meta details.
#'
#' Returns the file meta information: name, record details and channel details.
#'
#' @param file_path Path to the PNRF file location, full path or file name in current directory.
#' @param ... data_type: Type of data to retrieve : continuous 1, sweeps = 2, mixed = 3 (default = 3)
#' @return A list containing file meta-information: title, recordCount, channelCount, records(list)
#' @examples
#' result <- read_file_meta(system.file("extdata", "VT094.pnrf", package="pnrfr"))
#' result <- read_file_meta(system.file("extdata", "VT094.pnrf", package="pnrfr"), data_type = 1)
#' @export
read_file_meta <- function(file_path, ...){
  file_path = check('f', file_path)
  arguments <- list(...)
  data_type <- get_data_type(arguments)

  # File meta data
  result <- list(
    title = "",
    recordCount = 0,
    channelCount = 0,
    records = list()
  )

  file_meta <- .C("readFileMeta",
                  filePath = as.character(file_path),
                  title = as.character(''),
                  recordCount = as.integer(0),
                  msg = as.character(''),
                  out = as.integer(1),
                  PACKAGE = 'pnrfr')
  if(file_meta$out != 0){
    stop_err(file_meta$msg)
  }

  result$title = file_meta$title
  result$recordCount = file_meta$recordCount

  record_meta <- .C("readFileRecordMeta",
                    filePath = as.character(file_path),
                    recordArray = as.integer(seq(1, file_meta$recordCount)),
                    recordArraySize = as.integer(file_meta$recordCount),
                    channelCountArray = as.integer(rep(0, file_meta$recordCount)),
                    msg = as.character(''),
                    out = as.integer(1),
                    PACKAGE = 'pnrfr')

  if(record_meta$out != 0){
    stop_err(file_meta$msg)
  }

  result$channelCount = sum(record_meta$channelCountArray)

  for(i in record_meta$recordArray){
    result$records[[paste0('rec', i)]] = list()
    for(j in 1:record_meta$channelCountArray[i]){
      channel_meta <- .C("readFileChannelMeta",
                         filePath = as.character(file_path),
                         record = as.integer(i),
                         channel = as.integer(j),
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
      if(channel_meta$out == 0){
        ch = list()
        ch[[paste0('ch', j)]] = list(
          channel = as.integer(channel_meta$channel),
          name = channel_meta$name,
          type = channel_meta$type,
          yunit = channel_meta$yunit,
          startTime = channel_meta$startTime,
          endTime = channel_meta$endTime,
          noofSamples = channel_meta$noOfSamples,
          sampleInterval = channel_meta$sampleInterval,
          segCount = channel_meta$segCount
        )

        result$records[[i]] <- c(result$records[[i]], ch)

      }
      else{
        stop_err(channel_meta$msg)
      }
    }
  }

  return(result)
}

#' PNRF file channel data size.
#'
#' Reads the total no of observations in a given channel.
#'
#' @param file_path Path to the PNRF file location, full path or current directory
#' @param channel_no Channel number to read
#' @param data_type Type of data to retrieve : continuous 1, sweeps = 2, mixed = 3 (default = 3)
#' @return Total number of observations in the channel specified (int)
#' @examples
#' result <- read_channel_data_obs(system.file("extdata", "VT094.pnrf", package="pnrfr"),
#' channel_no=3)
#' result <- read_channel_data_obs(system.file("extdata", "VT094.pnrf", package="pnrfr"),
#' channel_no=3, data_type = 1)
#' @export
read_channel_data_obs <- function(file_path, channel_no, data_type = 3){
  file_path = check('f', file_path)
  channel_no = check('c', channel_no)
  data_type = check('dt', data_type)

  # Reads the PNRF file and returns the data
  c_detail <- .C("getChannelDataSize",
                 filePath = as.character(file_path),
                 channel = as.integer(channel_no),
                 dataType = as.integer(data_type),
                 size = as.integer(0),
                 sampling = as.logical(FALSE),
                 samplingRate = as.integer(-1),
                 msg = as.character(''),
                 out = as.integer(1),
                 PACKAGE = 'pnrfr')

  if(c_detail$out != 0){
    stop_err(c_detail$msg)
  }

  return(c_detail$size)
}

#' Read the channel data from a PNRF file
#'
#' @param file_path Path to the PNRF file location, full path or current directory
#' @param channel_no Channel number to read
#' @param ... data_type: Type of data to retrieve : continuous 1, sweeps = 2, mixed = 3 (default = 3)
#' @param ... sampling: Set to TRUE if sampling is required (default = FALSE)
#' @param ... sampling_rate: Rate of sampling (default = 100), applicable if sampling = TRUE
#' @return PNRFR file object
#' @examples
#' result <- read_channel_data(system.file("extdata", "VT094.pnrf", package="pnrfr"), 3)
#' result <- read_channel_data(system.file("extdata", "VT094.pnrf", package="pnrfr"), 3,
#' sampling = TRUE, sampling_rate = 1000)
#' @export
read_channel_data <- function(file_path, channel_no, ...){
  file_path = check('f', file_path)
  channel_no = check('c', channel_no)

  arguments <- list(...)
  data_type <- get_data_type(arguments)
  sampling <- get_sampling_rate(arguments)

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
                 sampling = as.logical(sampling[[1]]),
                 samplingRate = as.integer(sampling[[2]]),
                 msg = as.character(''),
                 out = as.integer(1),
                 PACKAGE = 'pnrfr')

  if(c_detail$out != 0){
    stop_err(c_detail$msg)
  }

  # read all of channel data
  c_data <- .C("readChannelData",
              filePath = as.character(file_path),
              channel = as.integer(channel_no),
              dataType = as.integer(data_type),
              timeArray = as.double(rep(0, c_detail$size)),
              dataArray = as.double(rep(0, c_detail$size)),
              sampling = as.logical(sampling[[1]]),
              samplingRate = as.integer(sampling[[2]]),
              msg = as.character(''),
              out = as.integer(0),
              PACKAGE = 'pnrfr')

  if(c_data$out != 0){
    stop_err(c_data$msg)
  }

  result <- create_pnrfr_obj(file_path,
                             channel_meta, c_detail, data_type,
                             sampling, 'signal', c_data)

  return(result)
}


#' Read all channel data from PNRF file
#' Always reads sampled data
#'
#' @param file_path Path to the PNRF file location, full path or current directory
#' @param sampling_rate Rate of sampling (default = 100)
#' @param ... data_type: Type of data to retrieve : continuous 1, sweeps = 2, mixed = 3 (default = 3)
#' @return PNRFR file object
#' @examples
#' result <- read_all_channel_data(system.file("extdata", "VT094.pnrf", package="pnrfr"))
#' @export
read_all_channel_data <- function(file_path, sampling_rate = 100, ...){
  file_path = check('f', file_path)
  arguments <- list(...)
  data_type <- get_data_type(arguments)
  sampling <- list(T, if(sampling_rate < 100) 100 else sampling_rate)

  file_meta <- read_file_meta(file_path, ...)

  ch_df <- NULL

  for(ch in file_meta$records[[1]]){
    # Reads the PNRF file and returns the data
    c_detail <- .C("getChannelDataSize",
                   filePath = as.character(file_path),
                   channel = as.integer(ch$channel),
                   dataType = as.integer(data_type),
                   size = as.integer(0),
                   sampling = as.logical(sampling[[1]]),
                   samplingRate = as.integer(sampling[[2]]),
                   msg = as.character(''),
                   out = as.integer(1),
                   PACKAGE = 'pnrfr')

    if(c_detail$out != 0){
      stop_err(c_detail$msg)
    }

    # read all of channel data
    c_data <- .C("readChannelData",
                 filePath = as.character(file_path),
                 channel = as.integer(ch$channel),
                 dataType = as.integer(data_type),
                 timeArray = as.double(rep(0, c_detail$size)),
                 dataArray = as.double(rep(0, c_detail$size)),
                 sampling = as.logical(sampling[[1]]),
                 samplingRate = as.integer(sampling[[2]]),
                 msg = as.character(''),
                 out = as.integer(0),
                 PACKAGE = 'pnrfr')

    if(c_data$out != 0){
      stop_err(c_data$msg)
    }

    result <- create_pnrfr_obj(file_path,
                               ch, c_detail, data_type,
                               sampling, 'signal', c_data)

    if(is.null(ch_df)){
      ch_df <- result
    }
    else{
      ch_df$channels <- rbind(ch_df$channels, result$channels)
    }
  }

  return(ch_df)
}

#' Reads the segment data corresponding to a channel
#'
#' @param file_path Path to the PNRF file location, full path or current directory
#' @param channel_no Channel number to retrieve data
#' @param segment_no Segment number of the corresponding channel
#' @param ... data_type: Type of data to retrieve : continuous 1, sweeps = 2, mixed = 3 (default = 3)
#' @param ... sampling: If sampling is required (default = FALSE)
#' @param ... sampling_rate: Rate of sampling (default = 100), applicable if sampling = TRUE
#' @return PNRFR file object
#' @examples
#' result <- read_channel_data(system.file("extdata", "VT094.pnrf", package="pnrfr"),
#' channel_no = 3, segment_no = 101)
#' result <- read_channel_data(system.file("extdata", "VT094.pnrf", package="pnrfr"),
#' channel_no = 3, segment_no = 101, sampling = TRUE)
#' result <- read_channel_data(system.file("extdata", "VT094.pnrf", package="pnrfr"),
#' channel_no = 3, segment_no = 101, sampling = TRUE, sampling_rate = 1000)
#' @export
read_segment_data <- function(file_path, channel_no, segment_no, ...){
  file_path = check('f', file_path)
  channel_no = check('c', channel_no)
  segment_no = check('s', segment_no)

  arguments <- list(...)
  data_type <- get_data_type(arguments)
  sampling <- get_sampling_rate(arguments)

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

  s_detail <- .C("getSegmentDataSize",
                 as.character(file_path),
                 channel = as.integer(channel_no),
                 segment = as.integer(segment_no),
                 dataType = as.integer(data_type),
                 size = as.integer(0),
                 sampling = as.logical(sampling[[1]]),
                 samplingRate = as.integer(sampling[[2]]),
                 msg = as.character(''),
                 out = as.integer(0),
                 PACKAGE = 'pnrfr')
  if(s_detail$out != 0){
    stop_err(s_detail$msg)
  }

  s_data <- .C("readSegmentData",
               as.character(file_path),
               channel = as.integer(channel_no),
               segment = as.integer(segment_no),
               dataType = as.integer(data_type),
               timeArray = as.double(rep(0, s_detail$size)),
               dataArray = as.double(rep(0, s_detail$size)),
               sampling = as.logical(sampling[[1]]),
               samplingRate = as.integer(sampling[[2]]),
               msg = as.character(''),
               out = as.integer(0),
               PACKAGE = 'pnrfr')
  if(s_data$out != 0){
    stop_err(s_data$msg)
  }

  result <- create_pnrfr_obj(file_path,
                             channel_meta, s_detail, data_type,
                             sampling, 'signal', s_data)
  return(result)
}

#' Saves channel data as a csv file
#'
#' @param file_path Path to the PNRF file location, full path or current directory
#' @param save_path Path to the converted csv file location, full path or current directory
#' @param channel_no Channel number to retrieve data
#' @param ... data_type: Type of data to retrieve : continuous 1, sweeps = 2, mixed = 3 (default = 3)
#' @param ... sampling: If sampling is required (default = FALSE)
#' @param ... sampling_rate: Rate of sampling (default = 100), applicable if sampling = TRUE
#' @return Noneo
#' @examples
#' save_channel_data.to_csv(system.file("extdata", "VT094.pnrf", package="pnrfr"),"VT094.csv", 3,
#' sampling = TRUE, sampling_rate = 1000)
#' @export
save_channel_data.to_csv <- function(file_path, save_path, channel_no, ...){
  file_path = check('f', file_path)
  save_path = check('dw', save_path)
  channel_no = check('c', channel_no)

  arguments <- list(...)
  data_type <- get_data_type(arguments)
  sampling_list <- get_sampling_rate(arguments)

  # Reads the PNRF file and saves the data as .csv file
  result <- .C("saveChannelData",
               filePath = as.character(file_path),
               channel = as.integer(channel_no),
               dataType = as.integer(data_type),
               savePath = as.character(save_path),
               sampling = as.logical(sampling_list[[1]]),
               samplingRate = as.integer(sampling_list[[2]]),
               msg = as.character(''),
               out = as.integer(1),
               PACKAGE = 'pnrfr')

  if(result$out != 0){
    warning(result$msg)
  }
}






