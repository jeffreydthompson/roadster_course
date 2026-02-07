#!/usr/bin/bash
# Compile and upload Arduino sketches to the ESP32 using arduino-cli

# CONFIG
BOARD_FQBN="esp32:esp32:esp32c3"

if ! command arduino-cli &> /dev/null; then
  echo "Error: arduino-cli not found.  Please install it first."
fi

# FUNCTIONS
usage() {
  echo "Usage: $0 {compile|upload} sketch_folder [port]"
  echo "  compile: Compile the sketch"
  echo "  upload: Upload the sketch to ESP32 (requires port)"
  echo "Example:"
  echo "  $0 compile blink"
  echo "  $0 upload blink /dev/ttyUSB0"
}

compile_sketch() {
  local sketch="$1"
  echo "Compiling sketch: $sketch for $BOARD_FQBN"
  arduino-cli compile --fqbn "$BOARD_FQBN" "$sketch" || { echo "Compile failed"; exit 1; }
  echo "Compile successful"
}

upload_sketch() {
  local sketch="$1"
  local port="$2"

  if [ -z "$port" ]; then
     echo "Error: You must specify the port for upload"
     usage
     exit 1
  fi

  echo "Uploading sketch: $sketch to port $port ..."
  arduino-cli upload -p "$port" --fqbn "$BOARD_FQBN" "$sketch" || { echo "Upload failed"; exit 1; }
  echo "Upload successful"
}

# MAIN
if [ $# -lt 2 ]; then
  usage
  exit 1
fi

COMMAND="$1"
SKETCH="$2"
PORT="$3"

case "$COMMAND" in 
  compile)
    compile_sketch "$SKETCH"
    ;;
  upload)
    upload_sketch "$SKETCH" "$PORT"
    ;;
  *)
    echo "Error: Unknown command '$COMMAND'"
    usage
    exit 1
    ;;
esac
