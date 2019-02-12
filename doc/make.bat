SET INKSCAPE="C:\Program Files (x86)\Inkscape\inkscape.exe"

%INKSCAPE% bilder\Blockdiagram.svg --export-png=bilder\Blockdiagram.png --export-dpi=300

pandoc --epub-cover-image=bilder\rs232kabel.gif Betriebsdatenerfassung.md -o Betriebsdatenerfassung.epub

REM pandoc Ebus.md --toc -o eBus.pdf
pandoc -N --template=templates\mytemplate.tex --variable mainfont=Georgia --variable sansfont=Arial --variable monofont="Bitstream Vera Sans Mono" --variable fontsize=11pt --variable version=1.00 Betriebsdatenerfassung.md --toc -o Betriebsdatenerfassung.pdf