extractAllChannels <- function(file){
  data <- readStructurized(normalizePath(file, mustWork = T))
  
  ret <- lapply(data$streams, function(si){
    flt <- Filter(
      function(b) identical(SI(b), si) ,
      data$blocks
    )
    if(length(flt)>0){
      do.call(
        DBcombine,
        flt
      )
    } else {
      makeEmpty(si)
    }
  })
  
  names(ret) <- sapply(data$streams, '[[', 'name')
  ret 
}