R3view <- function(file){

  if(!any(.packages(all.available = T) == 'shiny')){
    cat('Install package "shiny"\n execute following command in console:\ninstall.packages(\'shiny\')')
    stop()
  }  
  
  if(!any(.packages(all.available = T) == 'Resonance')){
    cat('Install package "Resonance"\n execute following command in console:\ninstall.packages(\'Resonance\')')
    stop()
  }  
  library(shiny)
  library(Resonance)
  library(R.cache)
  
  data <- memoizedCall(extractAllChannels, file)
  
  minmax <- sapply(data, function(stream){
    switch(
      SI(stream)$type,
      channels = 
        c(min(attr(stream, 'TS')), max(attr(stream, 'TS')))
      ,
      event = {
        times <- sapply(stream, attr, 'TS')
        c(min(times), max(times))
      }
    )
  })
  minmax <- c( min(minmax[1,]), max(minmax[2,]) )
  
  # Define UI for application that draws a histogram
  ui <- shinyUI(fluidPage(
    
    # Application title
    h4(file),
    
    # Sidebar with a slider input for number of bins 
    plotOutput("signalPlot",height = "500px"),
    
    column(
      width = 12,
      sliderInput("timeframe", 
                  "Time",
                  min = 0,
                  max = ceiling((minmax[2]-minmax[1])/1E6), 
                  value=c(0,1),
                  width="100%",
                  sep="")
    ),
    column(
      width=1,
      actionButton("fastLeft","<<")
      ),
    column(
      width=1,
      actionButton("slowLeft","<")
    ),
    column(
      width=8,
      div(style="display:inline-block",
          tags$label("from", `for` = 'windowBegin'), 
          tags$input(id = 'windowBegin', type = "number", value = 0, min=0, max=ceiling((minmax[2]-minmax[1])/1E6)),
          
          tags$label("size", `for` = 'windowSize'), 
          tags$input(id = 'windowSize', type = "number", value = 1, min=0, max=ceiling((minmax[2]-minmax[1])/1E6)),
          
          tags$label("to", `for` = 'windowEnd'), 
          tags$input(id = 'windowEnd', type = "number", value = 1, min=0, max=ceiling((minmax[2]-minmax[1])/1E6))
          ),
      div(style="display:inline-block",
          tags$label("highpass", `for` = 'lowFreq'), 
          tags$select(id = 'lowFreq', shiny:::selectOptions(list('Off'='Off', '0.01'=0.01, '0.05'=0.05, '0.1'=0.1, '0.5'=0.5, '1'=1, '2'=2, '5'=5 )) ),
          
          tags$label("lowpass", `for` = 'highFreq'), 
          tags$select(id = 'highFreq', shiny:::selectOptions(list('Off'='Off', '5'=5, '10'=10, '15'=15, '20'=20, '25'=25, '40'=40, '60'=60, '100'=100 )) ),
          
          tags$label("notch", `for` = 'notch'), 
          tags$select(id = 'notch', shiny:::selectOptions(list('Off'='Off', '50'=50, '60'=60)) )
      )
      
      
    ),
    column(
      width=1,
      actionButton("slowRight",">")
    ),
    column(
      width=1,
      actionButton("fastRight",">>")
    )
  ))
  
  # Define server logic required to draw a histogram
  server <- shinyServer(function(input, output, session) {
    updateBeginEnd <- function(begin, end){
      if(input$windowBegin!=begin) updateNumericInput(
        session,
        'windowBegin',
        value = begin
      )
      if(input$windowEnd!=end) updateNumericInput(
        session,
        'windowEnd',
        value = end
      )
      if(input$windowSize!= end-begin) updateNumericInput(
        session, 
        "windowSize",
        value= end-begin
      )
      if(any(input$timeframe != c(begin,end))) updateSliderInput(
        session, 
        "timeframe", 
        value=c(begin, end)
      )
    }
    
    observeEvent(input$slowLeft, {
      shift <- (input$timeframe[2]-input$timeframe[1])/2
      if(input$timeframe[1] - shift<0){
        shift <- input$timeframe[1]
      }
      updateBeginEnd(input$windowBegin - shift, input$windowEnd - shift)
    })
    
    observeEvent(input$fastLeft, {
      shift <- (input$timeframe[2]-input$timeframe[1])
      if(input$timeframe[1] - shift<0){
        shift <- input$timeframe[1]
      }
      updateBeginEnd(input$windowBegin - shift, input$windowEnd - shift)
    })
    
    observeEvent(input$slowRight, {
      shift <- (input$timeframe[2]-input$timeframe[1])/2
      if(input$timeframe[2] + shift > minmax[2]){
        shift <- minmax[2]-input$timeframe[2]
      }
      updateBeginEnd(input$windowBegin + shift, input$windowEnd + shift)
    })
    
    observeEvent(input$fastRight, {
      shift <- (input$timeframe[2]-input$timeframe[1])
      if(input$timeframe[2] + shift > minmax[2]){
        shift <- minmax[2]-input$timeframe[2]
      }
      updateBeginEnd(input$windowBegin + shift, input$windowEnd + shift)
    })
    
    observeEvent(input$windowSize, {
      begin <- input$windowBegin
      end <- input$windowBegin + input$windowSize
      updateBeginEnd(begin, end)
    })
    
    observeEvent(input$windowBegin, {
      begin <- input$windowBegin
      end <- input$windowBegin + input$windowSize
      updateBeginEnd(begin, end)
    })
    
    observeEvent(input$windowEnd, {
      begin <- input$windowEnd - input$windowSize
      end <- input$windowEnd
      updateBeginEnd(begin, end)
    })
    
    observeEvent(input$timeframe, {
      begin <- input$timeframe[1]
      end <- input$timeframe[2]
      updateBeginEnd(begin, end)
    })
    
    filtered <- reactive({
      
      lapply(data, function(stream){
        if(SI(stream)$type=='channels'){
          
          lf <- if(input$lowFreq=='Off') FALSE else as.numeric(input$lowFreq)
          hf <- if(input$highFreq=='Off') FALSE else as.numeric(input$highFreq)
          nf <- if(input$notch=='Off') FALSE else as.numeric(input$notch)
          
          pipe.bandFilter(stream, lf, hf, nf)
        } else {
          stream
        }
      })
      
    })
    
    output$signalPlot <- renderPlot({
      
      minTS <- minmax[1] + input$windowBegin*1E6
      maxTS <- minmax[1] + input$windowEnd*1E6
      
      if(maxTS<=minTS) return()
      
      toPlot <- toPlotInit()
      
      for(stream in filtered()){
      
        if(SI(stream)$type=='channels'){
          TS <- attr(stream, 'TS')
          
          ind <- which(TS>=minTS & TS<=maxTS)
          toPlot <- toPlotChannels(toPlot, stream[ind, ], TS[ind])
          
        }
        if(SI(stream)$type == 'event'){
          marks <- sapply(stream, function(ev){
            if(minTS <= attr(ev, 'TS') && attr(ev, 'TS') <= maxTS)
              attr(ev,'TS')
            else
              NA
          })
          
          marks <- na.omit(marks)
          toPlot <- toPlotMarks(toPlot, marks)
        }
      }
      
      letsPlot(toPlot)
    })
  })
  
  # Run the application 
  shinyApp(ui = ui, server = server)
  
}