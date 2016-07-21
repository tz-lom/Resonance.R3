extractChannel <- function(file, channelId){
  data <- readStructurized(normalizePath(file, mustWork = T))
  
  si <- Filter(function(x) x$id == channelId, data$streams)[[1]]
  
  do.call(
    merge,
    Filter(
      function(b) identical(SI(b), si) ,
      data$blocks
    )
  )
}