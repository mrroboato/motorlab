# build.sh

mkdir mac
mkdir windows
mkdir linux

GOOS=darwin GOARCH=amd64 go build -o mac/gui gui/*.go
cp -r gui/static mac/
zip -r mac.zip mac
mv mac.zip executables

GOOS=windows GOARCH=amd64 go build -o windows/gui.exe gui/*.go
cp -r gui/static windows/
zip -r windows.zip windows
mv windows.zip executables

GOOS=linux GOARCH=amd64 go build -o linux/gui gui/*.go
cp -r gui/static linux/
zip -r linux.zip linux
mv linux.zip executables

rm -r mac
rm -r windows
rm -r linux





