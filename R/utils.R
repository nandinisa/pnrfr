check <- function(type, arg){

  check_file <- function(){
    if(!file.exists(arg) | (file.access(arg, mode = 4) != 0)){
      stop("Please check if the file exists and you have read access")
    }

    get_file_path <- function(){
      if(R.utils::isAbsolutePath(arg)){
        return(arg)
      }

      return(file.path(getwd(), arg))
    }

    return(get_file_path())
  }

  check_directory <- function(path, access_mode){
    if(R.utils::isAbsolutePath(path)){
      dir = dirname(path)
      if(!file.exists(dir) | (file.access(dir, mode = access_mode) != 0)){
        stop(paste("Please check if the directory exists and you have read/write access to dir -", dir))
      }
      else{
        return (path)
      }
    }
    else{
      return (check_directory(file.path(getwd(), path), access_mode))
    }
  }

  check_channel <- function(){
    if(arg < 1) stop("Invalid channel, channel number must be greater or eqaul to 1")
    return(arg)
  }

  check_segment <- function(){
    if(arg < 1) stop("Invalid segment, segment number must be greater or eqaul to 1")
    return(arg)
  }

  check_data_type <- function(){
    if(arg < 1 || arg > 3) stop("Invalid data type, data type can be continuous(1), sweeps (2) or mixed (3)")
    return(arg)
  }

  switch (type,
    f = check_file(),
    dr = check_directory(arg, access_mode = 4),
    dw = check_directory(arg, access_mode = 2),
    c = check_channel(),
    s = check_segment(),
    dt = check_data_type()
  )
}

create_named_list <- function(name, vec){
  new_list = as.list(vec)
  names(new_list) <- paste(name, 1:length(vec), sep = "")
  return(new_list)
}

stop_err <- function(err){
  stop(paste("Error occurred in pnrfr.dll", err, sep = ' - '))
}

disp_msg <- function(msg){
  if(!is.null(print.alert) & print.alert == T){
    print(msg)
  }
}
