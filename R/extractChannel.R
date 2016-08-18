extractChannel <- function(file, channel){
  data <- readStructurized(normalizePath(file, mustWork = T))
  
  if(is.numeric(channel)){
    si <- Filter(function(x) x$id == channel, data$streams)[[1]]
  } else {
    si <- Filter(function(x) x$name == channel, data$streams)[[1]]
  }
  
  do.call(
    merge,
    Filter(
      function(b) identical(SI(b), si) ,
      data$blocks
    )
  )
}