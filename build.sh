for FILE in $(find . -type f -name '*.render.Rmd' | grep -v "_site")
do
    echo $FILE
    R -e "rmarkdown::render(\"$FILE\")" || exit
done