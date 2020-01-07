#' Computes the moving average of a given channel within the PNRF file
#'
#' @param file_path Path to the PNRF file location, full path or current directory
#' @param channel_no Channel number to retrieve data
#' @param window_time Moving average time interval
#' @param data_type Type of data to retrieve : continuous 1, sweeps = 2, mixed = 3 (default = 3)
#' @param sampling If sampling is required (default = F)
#' @param sampling_rate Rate of sampling (default = 100), applicable if sampling = T
#' @examples
#' read_channel_data("example.PNRF", 3, 0.01)
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


#' Compuates if an alarm needs to be generated based on the positive derivative of the slope
#'
#' @param dat DataFrame containing x = time, y = RMS values
#' @param window_width The interval over which to calculate the slope
#' @param time_limit Limit over time (default = 10s)
#' @param threshold Cut off for change that is indicative of alarm (default = 0.01)
#' @examples
#' compute_alarm(df, 100)
#' @export
check_alarm <-
  function(dat, ...) {

    arguments <- list(...)
    alarm_info <- get_alarm_info(arguments)
    if (is.null(dat) |
        (length(dim(dat)) != 2) |
        (dim(dat)[1] <= 1)) {
      stop('Data is expected to have 2 dimensions with more than 1 row')
    }

    # dat has two columns, time stamp and impedence
    total_time <-  0
    num_times <-  floor((dim(dat)[1]) / alarm_info[['window_width']])
    start_row <-  1
    end_row <-  alarm_info[['window_width']]
    alarm.before <- FALSE
    al.time <- c()

    for(i in 1:num_times){
      x_vals = dat[start_row:end_row,1]
      y_vals = dat[start_row:end_row,2];
      fit_line <-  lm(y_vals~ x_vals);
      total_time = total_time +  x_vals[length(x_vals)] - x_vals[1];

      #Generate an ALARM if the slope is continuously greater than the threshold for T time interval.
      if(total_time < alarm_info[['time_limit']]){
        if(fit_line$coefficients[2] > alarm_info[['threshold']]){
          disp_msg("Alert")
        }else{
          total_time = 0;
        }
      }else{
        if(fit_line$coefficients[2] > alarm_info[['threshold']]){
          disp_msg(paste( "ALARM!!!  Object on powerline for ", signif(total_time/2, digits=3), "s" ))
          if(!alarm.before){
            # last reading of x_val and yVal
            al.time <- cbind(x_vals[length(x_vals)],y_vals[length(x_vals)])
            alarm.before <- TRUE
          }
        }else{
          total_time = 0;
        }
      }
      start_row = start_row + (alarm_info[['window_width']] / 2);
      end_row = end_row + (alarm_info[['window_width']] / 2);
    }

    return(c(al.time, alarm.before))
  }



#' Compuates and plot if an alarm needs to be generated based on the positive derivative of the slope
#'
#' @param rms_data DataFrame containing x = time, y = RMS values
#' @param plot_graph T/F for plotting alarm graph
#' @param window_width The interval over which to calculate the slope
#' @param time_limit Limit over time (default = 10s)
#' @param x_lab x-axis label for graph, if plot_graph = T
#' @param y_lab y-axis label for graph, if plot_graph = T
#' @param title Title for graph, if plot_graph = T
#' @examples
#' compute_and_plot_alarm(df, 100)
#' @export
compute_alarm <- function(rms_data, plot_graph = T, ...){
  al.time <- compute_alarm(rms_data, ...)
  arguments <- list(...)
  x_lab <- if(!is.null(arguments[['x_lab']]) && arguments[['x_lab']] !='') arguments[['x_lab']] else 'Time'
  y_lab <- if(!is.null(arguments[['y_lab']]) && arguments[['y_lab']] =='') arguments[['y_lab']] else 'RMS'
  title <- if(!is.null(arguments[['title']]) && arguments[['title']] =='') arguments[['title']] else 'RMS current'
  if(plot_graph){
    plot(result[,2],type="l", main= title, ylab=y_lab, xlab = x_lab)
    if((!is.null(al.time)) & (length(al.time) == 3) & (al.time[3]== T)){
      points(al.time[1]*100, al.time[2], pch=19, col="red", cex=2)
    }
  }
}


#' Compuates second derivative of the time series
#'
#' @param x DataFrame of time series RMS value
#' @param window_size Window size
#' @export
compute_second_derivative <- function(dat, window_size = 1){
  if(window_size > 1){
    # https://www.r-bloggers.com/a-quick-way-to-do-row-repeat-and-col-repeat-rep-row-rep-col/
    dat <- matrix(rep(dat, each = window_size), ncol = window_size, byrow = T)
    end <- dim(dat)[1]
    # for moving avg calculation using vector
    dat <- ldply(lapply(1:ncol(dat), function(idx) {
      (c(rep(NA, (idx - 1)), dat[,idx]))
      }), rbind)

    dat <- dat[3:end]
  }

  if(class(dat) == 'numeric' || class(dat) == 'integer'){
    window_size <- length(dat)
    dat <- data.frame(v = dat)
  }

  second.deriv <- apply(dat, 2, function(x) {
    fit <- lm(x ~ poly(1:window_size,2), na.action = na.omit)
    coeffs <- fit$coefficients
    out <- 2 * coeffs[3] * (length(x)-1)
    return(out)
  })

  return(second.deriv)
}

#' Compuates ts features of a given window size using ACM package
#'
#' @param rms_data DataFrame of RMS values
#' @param window_size Window size
#' @param norm = T, Normalise the features
#' @export
#' @importFrom anomalousACM tsmeasures
compute_feautres <- function(rms_data, window_size = 50, norm = T){
  if((!is.data.frame(rms_data)) || (!all(dim(rms_data) > 0))){
    stop('Data should have a min of 2 rows')
  }
  if(window_size < 1){
    stop('Window size should be greater than 1')
  }

  rms.m <- rms_data[, length(rms_data)]
  if((length(rms.m) %% window_size) != 0){
    add.nas <- window_size - (length(rms.m) %% window_size)
    if(window_size - add.nas > 30){
      rms.m <- c(rms.m, rep(NA, add.nas))
    }
    else{
      size <- trunc(length(rms.m)/window_size) * window_size
      rms.m <- rms.m[1:size]
    }
  }

  rms.m <- matrix(rms.m, nrow = window_size)
  second.deriv <- apply(rms.m, 2, compute_second_derivative)
  features.m <- tsmeasures(rms.m, normalise = norm)
  features.m <- cbind(features.m, second.deriv)
  return(features.m)
}

