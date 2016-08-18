extractAllChannels <- function(file){
  data <- readStructurized(normalizePath(file, mustWork = T))
  
  ret <- lapply(data$streams, function(si){
    do.call(
      merge,
      Filter(
        function(b) identical(SI(b), si) ,
        data$blocks
      )
    )
  })
  
  names(ret) <- sapply(data$streams, '[[', 'name')
  ret 
}