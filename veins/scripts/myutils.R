require(Hmisc)

plot_ecdf <- function(vec, clr, style, thick, xlen) {
  #get a function to get the ecdf
  e_ <- ecdf(vec)
  #make a new plot
  #plot.new()
  
  #set plot limits
  #plot.window(xlim=c(0,max(vec, vec)), ylim=c(0,1))
  
  #get 1000 values from 0 to max(data$channelUtilization)
  val <- seq(0, xlen, length.out=1000)
  #draw some lines
  lines(val, e_(val), col=clr, lty=style, lwd=thick)
  #axes
  axis(1)
  #las=2 to draw y-axis horizontal
  axis(2, las=2)
  #draw a box
  box()
  
  #write some axis text
  #title(xlab=x)
  #title(ylab=y)
}

plot_new_ecdf <- function(vec, xlab, ylab, clr, thick) {
  #get a function to get the ecdf
  e_ <- ecdf(vec)
  #make a new plot
  #plot.new()
  
  #set plot limits
  #plot.window(xlim=c(0,max(vec, vec)), ylim=c(0,1))
  
  #get 1000 values from 0 to max(data$channelUtilization)
  val <- seq(0,max(vec, vec), length.out=1000)
  #draw some lines
  lines(val, e_(val), col=clr, lwd=thick)
  #axes
  axis(1)
  #las=2 to draw y-axis horizontal
  axis(2, las=2)
  #draw a box
  box()
  
  #write some axis text
  title(xlab=xlab)
  title(ylab=ylab)
  
  mean <- mean(vec)
  abline(v =mean, col=clr, lty="dotted")
  # if (!print_title) {
  #   confint <- t.test(vec)$conf.int
  #   confint_l <- confint[1]
  #   confint_h <- confint[2]
  # }
  
  # title(main=paste("mean:", toString(mean)), sub=paste("conf:[", toString(confint_l), ", ", toString(confint_h), "]"))
  # if (print_title) {
  #   confint <- t.test(vec)$conf.int
  #   confint_l <- confint[1]
  #   confint_h <- confint[2]
  #   title(main=paste("mean:", toString(mean), "\n conf:[", toString(confint_l), ", ", toString(confint_h), "]"))
  # } else {
  #   title(main=paste("mean:", toString(mean)))
  # }
  
  # legend(title = paste("mean:", toString(mean), "conf:[", toString(confint_l), ", ", toString(confint_h), "]"), )
}

saveAllPlots <- function(pdfName, plots) {
  pdf(pdfName, onefile=TRUE)
  for (plot in plots) {
    replayPlot(plot)
  }
  # legend(1, legend=c("LTE", "No LTE"), col=c("red", "blue"), lty=1)
  graphics.off()
}
