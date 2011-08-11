#! /bin/sh
#

#indent_opts="-npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -psl"
indent_opts="indent -linux -nlp -ci8"

ind="indent $indent_opts"

fsrc() {
  prune="
-name .git 
"
  skip="
-name cscope\*
"

  find $1 \( $prune \) -prune -o -type f ! \( $skip \) -name \*.[ch] -print
}

if test -d $p; then
  for file in $(fsrc $p); do
      $ind -st $file | diff -u $file -
  done
else
  file=$p
  $ind -st $file
fi

