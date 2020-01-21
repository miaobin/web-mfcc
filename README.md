# Web-MFCC

Calculate Mel-frequency cepstral coefficients (MFCCs) in the browser from prepared audio or receive live audio input from the microphone using Javascript [Web Audio API](https://github.com/WebAudio/web-audio-api).

Implement and accelerate Tensorflow 'AudioSpectrogram' and 'Mfcc' operators by compiling the [TensorFlow/lite/kernels](https://github.com/tensorflow/tensorflow/tree/master/tensorflow/lite/kernels) use [emscripten](emscripten.org). So it uses the same parameters as TensorFlow.


# Compile the code

1. Download and install emscripten follow the [instructions](https://emscripten.org/docs/getting_started/downloads.html#installation-instructions).

2. Compile each .c/.cc file to bitcode:
    ```
    emcc -O3 test.cc -o test.bc
    ```
3. Compile all the .bc file to tf_mfcc.bc:
    ```
    emcc *.bc -o tf_mfcc.bc
    ```
4. Compile tf_mfcc.bc to WASM:
    ```
    mkdir wasm

    emcc -O3 -s WASM=1 -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap']" tf_mfcc.bc -o ./wasm/mfcc.js
    ```
    After compile you will get the following files int the wasm folder:
    ```
    wasm
    ├── mfcc.js
    └── mfcc.wasm
    ```

# Run the example
1. Download [release package](https://github.com/miaobin/web-mfcc/releases) and unzip it.
    ```
    tar xzvf web-mfcc-version.tar.gz
    ```

2. Put it in to the sample folder:
    ```
    example
    ├── favicon.ico
    ├── index.html
    ├── main.js
    ├── wasm
    │   ├── mfcc.js
    │   └── mfcc.wasm
    └── yes.wav
    ```

3. Start an http server in the example folder. You can install [http-server](https://github.com/indexzero/http-server) via:
    ```
    npm install http-server -g

    http-server
    ```
4. Open up the browser and access this URL:
    
    http://localhost:8080/

5. Click on Play button to see results from console.
