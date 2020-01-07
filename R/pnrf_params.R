get_data_type <- function(arguments){
  data_type <- arguments[['data_type']]
  if(is.null(data_type) ||
     !is.numeric(data_type) ||
     !check('dt', data_type)){
    data_type <- 3
  }

  return(data_type)
}

get_sampling_rate <- function(arguments){
  sampling <- arguments[['sampling']]
  sample_list <- list()

  sample_list[[1]] <- F
  sample_list[[2]] <- -1
  if(!is.null(sampling) && is.logical(sampling) && sampling == T){
    sample_list[[1]] <- T
    sample_list[[2]] <- 100
    sampling_rate <- arguments[['sampling_rate']]
    if(!is.null(sampling_rate) && is.numeric(sampling_rate) && sampling_rate > 100){
      sample_list[[2]] <- sampling_rate
    }
  }

  return (sample_list)
}

create_pnrfr_obj <- function(file_path, channel_meta,
                             c_detail, data_type,
                             sampling_list, type, c_data){
  result <- structure(list(), class = "pnrfr")
  result[['name']] <- basename(file_path)
  result[['channels']] <- data.frame(number = channel_meta$channel,
                                     name = channel_meta$name,
                                     type = channel_meta$type,
                                     yunit = channel_meta$yunit,
                                     start_time = channel_meta$startTime,
                                     end_time = channel_meta$endTime,
                                     no_of_samples = c_detail$size,
                                     sampling_rate =
                                       if(sampling_list[[2]] == -1) NA else sampling_list[[2]],
                                     data_type = data_type,
                                     stringsAsFactors = F)
  result[['channels']][[type]] <- list(data.frame(
    X = c_data$timeArray,
    Y = c_data$dataArray))

  return(result)
}

# Gets default params for alarm generation
# window_width, time_limit, threshold
get_alarm_info <- function(arguments){
  window_width <- 100
  time_limit <- 10
  threshold <- 0.01


  check_numbers <- function(varible){
    return (!is.null(arguments[[varible]]) && is.numeric(arguments[[varible]])
            && as.numeric(arguments[[varible]]))
  }
  if(check_numbers('window_width')){
    window_width <- arguments[['window_width']]
  }
  if(check_numbers('time_limit')){
    time_limit <- arguments[['time_limit']]
  }
  if(check_numbers('threshold')){
    threshold <- arguments[['threshold']]
  }

  alarm_info <- list('window_width' = window_width,
                     'time_limit' = time_limit,
                     'threshold' = threshold)

  return(alarm_info)
}
