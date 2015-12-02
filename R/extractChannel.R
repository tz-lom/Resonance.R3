extractChannel <- function(file, channelId){
  blocks <- blockLevelRead(normalizePath(file, mustWork = T))
  
  streams <- list()
  
  for(h in 1:4){
    if(inherits( blocks[[h]], 'StreamDescription')){
      streams[ blocks[[h]]$name ] <- blocks[[h]]$id
    }
  }
  
  eegL <- Filter(
    function(b){
      if(!inherits(b, "DataBlock")) return(F)
      if(attr(b, 'stream')!=channelId) return(F)
      T
    },
    blocks
  )
  
  if(is.matrix(eegL[[1]])){
    cols <- ncol(eegL[[1]])
    do.call(
      rbind,
      lapply(eegL, matrix, ncol=cols)
    )
  } else {
    eegL
  }
  
}