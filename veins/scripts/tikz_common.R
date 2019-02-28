paper.width <<- 345/72
paper.height <<- 345/72

require('tikzDevice')
options(tikzLatexPackages=c(getOption('tikzLatexPackages'), "\\usepackage[utf8]{inputenc}", "\\usepackage[T1]{fontenc}", "\\usetikzlibrary{backgrounds}", "\\usepackage{amsmath}", "\\usepackage{amsthm}", "\\usepackage{amsfonts}", "\\usepackage{amssymb}")) #, "\\setmathfont{Times New Roman}"
#options(tikzLatexPackages=c(getOption('tikzLatexPackages'), "\\usepackage[utf8]{inputenc}", "\\usepackage{lmodern}", "\\usepackage{helvet}", "\\usepackage[T1]{fontenc}", "\\usetikzlibrary{backgrounds}", "\\usepackage{amsmath}", "\\usepackage{amsthm}", "\\usepackage{amsfonts}", "\\usepackage{amssymb}"))
#options(tikzDocumentDeclaration = "\\LoadClass[a4paper,10pt,onecolumn,oneside,openright]{report}")
#options(tikzDocumentDeclaration = "\\documentclass[10pt,report,a4paper]{report}")
myps <- function(file, paper="special", width=6, height=4, family="Times New Roman") {
    tikz(paste(file, ".tex", sep=""), standAlone = FALSE, width=width*0.95, height=height*0.95)
    done <- function() {
        graphics.off()
        #system(paste("pdflatex ", file, ".tex", sep=""))
    }
    return(done)
}
