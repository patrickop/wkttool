# WKT TOOL

a graphical tool for debugging boost geometries

## Building

1. Install conan and additional repos
  ```
  pip install conan
  conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
  conan remote add qchateau https://api.bintray.com/conan/qchateau/qchateau
  ```
2. Build and run the app
  ```
  make
  make run
  ```


## Usage

Enter WKT strings into the Editor window. Multiple strings can be seperated by semicolon.
Click import to display the shapes.
They can be removed by the buttons in the edit window.

